#pragma once
#include "../Core.hpp"


RT_START

namespace log {

#ifdef RT_PLATFORM_WINDOWS
	constexpr char* reset = "\x1B[0m";
	constexpr char* green = "\033[92m";
	constexpr char* yellow = "\033[93m";
	constexpr char* red = "\033[91m";
	constexpr char* colors[] = { reset, green, yellow, red };
	

#else
#error Only windows currently supported
#endif
	enum class LogLevel
	{
		INFO = 1,
		WARNING = 2,
		ERROR = 3,
		FATAL = 4
	};

	constexpr char* levels[] = { "", "INFO", "WARNING", "ERROR", "FATAL"};

	template <typename T>
	void log(LogLevel level, bool s, T msg)
	{
		const char* color = colors[(int)level];
		std::cout << color << msg << reset << std::endl;
	}

	template <typename T, typename... Args>
	void log(LogLevel level, bool s, T msg, Args... msgs)
	{	
		const char* color = colors[(int)level];
		if (s)
		{
			std::cout << color << "[" << levels[(int)level] << "]  ";
		}
		
		std::cout << msg << " ";
		log(level, false, msgs...);
	}

}


RT_END