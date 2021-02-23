#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "log_sink.hpp"

namespace klog
{
	template <class Mutex  = NoneMutex   > 
    class FileSink : public LogSink
    {

        public:
            explicit FileSink(const std::string &filePath) : logfile(filePath, std::ofstream::app) { }
			struct RotatingRule{
    		}; 

            virtual ~FileSink(){
                if (logfile.is_open()) {
                    logfile.close();
                }
            }

            virtual void flush(const std::string & log = "")
			{

                buffer.append(log);
                if (logfile.is_open()){
					logfile << buffer << "\n"; 	
                    logfile.flush();                    
                }
                buffer.clear();
      
            }

            virtual int32_t write(int level , const std::string &msg) {

                if (logfile.is_open())
                {
                    buffer.append(msg);
                }
                return 0;
            }

        private:
            std::ofstream logfile;
            Mutex log_mutex; 
            static thread_local std::string buffer;
    };

template <class Mutex >
	thread_local std::string FileSink<Mutex >::buffer;


} // namespace klog
