#include "image.h"
#include "superPixel.h"
#include "partition.h"
#include "optimizer.h"
#include "processing.h"
#define MAX_ITER 20

extern Image input_image;
extern Partition* tess;
extern Optimizer* optimizer;
extern PostProcessor *cell;

SuperPixel::SuperPixel()
{
	contracted_num = 100;
}

SuperPixel::~SuperPixel()
{
	delete cell;
}

void SuperPixel::loadImage(string input_img)
{
	input_image.readImage(input_img); 
	cout << "reading  "  << input_img.c_str() << "..." << endl;
}

void SuperPixel::init()
{
	if (optimizer != NULL)
		delete optimizer;

	int nPixel = input_image.height * input_image.width;
	/*
	In paper, we treat each pixel as an initial superpixel and do the "merging" procedure. To speed up the processing, we initialize
	the superpixel with a block of pixels in advance.
	*/
	int target_blocks = contracted_num * 25;
	float d_theory = sqrt((float)nPixel/target_blocks);
	int dx = d_theory > 1 ? d_theory : 1;
	int dy = d_theory > 1 ? d_theory : 1;

	optimizer = new Optimizer(dx, dy);
	double qslim_t = get_cpu_time();
	optimizer->init_cov();
	qslim_t = get_cpu_time() - qslim_t;
	//cerr << "init  use time "<< qslim_t << " sec" << endl;
}

void SuperPixel::decimate()
{
	if (optimizer != NULL)
	{
		//double decimate_t = get_cpu_time();
		optimizer->decimate(contracted_num);
		//decimate_t = get_cpu_time() - decimate_t;
		//cerr << "decimate  use time "<< decimate_t << " sec" << endl;
	}	
}

void SuperPixel::init_swap()
{
	if (optimizer != NULL)
	{
		optimizer->init_pixel_swap();
	}
}

void SuperPixel::swap_once()
{
	if (optimizer != NULL)
	{
		double qswap_t = get_cpu_time();
		optimizer->swap_pixel_once();
		qswap_t = get_cpu_time() - qswap_t;
		cerr << "  use time "<< qswap_t << " sec" << endl;
	}
}

void SuperPixel::swap_always()
{
	if (optimizer != NULL)
	{
		//double qswap_t;
		//double qswap_total_t = 0;
		//qswap_t = get_cpu_time();
		int iter = 0;
		while(optimizer->swap_pixel_once() && iter <= MAX_ITER)
		{
			//qswap_t = get_cpu_time() - qswap_t;
			iter++;
			//qswap_total_t += qswap_t;
			//cerr << "    iteration  : " << iter << "	total time use: "<< qswap_total_t << " sec " << "	# swapping pixel:" << optimizer->info_set.size() << endl;
			//qswap_t = get_cpu_time();
		}	
	}	
}

void SuperPixel::make_superpixel(string sp_folder, string par_folder, string file_name)
{
	if (optimizer != NULL)
	{
		cell = new PostProcessor();
		cell->cleanClusters();
		cell->save_results(sp_folder, par_folder, file_name);
	}
}
