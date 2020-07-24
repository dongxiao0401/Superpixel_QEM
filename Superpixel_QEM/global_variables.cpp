#include "image.h"
#include "optimizer.h"
#include "partition.h"
#include <vector>
#include "processing.h"
#include <string>
using namespace std;

Image input_image;
int nClusters;
Optimizer * optimizer = NULL;

//cluster information
Partition* tess = NULL;
//map
int * valid_mapping = NULL;

PostProcessor *cell = NULL;
string image_file_name;
