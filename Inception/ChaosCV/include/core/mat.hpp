#pragma once

#include "def.hpp"

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <numeric>
#include <iomanip>
#include <typeindex>
#include <functional>

#ifdef USE_OPENCV
#include <opencv2\opencv.hpp>
#endif

namespace chaos
{
	// Point + Size
	// Point + Point
	template<class Type>
	class TPoint
	{
	public:
		TPoint() : x(0), y(0) {}
		TPoint(Type x, Type y) : x(x), y(y) {}

		TPoint(const TSize<Type>& siz) : x(siz.width), y(siz.height) {}

		bool Inside(const TRect<Type>& rect) const
		{
			return rect.tl <= *this && *this <= rect.br;
		}

		TPoint<Type>& operator+=(const TPoint<Type>& pt)
		{
			x += pt.x;
			y += pt.y;
			return *this;
		}
		TPoint<Type>& operator+=(const TSize<Type>& siz)
		{
			x += siz.width;
			y += siz.height;
			return *this;
		}
		TPoint<Type>& operator-=(const TPoint<Type>& pt)
		{
			x -= pt.x;
			y -= pt.y;
			return *this;
		}
		TPoint<Type>& operator-=(const TSize<Type>& siz)
		{
			x -= siz.width;
			y -= siz.height;
			return *this;
		}
		TPoint<Type> operator*=(const Type value)
		{
			x *= value;
			y *= value;
			return *this;
		}
		TPoint<Type> operator/=(const Type value)
		{
			x /= value;
			y /= value;
			return *this;
		}

		// 强制转换为TSize类型
		operator TSize<Type>() const
		{
			return { x, y };
		}

		friend TPoint<Type> operator+(const TPoint<Type>& pt, const TSize<Type>& siz)
		{
			return { pt.x + siz.width, pt.y + siz.height };
		}
		friend TPoint<Type> operator+(const TPoint<Type>& pt1, const TPoint<Type>& pt2)
		{
			return { pt1.x + pt2.x, pt1.y + pt2.y };
		}
		friend TPoint<Type> operator-(const TPoint<Type>& pt, const TSize<Type>& siz)
		{
			return { pt.x - siz.width, pt.y - siz.height };
		}
		friend TPoint<Type> operator-(const TPoint<Type>& pt1, const TPoint<Type>& pt2)
		{
			return { pt1.x - pt2.x, pt1.y - pt2.y };
		}
		friend TPoint<Type> operator*(const TPoint<Type>& pt, const Type value)
		{
			return { pt.x * value, pt.y * value };
		}
		friend TPoint<Type> operator/(const TPoint<Type>& pt, const Type value)
		{
			return { pt.x / value, pt.y / value };
		}
		friend bool operator==(const TPoint<Type>& pt1, const TPoint<Type>& pt2)
		{
			return pt1.x == pt2.x && pt1.y == pt2.y;
		}
		friend bool operator>=(const TPoint<Type>& pt1, const TPoint<Type>& pt2)
		{
			return pt1.x >= pt2.x && pt1.y >= pt2.y;
		}
		friend bool operator<=(const TPoint<Type>& pt1, const TPoint<Type>& pt2)
		{
			return pt1.x <= pt2.x && pt1.y <= pt2.y;
		}
		friend bool operator<(const TPoint<Type>& pt1, const TPoint<Type>& pt2)
		{
			return pt1.x < pt2.x && pt1.y < pt2.y;
		}
		friend bool operator>(const TPoint<Type>& pt1, const TPoint<Type>& pt2)
		{
			return pt1.x > pt2.x && pt1.y > pt2.y;
		}
		friend bool operator!=(const TPoint<Type>& pt1, const TPoint<Type>& pt2)
		{
			return pt1.x != pt2.x && pt1.y != pt2.y;
		}
		
		friend std::ostream& operator<<(std::ostream& stream, const TPoint<Type>& pt)
		{
			stream << "[" << pt.x << ", " << pt.y << "]";
			return stream;
		}

		Type x, y; // 只读属性
	};

	// Size + Size
	template<class Type>
	class TSize
	{
	public:
		TSize() : width(0), height(0), area(0) {}
		TSize(Type width, Type height) : width(fabs(width)), height(fabs(height)), area(fabs(width*height)) {}

		TSize(const TPoint<Type>& pt) : width(fabs(pt.x)), height(fabs(pt.y))
		{
			area = width * height;
		}

		TSize<Type>& operator+=(const TSize<Type>& siz)
		{
			width = abs(width + siz.width);
			height = abs(height + siz.height);
			return *this;
		}
		TSize<Type>& operator-=(const TSize<Type>& siz)
		{
			width = abs(width - siz.width);
			height = abs(height - siz.height);
			return *this;
		}
		TSize<Type>& operator*=(const Type value)
		{
			width *= abs(value);
			height *= abs(value);
			return *this;
		}
		TSize<Type>& operator/=(const Type value)
		{
			width /= abs(value);
			height /= abs(value);
			return *this;
		}

		// 强制转换为TPoint类型
		operator TPoint<Type>() const
		{
			return { width, height };
		}

		friend TSize<Type> operator+(const TSize<Type>& siz1, const TSize<Type>& siz2)
		{
			return { siz1.width + siz2.width, siz1.height + siz2.height };
		}
		friend TSize<Type> operator-(const TSize<Type>& siz1, const TSize<Type>& siz2)
		{
			return { siz1.width - siz2.width, siz1.height - siz2.height };
		}
		friend TSize<Type> operator*(const TSize<Type>& siz1, const Type value)
		{
			return { siz1.width * value, siz1.height * value };
		}
		friend TSize<Type> operator/(const TSize<Type>& siz1, const Type value)
		{
			return { siz1.width / value, siz1.height / value };
		}
		friend bool operator==(const TSize<Type>& siz1, const TSize<Type>& siz2)
		{
			return siz1.area == siz2.area;
		}
		friend bool operator>=(const TSize<Type>& siz1, const TSize<Type>& siz2)
		{
			return siz1.area >= siz2.area;
		}
		friend bool operator<=(const TSize<Type>& siz1, const TSize<Type>& siz2)
		{
			return siz1.area <= siz2.area;
		}
		friend bool operator!=(const TSize<Type>& siz1, const TSize<Type>& siz2)
		{
			return siz1.area != siz2.area;
		}
		friend bool operator>(const TSize<Type>& siz1, const TSize<Type>& siz2)
		{
			return siz1.area > siz2.area;
		}
		friend bool operator<(const TSize<Type>& siz1, const TSize<Type>& siz2)
		{
			return siz1.area < siz2.area;
		}
		friend std::ostream& operator<<(std::ostream& stream, const TSize<Type>& siz)
		{
			stream << "[" << siz.width << ", " << siz.height << "]";
			return stream;
		}

		Type width, height; // 只读属性
		Type area;  // 只读属性
	};

	template<class Type>
	class TRect
	{
	public:
		TRect() : tl(0, 0), br(0, 0), size(0, 0) {}
		TRect(TPoint<Type> pt1, TPoint<Type> pt2)
		{
			tl = pt1 < pt2 ? pt1 : pt2;
			br = pt1 > pt2 ? pt1 : pt2;
			size = br - tl;
		}
		TRect(TPoint<Type> tl, TSize<Type> siz) : tl(tl), br(tl + siz), size(siz) {}

		TRect(Type x, Type y, Type width, Type height) : tl(x, y), br(x + width, y + height), size(width, height) {}

		bool Contain(const TPoint<Type>& pt) const
		{
			return tl <= pt && pt <= br;
		}

		friend TRect<Type> operator+(const TRect<Type>& ret, const TSize<Type> siz)
		{
			return { ret.tl, ret.size + siz };
		}
		friend TRect<Type> operator+(const TRect<Type>& ret, const TPoint<Type> pt)
		{
			return { ret.tl + pt, ret.br + pt };
		}
		friend TRect<Type> operator-(const TRect<Type>& ret, const TSize<Type> siz)
		{
			return { ret.tl, ret.size - siz };
		}
		friend TRect<Type> operator-(const TRect<Type>& ret, const TPoint<Type> pt)
		{
			return { ret.tl - pt, ret.br - pt };
		}
		friend TRect<Type> operator&(const TRect<Type>& ret1, const TRect<Type> ret2)
		{
			auto tl = ret1.tl > ret2.tl ? ret1.tl : ret2.tl;
			auto br = ret1.br < ret2.br ? ret1.br : ret2.br;
			return { tl, br };
		}
		friend TRect<Type> operator|(const TRect<Type>& ret1, const TRect<Type> ret2)
		{
			auto tl = ret1.tl < ret2.tl ? ret1.tl : ret2.tl;
			auto br = ret1.br > ret2.br ? ret1.br : ret2.br;
			return { tl, br };
		}
		friend std::ostream& operator<<(std::ostream& stream, const TRect<Type> ret)
		{
			stream << "[" << ret.size.width << " x " << ret.size.height << " at (" << ret.tl.x << ", " << ret.tl.y << ")]";
			return stream;
		}

		TPoint<Type> tl;
		TPoint<Type> br;
		TSize<Type> size;
	};
	
	class CHAOS_EXPORT MatSize
	{
	public:
		MatSize();
		MatSize(const Size size);
		MatSize(const size_t num, const size_t chs, const size_t height, const size_t width);
		MatSize(const std::vector<size_t> dims);
		MatSize(const MatSize& size);

		// 返回一个Slice的Size
		Size operator()() const;
		size_t operator[](size_t idx) const;

		size_t siz[4]; // NCHW
	};

	class CHAOS_EXPORT MatStep
	{
	public:
		MatStep();
		MatStep(const MatSize& siz);

		size_t operator[](size_t idx) const;

		size_t stp[3];
		size_t slice_cnt;
	};

	// 关于深度的问题，还有待考虑
	class CHAOS_EXPORT Mat
	{
	public:
		Mat();
		Mat(const size_t width, const size_t height, const MatDepth depth);
		Mat(const std::vector<size_t> dims, const MatDepth depth);
		Mat(const MatSize siz, const MatDepth depth);
		Mat(const Size siz, const MatDepth depth);

		Mat(const size_t width, const size_t height, const MatDepth depth, void* data);
		Mat(const std::vector<size_t> dims, const MatDepth depth, void* data);
		Mat(const MatSize siz, const MatDepth depth, void* data);
		Mat(const Size siz, const MatDepth depth, void* data);

		Mat(const Mat& mtx, const Rect& roi);

		Mat(const Mat& mtx);
		Mat& operator=(const Mat& mtx);
		Mat operator()(const Rect& roi);

		~Mat();

		void Release();
		// 等实现roi的获取，再考虑clone怎么实现
		Mat Clone() const;

		template<class Type>
		Type* GetPtr(int num, int channel, int row, int col)
		{
			// 对边界进行判断
			CHECK(0 <= num && num < size[0]);
			CHECK(0 <= channel && channel < size[1]);
			CHECK(0 <= row && row < size[2]);
			CHECK(0 <= col && col < size[3]);

			auto ptr = (Type*)data_start;
			ptr += (num * step[0] + channel * step[1] + row * step[2] + col);
			return ptr;
		}

		//static Mat Zeros(const Size siz, const int depth);
		//static Mat Ones(const Size siz, const int depth);
		//Mat Diag(int d = 0);


		CHAOS_EXPORT friend std::ostream& operator<<(std::ostream& stream, const Mat& mtx);
		
	public:
		size_t* ref_cnt = nullptr;
		uchar* data;
		uchar* data_start; // 在使用数据的时候，的起始指针（roi）
		uchar* data_end; // 数据终止，迭代的时候用么？
		MatSize size;
		MatStep step;
		MatDepth depth;
		bool is_submatrix = false; // 是否是子矩阵
	};

#pragma region Data Depth
	template<class Type> class DataDepth
	{
	public:
		static constexpr MatDepth depth = DEPTH_UNKNOW;
	};

	template<> class DataDepth<uchar>
	{
	public:
		static constexpr MatDepth depth = DEPTH_8U;
	};
	template<> class DataDepth<char>
	{
	public:
		static constexpr MatDepth depth = DEPTH_8S;
	};
	template<> class DataDepth<ushort>
	{
	public:
		static constexpr MatDepth depth = DEPTH_16U;
	};
	template<> class DataDepth<short>
	{
	public:
		static constexpr MatDepth depth = DEPTH_16S;
	};
	template<> class DataDepth<int>
	{
	public:
		static constexpr MatDepth depth = DEPTH_32S;
	};
	template<> class DataDepth<float>
	{
	public:
		static constexpr MatDepth depth = DEPTH_32F;
	};
	template<> class DataDepth<double>
	{
	public:
		static constexpr MatDepth depth = DEPTH_64F;
	};
#pragma endregion

	template<class Type>
	class TMat : public Mat
	{
	public:
		TMat() : Mat() {}

		TMat(std::vector<size_t>&& dims) : Mat((dims), DataDepth<Type>::depth)
		{
		}

		TMat(std::vector<size_t>&& dims, Type* data) : Mat((dims), DataDepth<Type>::depth, data)
		{
		}

		template<class ValueType>
		TMatInitializer<Type> operator<<(ValueType value)
		{
			TMatInitializer<Type> initializer(this);
			return (initializer, value);
		}
	};

	template<class Type>
	class TMatIterator
	{
	public:
		TMatIterator(TMat<Type>* mtx) : mtx(mtx)
		{
			ptr = it_start = (Type*)mtx->data;
		}

		template<class ValueType>
		TMatIterator& operator=(ValueType value)
		{
			*ptr = (Type)value;
			return *this;
		}

		TMatIterator& operator++()
		{
			ptr++;
			return *this;
		}

	public:
		Mat * mtx;
		Type* it_start;
		Type* it_end;
		Type* ptr;
	};

	template<class Type>
	class TMatInitializer
	{
	public:
		TMatInitializer(TMat<Type>* mtx) : it(mtx)
		{
		}

		template<class ValueType>
		TMatInitializer& operator,(ValueType value)
		{
			it = value;
			++it;
			return *this;
		}

		operator TMat<Type>()
		{
			return *(TMat<Type>*)it.mtx;
		}

	private:
		TMatIterator<Type> it;
	};


	class FormattedMat
	{
	public:
		enum State
		{
			STATE_PROLOGUE, STATE_EPILOGUE, 
			STATE_CHANNEL_OPEN, STATE_CHANNEL_CLOSE, 
			STATE_ROW_OPEN, STATE_ROW_CLOSE, 
			STATE_VALUE, STATE_FINISHED, STATE_VALUE_SEPARATOR,
			STATE_LINE_SEPARATOR, STATE_CHANNEL_SEPARATOR, STATE_NUM_SEPARATOR,
		};

		enum Brace
		{
			BRACE_ROW_OPEN, BRACE_ROW_CLOSE, 
			BRACE_ROW_SEP, BRACE_CHANNEL_OPEN, 
			BRACE_CHANNEL_CLOSE, BRACE_CHANNEL_SEP, 
			BRACE_NUM_SEP,
		};

		FormattedMat(std::string pl, std::string el, const Mat& mtx, char br[7]);

		friend std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<FormattedMat>& formatted);	

		const std::string Next();

		Mat mtx;
		std::string prologue, epilogue;
		char braces[7];
		State state;
		// NCHW
		size_t num, channel, row, col; // row = h, col = w

		char buff[32];   // enough for double with precision up to 20
		std::function<void(void)> Convert;
		void Convert8U() { sprintf_s(buff, "%3d", (int)mtx.GetPtr<uchar>(num, channel, row, 0)[col]); };
		void Convert8S() { sprintf_s(buff, "%3d", (int)mtx.GetPtr<char>(num, channel, row, 0)[col]); };
		void Convert16U() { sprintf_s(buff, "%d", mtx.GetPtr<ushort>(num, channel, row, 0)[col]); };
		void Convert16S() { sprintf_s(buff, "%d", mtx.GetPtr<short>(num, channel, row, 0)[col]); };
		void Convert32S() { sprintf_s(buff, "%d", mtx.GetPtr<int>(num, channel, row, 0)[col]); };
		void Convert32F() { sprintf_s(buff, "%f", mtx.GetPtr<float>(num, channel, row, 0)[col]); };
		void Convert64F() { sprintf_s(buff, "%lf", mtx.GetPtr<double>(num, channel, row, 0)[col]); };
	};

	class MatFormatter
	{
	public:
		virtual std::shared_ptr<FormattedMat> Format(const Mat& mtx) = 0;

		static std::shared_ptr<MatFormatter> Get(MatFormatType fmt = MFT_DEFAULT);
	};
	

} // namespace chaos

