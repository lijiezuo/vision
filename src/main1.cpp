#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
/************************************************* 
Function:       extractColor   
Description:    提取指定颜色的区域
Input:              src:     源图像
                color：选定的颜色
Output:             colorRegion: 提取的颜色区域   
Others:      
*************************************************/  
enum COLOR_TYPE{RED, GREEN, BLUE};
void extractColor(Mat src, COLOR_TYPE color, Mat &colorRegion)
{
	if(src.channels()!=3)
            return;
       /*preprocessing*/
	GaussianBlur(src,src,Size(5,5),0,0);
    	/*to hsv color space*/
   	Mat hsv;
	vector<Mat> hsvSplit;
	cvtColor(src, hsv, CV_BGR2HSV );
	split(hsv, hsvSplit);
	equalizeHist(hsvSplit[2],hsvSplit[2]);
	merge(hsvSplit,hsv);

	static int iLowH;			/*threshold of  hue*/
	static int iHighH ;
	static int iLowS;			/*threshold of saturation*/
	static int iHighS;
	static int iLowV;			/*threshold of value*/
	static int iHighV;
	static int bgrThresh1;		/*threshold of r-g g-r b-g ... depending on the color*/
	static int bgrThresh2;		/*threshold of r-b g-r b-g...depending on the color*/

	if(color==RED)
	{		
		iLowH = 0;		
		iHighH = 210;
		iLowS = 0;			
		iHighS = 255;
		iLowV = 0;			
		iHighV = 255;
		bgrThresh1=40;		
		bgrThresh2=30;		
	}
	else if(color == BLUE)
	{	
		iLowH = 49;
		iHighH = 155;
		iLowS = 0;
		iHighS = 255;
		iLowV = 0;
		iHighV = 255;
		bgrThresh1=5;		
		bgrThresh2=5;		
	}
	else if(color == GREEN)
	{
		iLowH = 49;
		iHighH = 155;
		iLowS = 0;
		iHighS = 255;
		iLowV = 0;
		iHighV = 255;
		bgrThresh1=20;		
		bgrThresh2=20;		
	}

	/*Create a gui to control color threshold*/
	 namedWindow("Control", CV_WINDOW_AUTOSIZE); 
	 imshow("Control",src);
	 cvCreateTrackbar("LowH", "Control", &iLowH, 255); /*control trackbar of  hue*/
	 cvCreateTrackbar("HighH", "Control", &iHighH, 255);
	 cvCreateTrackbar("LowS", "Control", &iLowS, 255); /*control trackbar of  saturation*/
	 cvCreateTrackbar("HighS", "Control", &iHighS, 255);
	 cvCreateTrackbar("LowV", "Control", &iLowV, 255); 
	 cvCreateTrackbar("HighV", "Control", &iHighV, 255);
	 cvCreateTrackbar("rg", "Control", &bgrThresh1, 255); 
	 cvCreateTrackbar("rb", "Control", &bgrThresh2, 255);

	 /*extract region with needed hsv*/
	inRange(hsv, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV),hsv); 
	vector<Mat > bgrSplit;
	split(src,bgrSplit);
	Mat bgr1, bgr2,bgr3;
	if(color==RED)
	{
		bgr1=bgrSplit.at(2);
		bgr2=bgrSplit.at(1);
		bgr3=bgrSplit.at(0);		
	}
	else if(color == BLUE)
	{
		bgr1=bgrSplit.at(0);
		bgr2=bgrSplit.at(1);
		bgr3=bgrSplit.at(2);		
	}
	else if(color == GREEN)
	{
		bgr1=bgrSplit.at(1);
		bgr2=bgrSplit.at(0);
		bgr3=bgrSplit.at(2);		
	}
	/*extract region that r>b and |r-b|>threshold */
	Mat large, 	/*r larger than b or g*/ 
	abs, 		/*|r-b or g|>threshold */
	region1,region2;
	cv::compare(bgr1,bgr2,large,CMP_GT);
	cv::absdiff(bgr1,bgr2,abs);
	cv::threshold(abs,abs,bgrThresh1,255,THRESH_BINARY);
	cv::bitwise_and(large,abs,region1);
	/*extract region that r>g and |r-g|>threshold */
	cv::compare(bgr1,bgr3,large,CMP_GT);
	cv::absdiff(bgr1,bgr3,abs);
	cv::threshold(abs,abs,bgrThresh2,255,THRESH_BINARY);
	cv::bitwise_and(large,abs,region2);
	/*all region merge together*/
	cv::bitwise_and(region1,region2,colorRegion);
	cv::bitwise_and(colorRegion,hsv,colorRegion);	
	imshow("Color",colorRegion);
}

/************************************************* 
Function:  		imageToReadDistance
Description:	计算已知尺寸物体离相机的水平距离
Input:       		imageLength: 物体在图像中的长度，单位：像素 
				imageDistance: 物体在图像中距离图像中心的距离，单位：像素
				realLength: 物体实际的物理尺寸，单位：mm
Output:         	realDistance: 物体实际到相机中心的水平距离，单位：mm    
Others:      		只有当已知物体实际尺寸时可用
*************************************************/  
double imageToRealDistance(double imageLength, double imageDistance, double realLength)
{
	double realDistance = (realLength/imageLength)*imageDistance;
	return realDistance;
}

/************************************************* 
Function:  		imageToReadDistance
Description:	计算已知尺寸物体离相机的水平距离
Input:       		imageLength: 物体在图像中的长度，单位：像素 
				realLength: 物体实际的物理尺寸，单位：mm
Output:         	realHeight: 物体到相机平面的实际距离，单位：mm    
Others:      		只有当已知物体实际尺寸时可用
*************************************************/  
double imageToHeight(double imageLength, double realLength)
{
	static double f=933.0; /*camera parameter*/
	double realHeight = (realLength/imageLength)*f;
	return realHeight;
}

int main(int argc, char *argv[])
{
     cv::namedWindow("frame",1);
     //    cv::VideoCapture cap("E:/ATSJTU/robotics/robomasters/vision/video/test.mp4");
     cv::VideoCapture cap(0);
     if(!cap.isOpened())
        return -1;
    // cap.set(CV_CAP_PROP_POS_FRAMES,125);
    Mat frame;
    while(1)
    {
          // if(cap.get(CV_CAP_PROP_POS_FRAMES)>160)
          //           cap.set(CV_CAP_PROP_POS_FRAMES,125);
          cap>>frame;
          cv::resize(frame,frame,Size(640,480));
          imshow("frame",frame);
          Mat redRegion;
          extractColor(frame,BLUE,redRegion);
          cv::waitKey(1);
    }

    return 0;
}