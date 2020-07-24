This program is the implementation of the paper "Superpixel Generation by Agglomerative Clustering with Quadratic Error Minimization".

Acknowledgement:
The "merging-swapping" optimization code is built on the basis of source code release by Dr. Michael Garland (http://mgarland.org/papers.html)

Dependency:
OpenCV

Configuration:
Set up the corresponding OpenCV include and lib paths for the VS project. It should work on most windows systems.

Command Line Usage: The bin folder provides the compiled .exe file to use our algorithm.
Superpixel_QEM.exe  in_folder  out_folder  #superpixel  location_ratio
in_folder: folder for the set of images
out_folder: folder to store the superpixels and partition results
#superpixel: the desired number of superpixels
loation_ratio: the ratio balance the relative importance of the color homogenity energy and the location regularity energy. 
Typical values ranges from 0.1 to 5 depending on the application. Try 0.5 for example.

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
