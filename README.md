# klog
tiny easy log lib wrapper

### Usage  : 

### Provide three different apis 


```cpp
    //xput api
    dput("hello " , 2021 ); // no format ,put all in parameters 
    
    //xlog api 
    dlog("hello {}", 2021); // python like format use {} as format

    //xout api  
    dout <<  "hello"  << 2021 ; // stream output 

```