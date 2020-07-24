#ifndef PCCVT_INCLUDED // -*- C++ -*-
#define PCCVT_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

#include "covariance.h"
#include "image.h"
#include <set>
#include <string>
#include <vector>
#include "MxStdSlim.h"

typedef int PixelIdx;
typedef int ClusterIdx;
const float energy_convergence = 0.0001;// 0.0000001;
using namespace std;

class Cluster_Pair : public MxHeapable
{
public:
	PixelIdx p1, p2;
};  

class Pixel_swap: public MxHeapable
{
public:
	ClusterIdx from_id, to_id;
	PixelIdx face_id;

	Pixel_swap(){ }
};
typedef Cluster_Pair cluster_pair;

class Optimizer
{
public:
	MxHeap heap;
	int dx;
	int dy;
	int gridw;
	int gridh;
	int nPixel;
	vector<CovDet> __cov;
	vector<CovDet> initial_cov;
	vector<vector<Cluster_Pair*>> contractions; 

	vector<vector<cluster_pair>> cluster_neighbor_links;
	vector<double> cluster_energy;
	vector<set<PixelIdx>> cluster_boundary_pixels;

	vector<Pixel_swap *> info_set;
	bool swap_started;
	double energy_previous_iter;

public:
    Optimizer();
	Optimizer(int dx, int dy);
	~Optimizer();
	void init_cov();
	bool decimate(uint target);

	void init_pixel_swap();
	bool swap_pixel_once();
	float print_total_energy();
	//aux
	int pixelID(int x, int y);
	vector<int> find_neighbor_clusters(int before_mapping);
	int find_cluster_id(int after_mapping);	
public:
	void allocate_space();
	void init_covariance();

	void collect_contraction();
	void create_pair(PixelIdx j, PixelIdx k);
	void compute_pair_info(Cluster_Pair* info);
	void compute_energy(Cluster_Pair *info);
	void finalize_pair_update(Cluster_Pair *info);
	void erase_pair_counterpart(ClusterIdx u, Cluster_Pair *info);

	void apply_contraction(Cluster_Pair *info);
	void update_valid_pairs(Cluster_Pair *info);
	int find_contracted_neighbors(PixelIdx v1, set<PixelIdx> & neighbors);
	void update_energy_ratio();

	void prepare_space();	
	void update_cluster_information(ClusterIdx j);
	void collect_cluster_neighbor();
	void collect_cluster_swaps(ClusterIdx i);
	double calc_energy_delta(ClusterIdx from, ClusterIdx to, PixelIdx face_id, Pixel_swap *info);
	void store_swap(float energy_saved, ClusterIdx from, ClusterIdx to, PixelIdx face_id, Pixel_swap *info);

	void apply_swap_set(vector<Pixel_swap *> info_set, set<ClusterIdx>& recollect_domain);
	void apply_swap(Pixel_swap* info, set<ClusterIdx>& update_domain);
public:
	uint cov_count() const { return __cov.size(); }
	CovDet&       covariance(uint i)       { return __cov[i]; }
	const CovDet& covariance(uint i) const { return __cov[i]; }
	CovDet&       init_covariance(uint i)       { return initial_cov[i]; }
	const CovDet& init_covariance(uint i) const { return initial_cov[i]; }
};

#endif
