#pragma once

#include "def.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <mutex>


namespace chaos
{
	class CHAOS_EXPORT LogMessage
	{
	public:
		LogMessage(const char* file, int line, LogSeverity severity);
		~LogMessage();

		std::stringstream& Stream();

	public:
		static std::string log_name; // ��־�ļ�������ʱ�����е���־����ͬһ���ļ���
		static int log_level;

	private:
		void Flush();

	private:
		static std::mutex mtx;
		static std::vector<std::string> log_severity_text;
		std::stringstream message_data; // �����������LogMessageData
		time_t time_stamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

		LogSeverity severity;
	};

	// ��ʵstringstreamҲ�̳���ostream����������ع�ostream��&����Ϳ�����
	class CHAOS_EXPORT LogMessageVoidify {
	public:
		LogMessageVoidify() { }
		// This has to be an operator with a precedence lower than << but
		// higher than ?:
		void operator&(std::ostream&) { }
	};

	CHAOS_EXPORT void InitLogging(const std::string argv0, int level = 0);
}