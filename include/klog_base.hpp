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

//#ifndef KLOG_THREAD_SAFE 
//#define KLOG_THREAD_SAFE 1 
//#endif  

    enum KLogLevel
    {
        KLOG_LEVEL_NULL,
        KLOG_LEVEL_ERROR,
        KLOG_LEVEL_WARN,
        KLOG_LEVEL_DEBUG,
        KLOG_LEVEL_INFO,
    };

    static const char* kLogLevelPrefix[] = {
        "",
        "[ERROR]",
        "[WARN]",
        "[DEBUG]",
        "[INFO]"
    };

    inline std::tm  log_time() {
        return fmt::localtime(std::time(nullptr));
    }


    struct  NoneMutex {
        inline void lock() {}
        inline void unlock() {}
    };

    enum KLogPrefixType {
        KLOG_PREFIX_NONE,
        KLOG_PREFIX_TIME,
        KLOG_PREFIX_DATETIME,

    };

    template <int  >
    struct KLogPrefix {
        inline const char* operator()(KLogLevel lv) {
            return  kLogLevelPrefix[lv];
        }
    };

    template <  >
    struct KLogPrefix<KLOG_PREFIX_NONE> {
        inline const char* operator()(KLogLevel lv) {
            return "";
        }
    };
    template <  >
    struct KLogPrefix<KLOG_PREFIX_TIME> {

        inline const char* operator()(KLogLevel lv) {
            std::tm curTime = log_time();
            int ret = sprintf(szBuf,  "[%02d/%02d/%02d %02d:%02d:%02d] %s",curTime.tm_year %100, curTime.tm_mon +1, curTime.tm_mday, curTime.tm_hour, curTime.tm_min, curTime.tm_sec, kLogLevelPrefix[lv] );
            szBuf[ret] = '\0';
            return szBuf;
        }

        char szBuf[64];
    };


}
