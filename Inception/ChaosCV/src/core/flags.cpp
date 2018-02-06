#include "core\flags.hpp"
#include "core\log_message.hpp"

#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <algorithm>

namespace chaos
{
	DEFINE_BOOL(help, false, "show help on all flags.");

	class FlagsPool
	{
	public:
		void Help(std::string restrict_file = std::string())
		{
			for (auto file : file_path)
			{
				if (!restrict_file.empty() && restrict_file != file.first)
					continue;

				std::string file_name = file.second;
				size_t pos = file_name.find_last_of('\\');
				std::cout << "Flags from " << file_name.substr(pos + 1) << ":" << std::endl;

				auto it = file_list.begin();
				while (true)
				{
					it = std::find_if(it, file_list.end(), [=](std::map<std::string, std::string>::value_type& value) { return value.second == file.first; });
					if (it == file_list.end()) break;

					// ��ʾ����
					std::string name = it->first;
					std::cout << "  -" << name << " (" << help_list[name] << ") type: " << type_list[name]
						<< " default: " << default_list[name] << std::endl;
					it++;
				}

				std::cout << std::endl;
			}

			// �鿴����һ�㶼���ڳ�����ʽ��ʼ֮ǰ�������������������ֹ����
			exit(0);
		}

		// �Բ�������ע��
		void Register(std::string name, Flag flag, std::string help, std::string type, std::string default_value, std::string file)
		{
			flag_list[name] = flag;
			help_list[name] = help;
			type_list[name] = type;
			default_list[name] = default_value;

			// ��ȡfile���ļ���������·��
			size_t pos = file.find_last_of('\\') + 1;
			std::string file_name = file.substr(pos);

			file_list[name] = file_name;
			file_path[file_name] = file;
		}

		template<class Type>
		void Set(std::string name, Type value)
		{
			// ����flag_list���Ƿ��иò���
			if (flag_list.end() != flag_list.find(name))
			{
				flag_list[name] = value;
			}
			else
			{
				LOG(FATAL) << "Unknown flag " << name << ".";
			}
		}

		// ���������char*��ʱ��Ҳ����string����
		template<>
		void Set(std::string name, const char* value)
		{
			// ����flag_list���Ƿ��иò���
			if (flag_list.end() != flag_list.find(name))
			{
				flag_list[name] = std::string(value);
			}
			else
			{
				LOG(FATAL) << "Unknown flag " << name << ".";
			}
		}

		std::string GetType(std::string name)
		{
			if (type_list.end() != type_list.find(name))
				return type_list[name];
			else
				return std::string();
		}

		// ���õ���ģʽ
		static std::shared_ptr<FlagsPool> Create()
		{
			static auto pool = std::shared_ptr<FlagsPool>(new FlagsPool());
			return pool;
		}

	public:
		std::string usage_message;
		std::vector<std::string> rest_argv;

	private:
		// ������Ա���Ϊ�͹�ϵ�����ݿ�����
		std::map<std::string, Flag> flag_list;
		std::map<std::string, std::string> help_list;
		std::map<std::string, std::string> file_list;

		std::map<std::string, std::string> type_list;
		std::map<std::string, std::string> default_list;

		// ���ƹ�ϵ���ݿ⣬�����ڶ���������ڴ洢file�Լ�file�ж�Ӧ��
		// ����������������file_list�н��в��Ҳ���ʾ������ʾ������ʱ��
		// �������Ϊ�������з�����ʾ
		// �������ļ������ܻ�������
		std::map<std::string, std::string> file_path;
	};

	FlagRegisterer::FlagRegisterer(std::string name, Flag flag, std::string help, std::string type, std::string default_value, std::string file)
	{
		auto pool = FlagsPool::Create();
		pool->Register(name, flag, help, type, default_value, file);
	}

	template<class Type>
	Type ConvertFlagString(std::string flag_string)
	{
		std::stringstream stream;
		stream << flag_string;
		Type value;
		stream >> value;
		return value;
	}

	template<>
	bool ConvertFlagString(std::string flag_string)
	{
		if ("true" == flag_string)
			return true;
		return false;
	}


	void ParseCommondLineFlags(int* argc, char*** argv, bool remove_flags)
	{
		auto pool = FlagsPool::Create();

		// ѭ����������������������map�����Ѿ�����õ�
		int rest_argc = 0;
		for (int i = 0; i < *argc; ++i)
		{

			if ('-' != (*argv)[i][0])
			{
				pool->rest_argv.push_back((*argv)[i]);
				rest_argc++;
			}
			else
			{
				std::string arg_value{ (*argv)[i] };
				std::string name, flag_string;
				if ('-' == (*argv)[i][1])
				{
					// �ָ��ַ���
					int pos = arg_value.find('=');
					name = arg_value.substr(2, pos - 2);
					if (pos > 0)
						flag_string = arg_value.substr(pos + 1);
					else
						flag_string = "true";
				}
				else
				{
					name = arg_value.substr(1);
					if (i + 1 < *argc && '-' != (*argv)[i + 1][0])
						flag_string = std::string((*argv)[++i]);
					else
						flag_string = "true"; // bool����
				}

				std::string type = pool->GetType(name);
				CHECK(!type.empty()) << "Unknown flags: " << name;
				if (type == "int") pool->Set(name, ConvertFlagString<int>(flag_string));
				if (type == "float") pool->Set(name, ConvertFlagString<float>(flag_string));
				if (type == "bool") pool->Set(name, ConvertFlagString<bool>(flag_string));
				if (type == "string") pool->Set(name, ConvertFlagString<std::string>(flag_string));
			}
		}

		if (remove_flags)
		{
			char** rest_argv = new char*[rest_argc];
			for (int i = 0; i < rest_argc; i++)
			{
				rest_argv[i] = (char*)pool->rest_argv[i].c_str();
			}

			*argc = rest_argc;
			*argv = rest_argv;
		}

		if (flag_help)
			ShowUsageMessage();
	}

	void SetUsageMessage(std::string message)
	{
		auto pool = FlagsPool::Create();
		pool->usage_message = message;
	}

	void ShowUsageMessage(std::string restrict_file)
	{
		auto pool = FlagsPool::Create();
		std::cout << pool->usage_message << std::endl;

		if (flag_help || !restrict_file.empty())
		{
			std::cout << std::endl;
			pool->Help(restrict_file);
		}
	}
}