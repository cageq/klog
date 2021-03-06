#pragma once
#include <vector>
#include <thread>
#include <iostream>
#include "log_sink.hpp"


#ifdef _WIN32

inline const char* _red()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	return "";
}
inline const char* _green()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	return "";
}
inline const char* _yellow()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
	return "";
}

inline const char* _blue()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
	return "";
}

inline const char* _magenta()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED);
	return "";
}
inline const char* _cyan()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN);
	return "";
}
inline const char* _reset()
{
	SetConsoleTextAttribute(
		GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
	return "";
}

#define ANSI_COLOR_RED _red()
#define ANSI_COLOR_GREEN _green()
#define ANSI_COLOR_YELLOW _yellow()
#define ANSI_COLOR_BLUE _blue()
#define ANSI_COLOR_MAGENTA _magenta()
#define ANSI_COLOR_CYAN _cyan()
#define ANSI_COLOR_RESET _reset()
#else
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"
#endif


namespace klog
{



	template <class Mutex = NoneMutex, bool = true>
	class ConsoleSink : public LogSink {

		virtual ~ConsoleSink() {}

		int32_t write(int level, const std::string& msg)
		{
			if (line_level < 0)
			{
				line_level = level;
			}

			buffer.append(msg);
			return 0;
		}

		virtual void flush(const std::string& log = "")
		{
			std::lock_guard<Mutex> guard(log_mutex);
			buffer.append(log);
			std::cout << buffer << std::endl;
			buffer.clear();
			line_level = -1;
		}

		const char* get_level_color() {
			switch (line_level) {
			case klog::KLOG_LEVEL_INFO:
				return ANSI_COLOR_GREEN;
			case klog::KLOG_LEVEL_DEBUG:
				return ANSI_COLOR_CYAN;
			case klog::KLOG_LEVEL_WARN:
				return ANSI_COLOR_YELLOW;
			case klog::KLOG_LEVEL_ERROR:
				return ANSI_COLOR_RED;
			default:
				return "";
			}
		}


	private:

		int line_level = -1;
		static thread_local std::string buffer;
		Mutex log_mutex;
	};


	template <class Mutex , bool color >
	thread_local std::string ConsoleSink<Mutex, color> ::buffer;

	template <class Mutex    >
	class ConsoleSink<Mutex, false> : public LogSink
	{

	public:


		virtual ~ConsoleSink() {}

		int32_t write(int level, const std::string& msg)
		{
			if (line_level < 0)
			{
				line_level = level;
			}

			buffer.append(msg);
			return 0;
		}

		virtual void flush(const std::string& log = "")
		{

			std::lock_guard<Mutex> guard(log_mutex);
			buffer.append(log);

			std::cout << buffer << std::endl;
			buffer.clear();
			line_level = -1;
		}

		const char* get_level_color() {
			switch (line_level) {
			case klog::KLOG_LEVEL_INFO:
				return ANSI_COLOR_GREEN;
			case klog::KLOG_LEVEL_DEBUG:
				return ANSI_COLOR_CYAN;
			case klog::KLOG_LEVEL_WARN:
				return ANSI_COLOR_YELLOW;
			case klog::KLOG_LEVEL_ERROR:
				return ANSI_COLOR_RED;
			default:
				return "";
			}
		}


	private:

		int line_level = -1;
		static thread_local std::string buffer;
		Mutex log_mutex;
	};
	template <class Mutex >
	thread_local std::string ConsoleSink<Mutex, false >::buffer;





	template <class Mutex   >
	class ConsoleSink <Mutex, true> : public LogSink
	{

	public:


		virtual ~ConsoleSink() {}

		int32_t write(int level, const std::string& msg)
		{
			if (line_level < KLOG_LEVEL_NULL)
			{
				line_level = level;
			}

			buffer.append(msg);
			return 0;
		}

		virtual void flush(const std::string& log = "")
		{

			std::lock_guard<Mutex> guard(log_mutex);
			buffer.append(log);
			std::cout << get_level_color() << buffer << ANSI_COLOR_RESET << std::endl;
			buffer.clear();
			line_level = -1;
		}

		const char* get_level_color() {
			switch (line_level) {
			case klog::KLOG_LEVEL_INFO:
				return ANSI_COLOR_GREEN;
			case klog::KLOG_LEVEL_DEBUG:
				return ANSI_COLOR_CYAN;
			case klog::KLOG_LEVEL_WARN:
				return ANSI_COLOR_YELLOW;
			case klog::KLOG_LEVEL_ERROR:
				return ANSI_COLOR_RED;
			default:
				return "";
			}
		}


	private:

		int line_level = -1;
		static thread_local std::string buffer;

		Mutex log_mutex;

	};
	template <class Mutex>
	thread_local std::string ConsoleSink<Mutex, true> ::buffer;
 


} // namespace klog
