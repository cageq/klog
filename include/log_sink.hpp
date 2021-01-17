#pragma once
#include <string>
#include <memory>
#include "klog_base.hpp"

namespace klog
{
    class LogSink
    {
        public:
            virtual int32_t write(int level , const std::string &)
            { 
                return 0;
            }
            virtual void flush(const std::string & log = ""){} 
    };

    using LogSinkPtr = std::shared_ptr<LogSink>;

} // namespace klog
