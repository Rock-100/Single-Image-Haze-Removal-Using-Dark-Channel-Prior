#ifndef __HAZEREMOVAL__
#define __HAZEREMOVAL__

#include "Common.h"

extern void HazeRemoval(Mat &source, Mat &output, int minr = 5, int maxA = 230, double w = 0.98, int guider = 30, double guideeps = 0.001, int L = 0);

#endif
