#pragma once
#include <stdio.h>
#include <iostream>
#include <vector>
#include "Transform.h"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"

using namespace std;
using namespace cv;

void featureMatch(cv::Mat img_object, cv::Mat img_scene, std::vector<Point2f> &obj, std::vector<Point2f> &sce, int minHessian = 500);
