#include "test.h"
using namespace sqm;

void test_nl_means_denoising()
{
	//ElementType **src_image, **dest_image;
	
	ElementType src_image[10][10] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, };
	ElementType **image = (ElementType **)&src_image[0][0];
	ElementType dest_image[10][10] = { 0 };
	int channels = 3;
	int width = 10;
	int height = 10;
	NlMeansDenoising test_nlMeansDenoising((ElementType **)&src_image[0][0], (ElementType **)&dest_image[0][0], 
		width, height, channels);
	// test for lut
	ElementType search_val[10] = { 0.0, 0.256, 1.0, 1.2556, 2.0, 11.0};
	ElementType output_val[10] = { 0.0 };
	for (int i = 0; i < 10; ++i){
		output_val[i] = test_nlMeansDenoising.SearchLut(search_val[i]);
		printf("lut_val:%.10f, true_val:%.10f \n", float(output_val[i]), expf(-search_val[i]));
	}

	
	// test for calculate dist
	int anch_x = 2, anch_y = 2;
	int search_x = 7, search_y = 7;
	ElementType dist = test_nlMeansDenoising.CalcDist2(anch_x, anch_y, search_x, search_y, 2);
	int x = 0;
}

void test_translate_image()
{
	std::string file_name = "data/lena.jpg";
	cv::Mat image = cv::imread(file_name);
	cv::imshow("load_lena", image);
	cv::waitKey();

	unsigned int width = 0, height = 0, channel = 0;
	float **arrayImg = NULL;
	arrayImg = cvMat2Array(image, width, height, channel);

	cv::Mat res_image = array2cvMat(arrayImg, width, height, channel);
	cv::imshow("translated_lena", image);
	cv::waitKey();
}