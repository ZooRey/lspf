#产生同步调用代码
thrift --gen cpp -r threads.thrift 
#产生异步调用代码
thrift --gen cpp:cob_style -r cob_threads.thrift 
