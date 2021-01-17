#pragma once
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <iostream>
#include <errno.h>
#include <vector>
 
#include <thread>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <chrono>
#include <fmt/chrono.h>
 
#include "klog_base.hpp"
#include "sync_queue.hpp"
#include "console_sink.hpp"
#include "log_sink.hpp"
#ifndef KLOG_LEVEL
#define KLOG_LEVEL 5
#endif


namespace klog
{
 


    /*
    inline uint32_t args_length(){ return 0; }
    template <class T, class ... Args> 
        uint32_t args_length( T, Args ... args) {
            return sizeof(T) + args_length( args...); 
        }
        */

    inline void format_log_postfix(fmt::memory_buffer &buf) {}

    template <class P, class... Args>
        void format_log_postfix(fmt::memory_buffer &buf, P first, Args... rest)
        {
            fmt::format_to(buf, KLOG_SEPRATOR "{{}}");
            format_log_postfix(buf, rest...);
        }

    template <class P, class... Args>
        void format_log_prefix(const std::string & prefix, fmt::memory_buffer &buf, P first, Args... rest)
        {
            fmt::format_to(buf, prefix+ "{{}}");
            format_log_postfix(buf, rest...);
        }

   
        class KLog
        {
            typedef std::basic_ostream<char, std::char_traits<char>> CoutType;
            // this is the function signature of std::endl
            typedef CoutType &(*StandardEndLine)(CoutType &);

            class FlowHelper{
                public:
                    FlowHelper( const FlowHelper& other)  = delete ;

                    const FlowHelper& operator=( const FlowHelper& other)  = delete;

                    FlowHelper(KLog *log = nullptr):logger(log){   }

                    FlowHelper(FlowHelper&& other ){
                        this->logger = other.logger;
                        other.logger = nullptr;
                    }
                    FlowHelper& operator=(FlowHelper&& other) {
                        logger = other.logger;
                        other.logger = nullptr;
                        return *this;
                    }

                    FlowHelper &operator<<(StandardEndLine manip) {
                        if (logger){
                            has_end = true;
                            logger->write_sinks(logger->line_level, fmt::to_string(logger->buffer)); 
                            logger->flush();
                        }
                        return *this;
                    }

                    template <class T>
                        FlowHelper &  operator<<(const T &log) {
                            if (logger){
                                fmt::format_to(logger->buffer, " {}", log);
                            }
                            return *this;
                        }

                    ~FlowHelper() {
                        if (logger  && !has_end) {
                            logger->write_sinks(logger->line_level, fmt::to_string(logger->buffer)); 
                            logger->flush();
                        }
                    }
                    bool has_end = false;
                    KLog *logger = nullptr;
            };

            class EmptyFlow{
                public:
                    template <class T>
                        inline	EmptyFlow &  operator<<(const T &log) { return *this; }
            };


            public:
            KLog(const std::string & logName = "default"):dout(*this),iout(*this),wout(*this),eout(*this){
                log_name  = logName; 
            }

             struct NullLogger{
                NullLogger(KLog & log):logger(log) { }
                template <class T>
                    FlowHelper operator<<(const T &log) {
                        return logger.null_logger() << log ; 
                    }

                KLog & logger; 
            };

            struct DebugLogger{
                DebugLogger(KLog & log):logger(log) { }
                template <class T>
                    FlowHelper operator<<(const T &log) {
#if KLOG_LEVEL >= KLOG_LEVEL_DEBUG   
                        return logger.debug_logger() << log ; 
#else 
                        return logger.null_logger() << log ; 
#endif 
                    }

                KLog & logger; 
            };

            struct InfoLogger{
                InfoLogger(KLog & log):logger(log) { }
                template <class T>
                    FlowHelper operator<<(const T &log) {
                        return logger.info_logger() << log ; 
                    }

                KLog & logger; 
            };
            struct WarnLogger{
                WarnLogger(KLog & log):logger(log) { }
                template <class T>
                    FlowHelper operator<<(const T &log) {
                        return logger.warn_logger() << log ; 
                    }

                KLog & logger; 
            };

            struct ErrorLogger{
                ErrorLogger(KLog & log):logger(log) { }
                template <class T>
                    FlowHelper operator<<(const T &log) {
                        return logger.error_logger() << log ; 
                    }

                KLog & logger; 
            };


            DebugLogger dout; 
            InfoLogger iout; 
            WarnLogger wout; 
            ErrorLogger eout; 
            ~KLog()
            {
                // if (buffer.size() > 0) {
                // 	fmt::print(  fmt::to_string(buffer) );
                // }
            }
            const std::string & get_name() const {
                return log_name; 
            }

            void write_sinks(int level , const std::string & log ){
                for(auto & sink :log_sinks){
                    sink->write(level, log);
                }
                buffer.clear(); 
            }

            public:
            void init_async(bool async = false){

                if (async && sink_thread.joinable())
                {
                    sink_thread = std::thread([this](){
                            log_queue.process([this](const std::string &log){
                                    this->write_sinks(line_level, log);
                                    return true;
                                    });
                            });
                }
            }

            void add_console(){                                                                                                
                add_sink(std::make_shared<ConsoleSink>());                                                                     
            }          

            void add_sink(LogSinkPtr sink)
            {
                log_sinks.push_back(sink);
            }

            template <class T, class ... Args> void add_sink( Args &&... args){
                log_sinks.push_back(std::make_shared<T>(args ... ));
            }

            KLog & operator<<(StandardEndLine manip) {
                flush();
                return *this;
            }


            //	FlowHelper &operator<<(StandardEndLine manip)
            //	{
            //	    flush();
            //	    return std::move(FlowHelper(this);
            //	}
            //		template <class T>
            //			KLog & operator<<(const T &log)
            //			{
            //				fmt::format_to(buffer, "{}", log);
            //				if (buffer.size() > 1024) {
            //					flush();
            //				}
            //				//return std::move(FlowHelper(this));
            //				return  *this;
            //			}
            //

            template <class T>
                FlowHelper operator<<(const T &log) {
                    
                    fmt::format_to(buffer,  "{}",  log);
                    if (buffer.size() > KLOG_MAX_LINE_LENGTH) {
                        flush();
                    }
                    return FlowHelper(this);
                }

            inline EmptyFlow & null_logger(){
                static EmptyFlow  empty_flow;
                return empty_flow;
            }

            
            inline KLog & debug_logger(){
                line_level = KLOG_LEVEL_DEBUG; 
                fmt::format_to(buffer, kLogLevelPrefix[line_level]);
                return *this;
            }

            inline KLog & info_logger(){
                line_level = KLOG_LEVEL_INFO; 
                fmt::format_to(buffer, kLogLevelPrefix[line_level]);
                return *this;
            }

            inline KLog & warn_logger(){
                line_level = KLOG_LEVEL_WARN; 
                fmt::format_to(buffer, kLogLevelPrefix[line_level]);
                return *this;
            }

            inline KLog & error_logger(){
                line_level  = KLOG_LEVEL_ERROR;
                fmt::format_to(buffer, kLogLevelPrefix[line_level]);
                return *this;
            }

            void flush() {
                for(auto & sink :log_sinks){
                    sink->flush(); 
                }
                buffer.clear();
            }


            template <class... Args>
                KLog &debug_put(Args... args)
                {
                    if (level >= 3)
                    {
                        fmt::memory_buffer fmtBuf; 
                        fmtBuf.reserve(256 ); 
                        format_log_prefix("[DEBUG]", fmtBuf, args...);               
                        fmt::format_to(buffer, fmt::to_string(fmtBuf),   args...); 
                        this->write(KLOG_LEVEL_DEBUG, fmt::to_string(buffer));
                        this->flush(); 
                        buffer.clear();
                    }
                    return *this;
                }


            static void dump_hex(const char *title, const char *buf, size_t bufLen, uint32_t line = 8)
            {
                fprintf(stdout, "%s length %zd\n", title, bufLen);
                for (uint32_t i = 0; i < bufLen; ++i)
                {
                    fprintf(stdout, "%02X%s", (unsigned char)buf[i], (i + 1) % line == 0 ? "\n" : " ");
                }
                fprintf(stdout, "\n");
            }

            template <class... Args>
                KLog &dlog(const std::string &fmt, Args... args) {
                    return debug_format( fmt, args ...); 
                }

            template <class... Args>
                KLog &debug_format(const std::string &fmt, Args... args)
                {
                    if (level >= 3)
                    {

                        fmt::format_to(buffer, "[DEBUG] " + fmt , args... ); 
                        this->write(KLOG_LEVEL_DEBUG, fmt::to_string(buffer));
                        this->flush();  
                    }
                    return *this;
                }


            void write(int32_t level, const std::string &msg)
            {
                for (auto &sink : log_sinks)
                {
                    sink->write(level, msg);
                }
            }

            template <class... Args>
                KLog &info_put(Args... args)
                {
                    if (level >= 2)
                    {
                        fmt::memory_buffer fmtBuf;
                        format_log_prefix("[INFO]", fmtBuf, args...);

                        fmt::format_to(buffer, fmt::to_string(fmtBuf), args...); 
                        this->write(KLOG_LEVEL_INFO, fmt::to_string(buffer));
                        this->flush(); 


                        buffer.clear();
                    }
                    return *this;
                }


            template <class... Args>
                KLog &ilog(const std::string &fmt, Args... args){
                    return info_format(fmt, args ...); 
                }

            template <class... Args>
                KLog &info_format(const std::string &fmt, Args... args)
                {
                    if (level >= 2)
                    {
                        fmt::format_to(buffer, "[INFO] " + fmt  , args... ); 
                        this->write(KLOG_LEVEL_INFO, fmt::to_string(buffer));
                        this->flush(); 
                    }
                    return *this;
                }

            template <class... Args>
                KLog &warn_put(Args... args)
                {
                    if (level >= 1)
                    {
                        fmt::memory_buffer fmtBuf; 
                        fmtBuf.reserve(256 ); 
                        format_log_prefix("[WARN]", fmtBuf, args...);

                        fmt::format_to(buffer, fmt::to_string(fmtBuf), args...); 
                        this->write(KLOG_LEVEL_WARN, fmt::to_string(buffer));
                        this->flush(); 
                        buffer.clear();

                    }
                    return *this;
                }

            template <class... Args>
                KLog &wlog(const std::string &fmt, Args... args){
                    return warn_format(fmt, args ...); 
                }

            template <class... Args>
                KLog &warn_format(const std::string &fmt, Args... args)
                {
                    if (level >= 1)
                    {
                        fmt::format_to(buffer, "[WARN] " + fmt , args... ); 
                        this->write(KLOG_LEVEL_WARN, fmt::to_string(buffer));
                        this->flush(); 
                    }
                    return *this;
                }

            template <class... Args>
                KLog &error_put(Args... args)
                {
                    if (level >= 0)
                    { 

                        fmt::memory_buffer fmtBuf;  
                        format_log_prefix("[ERROR]", fmtBuf, args...);

                        fmt::format_to(buffer, fmt::to_string(fmtBuf), args...); 
                        this->write(KLOG_LEVEL_ERROR, fmt::to_string(buffer));
                        this->flush(); 
                        buffer.clear();

                    }
                    return *this;
                }

            template <class... Args>
                KLog &elog(const std::string &fmt, Args... args){
                    return error_format(fmt, args ...); 
                }
            
            template <class... Args>
                KLog &error_format(const std::string &fmt, Args... args)
                {
                    if (level >= 0)
                    {
                        fmt::format_to(buffer, "[ERROR] " + fmt , args... ); 
                        this->write(KLOG_LEVEL_ERROR, fmt::to_string(buffer));
                        this->flush(); 
                    }
                    return *this;
                }

#if WITH_COMMA_SUPPORT
            template <class T>
                KLog &operator,(const T &val)
                {
                    fmt::format_to(buffer, "{}", val);
                    return *this;
                }

            KLog &operator,(StandardEndLine val)
            {
                fmt::format_to(buffer, "\n");
                return *this;
            }
#endif

            static KLog &instance()
            {
#ifdef KLOG_THREAD_SAFE  
                thread_local static KLog * _instance; 
                thread_local static std::once_flag initFlag;
#else 
                static KLog *_instance;
                static std::once_flag initFlag;
#endif 
                std::call_once(initFlag, [&] { _instance = new KLog(); });
                return *_instance;
            }

            inline void set_level(uint32_t lv) { level = lv; }

            private:
            fmt::memory_buffer buffer;
            std::vector<LogSinkPtr> log_sinks;
            uint32_t level = KLOG_LEVEL;
            uint32_t line_level = KLOG_LEVEL; 
            std::thread sink_thread;
            SyncQueue<std::string>  log_queue;
            
            std::string log_name = "default"; 
        };

    template <class... Args>
        inline std::string _fmt(Args... args)
        {
            return std::move(fmt::format(args...));
        }
} // namespace klog

#define kLogIns  klog::KLog::instance()

 
#if KLOG_LEVEL >= KLOG_LEVEL_DEBUG
template <class... Args>
inline klog::KLog &dput(Args... args){
    return klog::KLog::instance().debug_put(  args ...); 
}
#else 
template <class... Args>
inline klog::KLog &dput(Args... args){
    return klog::KLog::instance(); 
}
#endif 

#if KLOG_LEVEL >= KLOG_LEVEL_INFO
template <class... Args>
inline klog::KLog &iput(Args... args){
    return klog::KLog::instance().info_put(  args ...); 
}
#else 
template <class... Args>
inline klog::KLog &iput(Args... args){
    return klog::KLog::instance(); 
}
#endif  


#if KLOG_LEVEL >= KLOG_LEVEL_WARN
template <class... Args>
inline klog::KLog &wput(Args... args){
    return klog::KLog::instance().warn_put(  args ...); 
}
#else 
template <class... Args>
inline klog::KLog &wput(Args... args){
    return klog::KLog::instance(); 
}
#endif 


#if KLOG_LEVEL >= KLOG_LEVEL_ERROR
template <class... Args>
inline klog::KLog &eput(Args... args){
    return klog::KLog::instance().error_put(  args ...); 
}
#else 
template <class... Args>
inline klog::KLog &eput(Args... args){
    return klog::KLog::instance(); 
}
#endif 


#if KLOG_LEVEL >= KLOG_LEVEL_DEBUG
template <class... Args>
inline klog::KLog &dlog(const std::string &fmt, Args... args) {
    return klog::KLog::instance().debug_format( fmt, args ...); 
}
#else 

template <class... Args>
inline klog::KLog &dlog(const std::string &fmt, Args... args) {
    return klog::KLog::instance(); 
}
#endif  


#if KLOG_LEVEL >= KLOG_LEVEL_INFO
template <class... Args>
inline klog::KLog &ilog(const std::string &fmt, Args... args) {
    return klog::KLog::instance().info_format( fmt, args ...); 
}
#else 
template <class... Args>
inline klog::KLog &ilog(const std::string &fmt, Args... args) {
    return klog::KLog::instance(); 
}
#endif  


#if KLOG_LEVEL >= KLOG_LEVEL_WARN
template <class... Args>
inline klog::KLog &wlog(const std::string &fmt, Args... args) {
    return klog::KLog::instance().warn_format( fmt, args ...); 
}
#else 
template <class... Args>
inline klog::KLog &wlog(const std::string &fmt, Args... args) {
    return klog::KLog::instance(); 
}
#endif  



#if KLOG_LEVEL >= KLOG_LEVEL_ERROR

template <class... Args>
inline klog::KLog &elog(const std::string &fmt, Args... args) {
    return klog::KLog::instance().error_format( fmt, args ...); 
}
#else 
template <class... Args>
inline klog::KLog &elog(const std::string &fmt, Args... args) {
    return klog::KLog::instance(); 
}
#endif  
 
#if KLOG_LEVEL >= KLOG_LEVEL_DEBUG
static klog::KLog::DebugLogger dout(klog::KLog::instance()); 
#else //
static klog::KLog::NullLogger dout(klog::KLog::instance()); 
#endif 

#if KLOG_LEVEL >= KLOG_LEVEL_INFO
static klog::KLog::InfoLogger iout(klog::KLog::instance()); 
#else 
static klog::KLog::NullLogger iout(klog::KLog::instance()); 
#endif 

#if KLOG_LEVEL >= KLOG_LEVEL_WARN
static klog::KLog::WarnLogger wout(klog::KLog::instance()); 
#else 
static klog::KLog::NullLogger wout(klog::KLog::instance()); 
#endif 

#if KLOG_LEVEL >= KLOG_LEVEL_ERROR
static klog::KLog::ErrorLogger eout(klog::KLog::instance()); 
#else 
static klog::KLog::NullLogger eout(klog::KLog::instance()); 
#endif 
 
 
