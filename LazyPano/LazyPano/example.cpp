#include "example.h"
#include "opencv2/stitching/stitcher.hpp"
#include <time.h>

void mySurfPano(vector<Mat> imgs, Mat &pano, double fov = -1);

void example_mySurf()
{	
	Mat img_object2 = imread("D:\\2015VScode\\CV_HW2\\CV_HW2\\TestData\\camp\\Img00.JPG");
	Mat img_scene2 = imread("D:\\2015VScode\\CV_HW2\\CV_HW2\\TestData\\camp\\Img01.JPG");//, CV_LOAD_IMAGE_GRAYSCALE );
	vector<Mat> imgs;
	string result_name = "result.jpg";
	Mat img = imread("D:\\2015VScode\\CV_HW2\\CV_HW2\\TestData\\camp\\Img00.JPG");
	imgs.push_back(img);
	img = imread("D:\\2015VScode\\CV_HW2\\CV_HW2\\TestData\\camp\\Img01.JPG");
	imgs.push_back(img);
	img = imread("D:\\2015VScode\\CV_HW2\\CV_HW2\\TestData\\camp\\Img02.JPG");
	imgs.push_back(img);
	img = imread("D:\\2015VScode\\CV_HW2\\CV_HW2\\TestData\\camp\\Img03.JPG");
	imgs.push_back(img);
	img = imread("D:\\2015VScode\\CV_HW2\\CV_HW2\\TestData\\camp\\Img04.JPG");
	imgs.push_back(img);
	img = imread("D:\\2015VScode\\CV_HW2\\CV_HW2\\TestData\\camp\\Img05.JPG");
	imgs.push_back(img);
	img = imread("D:\\2015VScode\\CV_HW2\\CV_HW2\\TestData\\camp\\Img06.JPG");
	imgs.push_back(img);
	clock_t start_time = clock();
	Mat pano;

	mySurfPano(imgs, pano, 40);
	imwrite("x.jpg", pano);
	imshow("v", pano);
	waitKey(-1);
/*	clock_t start_time = clock();

	Mat img_object = cylinderTrans(img_scene2, 46);
	Mat img_scene = cylinderTrans(img_object2, 46);

	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	featureMatch(img_object, img_scene, obj, scene, 1000);

	Mat H = getHomographyByOpenCV(obj, scene);
	newBound bounds;
	bounds = getNewBound(H, img_object.cols, img_object.rows, img_scene.cols, img_scene.rows);

	MyBlending blender(img_object, img_scene, bounds, H);

	clock_t end_time = clock();
	cout << "Running time is: " << static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000 << "ms" << endl;//输出运行时间
	imshow("v", blender.pano);
	waitKey(-1);*/
}

void example_cvStitcher()
{
	bool try_use_gpu = false;
	vector<Mat> imgs;
	string result_name = "result.jpg";
	Mat img = imread("D:\\2015VScode\\CV_HW2\\CV_HW2\\TestData\\camp\\Img00.JPG");
	imgs.push_back(img);
	img = imread("D:\\2015VScode\\CV_HW2\\CV_HW2\\TestData\\camp\\Img01.JPG");
	imgs.push_back(img);
	clock_t start_time = clock();
	Mat pano;
	Stitcher stitcher = Stitcher::createDefault(try_use_gpu);
	Stitcher::Status status = stitcher.stitch(imgs, pano);

	if (status != Stitcher::OK)
	{
		cout << "Can't stitch images, error code = " << int(status) << endl;
		exit(-1);
	}
	clock_t end_time = clock();
	cout << "Running time is: " << static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000 << "ms" << endl;//输出运行时间
	imshow("w", pano);
	waitKey(-1);
}

void mySurfPano(vector<Mat> imgs, Mat &pano, double fov)
{
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;
	int size = imgs.size();
	
	//Mat img_scene = imgs[0];
	Mat img_scene; 
	Mat img_object = cylinderTrans(imgs[0], fov);
	for (int i = 1; i < size; i++)
	{
		obj.clear();
		scene.clear();
		img_scene = cylinderTrans(imgs[i], fov);
		/*imshow("1", img_scene);
		waitKey(0);
		imshow("2", img_object);
		waitKey(0);
		*/
		featureMatch(img_object, img_scene, obj, scene, 500);
		cout << i <<" "<<obj.size()<<	 endl;
		Mat H = getHomographyByOpenCV(obj, scene);
		double* Hi = (double*)(H.data);
		for (int i = 0; i < 6; i++) cout << Hi[i] << " ";
		cout << endl;
		getchar();
		newBound bounds;
		bounds = getNewBound(H, img_object.cols, img_object.rows, img_scene.cols, img_scene.rows);

		MyBlending blender(img_object, img_scene, bounds, H);
		blender.pano.convertTo(img_object, CV_8UC3, 255.0);
		
		imshow("v", img_object);
		waitKey(0);
	}	
	pano = img_object;
	
}


