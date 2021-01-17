#pragma once //


namespace klog {

#ifndef KLOG_SEPRATOR
#define KLOG_SEPRATOR " "
#endif

#ifndef KLOG_MAX_LINE_LENGTH
#define KLOG_MAX_LINE_LENGTH       1024
#endif  

#ifndef KLOG_WITH_COMMA_SUPPORT
#define KLOG_WITH_COMMA_SUPPORT 0
#endif 

#ifndef KLOG_THREAD_SAFE 
#define KLOG_THREAD_SAFE 1 
#endif  

    enum KLogLevel
    {
        KLOG_LEVEL_NULL,
        KLOG_LEVEL_ERROR,
        KLOG_LEVEL_WARN,
        KLOG_LEVEL_DEBUG,
        KLOG_LEVEL_INFO,
    };

    const char* kLogLevelPrefix[] = {
        "",
        "[ERROR]",
        "[WARN]",
        "[DEBUG]",
        "[INFO]"
    };

    inline std::tm  log_time() {
        return fmt::localtime(std::time(nullptr));
    }


    struct  NoneMutex{
		inline void lock(){}
		inline void unlock(){}
	}; 

}