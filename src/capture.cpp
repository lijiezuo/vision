#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

int main()
{
    VideoCapture capture(1);//如果是笔记本，0打开的是自带的摄像头，1 打开外接的相机
    double rate = 25.0;//视频的帧率
    Size videoSize(640,480);
    VideoWriter writer("V2..avi", CV_FOURCC('M', 'J', 'P', 'G'), rate, videoSize);
    Mat frame;

    while (capture.isOpened())
    {
        capture >> frame;
        writer << frame;
        imshow("video", frame);
        if (waitKey(20) == 27)//27是键盘摁下esc时，计算机接收到的ascii码值
        {
            break;
        }
    }
    return 0;
}
