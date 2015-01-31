#include <iostream>
#include "SurfMatch.h"
#include "Transform.h"
using namespace std;
using namespace cv;

int main()
{	
	cout << "HelloWorld" << endl;
	Mat img_object2 = imread("D:\\2015VScode\\CV_HW2\\CV_HW2\\TestData\\grass\\Img00.JPG");
	Mat img_scene2 = imread("D:\\2015VScode\\CV_HW2\\CV_HW2\\TestData\\grass\\Img01.JPG");//, CV_LOAD_IMAGE_GRAYSCALE );

	Mat img_object = cylinderTrans(img_scene2, 47);
	Mat img_scene = cylinderTrans(img_object2, 47);

	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	featureMatch(img_object2, img_scene2, obj, scene, 1000);
	cout << "HelloWorld" << endl;
	getchar();
	return 0;
}