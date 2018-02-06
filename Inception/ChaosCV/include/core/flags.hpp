#pragma once

#include "def.hpp"

#include <string>
#include <typeindex>

namespace chaos
{
	class CHAOS_EXPORT Flag
	{
	public:
		Flag() {}

		template<class Type>
		Flag(Type* ptr) : ptr((void*)ptr), idx(typeid(Type)) {}

		template<class Type>
		Flag operator = (Type value)
		{
			if (idx == std::type_index(typeid(Type)))
			{
				*(Type*)ptr = value;
			}
			else
			{
				LOG(FATAL) << "Flag type error.";
			}
			return *this;
		}

	private:
		void* ptr = nullptr;
		std::type_index idx = typeid(void*);
	};

	class CHAOS_EXPORT FlagRegisterer
	{
	public:
		FlagRegisterer(std::string name, Flag flag, std::string help, std::string type, std::string default_value, std::string file);
	};

	// 需要实现的参数解析需要修改argc以及argv
	CHAOS_EXPORT void ParseCommondLineFlags(int* argc, char*** argv, bool remove_flags = true);

	CHAOS_EXPORT void SetUsageMessage(std::string message);
	CHAOS_EXPORT void ShowUsageMessage(std::string restrict_file = std::string());

} // namespace chaos