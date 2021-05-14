#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<iostream>
#include<math.h>


using namespace std;
using namespace cv;


//Global declarations


Mat imgOriginal,imgThre,imgDil,imgErode,imgWarp,imgCrop;
vector<Point> initialPoints;


//float w = 420, h = 596;


Mat preProcessing(Mat refImage)
{
	Mat imgGray, imgBlur, imgCanny;
	cvtColor(refImage, imgGray, COLOR_BGR2GRAY);  //Change Image Colorspace to B/W
	GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0); //Blurring the image for Canny step
	Canny(imgBlur, imgCanny, 25, 75);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));

	dilate(imgCanny, imgDil, kernel);

	return imgDil;
}

vector<Point> getContours(Mat refImage)
{
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(refImage, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);


	vector<vector<Point>> conPoly(contours.size());

	vector<Rect> boundRect(contours.size());

	vector<Point> biggestPts;

	int maxArea = 1000;

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		float perimeter = arcLength(contours[i], true);
		approxPolyDP(contours[i], conPoly[i], 0.02 * perimeter, true);

		if (area > maxArea && conPoly[i].size()==4)
		{
			maxArea = area;
			
			//drawContours(refImage, conPoly, i, Scalar(255, 0, 255), 7);
			biggestPts = { conPoly[i][0],conPoly[i][1],conPoly[i][2],conPoly[i][3] };  //Append the corner points of the biggest rectangle contour in the image

			//drawContours(refImage, conPoly, i, Scalar(255, 0, 255), 2);
		}
	}

	return biggestPts;

}


void drawPoints(vector<Point> points, Scalar color,  Mat imgRef)
{
	for (int i = 0; i < points.size(); i++)
	{
		circle(imgRef, points[i], 10, color, FILLED);
		putText(imgRef, to_string(i), points[i], FONT_ITALIC, 4, color, 4);
	}
}

vector<Point> reorderPoints(vector<Point> points)
{
	vector<Point> reorderedPoints;

	vector<int> sumPoints, subPoints;

	for (int i = 0; i < points.size(); i++)
	{
		sumPoints.push_back(points[i].x + points[i].y);
		subPoints.push_back(points[i].x - points[i].y);
	}

	reorderedPoints.push_back(points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);
	reorderedPoints.push_back(points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);
	reorderedPoints.push_back(points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);
	reorderedPoints.push_back(points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);


	return reorderedPoints;
}

Mat getWarpedImage(Mat refImg, vector<Point> docPoints, float width, float height)
{
	Mat warpedImage;
	Point2f src[4] = { docPoints[0],docPoints[1],docPoints[2],docPoints[3] };

	
	;


	Point2f dst[4] = { {0.0f,0.0f},{width,0.0f},{0.0f,height},{width,height} };


	Mat matrix = getPerspectiveTransform(src, dst);

	warpPerspective(refImg, warpedImage, matrix, Point(width, height));


	return warpedImage;
}

float DistanceBetweenPoints(Point p1,Point p2)
{	
	float diff_x = p1.x - p2.x;
	float diff_y = p1.y - p2.y;
	float power = pow(diff_x, 2) + pow(diff_y, 2);

	float distance = sqrt(power);

	return distance;
}
void main() {


	string path = "Resources/shapes.png";
	Mat imgOriginal = imread(path);
	resize(imgOriginal, imgOriginal, Size(), 0.5, 0.5);

	//Preprocessing
	imgThre = preProcessing(imgOriginal);
	//Get Contours - Get Biggest contour
	initialPoints = getContours(imgThre);

	vector<Point> docPoints = reorderPoints(initialPoints);
	float w = DistanceBetweenPoints(docPoints[0], docPoints[1]);
	float h = DistanceBetweenPoints(docPoints[0], docPoints[3]);


	cout << h << endl << w;
	//drawPoints(initialPoints, Scalar(0, 0, 255),imgOriginal);

	drawPoints(docPoints, Scalar(0, 255, 0), imgOriginal);
	//Cropping
	Rect roi(10, 10, w - (2 * 10), h - (2 * 10));

	
	
	//Warp Image
	imgWarp = getWarpedImage(imgOriginal,docPoints,w,h);
	imgCrop = imgWarp(roi);
	imshow("Image", imgOriginal);
	imshow("Image Pre Processed", imgThre);

	imshow("Image Warped", imgWarp);
	imshow("Image Cropped", imgCrop);
	waitKey(0);
}
