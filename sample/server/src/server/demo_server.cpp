#include "demo_server.h"
#include <iostream>
#include <stdio.h>
/**
* A method definition looks like C code. It has a return type, arguments,
* and optionally a list of exceptions that it may throw. Note that argument
* lists and exception lists are specified using the exact same syntax as
* field lists in struct or exception definitions.
*/
void DemoHandler::ping() {
    // Your implementation goes here
    printf("ping\n");
}

int32_t DemoHandler::add(const int32_t num1, const int32_t num2) {
    // Your implementation goes here
    printf("add\n");
    return num1 + num2;
}

int32_t DemoHandler::calculate(const int32_t logid, const Work& work) {
    // Your implementation goes here
    printf("calculate\n");
    std::cout << "calculate(" << logid << ", " << work << ")" << std::endl;
    int32_t val;

    switch (work.op) {
    case Operation::ADD:
      val = work.num1 + work.num2;
      break;
    case Operation::SUBTRACT:
      val = work.num1 - work.num2;
      break;
    case Operation::MULTIPLY:
      val = work.num1 * work.num2;
      break;
    case Operation::DIVIDE:
      if (work.num2 == 0) {
        InvalidOperation io;
        io.whatOp = work.op;
        io.why = "Cannot divide by 0";
        throw io;
      }
      val = work.num1 / work.num2;
      break;
    default:
      InvalidOperation io;
      io.whatOp = work.op;
      io.why = "Invalid Operation";
      throw io;
    }

    return val;
}

/**
* This method has a oneway modifier. That means the client only makes
* a request and does not listen for any response at all. Oneway methods
* must be void.
*/
void DemoHandler::zip() {
    // Your implementation goes here
    printf("zip\n");
}

void DemoHandler::query(std::string& _return, const std::string& request) {
    // Your implementation goes here
    // Your implementation goes here
    printf("query\n");
	printf("request=%s\n", request.c_str());

	_return = "World";
}
