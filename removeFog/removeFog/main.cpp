//
//  main.cpp
//  removeFog
//
//  Created by yanfa on 2019/8/20.
//  Copyright © 2019 marunxin. All rights reserved.
//

#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/core.hpp"
using namespace cv;

Mat removeFog(Mat img)
{
    float w0=0.6;
    float t0=0.1;
    
    Mat gray;
    cvtColor(img, img, COLOR_RGBA2RGB);
    cvtColor(img, gray, COLOR_RGB2GRAY);
    
    MatND dstHist;
    int dims = 1;
    float hranges[2] = {0, 255};
    const float *ranges[1] = {hranges};
    int size = 256;
    int channels = 0;
    calcHist(&gray, 1, &channels, Mat(), dstHist, dims, &size, ranges);
    
    float *h = (float*)dstHist.data;
    
    int under_50 = 0;
    for(int i=0;i<50;++i)
        under_50+=h[i];
    
    int total = img.cols*img.rows*3;
    float percent = (float)under_50/(float)total;
    
    Mat res = img;
    if(percent>0.02)
        return res;
    else if(percent<0.001)
        w0=0.6;
    else if(percent>0.01)
        w0=0.3;
    else
        w0=0.45;
    
    Mat darkImg = Mat::zeros(gray.size(), CV_8UC1);
    double maxDarkChannel = 0;
    for(int i=0;i<img.rows;++i)
    {
        for(int j=0;j<img.cols;++j)
        {
            darkImg.at<uchar>(i,j) = img.at<Vec3b>(i,j)[0];
            darkImg.at<uchar>(i,j) = min(darkImg.at<uchar>(i,j), img.at<Vec3b>(i,j)[1]);
            darkImg.at<uchar>(i,j) = min(darkImg.at<uchar>(i,j), img.at<Vec3b>(i,j)[2]);
            maxDarkChannel = max(maxDarkChannel, (double)darkImg.at<uchar>(i,j));
        }
    }
    
    Mat tImg = Mat::zeros(gray.size(), CV_32F);
    
    
    for(int i=0;i<img.rows;++i)
    {
        for(int j=0;j<img.cols;++j)
        {
            tImg.at<float>(i,j) = 1.0 - w0/maxDarkChannel*(float)darkImg.at<uchar>(i,j);
            tImg.at<float>(i,j) = max(t0,tImg.at<float>(i,j));
            
            res.at<Vec3b>(i,j)[0] = (uchar)MIN(255,(((float)img.at<Vec3b>(i,j)[0] - maxDarkChannel * (1-tImg.at<float>(i,j))) / tImg.at<float>(i,j)));
            res.at<Vec3b>(i,j)[1] = (uchar)MIN(255,(((float)img.at<Vec3b>(i,j)[1] - maxDarkChannel * (1-tImg.at<float>(i,j))) / tImg.at<float>(i,j)));
            res.at<Vec3b>(i,j)[2] = (uchar)MIN(255,(((float)img.at<Vec3b>(i,j)[2] - maxDarkChannel * (1-tImg.at<float>(i,j))) / tImg.at<float>(i,j)));
        }
    }
    
    return res;
}

int main(int argc, const char * argv[]) {
    
    Mat res = imread("/Users/yanfa/Desktop/res.png");
    Mat dst = removeFog(res);
    imwrite("/Users/yanfa/Desktop/dst.jpg", dst);
    std::cout << "removeFog finished!\n";
    
    return 0;
}
