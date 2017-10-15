#ifndef _COMMON_H_
#define _COMMON_H_

#include <opencv2/core/core.hpp>


float **cvMat2Array(cv::Mat& _srcImg, unsigned int &_width, unsigned int &_height, unsigned int &_channel);

cv::Mat array2cvMat(float **_srcImg, unsigned int _width, unsigned int _height, unsigned int _channel);

#endif