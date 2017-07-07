#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

int main()
{
    VideoCapture capture(1);//����ǱʼǱ���0�򿪵����Դ�������ͷ��1 ����ӵ����
    double rate = 25.0;//��Ƶ��֡��
    Size videoSize(640,480);
    VideoWriter writer("V2..avi", CV_FOURCC('M', 'J', 'P', 'G'), rate, videoSize);
    Mat frame;

    while (capture.isOpened())
    {
        capture >> frame;
        writer << frame;
        imshow("video", frame);
        if (waitKey(20) == 27)//27�Ǽ�������escʱ����������յ���ascii��ֵ
        {
            break;
        }
    }
    return 0;
}
