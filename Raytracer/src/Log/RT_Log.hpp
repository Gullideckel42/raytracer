#pragma once
#include <ctime>

RT_START

#ifdef RT_DEBUG
#define rt_info(sender,...) RT_ log::log(RT_ log::LogLevel::RT_INFO, true, sender, __VA_ARGS__);
#define rt_warn(sender,...) RT_ log::log(RT_ log::LogLevel::RT_WARNING, true, sender, __VA_ARGS__);
#define rt_error(sender,...) RT_ log::log(RT_ log::LogLevel::RT_ERROR, true, sender, __VA_ARGS__);
#define rt_fatal(sender,...) RT_ log::log(RT_ log::LogLevel::RT_FATAL, true, sender, __VA_ARGS__);
#else
#define rt_info(...)
#define rt_warn(...)
#define rt_error(...)
#define rt_fatal(...)
#endif
namespace log {

#ifdef RT_PLATFORM_WINDOWS
	constexpr char* reset = "\x1B[0m";
	constexpr char* green = "\033[92m";
	constexpr char* yellow = "\033[93m";
	constexpr char* red = "\033[91m";
	constexpr char* blue= "\033[34m";
	constexpr char* purple= "\033[35m";
	constexpr char* colors[] = { reset, green, yellow, red, purple };
#else
#error Only windows currently supported
#endif
	enum class LogLevel
	{
		RT_INFO = 1,
		RT_WARNING = 2,
		RT_ERROR = 3,
		RT_FATAL = 4
	};

	constexpr char* levels[] = { "", "Info", "Warning", "Error", "FATAL"};


	// [INFO 20:02:23] @Core - 
	// [Error 20:02:23] @Core -
	// 20:02:23 [FATAL] @Core -
	inline void printLogHead(LogLevel level, const char* sender)
	{
#pragma warning( disable : 4244)
#pragma warning( disable : 4996)
		const char* color = colors[(int)level];
		std::time_t t = std::time(0);
		std::tm* now = std::localtime(&t);
		
		
		std::cout << color << ((now->tm_hour < 10) ? "0" : "") << now->tm_hour << ":"
			<< ((now->tm_min < 10) ? "0" : "") << now->tm_min << ":"
			<< ((now->tm_sec < 10) ? "0" : "") << now->tm_sec <<

			" [" << levels[(int)level] << "] " << blue << "@" << sender << color << " - ";
	}

	

	template <typename T>
	void log(LogLevel level, bool s, const char* sender, T msg)
	{
		const char* color = colors[(int)level];
		if (s)
		{
			printLogHead(level, sender);
		}

		std::cout << color << msg << reset << std::endl;
	}

	template <typename T, typename... Args>
	void log(LogLevel level, bool s, const char* sender, T msg, Args... msgs)
	{	
		const char* color = colors[(int)level];
		if (s)
		{
			printLogHead(level, sender);
		}
		
		std::cout << msg;
		log(level, false, sender, msgs...);
	}

}

RT_END