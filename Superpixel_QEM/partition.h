#ifndef DOMAIN_H
#define DOMAIN_H

#include <set>
#include <ctime>
typedef int PixelID;
using namespace std;
class Partition
{
public:
	float* red;
	float* green;
	float* blue;

	set<PixelID> * clusters;
	int * cluster_belong;
	int size;

public:
	Partition(int x, int y, int dx, int dy)
	{
		int gridw = x/dx + (x%dx? 1 : 0);
		int gridh = y/dy + (y%dy? 1 : 0);
		int nClusters = gridw * gridh;
		int nPixel = x * y;

		size = nClusters;
		red = new float[nClusters];
		green = new float[nClusters];
		blue = new float[nClusters];
		clusters = new set<PixelID>[nClusters];
		cluster_belong = new int[nPixel];

		for (int i=0; i< nClusters; i++)
		{
			red[i] = (rand()%21)/21.0;
			green[i] = (rand()%21)/21.0;
			blue[i] = (rand()%21)/21.0;
		}

		for (int i = 0; i < y; i++)
		{
			for (int j = 0; j < x; j++)
			{
				int cluster_id = i/dy * gridw + j/dx;
				int p_id = i * x + j;
				clusters[cluster_id].insert(p_id);
				cluster_belong[p_id] = cluster_id;
			}
		}
	}

	~Partition()
	{
		delete [] red;
		delete [] green;
		delete [] blue;
		delete [] clusters;
		delete [] cluster_belong;
	}

	void mergeDomain(int v1, int v2)
	{		
		//update containing and belonging information
		for (set<PixelID>::iterator it = clusters[v2].begin(); it !=clusters[v2].end(); it++)
		{
			clusters[v1].insert(*it);
			cluster_belong[*it] = v1;
		}
		clusters[v2].clear();	
	}
};

#endif