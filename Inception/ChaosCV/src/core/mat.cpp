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
		return stream << MatFormatter::Get(MFT_DEFAULT)->Format(mtx);
	}

#pragma endregion

#pragma region MatFormatter

	FormattedMat::FormattedMat(std::string pl, std::string el, const Mat& mtx, char br[7])
	{
		prologue = pl;
		epilogue = el;
		memcpy(braces, br, 7);

		state = STATE_PROLOGUE;

		switch (mtx.depth)
		{
		case DEPTH_8U:
			Convert = std::bind(&FormattedMat::Convert8U, this); break;
		case DEPTH_8S:
			Convert = std::bind(&FormattedMat::Convert8S, this); break;
		case DEPTH_16U:
			Convert = std::bind(&FormattedMat::Convert16U, this); break;
		case DEPTH_16S:
			Convert = std::bind(&FormattedMat::Convert16S, this); break;
		case DEPTH_32S:
			Convert = std::bind(&FormattedMat::Convert32S, this); break;
		case DEPTH_32F:
			Convert = std::bind(&FormattedMat::Convert32F, this); break;
		case DEPTH_64F:
			Convert = std::bind(&FormattedMat::Convert64F, this); break;
		default:
			LOG(chaos::FATAL) << "Unknown Depth Type";
		}

		this->mtx = mtx;

	}

	const std::string FormattedMat::Next()
	{
		switch (state)
		{
		case STATE_PROLOGUE:
		{
			num = 0; channel = 0;
			state = STATE_CHANNEL_OPEN;
			return prologue;
		}
		case STATE_EPILOGUE:
		{
			state = STATE_FINISHED;
			return epilogue;
		}
		case STATE_CHANNEL_OPEN:
		{
			state = STATE_ROW_OPEN;
			row = 0;
			char brace = braces[BRACE_CHANNEL_OPEN];
			return std::string(&brace, 1);
		}
		case STATE_CHANNEL_CLOSE:
		{
			channel++;
			if (channel == mtx.size[1])
			{
				num++;
				if (num == mtx.size[0])
				{
					state = STATE_EPILOGUE;
				}
				else
				{
					channel = 0;
					state = STATE_NUM_SEPARATOR;
				}
			}
			else
			{
				state = STATE_CHANNEL_SEPARATOR;
			}
			char brace = braces[BRACE_CHANNEL_CLOSE];
			return std::string(&brace, 1);
		}
		case STATE_CHANNEL_SEPARATOR:
		{
			char brace = braces[BRACE_CHANNEL_SEP];
			state = STATE_CHANNEL_OPEN;
			return std::string(&brace, 1) + "\n";
		}
		case STATE_NUM_SEPARATOR:
		{
			char brace = braces[BRACE_NUM_SEP];
			state = STATE_CHANNEL_OPEN;
			return std::string(&brace, 1) + "\n\n";
		}
		case STATE_ROW_OPEN:
		{
			state = STATE_VALUE;
			col = 0;
			char brace = braces[BRACE_ROW_OPEN];
			if (row == 0)
				return std::string();
			else
				return std::string(&brace, 1);
		}
		case STATE_ROW_CLOSE:
		{
			char brace = braces[BRACE_ROW_CLOSE];
			return std::string(&brace, 1);
			break;
		}
		case STATE_VALUE:
		{
			Convert();
			col++;
			if (col == mtx.size[3])
				state = STATE_LINE_SEPARATOR;
			else
				state = STATE_VALUE_SEPARATOR;
			return buff;
		}
		case STATE_VALUE_SEPARATOR:
		{
			state = STATE_VALUE;
			return ", ";
		}
		case STATE_LINE_SEPARATOR:
		{
			row++;
			char brace = braces[BRACE_ROW_SEP];
			if (row == mtx.size[2])
			{
				state = STATE_CHANNEL_CLOSE;
				return std::string();
			}
			else
			{
				state = STATE_ROW_OPEN;
				return std::string(&brace, 1) + "\n";
			}
			
			
		}
		default:
			return std::string();
		}
	}

	std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<FormattedMat>& formatted)
	{
		stream << formatted->prologue;

		while (formatted->state != FormattedMat::STATE_FINISHED)
		{
			stream << formatted->Next();
		}

		stream << formatted->epilogue;
		return stream;
	}

	class DefaultFormatter : public MatFormatter
	{
	public:
		std::shared_ptr<FormattedMat> Format(const Mat& mtx) final
		{
			char braces[7] = { '\0', '\0', ';', '[', ']', ',', ';' };
			return std::shared_ptr<FormattedMat>(new FormattedMat("", "", mtx, &*braces));
		}
	};

	//class MatlabFormatter : public MatFormatter
	//{
	//public:
	//	std::shared_ptr<FormattedMat> Format(const Mat& mtx) final
	//	{
	//		char braces[7] = { '\0', '\0', ';', '\0', '\0', '\0', '\0' };
	//		return std::shared_ptr<FormattedMat>(new FormattedMat("", "", mtx, &*braces));
	//	}
	//};

	//class PythonFormatter : public MatFormatter
	//{
	//public:
	//	std::shared_ptr<FormattedMat> Format(const Mat& mtx) final
	//	{
	//		char braces[7] = { '[', ']', ',', '[', ']', '\n', '\n' };
	//		return std::shared_ptr<FormattedMat>(new FormattedMat("[", "]", mtx, &*braces));
	//	}
	//};

	std::shared_ptr<MatFormatter> MatFormatter::Get(MatFormatType fmt)
	{
		switch (fmt)
		{
		//case chaos::MFT_MATLAB:
		//	return std::shared_ptr<MatFormatter>(new MatlabFormatter());
		//case chaos::MFT_PYTHON:
		//	return std::shared_ptr<MatFormatter>(new PythonFormatter());
		//case chaos::MFT_DEFAULT:
		default:
			return std::shared_ptr<MatFormatter>(new DefaultFormatter());
		}
	}
#pragma endregion
}