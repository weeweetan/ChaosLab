#include "core\mat.hpp"
#include "core\core.hpp"


namespace chaos
{
#pragma region MatSize

	MatSize::MatSize()
	{
		memset(siz, 0, 4 * sizeof(size_t));
	}

	MatSize::MatSize(const Size size)
	{
		siz[0] = siz[1] = 1;
		siz[2] = size.height;
		siz[3] = size.width;
	}

	MatSize::MatSize(const size_t num, const size_t chs, const size_t height, const size_t width)
	{
		siz[0] = num;
		siz[1] = chs;
		siz[2] = height;
		siz[3] = width;
	}

	MatSize::MatSize(const std::vector<size_t> dims)
	{
		CHECK_EQ(dims.size(), 4);
		for (size_t i = 0; i < 4; i++)
		{
			siz[i] = dims[i];
		}
	}

	MatSize::MatSize(const MatSize& size)
	{
		memcpy_s(siz, 4 * sizeof(size_t), size.siz, 4 * sizeof(size_t));
	}


	Size MatSize::operator()() const
	{
		return Size(siz[3], siz[2]);
	}

	size_t MatSize::operator[](size_t idx) const
	{
		return siz[idx];
	}

#pragma endregion

#pragma region MatStep
	MatStep::MatStep()
	{
		memset(stp, 0, 3*sizeof(size_t));
	}

	MatStep::MatStep(const MatSize& siz)
	{
		stp[0] = siz[1] * siz[2] * siz[3]; // block step: chs * h * w 
		stp[1] = siz[2] * siz[3]; // slice step: h * w // Outer Step
		stp[2] = siz[3]; // row step: w // Inner Step

		slice_cnt = siz[0] * siz[1]; // num * chs
	}


	size_t MatStep::operator[](size_t idx) const
	{
		return stp[idx];
	}
#pragma endregion

#pragma region Mat
	Mat::Mat() : size(MatSize()), step(MatStep()), depth(DEPTH_8U), data(nullptr), data_start(nullptr), ref_cnt(nullptr)
	{
	}

	Mat::Mat(const size_t width, const size_t height, const MatDepth depth) : size(1,1,height, width), step(size), depth(depth), ref_cnt(new size_t(1))
	{
		data = data_start = new uchar[size[0] * step[0] * std::powf(2, depth / 2)]();
	}
	Mat::Mat(const std::vector<size_t> dims, const MatDepth depth) : size(dims), step(size), depth(depth), ref_cnt(new size_t(1))
	{
		data = data_start = new uchar[size[0] * step[0] * std::powf(2, depth / 2)]();
	}
	Mat::Mat(const MatSize siz, const MatDepth depth) : size(siz), step(size), depth(depth), ref_cnt(new size_t(1))
	{
		data = data_start = new uchar[size[0] * step[0] * std::powf(2, depth / 2)]();
	}
	Mat::Mat(const Size siz, const MatDepth depth) : size(siz), step(size), depth(depth), ref_cnt(new size_t(1))
	{
		data = data_start = new uchar[size[0] * step[0] * std::powf(2, depth / 2)]();
	}

	Mat::Mat(const size_t width, const size_t height, const MatDepth, void* data) : size(1, 1, height, width), step(size), depth(depth), ref_cnt(nullptr)
	{
		this->data = data_start = (uchar*)data;
	}
	Mat::Mat(const std::vector<size_t> dims, const MatDepth, void* data) : size(dims), step(size), depth(depth), ref_cnt(nullptr)
	{
		this->data = data_start = (uchar*)data;
	}
	Mat::Mat(const MatSize siz, const MatDepth, void* data) : size(siz), step(size), depth(depth), ref_cnt(nullptr)
	{
		this->data = data_start = (uchar*)data;
	}
	Mat::Mat(const Size siz, const MatDepth, void* data) : size(siz), step(size), depth(depth), ref_cnt(nullptr)
	{
		this->data = data_start = (uchar*)data;
	}

	Mat::~Mat()
	{
		Release();
	}

	Mat::Mat(const Mat& mtx)
	{
		// 先判断原Mat是否有数据，如果有，则调用release
		Release();

		ref_cnt = mtx.ref_cnt;
		size = mtx.size;
		depth = mtx.depth;
		step = mtx.step;
		data = mtx.data;
		data_start = mtx.data_start;
		is_submatrix = mtx.is_submatrix;

		if (nullptr != ref_cnt) ++*ref_cnt;
	}

	// 取ROI
	Mat::Mat(const Mat& mtx, const Rect& roi)
	{
		// 先判断原Mat是否有数据，如果有，则调用release
		Release();

		ref_cnt = mtx.ref_cnt;
		size = mtx.size;
		depth = mtx.depth;
		step = mtx.step;
		data = mtx.data;

		if (nullptr != ref_cnt) ++*ref_cnt;

		// 修改size
		// 判断roi的两个点是否都在图像范围之内
		auto mat_rect = Rect(Point(0,0), size());
		CHECK(roi.br.Inside(mat_rect) && roi.tl.Inside(mat_rect))
			<< "The ROI is out of range.";

		// 设置roi大小，并修改step
		data_start = data + (roi.tl.x + roi.tl.y*size[3])*(int)std::powf(2, depth / 2);
		// 先不计算data_end的指针

		size.siz[2] = roi.size.height;
		size.siz[3] = roi.size.width;

		is_submatrix = true;
	}

	Mat& Mat::operator=(const Mat& mtx)
	{
		Release();

		ref_cnt = mtx.ref_cnt;
		size = mtx.size;
		depth = mtx.depth;
		step = mtx.step;
		data = mtx.data;
		data_start = mtx.data_start;
		is_submatrix = mtx.is_submatrix;

		ref_cnt = mtx.ref_cnt;
		if (nullptr != ref_cnt) ++*ref_cnt;

		return *this;
	}

	Mat Mat::operator()(const Rect& roi)
	{
		return Mat(*this, roi);
	}

	void Mat::Release()
	{
		if (nullptr == ref_cnt) return;

		if (0 == --*ref_cnt)
		{
			delete[] data;
			data = nullptr;

			delete ref_cnt;
			ref_cnt = nullptr;
		}
	}

	Mat Mat::Clone() const
	{
		Mat mtx(size, depth);
		
		auto ptr = data_start;
		auto dst = mtx.data;
		for (size_t slice = 0; slice < mtx.step.slice_cnt; slice++)
		{
			for (size_t row = 0; row < mtx.size[2]; row++)
			{
				auto data = ptr + row * step[2] * (int)std::powf(2, depth / 2);
				
				memcpy_s(dst, mtx.size[3] * std::powf(2, depth / 2), data, mtx.size[3] * std::powf(2, depth / 2));

				dst += mtx.size[3] * (int)std::powf(2, depth / 2);
			}
		}

		return mtx;
	}

	std::ostream & operator<<(std::ostream& stream, const Mat& mtx)
	{
		switch (mtx.depth)
		{
		case DEPTH_8U:
			return stream << TMatFormatter<uchar>(mtx);
		case DEPTH_8S:
			return stream << TMatFormatter<char>(mtx);
		case DEPTH_16U:
			return stream << TMatFormatter<unsigned short>(mtx);
		case DEPTH_16S:
			return stream << TMatFormatter<short>(mtx);
		case DEPTH_32S:
			return stream << TMatFormatter<int>(mtx);
		case DEPTH_32F:
			return stream << TMatFormatter<float>(mtx);
		case DEPTH_64F:
			return stream << TMatFormatter<double>(mtx);
		default:
			return stream;
		}
	}

#pragma endregion

}