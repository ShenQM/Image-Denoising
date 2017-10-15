#include "demos.h"

void nl_means_demo()
{
	std::string file_name = "data/lena_noised_gaussian_sigma=10.png";
	cv::Mat image = cv::imread(file_name);
	unsigned int width = 0, height = 0, channel = 0;
	float **arrayImg = NULL;
	arrayImg = cvMat2Array(image, width, height, channel);
	float **destImg = new float *[channel];
	for (unsigned int i = 0; i < channel; ++i){
		destImg[i] = new float[width*height];
	}

	sqm::NlMeansDenoising nl_means_denoising(arrayImg, destImg, width, height, channel);
	clock_t t_begin = clock();
	nl_means_denoising.NlMeans(1, 10, 10.0, 0.55);
	clock_t t_end = clock();
	double denoising_time = double((t_end - t_begin) / CLOCKS_PER_SEC);
	printf("Time for non-local means denoising: %fs!\n", denoising_time);

	cv::Mat denoisedImg = array2cvMat(destImg, width, height, channel);
	cv::imshow("noise image", image);
	cv::imshow("denoised image", denoisedImg);
	cv::imwrite("data/denoised image.png", denoisedImg);
	cv::waitKey();
}