
#include "MyKinect.h"

MyKinect::MyKinect()
{
	m_pCoordinateMapper = nullptr;
	m_pKinectSensor = nullptr;
	m_pColorFrameReader = nullptr;
	m_pDepthFrameReader = nullptr;
	m_pDepthFrameSource = nullptr;
	m_pColorFrameSource = nullptr;
	m_pDepthFrameDescription = nullptr;
	m_pColorFrameDescription = nullptr;
	m_pDepthFrame = nullptr;
	m_pColorFrame = nullptr;

	iDepthWidth = 0;
	iDepthHeight = 0;
	iColorWidth = 0;
	iColorHeight = 0;
	uDepthMin = 0;
	uDepthMax = 0;

	ImageColor = cv::Mat(cColorHeight, cColorWidth, CV_8UC4);
	ImageColor3ch = cv::Mat(cColorHeight, cColorWidth, CV_8UC3);
	ImageDepth = cv::Mat(cDepthHeight, cDepthWidth, CV_16UC1);
	ImageDepth8bit = cv::Mat(cDepthHeight, cDepthWidth, CV_8UC1);
	depth2colorSpacePoints = std::vector<ColorSpacePoint>(cDepthWidth * cDepthHeight);
	depth2cameraSpacePoints = std::vector<CameraSpacePoint>(cDepthWidth * cDepthHeight);
}


MyKinect::~MyKinect()
{
	ImageColor.release();
	ImageDepth.release();
	ImageDepth8bit.release();
	ImageColor3ch.release();

	m_pCoordinateMapper->Release();
	m_pCoordinateMapper = nullptr;
	// release frame reader
	m_pDepthFrameReader->Release();
	m_pColorFrameReader->Release();//释放指针
	m_pDepthFrameReader = nullptr;
	m_pColorFrameReader = nullptr;
	// release Frame source
	m_pDepthFrameSource->Release();
	m_pColorFrameSource->Release();
	m_pDepthFrameSource = nullptr;
	m_pColorFrameSource = nullptr;
	//  Close Sensor
	m_pKinectSensor->Close();
	//  Release Sensor
	m_pKinectSensor->Release();
	m_pKinectSensor = nullptr;
}

HRESULT	MyKinect::InitKinect(void)            //定义初始化kinect函数
{
	HRESULT result;
	// Get default Sensor
	result = GetDefaultKinectSensor(&m_pKinectSensor);
	//  Open sensor
	if (SUCCEEDED(result))//判断是否正确获取Kinect
	{
		std::cout << "Found Kinect Device" << std::endl;
	}
	else
	{
		return result;
	}
	m_pKinectSensor->Open();
	Sleep(500);//等待500ms，保证Kinect正确打开
	if (SUCCEEDED(result))
	{
		std::cout << "Success to open Kinect" << std::endl;
	}
	else
	{
		return result;
	}

	//  Get frame source
	result = m_pKinectSensor->get_DepthFrameSource(&m_pDepthFrameSource);//获取DepthFrameSource
	if (SUCCEEDED(result))
	{
		std::cout << "Get Depth Source" << std::endl;
	}
	else
	{
		return result;
	}
	result = m_pKinectSensor->get_ColorFrameSource(&m_pColorFrameSource);//获取ColorFrameSource
	if (SUCCEEDED(result))
	{
		std::cout << "Get Color Source" << std::endl;
	}
	else
	{
		return result;
	}

	m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
	//Get frame description
	m_pDepthFrameSource->get_FrameDescription(&m_pDepthFrameDescription);//获取帧格式描述
	m_pDepthFrameDescription->get_Width(&iDepthWidth);//帧宽度
	m_pDepthFrameDescription->get_Height(&iDepthHeight);//帧高度
	unsigned int depthbytesPerPixel = 0;
	m_pDepthFrameDescription->get_BytesPerPixel(&depthbytesPerPixel);//每一像素的大小，单位Byte
	std::cout << iDepthHeight << std::endl << iDepthWidth << std::endl << depthbytesPerPixel << std::endl;
	m_pDepthFrameDescription->Release();
	m_pDepthFrameDescription = nullptr;

	m_pColorFrameSource->get_FrameDescription(&m_pColorFrameDescription);//获取帧格式描述
	m_pColorFrameDescription->get_Height(&iColorHeight);//帧高度
	m_pColorFrameDescription->get_Width(&iColorWidth);//帧宽度
	unsigned int colorbytesPerPixel = 0;
	m_pColorFrameDescription->get_BytesPerPixel(&colorbytesPerPixel);//每一像素的大小，单位Byte
	std::cout << iColorHeight << std::endl << iColorWidth << std::endl << colorbytesPerPixel << std::endl;
	m_pColorFrameDescription->Release();
	m_pColorFrameDescription = nullptr;

	//  get some dpeth only meta
	m_pDepthFrameSource->get_DepthMinReliableDistance(&uDepthMin);
	m_pDepthFrameSource->get_DepthMaxReliableDistance(&uDepthMax);
	std::cout << "Reliable Distance: " << uDepthMin << " – " << uDepthMax << std::endl;

	// get frame reader
	result = m_pDepthFrameSource->OpenReader(&m_pDepthFrameReader);//获取图像Reader
	if (SUCCEEDED(result))
	{
		std::cout << "Get Depth Reader" << std::endl;
	}
	else
	{
		return result;
	}
	result = m_pColorFrameSource->OpenReader(&m_pColorFrameReader);//获取彩色图像Reader
	if (SUCCEEDED(result))
	{
		std::cout << "Get Color Reader" << std::endl;
	}
	else
	{
		return result;
	}

	return result;
}


HRESULT MyKinect::UpdateDepth(void)
{
	HRESULT result;


	//  Get last frame
	result = m_pDepthFrameReader->AcquireLatestFrame(&m_pDepthFrame);//获取最新的帧
	if (FAILED(result) || !m_pDepthFrame)//检测是否成功获取帧数据，并不是每一次都能成功获取的
	{
		std::cout << "Lose Depth data" << std::endl;
		return E_FAIL;
	}
	else
	{
		// copy the depth map to image
		m_pDepthFrame->CopyFrameDataToArray(iDepthWidth * iDepthHeight, reinterpret_cast<UINT16*>(ImageDepth.data));//得到深度数据
		m_pCoordinateMapper->MapDepthFrameToCameraSpace(cDepthWidth * cDepthHeight, reinterpret_cast<UINT16*>(ImageDepth.data), cDepthWidth * cDepthHeight, &depth2cameraSpacePoints[0]);//深度图到相机三维空间的映射
		m_pCoordinateMapper->MapDepthFrameToColorSpace(cDepthWidth * cDepthHeight, reinterpret_cast<UINT16*>(ImageDepth.data), cDepthWidth * cDepthHeight, &depth2colorSpacePoints[0]);//深度图到颜色的映射
		//  convert from 16bit to 8bit
		for (int row = 0; row < ImageDepth.rows; row++)
		{
			for (int col = 0; col < ImageDepth.cols; col++)
			{
				ImageDepth8bit.at<uchar>(row, col) = ImageDepth.at<UINT16>(row, col) % 256;
			}
		}
		//  release frame
		m_pDepthFrame->Release();//每一次获取帧后都要释放
	}
	return S_OK;
}


HRESULT MyKinect::UpdateColor(void)
{
	HRESULT result;
	result = m_pColorFrameReader->AcquireLatestFrame(&m_pColorFrame);//获取最新的彩色帧
	if (FAILED(result) || !m_pColorFrame)//检测是否成功获取帧数据，并不是每一次都能成功获取的
	{
		std::cout << "Lose Color data" << std::endl;
		return E_FAIL;
	}
	else
	{
		//std::cout << "Got Color data" << std::endl;
		m_pColorFrame->CopyConvertedFrameDataToArray(iColorHeight * iColorWidth * 4 * sizeof(BYTE), reinterpret_cast<BYTE*>(ImageColor.data), ColorImageFormat_Bgra);//图像数据流转换
		m_pColorFrame->Release();//每一次获取帧后都要释放
	}
	//Kinect读出的彩色图有四个通道，但是我们用的只有三个通道，转化一下即可
	std::vector<cv::Mat> channels(4);
	std::vector<cv::Mat> bgr(3);
	split(ImageColor, channels);//颜色通道分离
	bgr[0] = channels[0];
	bgr[1] = channels[1];
	bgr[2] = channels[2];
	merge(bgr, ImageColor3ch);//颜色通道合成
	return S_OK;
}


HRESULT	MyKinect::Mappering(cv::Mat & dst, int startDepth)//深度图映射到彩色空间
{
	HRESULT result;
	
	if (depth2colorSpacePoints.size() != 0)
	{
		dst = cv::Scalar(0, 0, 0, 0);
		for (int y = 0; y < cDepthHeight; y++) {
			for (int x = 0; x < cDepthWidth; x++) {
				unsigned int index = y *cDepthWidth + x;
				ColorSpacePoint point = depth2colorSpacePoints[index];
				int colorX = static_cast<int>(std::floor(point.X + 0.5));
				int colorY = static_cast<int>(std::floor(point.Y + 0.5));
				unsigned short depth = ImageDepth.at<unsigned short>(y, x);
				if ((colorX >= 0) && (colorX < cColorWidth) && (colorY >= 0) && (colorY < cColorHeight))
				{
					dst.at<cv::Vec4b>(y, x) = ImageColor.at<cv::Vec4b>(colorY, colorX);
					if (depth > startDepth && depth < (startDepth + 256))
						depth = depth - startDepth;
					else if (depth <= startDepth)
						depth = 0;
					else
						depth = 255;
					dst.at<cv::Vec4b>(y, x)[3] = static_cast<char>(depth);
				}
			}
		}
	}
	else
		return E_FAIL;

	return S_OK;
}


