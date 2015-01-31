#include "SurfMatch.h"

using namespace std;
using namespace cv;



void featureMatch(cv::Mat img_object, cv::Mat img_scene, std::vector<Point2f> obj, std::vector<Point2f> sce, int minHessian)
{
	//-- Step 1: Detect the keypoints using SURF Detector
	SurfFeatureDetector detector(minHessian);
	std::vector<KeyPoint> keypoints_object, keypoints_scene;
	detector.detect(img_object, keypoints_object);
	detector.detect(img_scene, keypoints_scene);

	//-- Step 2: Calculate descriptors (feature vectors)
	SurfDescriptorExtractor extractor;

	Mat descriptors_object, descriptors_scene;

	extractor.compute(img_object, keypoints_object, descriptors_object);
	extractor.compute(img_scene, keypoints_scene, descriptors_scene);
	
	//-- Step 3: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(descriptors_object, descriptors_scene, matches);

	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_object.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);
	
	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches;

	for (int i = 0; i < descriptors_object.rows; i++)
	{
		if (matches[i].distance < 3 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	Mat img_matches;
	drawMatches(img_object, keypoints_object, img_scene, keypoints_scene,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	imshow("Good Matches & Object detection", img_matches);

	waitKey(0);

	for (int i = 0; i < good_matches.size()-1; i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		sce.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}
}

void surf(cv::Mat img_object, cv::Mat img_scene, cv::Mat dst)
{
	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 2000;

	SurfFeatureDetector detector(minHessian);

	std::vector<KeyPoint> keypoints_object, keypoints_scene;

	detector.detect(img_object, keypoints_object);
	detector.detect(img_scene, keypoints_scene);

	//-- Step 2: Calculate descriptors (feature vectors)
	SurfDescriptorExtractor extractor;

	Mat descriptors_object, descriptors_scene;

	extractor.compute(img_object, keypoints_object, descriptors_object);
	extractor.compute(img_scene, keypoints_scene, descriptors_scene);

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(descriptors_object, descriptors_scene, matches);

	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_object.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);

	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches;

	for (int i = 0; i < descriptors_object.rows; i++)
	{
		if (matches[i].distance < 3 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	Mat img_matches;
	drawMatches(img_object, keypoints_object, img_scene, keypoints_scene,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (int i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}

	Mat H = findHomography(obj, scene, CV_RANSAC);
	double* Hi = (double*)(H.data);
	for (int i = 0; i < 6; i++) cout << Hi[i] << " ";
	cout << endl;
	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(img_object.cols, 0);
	obj_corners[2] = cvPoint(img_object.cols, img_object.rows); obj_corners[3] = cvPoint(0, img_object.rows);
	std::vector<Point2f> scene_corners(4);

	perspectiveTransform(obj_corners, scene_corners, H);
	newBound newBound;
	newBound.top = newBound.top_in = 0;
	newBound.bottom = newBound.bottom_in = img_scene.rows;
	newBound.left = newBound.left_in = 0;
	newBound.right = newBound.right_in = img_scene.cols;

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
		if (newBound.top_in < scene_corners[i].y && scene_corners[i].y < newBound.bottom / 2)
			newBound.top_in = scene_corners[i].y;
		if (newBound.bottom_in > scene_corners[i].y && scene_corners[i].y > newBound.bottom / 2)
			newBound.bottom_in = scene_corners[i].y;
		if (newBound.left_in < scene_corners[i].x && scene_corners[i].x < newBound.right / 2)
			newBound.left_in = scene_corners[i].x;
		if (newBound.right_in > scene_corners[i].x && scene_corners[i].x > newBound.right / 2)
			newBound.right_in = scene_corners[i].x;
		cout << endl << scene_corners[i].x << " " << scene_corners[i].y;
	}

	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	line(img_matches, scene_corners[0] + Point2f(img_object.cols, 0), scene_corners[1] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[1] + Point2f(img_object.cols, 0), scene_corners[2] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[2] + Point2f(img_object.cols, 0), scene_corners[3] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[3] + Point2f(img_object.cols, 0), scene_corners[0] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);

	//-- Show detected matches
	imshow("Good Matches & Object detection", img_matches);

	waitKey(0);
}