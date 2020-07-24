#include "processing.h"
#include "optimizer.h"
#include "partition.h"
#include <fstream>
#include <unordered_map>
using namespace std;
using namespace cv;
extern Image input_image;
extern Optimizer * optimizer;
extern Partition* tess;
extern int nClusters;
extern int * valid_mapping;
extern string init_mesh_name;

PostProcessor::PostProcessor()
{

}

void PostProcessor::cleanClusters()
{
	vector<bool> pixel_visited = vector<bool>(input_image.width * input_image.height, false);

	for (int i=0; i<nClusters; i++)
	{
		int cluster_id = valid_mapping[i];

		vector<PixelIdx> flooding_pixels;
		vector<set<PixelIdx>> sub_clusters;
		vector<set<int>> sub_cluster_neighbors;
		set<PixelIdx> remain_components = tess->clusters[cluster_id];
		int ncluster = 0;
		set<PixelIdx> new_cluster;
		set<int> new_cluster_neighbors;
		//separate each connected components as a sub-cluster
		while (!remain_components.empty())
		{
			ncluster++;
			flooding_pixels.clear();
			PixelIdx starting_face = *(remain_components.begin());
			flooding_pixels.push_back(starting_face);		
			remain_components.erase(starting_face);
			pixel_visited[starting_face] = true;

			new_cluster.clear();
			new_cluster_neighbors.clear();
			new_cluster.insert(starting_face);

			//find the new cluster by flooding
			while(!flooding_pixels.empty())
			{
				vector<int>::iterator it = flooding_pixels.begin();
				int p_id = *it;
				flooding_pixels.erase(it);
				vector<int> neighbor_pixels = input_image.pixel_neighbors[p_id];

				for (vector<int>::iterator f_it = neighbor_pixels.begin(); f_it != neighbor_pixels.end(); f_it++)
				{
					PixelIdx neighbor_face = *f_it;
					//the neighbors in the set should be marked as connected
					set<PixelIdx>::iterator cluster_neighbor_it = remain_components.find(neighbor_face);
					if ( cluster_neighbor_it != remain_components.end())
					{
						if ( !pixel_visited[*cluster_neighbor_it] )
						{
							flooding_pixels.push_back(*cluster_neighbor_it);
							pixel_visited[*cluster_neighbor_it] = true;
							new_cluster.insert(*cluster_neighbor_it);
							remain_components.erase(cluster_neighbor_it);
						}			
					}
					new_cluster_neighbors.insert(tess->cluster_belong[neighbor_face]);
				}
			}

			sub_clusters.push_back(new_cluster);
			new_cluster_neighbors.erase(cluster_id);
			sub_cluster_neighbors.push_back(new_cluster_neighbors);
		}

		int largest_component_id = 0;
		int largest_size = sub_clusters[0].size();
		for (int j=0; j< ncluster; j++)
		{
			if (sub_clusters[j].size() > largest_size)
			{
				largest_component_id = j;
				largest_size = sub_clusters[j].size();
			}
		}



		////hold the largest connected component as a cluster
		////update the cluster, covariance matrix, set the centroid as the node pos
		////keep clusters with more than 3 neighboring clusters
		//if (sub_cluster_neighbors[largest_component_id].size() >= 3) 
		//kepp clusters that contains more than 30 pixels, and the number of clusters may be less than the user desired.
		if (sub_clusters[largest_component_id].size() >= 30)
		{
			tess->clusters[cluster_id] = sub_clusters[largest_component_id];
			CovDet cov = CovDet();
			for(set<PixelIdx>::iterator f_it = sub_clusters[largest_component_id].begin(); f_it != sub_clusters[largest_component_id].end(); f_it++)
			{
				cov += optimizer->init_covariance(*f_it);
				pixel_visited[*f_it] = false;
			}
			optimizer->covariance(cluster_id) = cov;
		}
		else
		{
			largest_component_id = -1;
			tess->clusters[cluster_id].clear();
			optimizer->covariance(cluster_id) = CovDet();
		}

		


		//merge other connected components
		for (int component_id =0; component_id < ncluster; component_id ++)
		{
			if (component_id != largest_component_id)
			{
				CovDet cov_i = CovDet();
				for(set<PixelIdx>::iterator f_it = sub_clusters[component_id].begin(); f_it != sub_clusters[component_id].end(); f_it++)
				{
					cov_i += optimizer->init_covariance(*f_it);			
				}

				int cluster_closest = *(sub_cluster_neighbors[component_id].begin());
				double energy_increased_smallest = 1e20;
				//merge with its neighbors
				CovDet cov_j;
				CovDet C;
				for (set<int>::iterator cluster_it = sub_cluster_neighbors[component_id].begin(); cluster_it != sub_cluster_neighbors[component_id].end(); cluster_it++)
				{
					cov_j=optimizer->covariance(*cluster_it);
					C=cov_i;  C+=cov_j;

					double energy_increased = C.energy() - cov_i.energy() - cov_j.energy();
					//assert(energy_increased >= 0);
					if (energy_increased < energy_increased_smallest)
					{
						cluster_closest = *cluster_it;
						energy_increased_smallest = energy_increased;
					}
				}

				//update the cluster, covariance matrix, set the centroid as the node pos
				for(set<PixelIdx>::iterator f_it = sub_clusters[component_id].begin(); f_it != sub_clusters[component_id].end(); f_it++)
				{
					tess->clusters[cluster_closest].insert(*f_it);
					tess->cluster_belong[*f_it] = cluster_closest;
					pixel_visited[*f_it] = false;
				}
				optimizer->covariance(cluster_closest) += cov_i;
			}
		}
	}

	delete [] valid_mapping;
	valid_mapping = new int[nClusters];
	nClusters = 0;
	int n_domain = 0;
	for(PixelIdx j=0; j<tess->size; j++)
	{
		if (tess->clusters[j].size() != 0)
		{
			valid_mapping[nClusters] = j;
			nClusters ++;
		} 
	}
}

void PostProcessor::save_results(string sp_folder, string par_folder, string file_name)
{
	save_superpixels(sp_folder, file_name);
	//save_partition_img(par_folder, file_name);
	save_partition_txt(par_folder, file_name);
}


void PostProcessor::save_superpixels(string sp_folder, string file_name)
{
	int boundary_width = 1;
	Mat superpixel_mat = input_image.img.clone();
	//IplImage* super_pixel = cvCreateImage( cvGetSize(input_image.img), input_image.img->depth, 3 );
	for(int i=0; i< superpixel_mat.rows; i++)
	{
		uchar* ptr1 = superpixel_mat.ptr<uchar>(i);
		//uchar* ptr1 = (uchar*)(super_pixel->imageData + i*super_pixel->widthStep );
		//uchar* ptr2 = (uchar*)(input_image.img->imageData + i*input_image.img->widthStep );
		for (int j=0; j< superpixel_mat.cols; j++)
		{
			set<int> neighbor_clusters;
			for (int m = i-boundary_width; m < i+boundary_width; m++)
			{
				for (int n = j-boundary_width; n < j+boundary_width; n++)
				{
					if(m < 0 || m >= superpixel_mat.rows || n < 0 || n >= superpixel_mat.cols)
						neighbor_clusters.insert(-1);
					else
						neighbor_clusters.insert(tess->cluster_belong[m * superpixel_mat.cols + n]);
				}
			}
			if(neighbor_clusters.size() == 1)
			{
				/*ptr1[3*j] = ptr2[3*j];
				ptr1[3*j+1] = ptr2[3*j+1];
				ptr1[3*j+2] = ptr2[3*j+2];*/
			}
			else
			{
				ptr1[3*j] = 0;
				ptr1[3*j+1] = 0;
				ptr1[3*j+2] = 0;
			}
		}
	}
	string save_name = sp_folder + "\\" + file_name + ".png";
	cout << "save superpixel to " << save_name.c_str() << endl;
	imwrite(save_name, superpixel_mat);
	/*cvSaveImage(save_name.c_str(), super_pixel);
	cvReleaseImage(&super_pixel);*/
}

void PostProcessor::save_partition_img(string par_folder, string file_name)
{
	Mat partition_mat = Mat::zeros(input_image.img.size(), input_image.img.type());
	//IplImage *img = cvCreateImage( cvGetSize(input_image.img), input_image.img->depth, 3 );
	int pixel_id = 0;
	for(int i=0; i< partition_mat.rows; i++)
	{
		//uchar* ptr = (uchar*)(img->imageData + i*img->widthStep );
		uchar* ptr = partition_mat.ptr<uchar>(i);
		for (int j=0; j< partition_mat.cols; j++)
		{
			int cluster_id = tess->cluster_belong[pixel_id];
			ptr[3*j + 2] = 255 * tess->red[cluster_id];
			ptr[3*j] = 255 * tess->blue[cluster_id];
			ptr[3*j + 1] = 255 * tess->green[cluster_id];
			pixel_id++;
		}
	}
	string save_name = par_folder + "\\" + file_name + "_partition.png";
	imwrite(save_name, partition_mat);
	/*cvSaveImage(save_name.c_str(), img);
	cvReleaseImage(&img);*/
}

void PostProcessor::save_partition_txt(string par_folder, string file_name)
{
	unordered_map<int, int> cluster_mapping;
	for (int i = 0; i < nClusters; i++)
		cluster_mapping[valid_mapping[i]] = i;

	string save_name = par_folder + "\\" + file_name + ".txt";
	ofstream of(save_name);
	for(int i = 0; i < input_image.height; i++)
	{
		for (int j = 0; j < input_image.width; j++)
		{
			int pixel_id = i * input_image.width + j;
			int cluster_id = tess->cluster_belong[pixel_id];
			of << (cluster_mapping[cluster_id] + 1) << '\t';
		}
		of << '\n';
	}
	of.close();
}