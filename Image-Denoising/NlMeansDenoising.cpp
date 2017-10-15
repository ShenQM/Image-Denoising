#include "NlMeansDenoising.h"

namespace sqm{
	NlMeansDenoising::NlMeansDenoising(ElementType **_srcImg, ElementType **_destImg, unsigned int _width, unsigned int _height,
		unsigned int _channels) :m_srcImg(_srcImg), m_destImg(_destImg),m_lut(NULL), m_lutMaxValue(30.0), m_lutPrecision(1000.0),
		m_width(_width), m_height(_height), m_channels(_channels), m_lutLength(0), m_distMax(29.0)
	{
		//Initialize lut
		if (NULL == m_lut){
			m_lutLength = unsigned int (m_lutMaxValue*m_lutPrecision);
			m_lut = new ElementType[m_lutLength];
		}
		BuildLut();
	}

	NlMeansDenoising::~NlMeansDenoising()
	{
		if (NULL != m_lut){
			delete m_lut;
			m_lut = NULL;
		}
	}

	void NlMeansDenoising::BuildLut()
	{
		if (NULL != m_lut){
			for (unsigned int i = 0; i < m_lutLength; ++i){
				m_lut[i] = expf(-float(i) / m_lutPrecision);
				//printf("%f ", m_lut[i]);
			}
		}
		else{
			printf("%s, %s, Lut table not exist!\n", __FUNCTION__, __LINE__);
		}
	}

	ElementType NlMeansDenoising::SearchLut(ElementType _diff)
	{
		if (_diff >= m_distMax) return 0.0;

		ElementType x = _diff*m_lutPrecision;
		int x1 = int(floor(x));
		int x2 = x1 + 1;
		ElementType y1 = m_lut[x1];
		ElementType y2 = m_lut[x2];
		ElementType y = y1 + (y2 - y1)*(x - x1);
		return y;
	}

	ElementType NlMeansDenoising::CalcDist2(int _anch_x, int _anch_y, int _search_x, int _search_y, int _radius)
	{
		ElementType diff = 0.0;
		for (int idx_channel = 0; idx_channel < m_channels; ++idx_channel){
			for (int i = -_radius; i <= _radius; ++i){
				int patch_idx = (i + _anch_y)*m_width + _anch_x;
				int search_idx = (i + _search_y)*m_width + _search_x;
				for (int j = -_radius; j <= _radius; ++j){
					ElementType tmp_diff = m_srcImg[idx_channel][patch_idx + j] - m_srcImg[idx_channel][search_idx + j];
					diff += (tmp_diff * tmp_diff);
				}
			}
		}
		return diff;
	}

	void NlMeansDenoising::NlMeans(int _half_patch_win, int _half_search_win, double _sigma, double _filterPara)
	{
		using namespace std;
		for (int idx_channel = 0; idx_channel < m_channels; ++idx_channel){
			memset(&m_destImg[idx_channel][0], 0.0, sizeof(ElementType)*m_width*m_height);
		}
		ElementType *per_pixel_count = new ElementType[m_width*m_height];//estimated count for every pixel;
		memset(per_pixel_count, 0.0, sizeof(ElementType)*m_width*m_height);
		int patch_width = 2 * _half_patch_win + 1;
		int patch_size = patch_width*patch_width;
		double h = _sigma*_filterPara;
		double h2 = h*h*patch_size*m_channels;//for the distance is not normalized
		double sigma2 = 2 * _sigma*_sigma*patch_size*m_channels;//for the distance is not normalized

		ElementType **denoisedPatch = new ElementType *[m_channels];
		for (int i = 0; i < m_channels; ++i){
			denoisedPatch[i] = new ElementType[patch_size];
			memset(denoisedPatch[i], 0.0, sizeof(ElementType)*patch_size);
		}
		for (int y = 0; y < m_height; ++y){
			for (int x = 0; x < m_width; ++x){
				for (int i = 0; i < m_channels; ++i){
					memset(denoisedPatch[i], 0.0, sizeof(ElementType)*patch_size);
				}
				int radius = int(fmin(_half_patch_win, fmin(m_width - 1 - x, fmin(m_height - 1 - y, fmin(x, y)))));
				int search_x_min = int(fmax(radius, x - _half_search_win));
				int search_y_min = int(fmax(radius, y - _half_search_win));
				int search_x_max = int(fmin(m_width - 1 - radius, x + _half_search_win));
				int search_y_max = int(fmin(m_height - 1 - radius, y + _half_search_win));
				double weight_max = 0.0;
				double weight_total = 0.0;
				for (int yy = search_y_min; yy <= search_y_max; ++yy){
					for (int xx = search_x_min; xx <= search_x_max; ++xx){
						if (!(xx == x && yy == y)){//don't calulate for the point(x,y) itself;
							double diff = CalcDist2(x, y, xx, yy, radius);
							diff = fmax(diff, sigma2);
							diff = diff / h2;
							double weight = SearchLut(diff);
							if (weight > weight_max) weight_max = weight;
							weight_total += weight;
							for (int r = -radius; r <= radius; ++r){
								int patch_idx = (r + _half_patch_win)*patch_width + _half_patch_win;
								int search_idx = (yy + r)*m_width + xx;
								for (int c = -radius; c <= radius; ++c){
									int denoised_idx = patch_idx + c;
									int src_idx = search_idx + c;
									for (int idx_channel = 0; idx_channel < m_channels; ++idx_channel){
										denoisedPatch[idx_channel][denoised_idx] += weight * m_srcImg[idx_channel][src_idx];
									}
								}
							}
						}
					}
				}
				//calculate for the point(x,y) itself;
				double weight = weight_max;
				weight_total += weight;
				for (int r = -radius; r <= radius; ++r){
					int patch_idx = (r + _half_patch_win)*patch_width + _half_patch_win;
					int search_idx = (y + r)*m_width + x;
					for (int c = -radius; c <= radius; ++c){
						int denoised_idx = patch_idx + c;
						int src_idx = search_idx + c;
						for (int idx_channel = 0; idx_channel < m_channels; ++idx_channel){
							denoisedPatch[idx_channel][denoised_idx] += weight * m_srcImg[idx_channel][src_idx];
						}
					}
				}

				//get the estimated patch centered at point(x,y)
				if (weight_total > 1e-6){
					for (int r = -radius; r <= radius; ++r){
						int patch_idx = (r + _half_patch_win)*patch_width + _half_patch_win;
						int search_idx = (y + r)*m_width + x;
						for (int c = -radius; c <= radius; ++c){
							int denoised_idx = patch_idx + c;
							int dest_idx = search_idx + c;
							per_pixel_count[dest_idx] += 1.0;
							for (int idx_channel = 0; idx_channel < m_channels; ++idx_channel){
								m_destImg[idx_channel][dest_idx] += denoisedPatch[idx_channel][denoised_idx] / weight_total;
							}
						}
					}
				}
			}
		}

		//get the result by aggregation
		for (int y = 0; y < m_height; ++y){
			for (int x = 0; x < m_width; ++x){
				int spatio_idx = y*m_width + x;
				//printf("row:%d, col:%d, count:%f\n", y, x, per_pixel_count[spatio_idx]);
				if (per_pixel_count[spatio_idx] > 0.0){
					for (int idx_channel = 0; idx_channel < m_channels; ++idx_channel){
						m_destImg[idx_channel][spatio_idx] /= per_pixel_count[spatio_idx];
					}
				}
				else{
					for (int idx_channel = 0; idx_channel < m_channels; ++idx_channel){
						m_destImg[idx_channel][spatio_idx] = m_srcImg[idx_channel][spatio_idx];
					}
				}
			}
		}

		//release memory
		for (int i = 0; i < m_channels; ++i){
			delete [] denoisedPatch[i];
		}
		delete [] denoisedPatch;
		delete [] per_pixel_count;
	}
}