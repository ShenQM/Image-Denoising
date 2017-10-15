#include "common.h"

float** cvMat2Array(cv::Mat& _srcImg, unsigned int &_width, unsigned int &_height, unsigned int &_channel)
{
	_width = _srcImg.cols;
	_height = _srcImg.rows;
	_channel = _srcImg.channels();
	if (_width == 0 || _height == 0 || _channel == 0){
		return NULL;
	}

	CV_Assert(_srcImg.depth() == CV_8U);
	float **arrayImg = new float *[_channel];
	for (int i = 0; i < _channel; ++i){
		arrayImg[i] = new float[_width*_height];
		memset(arrayImg[i], 0.0f, sizeof(float)*_width*_height);
	}
	uchar* p;
	int nCols = _width*_channel;
	for (int i = 0; i < _height; ++i)
	{
		p = _srcImg.ptr<uchar>(i);
		for (int j = 0; j < _width; ++j)
		{
			int idx = i*_width + j;
			for (int idx_channel = 0; idx_channel < _channel; ++idx_channel){
				arrayImg[idx_channel][idx] = p[j*_channel + idx_channel];
			}
		}
	}
	return arrayImg;
}

cv::Mat array2cvMat(float **_arrayImg, unsigned int _width, unsigned int _height, unsigned int _channel)
{
	if (NULL == _arrayImg || _width <= 0 || _height <= 0 || _channel <= 0) return cv::Mat();
	int pixel_type[4] = { CV_8UC1, CV_8UC2, CV_8UC3, CV_8UC4 };
	cv::Mat image(_height, _width, pixel_type[_channel - 1]);
	uchar* p;
	int nCols = _width*_channel;
	for (int i = 0; i < _height; ++i)
	{
		p = image.ptr<uchar>(i);
		for (int j = 0; j < _width; ++j)
		{
			int idx = i*_width + j;
			for (int idx_channel = 0; idx_channel < _channel; ++idx_channel){
				p[j*_channel + idx_channel] = _arrayImg[idx_channel][idx];
			}
		}
	}
	return image;
}