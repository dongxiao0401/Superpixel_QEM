#include "superPixel.h"
#include <string>
#include <iostream>
#include<io.h>
#include<time.h>
using namespace std;

#include <windows.h>
#include <tchar.h> 
#include <strsafe.h>

string in_folder;
string out_folder;
int nSuperpixel;
float location_ratio;
string sp_folder;
string par_folder;
int image_index = 0;



void generate_superpixels_rec(string dir, string sub_path)
{
	if(dir.size() > 4  && (dir.substr(dir.size()-4, 4) == ".png" || dir.substr(dir.size()-4, 4) == ".ppm" || dir.substr(dir.size()-4, 4) == ".bmp" || dir.substr(dir.size()-4, 4) == ".jpg") ) //an image
	{
		cout << "image " << image_index << " processing..."<<endl;
		SuperPixel sPixel;
		sPixel.loadImage(dir);
		sPixel.setDecimateNum(nSuperpixel);
		sPixel.init();
		sPixel.decimate();
		sPixel.init_swap();
		sPixel.swap_always();
		sPixel.make_superpixel(sp_folder, par_folder, sub_path.substr(0, sub_path.size()-4));
		cout <<"done!"<<endl;
	}
	else
	{
		HANDLE hFind;
		WIN32_FIND_DATA data;
		string source_files = dir + "\\*.*";
		hFind = FindFirstFile(LPCTSTR(source_files.c_str()), &data);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if(string(data.cFileName) == "." || string(data.cFileName) == "..")
					continue;
				string new_dir= dir + "\\" + string(data.cFileName);
				string new_sub_path = sub_path + "\\" + string(data.cFileName);
				if(new_sub_path.substr(new_sub_path.size()-4, 4) != ".png" && new_sub_path.substr(new_sub_path.size()-4, 4) != ".ppm" 
					&& new_sub_path.substr(new_sub_path.size()-4, 4) != ".bmp" && new_sub_path.substr(new_sub_path.size()-4, 4) != ".jpg")
				{
					string sub_path = sp_folder + new_sub_path;
					string command = "md " + sub_path;
					if (_access(sp_folder.c_str(), 0) == -1)
					{
						system(command.c_str());
					}
					sub_path = par_folder + new_sub_path;
					command = "md " + sub_path;
					if (_access(par_folder.c_str(), 0) == -1)
					{
						system(command.c_str());
					}
				}
				image_index++;
				generate_superpixels_rec(new_dir,  new_sub_path);			
			} while (FindNextFile(hFind, &data));
			FindClose(hFind);
		}
	}	
}

int main(int argc, char *argv[])
{
	//usage : in_folder out_folder num_voxels
	/*in_folder = string(argv[1]);
	out_folder = string(argv[2]);
	nSuperpixel = atoi(argv[3]);
	location_ratio = atof(argv[4]);*/

	in_folder = string(".\\..\\test");
	out_folder = string(".\\..\\test_results");
	//number of superpixels
	nSuperpixel = 300;
	//parameter to balance the relative importance between color energy and position energy
	location_ratio = 0.5;


	sp_folder = out_folder + "\\" + "superpixels";//save superpixel 
	par_folder = out_folder + "\\" + "partition_results";//save txt results

	string command = "md " + sp_folder;
	if (_access(sp_folder.c_str(), 0) == -1)
	{
		system(command.c_str());
	}
	command = "md " + par_folder;
	if (_access(par_folder.c_str(), 0) == -1)
	{
		system(command.c_str());
	}

	
	generate_superpixels_rec(in_folder, "");
	//system("pause");
	return 1;
}
