#pragma once

#include "def.hpp"

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <numeric>
#include <iomanip>
#include <typeindex>

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

		static Mat Zeros(const Size siz, const int depth);
		static Mat Ones(const Size siz, const int depth);
		Mat Diag(int d = 0);


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
	template<> class DataDepth<unsigned short>
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
		// 反向查找
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

	template<class Type>
	class TMatFormatter
	{
	public:
		TMatFormatter(const Mat& mtx) : mtx(mtx) {}
		 
		friend std::ostream& operator << (std::ostream& stream, const TMatFormatter<Type>& formatted)
		{
			auto ptr = (Type*)formatted.mtx.data_start;

			std::cout << "[";
			for (size_t slice = 0; slice < formatted.mtx.step.slice_cnt; slice++)
			{
				if (slice % formatted.mtx.size[1] == 0)
					std::cout << "[";
				std::cout << "[";
				for (size_t row = 0; row < formatted.mtx.size[2]; row++)
				{
					auto data = ptr + row * formatted.mtx.step[2];
					for (size_t col = 0; col < formatted.mtx.size[3]; col++)
					{
						std::cout << data[col];
						if (row != formatted.mtx.size[2] - 1 || col != formatted.mtx.size[3] - 1)
							std::cout << " ";
					}
					if (row != formatted.mtx.size[2] - 1) std::cout << std::endl;
				}
				std::cout << "]";
				if (slice % formatted.mtx.size[1] == formatted.mtx.size[1] - 1)
					std::cout << "]";
				if (slice != formatted.mtx.step.slice_cnt - 1) std::cout << std::endl;
			}
			std::cout << "]";
			return stream;
		}

		Mat mtx;
	};

	

} // namespace chaos

