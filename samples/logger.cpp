

#include "klog.hpp"
#include "file_sink.hpp"
#include "console_sink.hpp"
#include <chrono>



using namespace klog;


int main(int argc, char **argv)
{

    kLogIns.init_async(true);
    kLogIns.add_sink<FileSink<> >("./klog.log");
    kLogIns.add_sink<ConsoleSink<std::mutex, true>  >();
    uint32_t index =0; 

    KLog<> mylog; 
    mylog.add_sink<ConsoleSink<NoneMutex, true>>(); 

	//dlog("start to log test {}", time(0) );
    while(true){

        mylog.dout << "dout log from my debug out " << index ; 
        mylog.dput("hello", 2021, "from dput"); 

        dout <<"dout log from xout " << index ;
        iout <<"iout log from xout " << index ;
        //mylog.dout() << "dout from my log xout" << index ; 
//        iout <<" iout log from xout " << index ;
//        wout <<" wout log from xout " << index ;
//        eout <<" eout log from xout " << index ; 
        //dlog("debug log from dlog"); 
        //elog("debug log from elog"); 
        //        dlog("debug log from xlog {}  ", index ); 
        //        ilog("info log from xlog  {}  ", index ); 
        //        wlog("warn log from xlog  {}  ", index ); 
        //        elog("error log from xlog {}  ", index ); 


   
        /*
        dput("dput log from xput", index); 
        iput("iput log from xput", index); 
        wput("wput log from xput", index); 
        eput("eput log from xput", index); 

        */

        index ++; 
        std::this_thread::sleep_for(std::chrono::seconds(1)); 

     //   mylog.dlog("debug log from mylog"); 


    }


    return 0;

}
