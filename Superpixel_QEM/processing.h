#ifndef REMESH_H
#define REMESH_H

#include "image.h"
#include <iterator>

class PostProcessor
{
public:
	PostProcessor();
	//guarantee each cluster has one connected component, for a cluster has more than one connected component	
	//(1) hold the largest one as a cluster
	//(2) split each connected component as a new cluster, let its neighbor clusters be the merge candidates, choose the least energy pair to merge
	void cleanClusters(); 
	void save_results(string sp_folder, string par_folder, string file_name);

	void save_superpixels(string sp_folder, string file_name);
	void save_partition_img(string par_folder, string file_name);
	void save_partition_txt(string par_folder, string file_name);
};
#endif