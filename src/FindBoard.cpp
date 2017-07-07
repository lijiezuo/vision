#include "FindArmorH.h"
#include "FindArmorV.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

#include <iostream>
using namespace std;

#define V
#ifdef V
int main()
{
    VideoCapture capture("V2.7m.avi");
    if(!capture.isOpened())
        return 1;
    double rate=capture.get(CV_CAP_PROP_FPS);
    bool stop(false);
    Mat frame;
    int frameH    = (int) capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    int frameW    = (int) capture.get(CV_CAP_PROP_FRAME_WIDTH);

    namedWindow("Video");
    int delay=1000/rate;
    static int binThresh=200;
    Point armorcenter,tmp1,tmp2,tmp;
    bool flag=0;
    while(!stop)
    {
        if(!capture.read(frame))
            break;
        Mat result=frame;
        vector<Point> armors;
        FindArmorV(frame,armors);

        double dis1;
        double dis2;
        if (armors.size()>1&&!flag)
            flag=1;
        if (flag)
        {
//            cout<<armors.size()<<endl;
            switch(armors.size())
            {
            case 0:
                break;
            case 1:
                tmp=armors.at(0);
                dis1=sqrt((tmp.x-tmp1.x)*(tmp.x-tmp1.x)+(tmp.y-tmp1.y)*(tmp.y-tmp1.y));
                dis2=sqrt((tmp.x-tmp2.x)*(tmp.x-tmp2.x)+(tmp.y-tmp2.y)*(tmp.y-tmp2.y));
                if (dis1<dis2&&dis1<10)
                {
                    tmp2+=tmp-tmp1;
                    tmp1=tmp;
                }
                else if (dis2<10)
                {
                    tmp1+=tmp-tmp2;
                    tmp2=tmp;
                }
                armorcenter.x=(tmp1.x+tmp2.x)/2;
                armorcenter.y=(tmp1.y+tmp2.y)/2;
//                circle(result,tmp,4,CV_RGB(0,255,0),-1);
                break;
            case 2:
                tmp1=armors.at(0);
                tmp2=armors.at(1);
                armorcenter.x=(tmp1.x+tmp2.x)/2;
                armorcenter.y=(tmp1.y+tmp2.y)/2;
                break;
            default:
                cout<<'3'<<endl;
                break;
            }
        }
        circle(result,armorcenter,4,CV_RGB(255,0,0),-1);
        //drawContours(result, lights, -1, Scalar(0,255,0), CV_FILLED);
        imshow("Video",result);

        if(waitKey(delay)>=0)
            stop=true;
    }
    capture.release();
}
#else
int main()
{
    VideoCapture capture("H2m.avi");
    if(!capture.isOpened())
        return 1;
    double rate=capture.get(CV_CAP_PROP_FPS);
    bool stop(false);
    Mat frame;
    int frameH    = (int) capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    int frameW    = (int) capture.get(CV_CAP_PROP_FRAME_WIDTH);

    namedWindow("Video");
    int delay=1000/rate;
    static int binThresh=200;
    Point armorcenter,tmp1,tmp2,tmp;
    bool flag=0;
    while(!stop)
    {
        if(!capture.read(frame))
            break;
        Mat result=frame;
        vector<Point> armors;
        FindArmorH(frame,armors);

        double dis1;
        double dis2;
        if (armors.size()>0){
            for (int i=0;i<armors.size();i++)
                circle(result,armors.at(i),4,CV_RGB(255,0,0),-1);
        }

        //drawContours(result, lights, -1, Scalar(0,255,0), CV_FILLED);
        imshow("Video",result);

        if(waitKey(delay)>=0)
            stop=true;
    }
    capture.release();
}

#endif


