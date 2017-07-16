#include "message.h"
#include <limits>
#include <stdio.h>

using namespace std;

namespace lspf {
namespace net{

map<int, SendToFunction> Message::send_to_functions;

int32_t Message::max_handle = 0;

queue<int> Message::free_handles;

map<std::string, MessageDriver*> Message::prefix_to_driver;

map<int, MessageDriver*> Message::handle_to_driver;

std::set<MessageDriver*> Message::all_driver;

std::set<int> Message::all_handles;

char Message::error_msg[256] = "";

const char *Message::p_error_msg = error_msg;

int Message::allocHandle() {
    if (!free_handles.empty()) {
        int handle = free_handles.front();
        free_handles.pop();
        all_handles.insert(handle);
        return handle;
    }

    if (max_handle == numeric_limits<int>::max()) {
        return -1;
    }

    const int ALLOC_BAT_NUM = 10;
    int n = min(ALLOC_BAT_NUM, numeric_limits<int>::max() - max_handle);
    for (int i = 0; i < n; i++) {
        free_handles.push(max_handle + i);
    }
    max_handle += n;
    return allocHandle();
}

void Message::deallocHandle(int handle) {
    free_handles.push(handle);
}

const char *Message::LastErrorMessage() {
    return p_error_msg;
}

int Message::RegisterDriver(MessageDriver * driver) {
    if (driver == NULL) {
        NET_LIB_ERR_RETURN("NULL Pointer");
    }
    if (prefix_to_driver.find(driver->Name()) != prefix_to_driver.end()) {
        NET_LIB_ERR_RETURN("Aready Registered");
    }
    prefix_to_driver[driver->Name()] = driver;
    return 0;
}

MessageDriver * Message::QueryDriver(int handle) {
    std::map<int, MessageDriver*>::iterator it = handle_to_driver.find(handle);
    return it == handle_to_driver.end() ? NULL : it->second;
}

int Message::NewHandle() {
    return allocHandle();
}

int Message::CheckDriver(int handle, MessageDriver * driver){

    map<string, MessageDriver*>::iterator iter = prefix_to_driver.find(driver->Name());
    if (iter == prefix_to_driver.end()) {
        NET_LIB_ERR_RETURN("Driver Not Registered");
    }

    if (handle_to_driver.find(handle) != handle_to_driver.end()) { // used handle
        NET_LIB_ERR_RETURN("Handle Used");
    }

    if (all_driver.find(driver) != all_driver.end()) { // used driver
        NET_LIB_ERR_RETURN("Driver Used");
    }
    else{
        all_driver.insert(driver);
    }

    return 0;
}


int Message::Run(int handle, MessageDriver * driver){
    //CheckDriver
    if (CheckDriver(handle, driver)){
        return -1;
    }

    SendToFunction send_to;
    int ret = driver->Run(handle, &send_to);

    if (0 != ret) {
        return ret;
    }

    handle_to_driver[handle] = driver;

    if (send_to) {
        send_to_functions[handle] = send_to;
    }

    return 0;
}


int Message::SendTo(int handle, const char* msg, size_t msg_len, boost::shared_ptr<MessageReply> peer_addr) {
    if (UNLIKELY(msg == NULL)) {
        NET_LIB_ERR_RETURN("NULL Pointer");
    }
    map<int, SendToFunction>::iterator iter = send_to_functions.find(handle);

    if (send_to_functions.end() == iter) {
        NET_LIB_ERR_RETURN("Handle Unbind Or Invalid");
    }

    try {
        return iter->second(msg, msg_len, peer_addr);
    } catch (boost::bad_function_call ) {
        NET_LIB_ERR_RETURN("Bad Function Call");
    }
}

int Message::sendToWrap(const SendFunction &send, const char* msg,
        size_t msg_len, boost::shared_ptr<MessageReply> peer_addr, int flag) {
    return send(msg, msg_len);
}

int Message::Send(int handle, const char* msg, size_t msg_len) {
    if (UNLIKELY(msg == NULL)) {
        NET_LIB_ERR_RETURN("NULL Pointer");
    }
    map<int, SendToFunction>::iterator iter = send_to_functions.find(handle);

    if (send_to_functions.end() == iter) {
        NET_LIB_ERR_RETURN("Handle Unbind Or Invalid");
    }

    try {
        return iter->second(msg, msg_len, boost::shared_ptr<MessageReply>());
    } catch (boost::bad_function_call ) {
        NET_LIB_ERR_RETURN("Bad Function Call");
    }
}

int Message::Start(int handle) {
    if (all_handles.find(handle) == all_handles.end()) { // invalid handle
        NET_LIB_ERR_RETURN("Handle Invalid");
    }

    map<int, MessageDriver*>::iterator iter = handle_to_driver.find(handle);

    if (iter == handle_to_driver.end()) { // unused handle
        all_handles.erase(handle);
        deallocHandle(handle);
        return 0;
    }

    int ret = iter->second->Start();

    if (0 != ret) {
        return ret;
    }

    return 0;
}

int Message::Stop(int handle) {
    if (all_handles.find(handle) == all_handles.end()) { // invalid handle
        NET_LIB_ERR_RETURN("Handle Invalid");
    }

    map<int, MessageDriver*>::iterator iter = handle_to_driver.find(handle);

    if (iter == handle_to_driver.end()) { // unused handle
        all_handles.erase(handle);
        deallocHandle(handle);
        return 0;
    }

    int ret = iter->second->Stop();

    if (0 != ret) {
        return ret;
    }

    return 0;
}

int Message::Close(int handle) {
    if (all_handles.find(handle) == all_handles.end()) { // invalid handle
        NET_LIB_ERR_RETURN("Handle Invalid");
    }

    map<int, MessageDriver*>::iterator iter = handle_to_driver.find(handle);

    if (iter == handle_to_driver.end()) { // unused handle
        all_handles.erase(handle);
        deallocHandle(handle);
        return 0;
    }

    int ret = iter->second->Close();

    if (0 != ret) {
        return ret;
    }

    send_to_functions.erase(handle);
    handle_to_driver.erase(handle);
    all_handles.erase(handle);
    deallocHandle(handle);

    return 0;
}
} /* namespace lspf */
} /* namespace net */

