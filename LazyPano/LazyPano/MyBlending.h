#pragma once
#include <iostream>
#include <stdio.h>
#include "Transform.h"
#include "LaplacianBlending.h"
#include "opencv2/imgproc/imgproc_c.h"

using namespace std;

class MyBlending
{		
public:
	newBound mybound;
	int *seam;
	Mat pano;
	MyBlending(cv::Mat &obj, cv::Mat &sce, newBound &bound, cv::Mat H){
		mybound = bound;
		Blend(obj, sce, H);
	}
	void Blend(cv::Mat &obj, cv::Mat &sce, cv::Mat H);
	void optionalSeam(cv::Mat &left, cv::Mat &right, int begin, int end, int type);
	Mat_<Vec3f> multiBlender(cv::Mat &left, cv::Mat &right);
};