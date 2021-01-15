#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "log_sink.hpp"

namespace klog
{

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
                if (logfile.is_open()){
					logfile << log << "\n"; 	
                    logfile.flush();
                }
            }

            virtual int32_t write(int level , const std::string &msg) {

                if (logfile.is_open())
                {
                    logfile << msg;
                }
                return 0;
            }

        private:
            std::ofstream logfile;
    };

} // namespace klog
