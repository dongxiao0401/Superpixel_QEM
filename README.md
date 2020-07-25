# Superpixel
This program is the implementation of the paper [Superpixel Generation by Agglomerative Clustering with Quadratic Error Minimization](https://onlinelibrary.wiley.com/doi/epdf/10.1111/cgf.13538).

Acknowledgement:
The "merging-swapping" optimization code is built on the basis of source code release by Dr. Michael Garland (http://mgarland.org/papers.html)

## Configuration:
Set up the corresponding OpenCV include and lib paths for the VS project. 

Parameters:
* in_folder: the folder that contains input images;
* out_folder: the folder that stores the superpixel results, including segmentation images and txt files;
* nSuperpixel: the number of superpixels;
* location_ratio: the parameter that balances the relative importance between color homogeneity and spatial compactness.

Cite:
If you find our code or paper useful, please consider citing

	@inproceedings{dong2019superpixel,
	  title={Superpixel generation by agglomerative clustering with quadratic error minimization},
	  author={Dong, Xiao and Chen, Zhonggui and Yao, Junfeng and Guo, Xiaohu},
	  booktitle={Computer Graphics Forum},
	  volume={38},
	  number={1},
	  pages={405--416},
	  year={2019},
	  organization={Wiley Online Library}
	}
