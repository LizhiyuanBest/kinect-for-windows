#pragma once
/*****    VisualStudio 2017 (开发工具)
		  OpenCV2.4.8 (显示界面库 vc11库)
		  KinectSDK-v2.0
		  Windows 10 (操作系统)                 ******/
/*****    lizhiyuan                             ******/
/*****    2019-9-30                             ******/

// Standard Library
#include <iostream>
// OpenCV Header
#include<opencv2/opencv.hpp>
// Kinect for Windows SDK Header
#include <Kinect.h>

//定义Kinect方法类
class MyKinect
{
public:
	static const int        cDepthWidth = 512;   //深度图的大小
	static const int        cDepthHeight = 424;

	static const int        cColorWidth = 1920;   //彩色图的大小
	static const int        cColorHeight = 1080;

	cv::Mat     ImageDepth;//原始深度图
	cv::Mat     ImageDepth8bit;//深度图8位
	cv::Mat     ImageColor;//原始彩色图
	cv::Mat     ImageColor3ch;//三通道彩色图
	std::vector<ColorSpacePoint> depth2colorSpacePoints;//深度图到彩色图的映射向量
	std::vector<CameraSpacePoint> depth2cameraSpacePoints;//深度图到相机坐标系的映射向量

	HRESULT					InitKinect(void);//初始化Kinect
	HRESULT					UpdateColor(void);//更新彩色图
	HRESULT					UpdateDepth(void);//更新深度数据
	HRESULT					Mappering(cv::Mat & dst, int startDepth);//深度图映射到彩色空间

	MyKinect();                                     //构造函数
	~MyKinect();                                     //析构函数

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


