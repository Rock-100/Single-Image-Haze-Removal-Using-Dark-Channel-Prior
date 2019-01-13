#include "GuideFilter.h"

static void makeDepth32f(Mat &source, Mat &output)
{
    if (source.depth() != CV_32F)
        source.convertTo(output, CV_32F);
    else
        output = source;
}

static void mynorm(Mat &source, Mat &output)
{
    for (int i = 0; i < source.rows; i++)
    {
        float *indata = source.ptr<float>(i);
        float *outdata = output.ptr<float>(i);
        for (int j = 0; j < source.cols * source.channels(); j++)
        {
            outdata[j] = indata[j] / 255.0;
        }
    }
}

void GuideFilter(Mat &source, Mat &guided_image, Mat &output, int radius, double epsilon)
{
    CV_Assert(radius >= 2 && epsilon > 0);
    CV_Assert(source.data != NULL && source.channels() == 1);
    CV_Assert(guided_image.channels() == 1);
    CV_Assert(source.rows == guided_image.rows && source.cols == guided_image.cols);

    Mat guided;
    if (guided_image.data == source.data)
    {
        //make a copy
        guided_image.copyTo(guided);
    }
    else
    {
        guided = guided_image;
    }

    //将输入扩展为32位浮点型，以便以后做乘法
    Mat source_32f, guided_32f;
    makeDepth32f(source, source_32f);
    mynorm(source_32f, source_32f);
    makeDepth32f(guided, guided_32f);
    mynorm(guided_32f, guided_32f);

    //计算I*p和I*I
    Mat mat_Ip, mat_I2;
    multiply(guided_32f, source_32f, mat_Ip);
    multiply(guided_32f, guided_32f, mat_I2);

    //计算各种均值
    Mat mean_p, mean_I, mean_Ip, mean_I2;
    Size win_size(2 * radius + 1, 2 * radius + 1);
    boxFilter(source_32f, mean_p, CV_32F, win_size);
    boxFilter(guided_32f, mean_I, CV_32F, win_size);
    boxFilter(mat_Ip, mean_Ip, CV_32F, win_size);
    boxFilter(mat_I2, mean_I2, CV_32F, win_size);

    //计算Ip的协方差和I的方差
    Mat cov_Ip = mean_Ip - mean_I.mul(mean_p);
    Mat var_I = mean_I2 - mean_I.mul(mean_I);
    var_I += epsilon;

    //求a和b
    Mat a, b;
    divide(cov_Ip, var_I, a);
    b = mean_p - a.mul(mean_I);

    //对包含像素i的所有a、b做平均
    Mat mean_a, mean_b;
    boxFilter(a, mean_a, CV_32F, win_size);
    boxFilter(b, mean_b, CV_32F, win_size);

    //计算输出 (depth == CV_32F)
    Mat tempoutput = mean_a.mul(guided_32f) + mean_b;

    output.create(source.rows, source.cols, CV_8U);

    for (int i = 0; i < source.rows; i++)
    {
        float *data = tempoutput.ptr<float>(i);
        uchar *outdata = output.ptr<uchar>(i);
        for (int j = 0; j < source.cols; j++)
        {
            outdata[j] = saturate_cast<uchar>(data[j] * 255);
        }
    }
}
