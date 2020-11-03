#pragma once
#include <vector>
#include <thread>
#include <iostream>
#include "log_sink.hpp"
 

#ifdef _WIN32
 
inline const char *_red()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	return "";
}
inline const char *_green()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	return "";
}
inline const char *_yellow()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
	return "";
}

inline const char *_blue()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
	return "";
}

inline const char *_magenta()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED);
	return "";
}
inline const char *_cyan()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN);
	return "";
}
inline const char *_reset()
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

	class ConsoleSink : public LogSink
	{

		public:
			ConsoleSink(bool color = true):with_color(color){

			}
			virtual ~ConsoleSink(){}

			int32_t write(int level , const std::string &msg)
			{ 
				if (line_level  < 0 )
				{
					line_level = level; 
				}
				
				buffer.append(msg); 
				return 0;
			}
			virtual void flush(const std::string & log = "")
			{
				buffer.append(log); 
				if (with_color)
				{
					std::cout << get_level_color() <<  buffer << ANSI_COLOR_RESET <<  std::endl; 
				}else {
					std::cout <<  buffer << std::endl; 
				}
				
				buffer.clear(); 
				line_level = -1; 
			}

			const char * get_level_color(  ){
				switch(line_level){
					case 0:
					return ANSI_COLOR_GREEN; 
					case 1: 
					return ANSI_COLOR_CYAN; 
					case 2: 
					return ANSI_COLOR_YELLOW; 
					case 3: 
					return ANSI_COLOR_RED; 
					default: 
					return ""; 
				}
			}


		private:
		bool with_color  = true; 
			int line_level = -1; 
			std::string buffer; 
	};

} // namespace klog
