#ifndef IMAGE_ANALYSIS_SERVICE_H
#define IMAGE_ANALYSIS_SERVICE_H

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <cmath>
using namespace cv;
using namespace std;

enum OutputImageType { REGION, PERIMETER };

enum Status {SUCCESS, FAILURE,INVALID_IMAGE, SEED_POINT_OUT_OF_RANGE};

struct Pixel
{
	int red;
	int green;
	int blue;
};

struct PointImg
{
public:
	PointImg(int x, int y)
	{
		X = x;
		Y = y;
	}
	int X;
	int Y;
};

class ImageAnalysisService
{
private:
	//private variables
	Mat m_inputImage;
	Mat m_regionImage;
	Mat m_perimeterImage;
	int m_rgbChannels;
	int m_width;
	int m_height;
	Pixel m_seedPixel;
	int m_tolerence;
	bool m_isRegionCalculated = false;
	bool m_imageLoaded = false;
	bool m_isPerimeterCalculated = false;
	std::vector<PointImg> m_listPt;
	const unsigned char WHITE = 255;
	const unsigned char BLACK = 0;

	//private methods
	void SHOW_MAT(const cv::Mat &image, std::string const &win_name);
	Status Flood_Fill_Forrest_Fire(int seedX, int seedY);
	Status Apply_Erosion(cv::Mat ipImage, cv::Mat opImage);
	Status Apply_Dialation(cv::Mat ipImage, cv::Mat opImage);
	Status Apply_Opening(cv::Mat ipImage, cv::Mat opImage);
	Status Apply_Closing(cv::Mat ipImage, cv::Mat opImage);
	Status Subtract_Image(cv::Mat firstImage, cv::Mat secondImage, cv::Mat opImage);
	Status Apply_Gaussian_Smoothing(cv::Mat ipImage, cv::Mat opImage);

public:
	//publically exposed properties
	Status INITIALIZE(string& filename);
	Status FIND_REGION(int x, int y, int tolerance = 5);
	Status FIND_PERIMETER();
	Status DISPLAY_IMAGE();
	Status DISPLAY_PIXELS(OutputImageType type);
	Status SAVE_PIXELS(OutputImageType type, std::string& filename);
	Status FIND_SMOOTH_PERIMETER();
	bool IsIntitialized();
	bool IsRegionCalculated();
	bool IsPerimeterCalculated();
	~ImageAnalysisService();
};

#endif
