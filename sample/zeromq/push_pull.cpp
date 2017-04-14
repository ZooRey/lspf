#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "zmq.h"
#include <boost/thread/thread.hpp>

void *m_z_ctx = zmq_ctx_new ();

std::vector<zmq_pollitem_t> m_poll_items;

int Push() {

    int rc;

    void *push = zmq_socket(m_z_ctx, ZMQ_PUSH);

    rc = zmq_connect(push, "inproc://a");

    if (rc != 0) {
        zmq_close(push);
        return rc;
    }

	char *msg = "push message";
	int msg_len = strlen(msg);
	
    zmq_msg_t empty;
    rc = zmq_msg_init(&empty);
    if (0 != rc) {
        return rc;
    }

    rc = zmq_sendmsg(push, &empty, ZMQ_SNDMORE | ZMQ_DONTWAIT);
    if (rc < 0) {
        return rc;
    }

    rc = zmq_send(push, msg, msg_len, ZMQ_DONTWAIT);
    if (rc < 0) {
        return rc;
    }
	
	zmq_close(push);
	
    return 0;
}
void *m_c_pull_socket ;
int Pull() {

    int rc;

    m_c_pull_socket = zmq_socket(m_z_ctx, ZMQ_PULL);

    rc = zmq_bind(m_c_pull_socket, "inproc://a");

    if (rc != 0) {
        zmq_close(m_c_pull_socket);
        return rc;
    }

    zmq_pollitem_t poll_item;
    poll_item.socket = m_c_pull_socket;
    poll_item.events = ZMQ_POLLIN;
    poll_item.fd = 0;
    m_poll_items.push_back(poll_item);

    return 0;
}

void thread_send()
{
	for (int i=0; i<100000; i++){
		Push();
		usleep(1);
	}
	
}

int main(){
	int rc;
	Pull();
	
    boost::thread_group group;

    for(int num=0;num<10;num++)
		group.create_thread(boost::bind(&thread_send));
	
	int count = 0;
	while(1)
	{
		rc = zmq_poll(m_poll_items.data(), m_poll_items.size(), 0);

		if (rc <= 0) {
			continue;
		}
		zmq_msg_t message;
		rc = zmq_msg_init(&message);
		if (rc != 0) {
			printf("zmq_msg_init failed\n");
			return 0;
		}

		for (uint32_t i = 0; i < m_poll_items.size(); i++) {
			if (m_poll_items[i].revents > 0) {
				while(1){
					if (m_poll_items[i].socket == m_c_pull_socket){
						void *socket = m_poll_items[i].socket;
						rc = zmq_msg_recv(&message, socket, ZMQ_DONTWAIT);
						if (rc <= 0){
							break;
						}
						if (zmq_msg_more(&message)){
							rc = zmq_msg_recv(&message, socket, ZMQ_DONTWAIT);
							printf("zmq_msg_more:%d, %d: %s\n", ++count, zmq_msg_size(&message),reinterpret_cast<char*>(zmq_msg_data(&message)));
						}else{
							printf("pull recv:%d, %d: %s\n", ++count, zmq_msg_size(&message),reinterpret_cast<char*>(zmq_msg_data(&message)));
						}
					}else{
						printf("it is not m_c_pull_socket\n");
						break;
					}
				}					
			}

		}
	}
}