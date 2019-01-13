#include "HazeRemoval.h"
#include "GuideFilter.h"
#include "DarkChannel.h"

int main()
{
    for (int i = 1; i <=8; i++)
    {
        Mat src = imread("imgs/" + to_string(i) + ".bmp");
        Mat res;

        HazeRemoval(src, res);

        imshow("Input", src);
        imshow("Dehaze", res);

        waitKey(0);
    }
    return 0;
}
