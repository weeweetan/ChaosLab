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

	Mat::Mat() : size(MatSize()), step(MatStep()), depth(0), data(nullptr), ref_cnt(nullptr)
	{
	}

	Mat::Mat(const size_t width, const size_t height, const size_t depth) : size(1,1,height, width), step(size), depth(depth), ref_cnt(new size_t(1))
	{
		data = new uchar[size[0] * step[0] * depth]();
	}
	Mat::Mat(const std::vector<size_t> dims, const int depth) : size(dims), step(size), depth(depth), ref_cnt(new size_t(1))
	{
		data = new uchar[size[0] * step[0] * depth]();
	}
	Mat::Mat(const MatSize siz, const int depth) : size(siz), step(size), depth(depth), ref_cnt(new size_t(1))
	{
		data = new uchar[size[0] * step[0] * depth]();
	}
	Mat::Mat(const Size siz, int depth) : size(siz), step(size), depth(depth), ref_cnt(new size_t(1))
	{
		data = new uchar[size[0] * step[0] * depth]();
	}

	Mat::Mat(const size_t width, const size_t height, const size_t depth, void* data) : size(1, 1, height, width), step(size), depth(depth), ref_cnt(nullptr)
	{
		this->data = (uchar*)data;
	}
	Mat::Mat(const std::vector<size_t> dims, const int depth, void* data) : size(dims), step(size), depth(depth), ref_cnt(nullptr)
	{
		this->data = (uchar*)data;
	}
	Mat::Mat(const MatSize siz, const int depth, void* data) : size(siz), step(size), depth(depth), ref_cnt(nullptr)
	{
		this->data = (uchar*)data;
	}
	Mat::Mat(const Size siz, int depth, void* data) : size(siz), step(size), depth(depth), ref_cnt(nullptr)
	{
		this->data = (uchar*)data;
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

		if (nullptr != ref_cnt) ++*ref_cnt;
	}

	Mat& Mat::operator=(const Mat& mtx)
	{
		Release();

		ref_cnt = mtx.ref_cnt;
		size = mtx.size;
		depth = mtx.depth;
		step = mtx.step;
		data = mtx.data;

		ref_cnt = mtx.ref_cnt;
		if (nullptr != ref_cnt) ++*ref_cnt;

		return *this;
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

		memcpy_s(mtx.data, mtx.size[0] * mtx.step[0] * depth, data, size[0] * step[0] * depth);

		return mtx;
	}

	std::ostream & operator<<(std::ostream & stream, const Mat & mtx)
	{
		std::cout << "Hello World" << std::endl;
		return stream;
	}

#pragma endregion


}