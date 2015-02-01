#include "Transform.h"
using namespace cv;

Mat cylinderTrans(Mat img, double focusSet)
{
	int width = img.cols;
	int height = img.rows;
	double focus;
	double fov;
	if (focusSet > 0)
	{
		fov = focusSet / 360 * 3.1415926;
		focus = width / 2 / tan(fov);
	}
	else
	{
		focus = sqrtf(img.rows * img.rows + img.cols * img.cols);
		fov = atanf(width / 2 / focus);
	}
//	printf("fov=%f, newwidth=%f, focus=%f\n", tan(fov), focus * sin(fov) * 2 + 1, focus);
	Mat cylImg(img.rows, int(focus * sin(fov) * 2 + 1), CV_8UC3, Scalar(0, 0, 0));

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			int x = focus * sin(atan((i - width / 2) / focus)) + focus * sin(fov);
			int y = focus * (j - height / 2) / sqrtf(powf(i - width / 2, 2) + focus * focus) + height / 2;
			uchar* data = img.ptr<uchar>(j);
			uchar* data_out = cylImg.ptr<uchar>(y);
			data_out[3 * x] = data[3 * i];
			data_out[3 * x + 1] = data[3 * i + 1];
			data_out[3 * x + 2] = data[3 * i + 2];
		}
	return cylImg;
}

Mat getHomographyByOpenCV(std::vector<Point2f> obj, std::vector<Point2f> sce)
{	
	Mat H = findHomography(obj, sce, CV_LMEDS);
	return H;
}

Mat getHomographyBySelf(vector<Point2f> obj, vector<Point2f> sce)
{
	//cvSolve
	Mat H = findHomography(obj, sce, CV_LMEDS);
	return H;
}

newBound getNewBound(cv::Mat H, int obj_col, int obj_row, int sce_col, int sce_row)
{
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(obj_col, 0);
	obj_corners[2] = cvPoint(obj_col, obj_row); obj_corners[3] = cvPoint(0, obj_row);
	std::vector<Point2f> scene_corners(4);

	perspectiveTransform(obj_corners, scene_corners, H);

	newBound newBound;
	newBound.top = newBound.top_in = 0;
	newBound.bottom = newBound.bottom_in = sce_row;
	newBound.left = newBound.left_in = 0;
	newBound.right = newBound.right_in = sce_col;

	for (int i = 0; i<4; i++)
	{
		if (newBound.top > scene_corners[i].y)
		{
			newBound.top = scene_corners[i].y;
			newBound.topP = scene_corners[i];
		}
		if (newBound.bottom < scene_corners[i].y)
		{
			newBound.bottom = scene_corners[i].y;
			newBound.bottomP = scene_corners[i];
		}
		if (newBound.left > scene_corners[i].x)
		{
			newBound.left = scene_corners[i].x;
			newBound.leftP = scene_corners[i];
		}
		if (newBound.right < scene_corners[i].x)
		{
			newBound.right = scene_corners[i].x;
			newBound.rightP = scene_corners[i];
		}
		if (newBound.top_in < scene_corners[i].y && (i == 0 || i == 1))
			newBound.top_in = scene_corners[i].y;
		if (newBound.bottom_in > scene_corners[i].y && (i == 2 || i == 3))
			newBound.bottom_in = scene_corners[i].y;
		if (newBound.left_in < scene_corners[i].x && (i == 0 || i == 3))
			newBound.left_in = scene_corners[i].x;
		if (newBound.right_in > scene_corners[i].x && (i == 1 || i == 2))
			newBound.right_in = scene_corners[i].x;
	}
	return newBound;
}

void matrixTo8(double num1[4][2], double num2[4][2], double A[9][9], double B[9])
{
	int i, j, n = 4;
	for (i = 0; i<9; i++)
		for (j = 0; j<9; j++) A[i][j] = 0;
	for (i = 1; i <= n; i++)
	{
		A[i][1] = num1[i - 1][0];
		A[i + n][4] = num1[i - 1][0];
		A[i][2] = num1[i - 1][1];
		A[i + n][5] = num1[i - 1][1];
		A[i][3] = 1.0;
		A[i + n][6] = 1.0;
		A[i][7] = -num1[i - 1][0] * num2[i - 1][0];
		A[i][8] = -num1[i - 1][1] * num2[i - 1][0];
		A[i + n][7] = -num1[i - 1][0] * num2[i - 1][1];
		A[i + n][8] = -num1[i - 1][1] * num2[i - 1][1];
		B[i] = num2[i - 1][0];
		B[i + n] = num2[i - 1][1];
	}
}
