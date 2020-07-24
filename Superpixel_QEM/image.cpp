#include"image.h"
#include <fstream>
#include <numeric>
using namespace std;
using namespace cv;
Image::Image()
{
	filename = "";
	img = NULL;
	labimg = NULL;
	width = 0;
	height = 0;
}

Image::~Image()
{
	/*cvReleaseImage(&img);
	cvReleaseImage(&labimg);*/
}

//void Image::resizeImage()
//{
//	cv::Size size = img->width > img->height? cv::Size(2405, 1605) : cv::Size(1605, 2405);
//	IplImage* rescale_img = cvCreateImage( size, img->depth, 3 );
//	cvResize(img, rescale_img);
//	cvReleaseImage(&img);
//	img = rescale_img;
//}

void Image::readImage(string filename_txt)
{
	filename = filename_txt;
	img  = imread(filename_txt);
	//resizeImage();
	labimg= img.clone();
	cvtColor(img, labimg, COLOR_BGR2Lab);
	width = img.cols;
	height = img.rows;
	cout << "width:" << width << "," << "height:" << height << endl;

	//init neighbors
	int nPixel = width * height;
	pixel_neighbors = vector<vector<int>>(nPixel);
	for (int i = 0; i < nPixel; i++)
		pixel_neighbors[i] = get_pixel_neighbors(i);
}

int Image::pixelID(int x, int y)
{
	return y * width + x;
}

vector<int> Image::get_pixel_neighbors(int id)
{
	int y = id/width;
	int x = id%width;
	vector<int> res;
	for(int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <=1; j++)
		{
			if(i == 0 && j == 0)
				continue;
			int x_new = x + i;
			int y_new = y + j;
			if(x_new >= 0 && x_new < width && y_new >= 0 && y_new < height)
				res.push_back(pixelID(x_new, y_new));
		}
	}
	return res;
}

vector<int> Image::get_pixel_4_neighbors(int id)
{
	int y = id / width;
	int x = id % width;
	vector<int> res;
	if (x > 0)
		res.push_back(id - 1);
	if (x + 1 < width)
		res.push_back(id + 1);
	if (y > 0)
		res.push_back(id - width);
	if (y + 1 < height)
		res.push_back(id + width);
	return res;
}