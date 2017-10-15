#include <iostream>
#include <math.h>

#ifndef _NLMEANSDENOISING_H_
#define _NLMEANSDENOISING_H_

namespace sqm{
	typedef float ElementType;

	class NlMeansDenoising
	{
	public:
		NlMeansDenoising(ElementType **_srcImg, ElementType **_destImg, unsigned int _width, unsigned int _height,
			unsigned int _channels);
		~NlMeansDenoising();

		void BuildLut();
		ElementType SearchLut(ElementType _diff);
		ElementType CalcDist2(int _anch_x, int _anch_y, int _search_x, int _search_y, int _radius);
		void NlMeans(int _half_path_win, int _half_search_win, double _sigma, double _filterPara);
	private:
		//source image related
		ElementType **m_srcImg;
		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_channels;
		ElementType **m_destImg;

		//NL means algorithm parameters related
		unsigned int m_halfPatchWin;
		unsigned int m_halfSearchWin;
		ElementType m_sigma;
		ElementType m_filterPara;
		ElementType *m_lut;
		unsigned int m_lutLength;
		ElementType m_lutMaxValue;
		ElementType m_distMax;
		ElementType m_lutPrecision;
	};
}
#endif