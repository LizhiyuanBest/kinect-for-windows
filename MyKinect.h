#pragma once
/*****    VisualStudio 2017 (��������)
		  OpenCV2.4.8 (��ʾ����� vc11��)
		  KinectSDK-v2.0
		  Windows 10 (����ϵͳ)                 ******/
/*****    lizhiyuan                             ******/
/*****    2019-9-30                             ******/

// Standard Library
#include <iostream>
// OpenCV Header
#include<opencv2/opencv.hpp>
// Kinect for Windows SDK Header
#include <Kinect.h>

//����Kinect������
class MyKinect
{
public:
	static const int        cDepthWidth = 512;   //���ͼ�Ĵ�С
	static const int        cDepthHeight = 424;

	static const int        cColorWidth = 1920;   //��ɫͼ�Ĵ�С
	static const int        cColorHeight = 1080;

	cv::Mat     ImageDepth;//ԭʼ���ͼ
	cv::Mat     ImageDepth8bit;//���ͼ8λ
	cv::Mat     ImageColor;//ԭʼ��ɫͼ
	cv::Mat     ImageColor3ch;//��ͨ����ɫͼ
	std::vector<ColorSpacePoint> depth2colorSpacePoints;//���ͼ����ɫͼ��ӳ������
	std::vector<CameraSpacePoint> depth2cameraSpacePoints;//���ͼ���������ϵ��ӳ������

	HRESULT					InitKinect(void);//��ʼ��Kinect
	HRESULT					UpdateColor(void);//���²�ɫͼ
	HRESULT					UpdateDepth(void);//�����������
	HRESULT					Mappering(cv::Mat & dst, int startDepth);//���ͼӳ�䵽��ɫ�ռ�

	MyKinect();                                     //���캯��
	~MyKinect();                                     //��������

private:

	ICoordinateMapper*      m_pCoordinateMapper;
	IKinectSensor*          m_pKinectSensor;// Current Kinect
	IDepthFrameSource*		m_pDepthFrameSource;
	IColorFrameSource*		m_pColorFrameSource;
	IDepthFrameReader*      m_pDepthFrameReader;// Depth reader   
	IColorFrameReader*      m_pColorFrameReader;// Color reader
	IFrameDescription*		m_pDepthFrameDescription;
	IFrameDescription*		m_pColorFrameDescription;
	IDepthFrame*			m_pDepthFrame;
	IColorFrame*			m_pColorFrame;

	int         iDepthWidth;
	int         iDepthHeight;
	int         iColorWidth;
	int         iColorHeight;
	UINT16		uDepthMin;
	UINT16		uDepthMax;
};


