#include "optimizer.h"
#include "image.h"
#include <numeric>
#include <unordered_map>
extern Image input_image;
#include "partition.h"
extern Partition* tess;
#include <vector>
extern int nClusters;
extern int * valid_mapping;
extern string image_file_name;
extern float location_ratio;
#include <fstream>

Optimizer::Optimizer()
{
	dx = 1;
	dy = 1;
	gridw = input_image.width;
	gridh = input_image.height;

	swap_started = false;
}

Optimizer::Optimizer(int dx, int dy)
{
	this->dx = dx;
	this->dy = dy;
	gridw = input_image.width/dx + (input_image.width%dx? 1 : 0);
	gridh = input_image.height/dy + (input_image.height%dy? 1 : 0);

	swap_started = false;
}

void Optimizer::allocate_space()
{
	nPixel = input_image.height * input_image.width;
	nClusters = gridw * gridh;
	__cov = vector<Covariance>(nClusters, Covariance());
	initial_cov = vector<Covariance>(nPixel, Covariance());

	cluster_neighbor_links = vector<vector<cluster_pair>>(nClusters); 
	cluster_boundary_pixels = vector<set<PixelIdx>>(nClusters);
	contractions= vector<vector<Cluster_Pair*>>(nClusters);
}

Optimizer::~Optimizer()
{
	delete tess;
	/*if(valid_mapping!=NULL)
		delete valid_mapping;*/
}

int Optimizer::pixelID(int x, int y)
{
	return y * input_image.width + x;
}

void Optimizer::init_cov()
{
	allocate_space();
	tess = new Partition(input_image.width, input_image.height, dx, dy);
	init_covariance();
	collect_contraction();
}

void Optimizer::init_covariance()
{
	for (int i = 0; i < input_image.height; i++)
	{
		//uchar* ptr = (uchar*)(input_image.labimg->imageData + i*input_image.labimg->widthStep);
		uchar* ptr = input_image.labimg.ptr<uchar>(i);
		for (int j=0; j<input_image.width; j++)
		{
			Vector5d coor;
			coor << j, i, ptr[3*j], ptr[3*j+1], ptr[3*j+2];
			Covariance cov(coor, 1.0);
			init_covariance(pixelID(j, i)) = cov;

			int cluster_id = i/dy * gridw + j/dx;
			covariance(cluster_id) += cov;
		}
	}
}

void Optimizer::collect_contraction()
{
	//8 neighbors
	for (int i = 0; i < gridh; i++)
	{
		for (int j = 0; j < gridw; j++)
		{
			int cluster_id = i * gridw + j;
			if(j+1 < gridw)
				create_pair(cluster_id, cluster_id+1);
			if(i+1 < gridh)
			{
				create_pair(cluster_id, cluster_id + gridw);
				if(j > 0)
					create_pair(cluster_id, cluster_id+gridw-1);
				if(j+1 < gridw)
					create_pair(cluster_id, cluster_id+gridw+1);
			}
		}
	}

	////4 neighbors
	//for (int i = 0; i < gridh; i++)
	//{
	//	for (int j = 0; j < gridw; j++)
	//	{
	//		int cluster_id = i * gridw + j;
	//		if (j + 1 < gridw)
	//			create_pair(cluster_id, cluster_id + 1);
	//		if (i + 1 < gridh)
	//		{
	//			create_pair(cluster_id, cluster_id + gridw);
	//			
	//		}
	//	/*	if (j > 0)
	//			create_pair(cluster_id, cluster_id - 1);
	//		if (i>0)
	//			create_pair(cluster_id, cluster_id -gridw);*/
	//	}
	//}
}

void Optimizer::create_pair(PixelIdx j, PixelIdx k)
{
	Cluster_Pair *info = new Cluster_Pair();
	info->p1 = j;
	info->p2 = k;
	compute_pair_info(info);

	contractions[j].push_back(info);
	contractions[k].push_back(info);
}

void Optimizer::compute_pair_info(Cluster_Pair* info)
{
	compute_energy(info);
	finalize_pair_update(info);
}

void Optimizer::compute_energy(Cluster_Pair *info)
{
	PixelIdx i=info->p1, j=info->p2;

	Covariance &Ci=covariance(i), &Cj=covariance(j);
	Covariance C=Ci;  C+=Cj;

	double err = C.energy() - Ci.energy() - Cj.energy();
	info->heap_key(-err);
}

void Optimizer::finalize_pair_update(Cluster_Pair *info)
{
	if( info->is_in_heap() )
		heap.update(info);
	else
		heap.insert(info);
}

bool Optimizer::decimate(uint target)
{
	while( nClusters > target )
	{
		Cluster_Pair *info = (Cluster_Pair *)heap.extract();
		if( !info ) 
		{
			return false;
		}
			
		PixelIdx v1=info->p1, v2=info->p2;
		if( !tess->clusters[v1].empty() && !tess->clusters[v2].empty() )
		{
			apply_contraction(info);
			nClusters--;
			delete info;
		}			
	}

	valid_mapping = new int[nClusters];
	int n_domain = 0;
	for(PixelIdx j=0; j<tess->size; j++)
	{
		if (tess->clusters[j].size() != 0)
		{
			valid_mapping[n_domain] = j;
			n_domain ++;
		} 
	}
	cout << "nClusters:" << nClusters << endl;
	

	////output decimate result
	//unordered_map<int, int> cluster_mapping;
	//for (int i = 0; i < nClusters; i++)
	//	cluster_mapping[valid_mapping[i]] = i;

	//string save_name = "decimate.txt";
	//ofstream of(save_name);
	//int pixel_id = 0;
	//for (int i = 0; i < input_image.height; i++)
	//{
	//	for (int j = 0; j < input_image.width; j++)
	//	{
	//		int cluster_id = tess->cluster_belong[pixel_id];
	//		of << (cluster_mapping[cluster_id] + 1) << '\t';
	//		pixel_id++;
	//	}
	//	of << '\n';
	//}
	//of.close();

	return true;
}

void Optimizer::apply_contraction(Cluster_Pair *info)
{
	int p1 = info->p1;
	int p2 = info->p2;
	tess->mergeDomain(p1, p2);
	covariance(p1) += covariance(p2);

	update_valid_pairs(info);
	for(uint i=0; i<contractions[p1].size(); i++)
		compute_pair_info(contractions[p1][i]);
}

void Optimizer::update_valid_pairs(Cluster_Pair *info)
{
	//find all the possible neighbors
	PixelIdx p1 = info->p1, p2 = info->p2;
	set<PixelIdx> neighbors;
	int n_neighbors = find_contracted_neighbors(p1, neighbors);
	uint j;

	//update the heap
	for(int i=0; i<contractions[p1].size(); i++)
	{
		Cluster_Pair *e = contractions[p1][i];
		PixelIdx u = (e->p1==p1)?e->p2:e->p1;
		heap.remove(e);
		erase_pair_counterpart(u,e);
		if(u != p2)
			delete e;
	}
	for(int i=0; i<contractions[p2].size(); i++)
	{
		Cluster_Pair *e = contractions[p2][i];
		PixelIdx u = (e->p1==p2)?e->p2:e->p1;
		heap.remove(e);
		erase_pair_counterpart(u,e);
		if(u != p1)
			delete e;
	}

	contractions[p1].clear();
	contractions[p2].clear();
	for (set<PixelIdx>::iterator it = neighbors.begin(); it != neighbors.end(); it++)
	{
		Cluster_Pair * info_to_add =  new Cluster_Pair();
		info_to_add->p1 = p1;
		info_to_add->p2 = *it;
		contractions[p1].push_back(info_to_add);
		contractions[*it].push_back(info_to_add);	
	}
}

int Optimizer::find_contracted_neighbors(PixelIdx v1, set<PixelIdx> & neighbors)
{
	neighbors.clear();

	for (set<PixelIdx>::iterator it = tess->clusters[v1].begin(); it != tess->clusters[v1].end(); it++)
	{
		for (vector<int>::iterator p_it = input_image.pixel_neighbors[*it].begin(); p_it != input_image.pixel_neighbors[*it].end(); p_it++)
		{
			PixelIdx neighbor_point = *p_it;
			if (tess->cluster_belong[neighbor_point] != v1)
				neighbors.insert(tess->cluster_belong[neighbor_point]);
		}
	}	
	return neighbors.size();
}

void Optimizer::erase_pair_counterpart(ClusterIdx u, Cluster_Pair *info)
{
	for (int i =0; i<contractions[u].size(); i++)
	{
		if(contractions[u][i] == info)
			contractions[u].erase(contractions[u].begin()+i);
	}
}

void Optimizer::update_energy_ratio()
{
	vector<double> cluster_energy_location(nClusters, 0);
	vector<double> cluster_energy_color(nClusters, 0);
	for (int i=0; i<nClusters; i++)
	{
		Covariance &Cj=covariance(valid_mapping[i]);
		cluster_energy_location[i] = Cj.energy1();
		cluster_energy_color[i] = Cj.energy2();
	}
	double energy_color = std::accumulate(cluster_energy_color.begin(), cluster_energy_color.end(), 0.0);
	double energy_location = std::accumulate(cluster_energy_location.begin(), cluster_energy_location.end(), 0.0);
	Covariance::ratio = location_ratio * energy_color/ energy_location;
	//cout << "location energy: " << energy_location << " color energy " << energy_color << " ratio " << Covariance::ratio << endl;
}

void Optimizer::init_pixel_swap()
{
	prepare_space();
	collect_cluster_neighbor();
	update_energy_ratio();

	for (int i=0; i< nClusters; i++)
		update_cluster_information(i);
	for (int i=0; i< nClusters; i++)
		collect_cluster_swaps(i);

	print_total_energy();
	//cout << "heap size is " << info_set.size() << endl;
}

void Optimizer::prepare_space()
{
	cluster_energy = vector<double>(nClusters, 0);
}

void Optimizer::collect_cluster_neighbor()
{
	//delete all the neighboring information
	for (int i=0; i< nClusters; i++)
	{
		cluster_neighbor_links[valid_mapping[i]].clear();
		cluster_boundary_pixels[valid_mapping[i]].clear();
	}
	
	for (int i=0; i< nClusters; i++)
	{
		set<int> neighbors;
		for (set<PixelIdx>::iterator it = tess->clusters[valid_mapping[i]].begin(); it != tess->clusters[valid_mapping[i]].end(); it++)
		{
			for (vector<int>::iterator p_it = input_image.pixel_neighbors[*it].begin(); p_it != input_image.pixel_neighbors[*it].end(); p_it++)
			{
				//这里使用的是8邻居，查看pixel_neighbors的生成
				PixelIdx neighbor_pixel = *p_it;
				//if it has a neighbor that not belong to v1, then it's at the boundary
				if (valid_mapping[i] != tess->cluster_belong[neighbor_pixel])
					cluster_boundary_pixels[valid_mapping[i]].insert(*it);
				//add to neighbor if necessary
				if (valid_mapping[i] < tess->cluster_belong[neighbor_pixel])
					neighbors.insert(tess->cluster_belong[neighbor_pixel]);
			}
		}

		for (set<int>::iterator it = neighbors.begin(); it != neighbors.end(); it++)
		{
			cluster_pair info;
			info.p1 = valid_mapping[i];
			info.p2 = *it;
			cluster_neighbor_links[valid_mapping[i]].push_back(info);
			cluster_neighbor_links[*it].push_back(info);
		}
	}
}

void Optimizer::update_cluster_information(ClusterIdx i)
{
	Covariance &Cj=covariance(valid_mapping[i]);
	cluster_energy[i] = Cj.energy();
}

void Optimizer::collect_cluster_swaps(ClusterIdx i)
{
	for (set<PixelIdx>::iterator pixel_it = cluster_boundary_pixels[valid_mapping[i]].begin(); pixel_it != cluster_boundary_pixels[valid_mapping[i]].end(); pixel_it++)
	{
		ClusterIdx cluster_closest = valid_mapping[i];
		double energy_saved_closest = 0;
		Pixel_swap *info = new Pixel_swap();

		//compute which cluster it should belong to
		for (vector<int>::iterator p_it = input_image.pixel_neighbors[*pixel_it].begin(); p_it != input_image.pixel_neighbors[*pixel_it].end(); p_it++)
		{
			PixelIdx neighbor_pixel = *p_it;
			ClusterIdx cluster_neighbor = tess->cluster_belong[neighbor_pixel];
			if (valid_mapping[i] != cluster_neighbor)
			{
				double energy_saved = calc_energy_delta(valid_mapping[i], cluster_neighbor, *pixel_it, info);
				if (energy_saved > energy_saved_closest)
				{
					cluster_closest = cluster_neighbor;
					energy_saved_closest = energy_saved;
				}
			}		
		}

		// insert
		if (energy_saved_closest > 0)
			store_swap(energy_saved_closest, valid_mapping[i], cluster_closest, *pixel_it, info);
		else
			delete info;
	}		
}

double Optimizer::calc_energy_delta(ClusterIdx from, ClusterIdx to, PixelIdx face_id, Pixel_swap *info)
{
	const Covariance &Ci=covariance(from), &Cj=covariance(to), &C_init = init_covariance(face_id);
	Covariance C_from = Ci;
	Covariance C_to = Cj;
	double energy_before = cluster_energy[find_cluster_id(from)] + cluster_energy[find_cluster_id(to)];
	C_from -= C_init;
	C_to += C_init;

	double energy_from = C_from.energy();
	double energy_to = C_to.energy();
	double energy_after = energy_from + energy_to;
	return energy_before - energy_after;
}

void Optimizer::store_swap(float energy_saved, ClusterIdx from, ClusterIdx to, PixelIdx face_id, Pixel_swap *info)
{
	info->from_id = from;
	info->to_id = to;
	info->face_id = face_id;
	info->heap_key(energy_saved);

	info_set.push_back(info);
}

bool Optimizer::swap_pixel_once()
{
	set<ClusterIdx> recollect_domain;//the domains need update	
	apply_swap_set(info_set, recollect_domain);
	collect_cluster_neighbor();
	for (vector<Pixel_swap *>::iterator it = info_set.begin(); it != info_set.end(); it++)
		delete *it;
	info_set.clear();
	for (set<ClusterIdx>::iterator seed_it = recollect_domain.begin(); seed_it != recollect_domain.end(); seed_it++)
	{
		collect_cluster_swaps(find_cluster_id(*seed_it));
	}

	double domain_energy = print_total_energy();
	if (!swap_started)
	{
		swap_started = true;
		energy_previous_iter = domain_energy;
		return true;
	}
	else
	{
		if ((energy_previous_iter - domain_energy)/energy_previous_iter > energy_convergence)
		{
			energy_previous_iter = domain_energy;
			return true;
		}
		else
			return false;
	}
}

void Optimizer::apply_swap_set(vector<Pixel_swap *> info_set, set<ClusterIdx>& recollect_domain)
{
	set<ClusterIdx> update_domain;// the domain sets that need to update information
	update_domain.clear();
	for (vector<Pixel_swap *>::iterator info_it = info_set.begin(); info_it != info_set.end(); info_it++)
	{
		apply_swap(*info_it, update_domain);
	}
	//update information
	for(set<ClusterIdx>::iterator seed_it = update_domain.begin(); seed_it != update_domain.end(); seed_it++)
		update_cluster_information(find_cluster_id(*seed_it));

	//domain's neighbors also need get updated
	recollect_domain.clear();
	for(set<ClusterIdx>::iterator seed_it = update_domain.begin(); seed_it != update_domain.end(); seed_it++)
	{
		for(uint j=0; j<cluster_neighbor_links[*seed_it].size(); j++)
		{
			recollect_domain.insert(cluster_neighbor_links[*seed_it][j].p1);
			recollect_domain.insert(cluster_neighbor_links[*seed_it][j].p2);
		}
	}
}

void Optimizer::apply_swap(Pixel_swap* info, set<ClusterIdx>& update_domain)
{
	if (covariance(info->from_id).area >= 2.0)
	{
		tess->clusters[info->from_id].erase(info->face_id);
		tess->clusters[info->to_id].insert(info->face_id);
		tess->cluster_belong[info->face_id]=info->to_id;

		//update covariance of v1 and v2 
		covariance(info->from_id) -= init_covariance(info->face_id);
		covariance(info->to_id) += init_covariance(info->face_id);

		//collect the domains that need update
		update_domain.insert(info->from_id);
		update_domain.insert(info->to_id);
	}
}

int Optimizer::find_cluster_id(int after_mapping)
{
	int lower_id = 0;
	int higher_id= nClusters-1;

	while (higher_id >= lower_id)
	{
		int imid = (higher_id + lower_id)/2;

		// determine which subarray to search
		if (valid_mapping[imid] < after_mapping)
			lower_id = imid + 1;
		else if (valid_mapping[imid] > after_mapping)
			higher_id = imid - 1;
		else
			return imid;
	}
	return -1;
}

float Optimizer::print_total_energy()
{
	float total_energy = 0;
	for (int i=0; i< nClusters; i++)
		total_energy += cluster_energy[i];
	//cout << "total energy is" << total_energy << endl;

	return total_energy;
}

vector<int> Optimizer::find_neighbor_clusters(int i)
{
	vector<int> neighbors;
	for (int j=0; j<cluster_neighbor_links[valid_mapping[i]].size(); j++)
	{
		Cluster_Pair e = cluster_neighbor_links[valid_mapping[i]][j];
		ClusterIdx u = (e.p1==valid_mapping[i])?e.p2:e.p1;
		int neighbor_domain_id = find_cluster_id(u);
		neighbors.push_back(neighbor_domain_id);
	}
	return neighbors;
}