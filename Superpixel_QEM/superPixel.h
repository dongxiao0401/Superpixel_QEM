#ifndef SUPERPIXEL_H
#define SUPERPIXEL_H
#include <string>
using namespace std;

class SuperPixel
{
public:
	SuperPixel();
	~SuperPixel();
	void loadImage(string input_img);
	void init();
	void decimate();
	void init_swap();
	void swap_once();
	void swap_always();
	void make_superpixel(string sp_folder, string par_folder, string file_name);
	void setDecimateNum(int target) { contracted_num = target; };

public:
	int contracted_num;
};

#endif // MESH_PCA_H
