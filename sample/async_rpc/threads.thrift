namespace cpp threads_thrift
namespace d threads_thrift
namespace java threads_thrift
namespace php threads_thrift
namespace perl threads_thrift
namespace haxe threads_thrift

//bool：布尔类型(true or value)，占一个字节
 
//byte：有符号字节
 
typedef i16 int16 //i16:16位有符号整型
 
typedef i32 int32 //i32:32位有符号整型
 
typedef i64 int64 //i64:64位有符号整型
 
//double：64位浮点数
 
//string：未知编码或者二进制的字符串

enum Operation {
  ADD = 1,
  SUBTRACT = 2,
  MULTIPLY = 3,
  DIVIDE = 4
}

struct Work {
  1: i32 num1 = 0,
  2: i32 num2,
  3: Operation op,
  4: optional string comment,
}

/**
 * Structs can also be exceptions, if they are nasty.
 */
exception InvalidOperation {
  1: i32 whatOp,
  2: string why
}

service Calculator{

  /**
   * A method definition looks like C code. It has a return type, arguments,
   * and optionally a list of exceptions that it may throw. Note that argument
   * lists and exception lists are specified using the exact same syntax as
   * field lists in struct or exception definitions.
   */

   void ping(),

   i32 add(1:i32 num1, 2:i32 num2),

   i32 calculate(1:i32 logid, 2:Work w) throws (1:InvalidOperation ouch),

   /**
    * This method has a oneway modifier. That means the client only makes
    * a request and does not listen for any response at all. Oneway methods
    * must be void.
    */
   oneway void zip()

}

