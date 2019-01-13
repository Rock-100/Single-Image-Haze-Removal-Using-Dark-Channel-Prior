#ifndef __GUIDEFILTER__
#define __GUIDEFILTER__

#include "Common.h"

extern void GuideFilter(Mat &source, Mat &guided_image, Mat &output, int radius, double epsilon);


#endif

