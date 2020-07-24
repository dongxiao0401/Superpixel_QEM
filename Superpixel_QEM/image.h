#ifndef _CVIMAGE_H
#define _CVIMAGE_H

#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include <iostream>
#include <vector>
#include "Vector3.h"
#include <string>
using namespace std;
class Image
{
public:
	Image();
	~Image();
	void readImage(string filename_txt);
	void setFileName(string name)	{ filename= name; }
	/*void resizeImage();*/

	int pixelID(int x, int y);
	vector<int> get_pixel_neighbors(int id);
	vector<int> get_pixel_4_neighbors(int id);
public:
	string filename;
	cv::Mat img;
	cv::Mat labimg;
	vector<vector<int>> pixel_neighbors;
	int width;
	int height;
};
#endif