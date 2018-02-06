#include "core\log_message.hpp"
#include "core\flags.hpp"

#include <iomanip>
#include <iostream>
#include <fstream>

namespace chaos
{
	DEFINE_STRING(log_dir, "", "Log DIR");

	std::string LogMessage::log_name = "";
	std::mutex LogMessage::mtx;
	int LogMessage::log_level = 0;
	std::vector<std::string> LogMessage::log_severity_text{ "INFO", "WARNING", "ERROR", "FATAL" };

	LogMessage::LogMessage(const char* file, int line, LogSeverity severity)
		: severity(severity)
	{
		//static std::vector<std::string> log_severity_text{ "INFO", "WARNING", "ERROR", "FATAL" };
		// 获取文件名
		std::string _file(file);
		int pos = _file.find_last_of('\\') + 1;

		std::string severity_str;

		//// 生成时间等标签
		tm time;
		localtime_s(&time, &time_stamp); // 线程安全版本，同时避免使用指针
		//tm* time = std::localtime(&time_stamp); // 因为不是通过new获得的，因此不能释放这个见鬼的指针

		// 设置显示的日志等级
		if (severity >= log_level)
		{
			message_data << "[" << log_severity_text[severity] << " "
				<< time.tm_year + 1900 << "-" << std::setfill('0') << std::setw(2)
				<< time.tm_mon + 1 << "-" << time.tm_mday << " "
				<< time.tm_hour << ":" << time.tm_min << ":" << time.tm_sec << " "
				<< _file.substr(pos).c_str() << ":" << line << "] ";
		}
	}

	LogMessage::~LogMessage()
	{
		Flush();

		// 如果是FATAL，则发出中断
		if (severity == FATAL) abort();
	}

	std::stringstream& chaos::LogMessage::Stream()
	{
		return message_data;
	}

	// 如果以后有写入文件的需求，应该可以在这里面写入文件
	void chaos::LogMessage::Flush()
	{
		if (severity >= log_level)
		{
			std::string message = message_data.str();
			std::cout << message << std::endl;

			if (flag_log_dir.empty())
				return;

			// 多线程加锁
			mtx.lock();
			// 保存日志文件
			std::fstream log_file(flag_log_dir + "\\" + log_name, std::ios::out | std::ios::app);
			if (log_file.is_open())
			{
				log_file << message << std::endl;
				log_file.close();
			}
			mtx.unlock();
		}
	}

	void chaos::InitLogging(const std::string argv0, int level)
	{
		size_t slash_pos = argv0.find_last_of('\\');
		size_t point_pos = argv0.find_last_of('.');
		std::string exe_name = argv0.substr(slash_pos + 1, point_pos - slash_pos - 1);

		time_t time_stamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		//// 生成时间标签
		tm time;
		localtime_s(&time, &time_stamp); // 线程安全版本，同时避免使用指针

		std::stringstream ss;
		ss << time.tm_year + 1900 << "." << std::setfill('0') << std::setw(2)
			<< time.tm_mon + 1 << "." << time.tm_mday << "."
			<< time.tm_hour << "." << time.tm_min << "." << time.tm_sec << "."
			<< exe_name << ".LOG";

		LogMessage::log_name = ss.str();
		//LogMessage::log_name = cv::format("%04d.%02d.%02d.%02d.%02d.%02d.%s.LOG",
		//	time.tm_year + 1900, time.tm_mon + 1, time.tm_mday,
		//	time.tm_hour, time.tm_min, time.tm_sec, exe_name.c_str());

		LogMessage::log_level = level;
	}


} // namespace chaos