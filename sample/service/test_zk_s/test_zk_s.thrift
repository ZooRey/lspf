namespace cpp payment_channel

//bool：布尔类型(true or value)，占一个字节
 
//byte：有符号字节
 
typedef i16 int16 //i16:16位有符号整型
 
typedef i32 int32 //i32:32位有符号整型
 
typedef i64 int64 //i64:64位有符号整型
 
//double：64位浮点数
 
//string：未知编码或者二进制的字符串

service CupsChannel{
   //消费
   string DoTrans(1:string logid, 2:string request)
}

//thrift -r -out gen-server --gen cpp test_zk_s.thrift