#pragma once

#define CHAOS_EXPORT __declspec(dllexport)

namespace chaos
{
	// «∞÷√…Í√˜
	class LogMessage;
	class LogMessageVoidify;

	class Flag;
	class FlagRegisterer;

	template<class Type> class TPoint;
	template<class Type> class TSize;
	template<class Type> class TRect;

	template<class Type> class TMat;
	template<class Type> class TMatIterator;
	template<class Type> class TMatInitializer;

	using uchar = unsigned char;
	using Point = TPoint<int>;
	using Size = TSize<int>;
	using Rect = TRect<int>;

	enum LogSeverity
	{
		INFO,
		WARNING,
		ERROR,
		FATAL,
	};

	enum MatDepth
	{
		DEPTH_8U,  // unsigned char
		DEPTH_8S, // char 1
		DEPTH_16U, // unsigned short
		DEPTH_16S, // short 2
		DEPTH_32S, // int
		DEPTH_32F, // float 4
		DEPTH_64F, // double 8

		DEPTH_UNKNOW = -1,
	};

	

} // namespace chaos

#define LOG(severity) chaos::LogMessage(					\
  __FILE__, __LINE__, severity).Stream()

#define CHECK(condition) condition ? (void)0 :				\
  chaos::LogMessageVoidify() & LOG(chaos::FATAL) <<			\
  "Check failed: " #condition ". "

#define CHECK_EQ(val1, val2) CHECK(val1 == val2)
#define CHECK_NE(val1, val2) CHECK(val1 != val2)
#define CHECK_LE(val1, val2) CHECK(val1 <= val2)
#define CHECK_LT(val1, val2) CHECK(val1 <  val2)
#define CHECK_GE(val1, val2) CHECK(val1 >= val2)
#define CHECK_GT(val1, val2) CHECK(val1 >  val2)

#define DO_NOT_IMIPLEMENT LOG(chaos::FATAL) << "DO NOT IMIPLEMENT";

#define	DEFINE_FLAGS(type, name, value, help)				\
  namespace chaos_##type {									\
    type flag_##name = value;								\
    chaos::Flag o_##name(&flag_##name);						\
    chaos::FlagRegisterer r_##name(#name, o_##name,			\
      help, #type, #value, __FILE__);						\
  }															\
  using chaos_##type::flag_##name;

#define DEFINE_INT(name, value, help)						\
  DEFINE_FLAGS(int, name, value, help)

#define DEFINE_FLOAT(name, value, help)						\
  DEFINE_FLAGS(float, name, value, help)

#define DEFINE_BOOL(name, value, help)						\
  DEFINE_FLAGS(bool, name, value, help)

#define DEFINE_STRING(name, value, help)					\
  using std::string;										\
  DEFINE_FLAGS(string, name, value, help)