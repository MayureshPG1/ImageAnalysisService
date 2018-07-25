#include "ImageAnalysisService.h"

Status ImageAnalysisService::INITIALIZE(string& filename)
{
	try
	{
		m_imageLoaded = false;
		m_inputImage = imread(filename, 1);

		if (!m_inputImage.data)
			return Status::INVALID_IMAGE;

		m_regionImage = Mat::zeros(m_inputImage.size(), CV_8UC1);
		m_perimeterImage = Mat::zeros(m_inputImage.size(), CV_8UC1);

		m_rgbChannels = m_inputImage.channels();
		m_width = m_inputImage.size().width;
		m_height = m_inputImage.size().height;
		m_imageLoaded = true;
		m_isRegionCalculated = false;
		m_isPerimeterCalculated = false;
		return Status::SUCCESS;
	}
	catch (...)
	{
		m_imageLoaded = false;
		return Status::FAILURE;
	}
}

bool ImageAnalysisService::IsIntitialized()
{
	return m_imageLoaded;
}

bool ImageAnalysisService::IsRegionCalculated()
{
	return m_isRegionCalculated;
}

bool ImageAnalysisService::IsPerimeterCalculated()
{
	return m_isPerimeterCalculated;
}

Status ImageAnalysisService::FIND_REGION(int seedX, int seedY, int tolerance)
{
	try
	{
		if ((seedX > m_width) || (seedX < 0) || (seedY > m_height) || (seedY < 0))
			return Status::SEED_POINT_OUT_OF_RANGE;

		//reset images
		m_isRegionCalculated = false;
		m_isPerimeterCalculated = false;
		m_regionImage = Mat::zeros(m_inputImage.size(), CV_8UC1);
		m_perimeterImage = Mat::zeros(m_inputImage.size(), CV_8UC1);

		//swap points for different conventions
		int tmpPt = seedX;
		seedX = seedY;
		seedY = tmpPt;

		Status val;

		Vec3b* tmp;
		tmp = m_inputImage.ptr<Vec3b>(seedX);

		Pixel oldPixel;
		oldPixel.red = tmp[seedY][0];
		oldPixel.green = tmp[seedY][1];
		oldPixel.blue = tmp[seedY][2];
		m_seedPixel = oldPixel;

		m_tolerence = tolerance;

		val = Flood_Fill_Forrest_Fire(seedX, seedY);
		if (val == Status::FAILURE)
			return val;

		//enhancements
		//Apply opening and closing to remove noise
		cv::Mat tmpImage = Mat::zeros(m_inputImage.size(), CV_8UC1);
		val = Apply_Opening(m_regionImage, tmpImage);
		if (val == Status::FAILURE)
			return val;

		val = Apply_Closing(tmpImage, m_regionImage);
		if (val == Status::FAILURE)
			return val;

		m_isRegionCalculated = true;

		return val;
	}
	catch (...)
	{
		m_isRegionCalculated = false;
		m_isPerimeterCalculated = false;
		return Status::FAILURE;
	}
}

Status ImageAnalysisService::Apply_Erosion(cv::Mat ipImage, cv::Mat opImage)
{
	try
	{
		//should only work with grayscale images
		if ((ipImage.type() != CV_8UC1) || (ipImage.type() != CV_8UC1))
			return Status::FAILURE;

		int sum = 0;
		uchar *ipPixel, *opPixel;
		for (int i = 1; i < m_height - 1; ++i)
		{
			for (int j = 1; j < m_width - 1; ++j)
			{
				sum = 0;
				ipPixel = ipImage.ptr<uchar>(i);
				sum += ipPixel[j];
				sum += ipPixel[j + 1];
				sum += ipPixel[j - 1];
				ipPixel = ipImage.ptr<uchar>(i + 1);
				sum += ipPixel[j];
				sum += ipPixel[j + 1];
				sum += ipPixel[j - 1];
				ipPixel = ipImage.ptr<uchar>(i - 1);
				sum += ipPixel[j];
				sum += ipPixel[j + 1];
				sum += ipPixel[j - 1];

				opPixel = opImage.ptr<uchar>(i);
				if (sum >= (9 * WHITE))
				{
					opPixel[j] = WHITE;
				}
				else
				{
					opPixel[j] = BLACK;
				}
			}
		}
		return Status::SUCCESS;
	}
	catch (...)
	{
		return Status::FAILURE;
	}
}

Status ImageAnalysisService::Apply_Dialation(cv::Mat ipImage, cv::Mat opImage)
{
	try
	{
		//should only work with grayscale images
		if ((ipImage.type() != CV_8UC1) || (ipImage.type() != CV_8UC1))
			return Status::FAILURE;

		int sum = 0;
		uchar *ipPixel, *opPixel;
		for (int i = 1; i < m_height - 1; ++i)
		{
			for (int j = 1; j < m_width - 1; ++j)
			{
				sum = 0;
				ipPixel = ipImage.ptr<uchar>(i);
				sum += ipPixel[j];
				sum += ipPixel[j + 1];
				sum += ipPixel[j - 1];
				ipPixel = ipImage.ptr<uchar>(i + 1);
				sum += ipPixel[j];
				sum += ipPixel[j + 1];
				sum += ipPixel[j - 1];
				ipPixel = ipImage.ptr<uchar>(i - 1);
				sum += ipPixel[j];
				sum += ipPixel[j + 1];
				sum += ipPixel[j - 1];

				opPixel = opImage.ptr<uchar>(i);
				if (sum >= WHITE)
				{
					opPixel[j] = WHITE;
				}
				else
				{
					opPixel[j] = BLACK;
				}
			}
		}
		return Status::SUCCESS;
	}
	catch (...)
	{
		return Status::FAILURE;
	}
}

Status ImageAnalysisService::Apply_Opening(cv::Mat ipImage, cv::Mat opImage)
{
	try
	{
		//should only work with grayscale images
		if ((ipImage.type() != CV_8UC1) || (ipImage.type() != CV_8UC1))
			return Status::FAILURE;

		cv::Mat tmpImage = Mat::zeros(m_inputImage.size(), CV_8UC1);
		Status val;

		val = Apply_Erosion(ipImage, tmpImage);
		if (val == Status::FAILURE)
			return val;

		val = Apply_Dialation(tmpImage, opImage);
		if (val == Status::FAILURE)
			return val;

		return val;
	}
	catch (...)
	{
		return Status::FAILURE;
	}
}

Status ImageAnalysisService::Apply_Closing(cv::Mat ipImage, cv::Mat opImage)
{
	try
	{
		//should only work with grayscale images
		if ((ipImage.type() != CV_8UC1) || (ipImage.type() != CV_8UC1))
			return Status::FAILURE;

		cv::Mat tmpImage = Mat::zeros(m_inputImage.size(), CV_8UC1);
		Status val;

		val = Apply_Dialation(ipImage, tmpImage);
		if (val == Status::FAILURE)
			return val;

		val == Apply_Erosion(tmpImage, opImage);
		if (val == Status::FAILURE)
			return val;

		return val;
	}
	catch (...)
	{
		return Status::FAILURE;
	}
}

Status ImageAnalysisService::Subtract_Image(cv::Mat firstImage, cv::Mat secondImage, cv::Mat opImage)
{
	try
	{
		//should only work with grayscale images
		if ((firstImage.type() != CV_8UC1) || (secondImage.type() != CV_8UC1) || (opImage.type() != CV_8UC1))
			return Status::FAILURE;

		uchar *firstImgPixel, *secondImgPixel, *opPixel;
		for (int i = 0; i < m_height; ++i)
		{
			firstImgPixel = firstImage.ptr<uchar>(i);
			secondImgPixel = secondImage.ptr<uchar>(i);
			opPixel = opImage.ptr<uchar>(i);
			for (int j = 0; j < m_width; ++j)
			{
				opPixel[j] = abs(firstImgPixel[j] - secondImgPixel[j]);
			}
		}
		return Status::SUCCESS;
	}
	catch (...)
	{
		return Status::FAILURE;
	}
}

Status ImageAnalysisService::Apply_Gaussian_Smoothing(cv::Mat ipImage, cv::Mat opImage)
{
	try
	{
		//should only work with grayscale images
		if ((ipImage.type() != CV_8UC1) || (ipImage.type() != CV_8UC1))
			return Status::FAILURE;

		float sum = 0.0;

		//Gaussian filter constants
		const int FILTER_SIZE = 9;
		const int FILTER_WEIGHT = 4;

		float GAUSSIAN_3X3[FILTER_SIZE] =
		{
			0.0625, 0.125, 0.0625,
			0.125, 0.25, 0.125,
			0.0625, 0.125, 0.0625
		};

		uchar *ipPixel, *opPixel;
		for (int i = 1; i < m_height - 1; ++i)
		{
			opPixel = opImage.ptr<uchar>(i);
			for (int j = 1; j < m_width - 1; ++j)
			{
				ipPixel = ipImage.ptr<uchar>(i - 1);
				sum += GAUSSIAN_3X3[0] * ipPixel[j - 1];
				sum += GAUSSIAN_3X3[1] * ipPixel[j];
				sum += GAUSSIAN_3X3[2] * ipPixel[j + 1];
				ipPixel = ipImage.ptr<uchar>(i);
				sum += GAUSSIAN_3X3[3] * ipPixel[j - 1];
				sum += GAUSSIAN_3X3[4] * ipPixel[j];
				sum += GAUSSIAN_3X3[5] * ipPixel[j + 1];
				ipPixel = ipImage.ptr<uchar>(i + 1);
				sum += GAUSSIAN_3X3[6] * ipPixel[j - 1];
				sum += GAUSSIAN_3X3[7] * ipPixel[j];
				sum += GAUSSIAN_3X3[8] * ipPixel[j + 1];
				
				//take average
				sum = sum / FILTER_SIZE;

				//central pixel's weight is 0.25, multiply by 4 to keep output bright enough and properly weighted
				sum = sum * FILTER_WEIGHT;

				//let it round off
				opPixel[j] = (unsigned char)sum;
			}
		}
		return Status::SUCCESS;
	}
	catch (...)
	{
		return Status::FAILURE;
	}
}

Status ImageAnalysisService::FIND_PERIMETER()
{
	try
	{
		if (!m_isRegionCalculated)
			return Status::FAILURE;

		//reset perimeter image
		m_perimeterImage = Mat::zeros(m_inputImage.size(), CV_8UC1);

		Status val;
		m_isPerimeterCalculated = false;

		//do morphological erosion
		val = Apply_Erosion(m_regionImage, m_perimeterImage);
		if (val == Status::FAILURE)
			return val;

		//subtrack from region image to get perimeter
		val = Subtract_Image(m_regionImage, m_perimeterImage, m_perimeterImage);
		if (val == Status::FAILURE)
			return val;

		m_isPerimeterCalculated = true;
		return Status::SUCCESS;
	}
	catch (...)
	{
		m_isPerimeterCalculated = false;
		return Status::FAILURE;
	}
}


Status ImageAnalysisService::DISPLAY_IMAGE()
{
	try
	{
		SHOW_MAT(m_inputImage, "Input Image");
		return Status::SUCCESS;
	}
	catch (...)
	{
		return Status::FAILURE;
	}
}

Status ImageAnalysisService::DISPLAY_PIXELS(OutputImageType type)
{
	try
	{
		switch (type)
		{
		case REGION:
			SHOW_MAT(m_regionImage, "Region Image");
			break;
		case PERIMETER:
			SHOW_MAT(m_perimeterImage, "Perimeter Image");
			break;
		default:
			return Status::FAILURE;
			break;
		}
		return Status::SUCCESS;
	}
	catch (...)
	{
		return Status::FAILURE;
	}
}

Status ImageAnalysisService::SAVE_PIXELS(OutputImageType type, std::string& filename)
{
	try
	{
		switch (type)
		{
		case REGION:
			imwrite(filename, m_regionImage);
			break;
		case PERIMETER:
			imwrite(filename, m_perimeterImage);
			break;
		default:
			return Status::FAILURE;
			break;
		}
		return Status::SUCCESS;
	}
	catch (...)
	{
		return Status::FAILURE;
	}
}

Status ImageAnalysisService::FIND_SMOOTH_PERIMETER()
{
	try
	{
		if (!m_isPerimeterCalculated)
			return Status::FAILURE;

		cv::Mat tmpImage = Mat::zeros(m_inputImage.size(), CV_8UC1);
		Apply_Gaussian_Smoothing(m_perimeterImage, tmpImage);
		m_perimeterImage = tmpImage;
		return Status::SUCCESS;
	}
	catch (...)
	{
		return Status::FAILURE;
	}
}

Status ImageAnalysisService::Flood_Fill_Forrest_Fire(int seedX, int seedY)
{
	try
	{
		Vec3b* tmp;
		Pixel currentPixel;
		uchar *grayPixel;

		PointImg pnt(seedX, seedY);

		//maintain a list of node
		m_listPt.push_back(pnt);
		while (!m_listPt.empty())
		{
			PointImg pnt = m_listPt.back();
			seedX = pnt.X;
			seedY = pnt.Y;

			//get last node
			m_listPt.pop_back();

			tmp = m_inputImage.ptr<Vec3b>(seedX);
			currentPixel.red = tmp[seedY][0];
			currentPixel.green = tmp[seedY][1];
			currentPixel.blue = tmp[seedY][2];

			grayPixel = m_regionImage.ptr<uchar>(seedX);

			//if already visited continue
			if (grayPixel[seedY] == WHITE)
				continue;

			if ((abs(currentPixel.red - m_seedPixel.red) < m_tolerence) && (abs(currentPixel.green - m_seedPixel.green) < m_tolerence) && (abs(currentPixel.blue - m_seedPixel.blue) < m_tolerence))
			{
				grayPixel[seedY] = WHITE;

				if ((seedX + 1) < m_width)
				{
					m_listPt.push_back(PointImg((seedX + 1), seedY));
				}
				if ((seedX - 1) > 0)
				{
					m_listPt.push_back(PointImg((seedX - 1), seedY));
				}
				if ((seedY + 1) < m_height)
				{
					m_listPt.push_back(PointImg(seedX, (seedY + 1)));
				}
				if ((seedY - 1) > 0)
				{
					m_listPt.push_back(PointImg(seedX, (seedY - 1)));
				}
			}
			else
			{
				continue;
			}
		}
		return Status::SUCCESS;
	}
	catch (...)
	{
		return Status::FAILURE;
	}

}

void ImageAnalysisService::SHOW_MAT(const cv::Mat &image, std::string const &win_name)
{
	try
	{
		namedWindow(win_name, WINDOW_AUTOSIZE);
		imshow(win_name, image);
		waitKey(0);
	}
	catch(...)
	{
		throw 0;
	}
}

ImageAnalysisService::~ImageAnalysisService()
{
	//no need as cv::Mat will deallocate itself
	//http://docs.opencv.org/2.4/modules/core/doc/intro.html#automatic-memory-management
}
