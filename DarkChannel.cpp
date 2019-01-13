#include "DarkChannel.h"

static void MinFilter(Mat &source, Mat &output, int r)
{
    Mat input;
    source.copyTo(input);

    output.create(source.rows, source.cols, CV_8U);
    for (int i = 0; i <= (input.rows - 1) / r; i++)
    {
        for (int j = 0; j <= (input.cols - 1) / r; j++)
        {
            int w = r;
            int h = r;
            if (i * r + h > input.rows)
            {
                h = input.rows - i * r;
            }
            if (j * r + w > input.cols)
            {
                w = input.cols - j * r;
            }

            Mat ROI = input(Rect(j * r, i * r, w, h));

            double mmin;
            minMaxLoc(ROI, &mmin, 0);

            Mat desROI = output(Rect(j * r, i * r, w, h));
            desROI.setTo(uchar(mmin));
        }
    }
}
void DarkChannel(Mat &source, Mat &output, int r)
{
    Mat input;
    input.create(source.rows, source.cols, CV_8U);

    for (int i = 0; i < source.rows; i++)
    {
        uchar *sourcedata = source.ptr<uchar>(i);
        uchar *indata = input.ptr<uchar>(i);
        for (int j = 0; j < source.cols * source.channels(); j += 3)
        {
            uchar mmin;
            mmin = min(sourcedata[j], sourcedata[j + 1]);
            mmin = min(mmin, sourcedata[j + 2]);

            indata[j / 3] = mmin;
        }
    }

    MinFilter(input, output, r);
}
