#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<iostream>

#define minArea 1000
////////////////////////Shapes and contours ////////////////////
using namespace std;
using namespace cv;


Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;


void getContours(Mat ImageDilation,Mat ImageDraw) {

	
	vector<vector<Point>> contours;
	
	vector<Vec4i> hierarchy;

	findContours(ImageDilation, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(ImageDraw, contours, -1, Scalar(255, 0, 255), 10);
	vector<vector<Point>> conPoly(contours.size());
	//vector<Rect> boundRect
	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		cout << area << endl;

		if (area > minArea)
		{
			float perimeter = arcLength(contours[i], true);

			approxPolyDP(contours[i], conPoly[i], 0.02 * perimeter,true);
			drawContours(ImageDraw, conPoly, i, Scalar(255, 0, 255), 10);
			cout << conPoly[i] << endl;
			boundingRect()
		}
	}
}

void main() {


	string path = "Resources/shapes.png";
	Mat img = imread(path);


	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
	Canny(imgBlur, imgCanny, 25, 75);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgCanny, imgDil, kernel);

	getContours(imgDil,img);

	imshow("Image", img);
	/*imshow("Image Gray", imgGray);
	imshow("Image Blur", imgBlur);
	imshow("Image Canny", imgCanny);
	imshow("Image Dil", imgDil);*/
	waitKey(0);
}
