#pragma once
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"

using namespace cv;
using namespace std;

struct newBound
{
	double top;
	double bottom;
	double left;
	double right;
	Point topP;
	Point bottomP;
	Point leftP;
	Point rightP;
	double top_in;
	double bottom_in;
	double left_in;
	double right_in;
	double persX;
	double persY;
};
Mat cylinderTrans(Mat img, double focusSet = -1);
Mat getHomographyByOpenCV(vector<Point2f> obj, vector<Point2f> sce);
newBound getNewBound(cv::Mat H, int obj_col, int obj_row, int sce_col, int sce_row);