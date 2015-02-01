#include "MyBlending.h"

double colorDelta(Mat img1, Mat img2, int begin, int end, int length, int width, int i_c, int j_c);

void MyBlending::Blend(cv::Mat &obj, cv::Mat &sce, cv::Mat H)
{
	double shift[9] = { 1, 0, -mybound.left, 0, 1, -mybound.top, 0, 0, 1 };
	Mat shiftMat(3, 3, H.type(), shift);
	Mat homoShift = shiftMat * H;

	Mat left(mybound.bottom - mybound.top, mybound.right - mybound.left, CV_8UC3);
	Mat right(mybound.bottom - mybound.top, mybound.right - mybound.left, CV_8UC3);

	IplImage objBack = obj;
	IplImage sceBack = sce;

	double* Hi = (double*)(H.data);

	IplImage leftsrc = left, rightsrc = right;
	if (Hi[2] < 0)
	{
		cvWarpPerspective(&objBack, &leftsrc, &(CvMat)homoShift);
		cvWarpPerspective(&sceBack, &rightsrc, &(CvMat)shiftMat);
	}
	else
	{
		cvWarpPerspective(&sceBack, &leftsrc, &(CvMat)shiftMat);
		cvWarpPerspective(&objBack, &rightsrc, &(CvMat)homoShift);
	}

	optionalSeam(left, right, mybound.left_in - mybound.left, mybound.right_in - mybound.left, 0);
	Mat result;
	
	if (Hi[2] < 0)
	{
		right.copyTo(result);
	}
	else
	{
		left.copyTo(result);
	}
	IplImage resultBack = result;
	//cout <<"result"<< result.cols << result.rows << endl;
	cvWarpPerspective(&objBack, &resultBack, &(CvMat)homoShift, CV_INTER_LINEAR);
	//imshow("3", result);
	//waitKey(0);
	for (int i = 0; i < result.rows; i++)
	{
		uchar* data = result.ptr<uchar>(i);
		data[3 * seam[i]] = 0;
		data[3 * seam[i] + 1] = 0;
		data[3 * seam[i] + 2] = 255;
	}
	//imshow("3", result);
	//waitKey(0);
	pano = multiBlender(left, right);
}

void MyBlending::optionalSeam(cv::Mat &left, cv::Mat &right, int begin, int end, int type)
{
	//cout << mybound.left_in - mybound.left << " " << mybound.right_in - mybound.left << endl;
	if (type == 0); //0 means images are side by side   1 means images are up and down
	int length = left.rows;
	int width = left.cols;
	seam = new int[length];
	double **colorDisSum;
	double **recorder;
	colorDisSum = new double *[length];
	recorder = new double *[length];
	for (int i = 0; i < length; i++)
	{
		colorDisSum[i] = new double[width];
		recorder[i] = new double[width];
	}
	for (int i = 0; i < length; i++)
		for (int j = 0; j <width; j++)
			colorDisSum[i][j] = -1;
	for (int i = 0; i < length; i++)
	{
		for (int j = begin; j <= end; j++)
		{
			if (i == 0)
			{
				colorDisSum[i][j] = colorDelta(left, right, begin, end, length, width, i, j);
				recorder[i][j] = j;
			}
			else
			{
				int minID, minOfThree;
				minOfThree = colorDisSum[i - 1][j];
				minID = j;
				if (colorDisSum[i - 1][j - 1] < minOfThree && colorDisSum[i - 1][j - 1] >= 0)
				{
					minOfThree = colorDisSum[i - 1][j - 1];
					minID = j - 1;
				}
				if (colorDisSum[i - 1][j + 1] < minOfThree && colorDisSum[i - 1][j + 1] >= 0)
				{
					minOfThree = colorDisSum[i - 1][j + 1];
					minID = j + 1;
				}
				colorDisSum[i][j] = colorDelta(left, right, begin, end, length, width, i, j)
					+ minOfThree;
				recorder[i][j] = minID;
			}
		}
	}
	int minID, minSeam;
	minSeam = colorDisSum[length - 1][begin];
	minID = begin;
	//cout << "colorDelta" << endl << colorDisSum[length - 1][begin] << endl;
	for (int j = begin; j <= end; j++)
	{
		if (colorDisSum[length - 1][j] < minSeam)
		{
			minSeam = colorDisSum[length - 1][j];
			minID = j;
		//	cout << colorDisSum[length - 1][j] << "\t" << j << endl;
		}

	}
	seam[length - 1] = minID;
	for (int i = length - 1; i >0; i--)
	{
		seam[i - 1] = recorder[i][seam[i]];
	}
}

double colorDelta(Mat img1, Mat img2, int begin, int end, int length, int width, int i_c, int j_c)
{
	int colorDis = 0;
	int x_neibor[2] = { -1, 1 };
	int y_neibor[2] = { -1, 1 };
	int neiborSize = 0;
	if (i_c == 0) x_neibor[0] = 0;
	if (i_c == length - 1)  x_neibor[1] = 0;
	if (j_c == begin) y_neibor[0] = 0;
	if (j_c == end)  y_neibor[1] = 0;
	for (int i = x_neibor[0]; i <= x_neibor[1]; i++)
	{
		for (int j = y_neibor[0]; j <= y_neibor[1]; j++)
		{
			uchar* data1 = img1.ptr<uchar>(0);
			uchar* data2 = img2.ptr<uchar>(0);
			neiborSize++;
			uchar *curPixel1 = data1 + 3 * ((i_c + i) * width + j_c + j);
			uchar *curPixel2 = data2 + 3 * ((i_c + i) * width + j_c + j);
			colorDis += pow((float)curPixel1[0] - curPixel2[0], 2)
				+ pow((float)curPixel1[1] - curPixel2[1], 2)
				+ pow((float)curPixel1[2] - curPixel2[2], 2);
		}
	}
	colorDis /= neiborSize;
	return colorDis;
}

Mat_<Vec3f> MyBlending::multiBlender(cv::Mat &left, cv::Mat &right)
{
	Mat_<Vec3f> l; left.convertTo(l, CV_32F, 1.0 / 255.0);//Vec3f表示有三个通道，即 l[row][column][depth]
	Mat_<Vec3f> r; right.convertTo(r, CV_32F, 1.0 / 255.0);

	Mat_<float> m(l.rows, l.cols, 0.0);					//将m全部赋值为0
	//m(Range::all(), Range(0, m.cols / 2)) = 1.0;
	for (int i = 0; i < left.rows-1; i++)
	{
		m(Range::Range(i, i+1), Range(0, seam[i])) = 1.0;	//取m全部行&[0,m.cols/2]列，赋值为1.0
	}

	Mat_<Vec3f> blend = LaplacianBlend(l, r, m);
	return blend;
}