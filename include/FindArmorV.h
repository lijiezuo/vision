/*************************************************
Function:           FindArmorV
Description:        垂直视野找出装甲板（飞机用）
Input:              src：源图像
Output:             armors：装甲板位置
Others:
*************************************************/
#define DRAW 0
#define PI 3.1415926535898
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

#include <iostream>
#include <vector>
using namespace std;

void FindArmorV(Mat src, vector<Point> &armors)
{
    if(src.channels()!=3)
        return;

    //灰度图
    Mat gray;
    cvtColor(src,gray,COLOR_RGB2GRAY);
    //二值化
    Mat bin;
    static int binThresh=200;
    threshold(gray,bin,binThresh,255,THRESH_BINARY);
    //膨胀
//        Mat element=getStructuringElement(MORPH_RECT,Size(2,2));
//        dilate(bin,bin,element,Point(-1,-1),1);
//    imshow("bin",bin);
//    vector< Point > armors;
    vector< vector<Point> > contours;
    vector< RotatedRect> lights;
    findContours(bin,contours,CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);
    if (contours.size()>0)
    {
        for (int i=0; i<contours.size(); i++)
        {
            //面积不能太大或太小
            int area=contourArea(contours.at(i));
            if (area>100||area<20)
                continue;
            RotatedRect rec=minAreaRect(contours.at(i));
            //不能太靠边
            int border=10;
            double recx=rec.center.x;
            double recy=rec.center.y;
            int srcH    = (int) src.size().height;
            int srcW    = (int) src.size().width;
            if (recx<border||recx>srcW-border||recy<border||recy>srcH-border)
                continue;
            //长宽比合适
            Size2f size=rec.size;
            double a=size.height>size.width?size.height:size.width;
            double b=size.height<size.width?size.height:size.width;
            if (a/b>5||a/b<1.2)
            {
                //cout<<a/b<<endl;
                continue;
            }
            //周围明显呈蓝色（或红色）

            Mat roi = src(Rect(recx-7,recy-7,15,15));
            Scalar avg;
            avg=mean(roi);
            if (avg.val[2]-avg.val[0]<10)
            {
                //cvResetIamgeROI(src);
                continue;
            }
            //cvResetIamgeROI(src);
            if (DRAW){
                Point2f vertices[4];
                rec.points(vertices);
                for(int i = 0; i < 4; ++i)
                    line(src,vertices[i],vertices[(i+1)%4],Scalar(0,255,0),2);
            }
            lights.push_back(rec);
        }
    }
    cout<<lights.size()<<endl;
    if (lights.size()>1)
    {

        for (int i=0; i<lights.size()-1; i++)
        {
            for (int j=i+1; j<lights.size(); j++)
            {
                Point2f pi=lights.at(i).center;
                Point2f pj=lights.at(j).center;
                double midx=(pi.x+pj.x)/2;
                double midy=(pi.y+pj.y)/2;
                Size2f sizei=lights.at(i).size;
                Size2f sizej=lights.at(j).size;
                double ai=sizei.height>sizei.width?sizei.height:sizei.width;
//                double b=sizei.height<sizei.width?sizei.height:sizei.width;
                double distance=sqrt((pi.x-pj.x)*(pi.x-pj.x)+(pi.y-pj.y)*(pi.y-pj.y));
                //灯条距离合适
                if (distance<3*ai||distance>4*ai)
                    continue;
                //灯条中点连线与灯条夹角合适
                double angeli=lights.at(i).angle;
                double angelj=lights.at(j).angle;
                if (sizei.width<sizei.height)
                    angeli+=90;
                if (sizej.width<sizej.height)
                    angelj+=90;
                double doti=abs(cos(angeli*PI/180)*(pi.x-pj.x)+sin(angeli*PI/180)*(pi.y-pj.y))/distance;
                double dotj=abs(cos(angelj*PI/180)*(pi.x-pj.x)+sin(angelj*PI/180)*(pi.y-pj.y))/distance;
                if (doti>0.4||dotj>0.4)
                    continue;
//                Mat roi = gray(Rect(midx-50,midy-5,10,10));
//                Mat avg,sd;
//                //normalize(roi,roi);
//                roi-=(mean(roi)[0]-50);
//                meanStdDev(roi,avg,sd);
//                //sds(i)
//                if (sd.at<double>(0,0)<0)
//                    continue;
//                if (avg.at<double>(0,0)<0)
//                    continue;
//                cout<<avg.at<double>(0,0)<<endl;
                armors.push_back(Point((int)midx,(int) midy));
            }
        }
    }

}


