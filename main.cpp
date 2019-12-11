#include "MyKinect.h"



using namespace cv;
using namespace std;
vector<Point> src;

ofstream fout("01.txt");
void writeMatToFile(cv::Mat& m, Point p, const char* filename)
{
	if (!fout)
	{
		cout << "File Not Opened" << endl;  return;
	}
	fout << p.x << " " << p.y << " " << m.at<unsigned short>(p.x, p.y) << endl;
	fout.close();
}

Point p;
void onMouse1(int event, int x, int y, int flags, void *param)
{
	static int i = 0;
	Mat *img = reinterpret_cast<Mat*>(param);
	if (event == CV_EVENT_LBUTTONDOWN)//������£���ȡ��ʼ���꣬����ͼ���ϸõ㴦��Բ  
	{
		i++;//ͳ�Ƶ���Ĵ���

		p.x = x;
		p.y = y;
		src.push_back(p);
		cout << p << static_cast<int>(img->at<unsigned short>(cv::Point(x, y))) << endl;
		cout << i << endl;

		fout << p.x << " " << p.y << " " << static_cast<int>(img->at<unsigned short>(cv::Point(x, y))) << endl;
	}
}

void onMouse2(int event, int x, int y, int flags, void *param)
{
	static int i = 0;
	Mat *img = reinterpret_cast<Mat*>(param);
	if (event == CV_EVENT_LBUTTONDOWN)//������£���ȡ��ʼ���꣬����ͼ���ϸõ㴦��Բ  
	{
		i++;//ͳ�Ƶ���Ĵ���

		p.x = x;
		p.y = y;
		cout << p << img->at<cv::Vec4b>(cv::Point(x, y)) << endl;
		cout << i << endl;
	}
}

int main()
{
	MyKinect kinect;
	
	cv::Mat mDepthImg;
	cv::Mat mDepthImg8bit;
	cv::Mat mColorImg;
	Mat coordinateMapperMat(kinect.cDepthHeight, kinect.cDepthWidth, CV_8UC4);
	kinect.InitKinect();
	Sleep(500);
	while (1)
	{
		
		HRESULT hr1 = kinect.UpdateColor();//������������ݣ���ȡ���ݲ���ʾ
		Sleep(10);
		HRESULT hr2 = kinect.UpdateDepth();

		if (SUCCEEDED(hr1) && SUCCEEDED(hr2))
		{
			mColorImg = kinect.ImageColor3ch.clone();
			mDepthImg8bit = kinect.ImageDepth8bit.clone();
			mDepthImg = kinect.ImageDepth.clone();
			imshow("color", mColorImg);
			imshow("depth", mDepthImg8bit);
			setMouseCallback("depth", onMouse1, &mDepthImg);
			Sleep(10);

			kinect.Mappering(coordinateMapperMat, 500);
			imshow("CoordinateMapper", coordinateMapperMat);
			setMouseCallback("CoordinateMapper", onMouse2, &coordinateMapperMat);

		}

		//imwrite("color.bmp", mColorImg);
		//imwrite("depth8.bmp", mDepthImg8bit);
		//imwrite("depth16.bmp", mDepthImg);
		if (cv::waitKey(1) >= 0)//����������˳�
		{
			break;
		}
	}

	return 0;
}

