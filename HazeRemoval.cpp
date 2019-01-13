#include "HazeRemoval.h"
#include "GuideFilter.h"
#include "DarkChannel.h"

void HazeRemoval(Mat &source, Mat &output, int minr, int maxA, double w, int guider, double guideeps, int L)
{
    Mat input;
    source.copyTo(input);

    //获得暗通道
    Mat dark;
    DarkChannel(input, dark, minr * 2);
    imshow("DarkChannel", dark);

    //计算大气光值
    int hash[256];
    memset(hash, 0, sizeof(hash));
    for (int i = 0; i < dark.rows; i++)
    {
        uchar *data = dark.ptr<uchar>(i);
        for (int j = 0; j < dark.cols; j++)
        {
            hash[data[j]]++;
        }
    }
    int num = dark.rows * dark.cols / 1000.0;
    int count = 0;
    uchar thres;
    for (int i = 0; i < 256; i++)
    {
        count += hash[255 - i];
        if (count >= num)
        {
            thres = 255 - i;
            break;
        }
    }
    num = count;
    double b_max = 0, B;
    double g_max = 0, G;
    double r_max = 0, R;
    for (int i = 0; i < dark.rows; i++)
    {
        uchar *data = dark.ptr<uchar>(i);
        uchar *indata = input.ptr<uchar>(i);
        for (int j = 0; j < dark.cols; j++)
        {
            if (data[j] >= thres)
            {
                B = indata[3 * j];
                G = indata[3 * j + 1];
                R = indata[3 * j + 2];
                b_max += B;
                g_max += G;
                r_max += R;
            }
        }
    }
    b_max /= num;
    g_max /= num;
    r_max /= num;
    uchar MMAX = maxA;
    if (b_max > MMAX) b_max = MMAX;
    if (g_max > MMAX) g_max = MMAX;
    if (r_max > MMAX) r_max = MMAX;

    //计算透射率
    Mat img_t;
    img_t.create(dark.rows, dark.cols, CV_8U);
    Mat temp;
    temp.create(dark.rows, dark.cols, CV_8UC3);
    double b_temp = b_max / 255;
    double g_temp = g_max / 255;
    double r_temp = r_max / 255;
    for (int i = 0; i < dark.rows; i++)
    {
        uchar *data = input.ptr<uchar>(i);
        uchar *tdata = temp.ptr<uchar>(i);
        for (int j = 0; j < dark.cols * 3; j += 3)
        {
            tdata[j] = saturate_cast<uchar>(data[j] / b_temp);
            tdata[j + 1] = saturate_cast<uchar>(data[j] / g_temp);
            tdata[j + 2] = saturate_cast<uchar>(data[j] / r_temp);
        }
    }
    Mat gray;
    cvtColor(temp, gray, CV_BGR2GRAY);
    DarkChannel(temp, temp, minr * 2);
    for (int i = 0; i < dark.rows; i++)
    {
        uchar *darkdata = temp.ptr<uchar>(i);
        uchar *tdata = img_t.ptr<uchar>(i);
        for (int j = 0; j < dark.cols; j++)
        {
            tdata[j] = 255 - w * darkdata[j];
        }
    }

    GuideFilter(img_t, gray, img_t, guider, guideeps);
    imshow("Transmissivity", img_t);

    //还原图像
    output.create(input.rows, input.cols, CV_8UC3);
    for (int i = 0; i < input.rows; i++)
    {
        uchar *tdata = img_t.ptr<uchar>(i);
        uchar *indata = input.ptr<uchar>(i);
        uchar *outdata = output.ptr<uchar>(i);
        for (int j = 0; j < input.cols; j++)
        {
            uchar b = indata[3 * j];
            uchar g = indata[3 * j + 1];
            uchar r = indata[3 * j + 2];
            double t = tdata[j];
            t /= 255;
            if (t < 0.1) t = 0.1;

            outdata[3 * j] = saturate_cast<uchar>((b - b_max) / t + b_max + L);
            outdata[3 * j + 1] = saturate_cast<uchar>((g - g_max) / t + g_max + L);
            outdata[3 * j + 2] = saturate_cast<uchar>((r - r_max) / t + r_max + L);
        }
    }
}
