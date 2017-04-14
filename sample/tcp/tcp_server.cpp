      // ����ĳfdΪO_NONBLOCKģʽ  
      int set_non_block(int fd);  
  
      // server��socket����:socket(),setsockopt(),bind(),listen(),set_non_block()������server_fd  
      int setup_tcp_server(int port);  
  
      // client��socket����:socket(),connect()���������ӵ�sockfd  
      int create_io_channel(const char *ipaddr, int port);  
  
1. �TCP Server  
      ������α���뷽ʽ������������ʡ��  
  
      int main(int argc, char *argv[])  
      {  
          // ��ʼ��  
          ��  
  
          // event��ʼ��  
          event_init();  
          init_server(port, params��);  
          event_dispatch();  
  
          return 0;  
      }  
  
      int init_server(short port, params��)  
      {  
          int listen_fd = setup_tcp_server(port);  
          set_non_block(listen_fd);  
  
          // ������Ĳ���params�� ��֯Ϊһ���ṹ����ָ��ķ�ʽ����accept_param  
          struct event* ev_accept = (struct event*)malloc(sizeof(struct event));  
          event_set(ev_accept, listen_fd, EV_READ|EV_PERSIST, on_accept, (void*)accept_param);  
          event_add(ev_accept, NULL);  
  
          return 0;  
      }  
  
      void on_accept(int fd, short ev, void *arg)  
      {  
          int client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_len);  
          set_non_block(client_fd);  
  
          // Disable the Nagle (TCP No Delay) algorithm  
          int flag = 1;  
          int ret = setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));  
  
          // ����buffer event  
          // ����client_t��һ������Ϊstruct bufferevent*����buf_ev  
          //     total_len: ָʾ�����ܳ���  
          //     cur_size:  ָʾ��ǰ�Ѿ����յ����󳤶�  
          //     data:      �������ݱ���  
          client_t *client = (client_t*)malloc(sizeof(client_t));  
          client->buf_ev = bufferevent_new(client_fd, buffer_on_read, buffer_on_write,buffer_on_error, client);  
          client->total_len = 0;  
          client->cur_size = 0;  
          client->data = NULL;  
          bufferevent_enable(client->buf_ev, EV_READ|EV_WRITE);  
      }  
  
      void buffer_on_read(struct bufferevent *ev_buf, void *opqaue)  
      {  
          // �����󲢴���  
  
          client_t *client = (client_t*)opaque;  
          size_t rdsz;  
          int sz;  
  
          if (client->total_len <= client->cur_size) {  
              // ��������  
  
              // �����������ܳ���  
              rdsz = bufferevent_read(ev_buf, &sz, sizeof(int));  
              client->total_len = sz;  
  
              // ��ʼ������������(һ�β�һ���ܶ���!)  
              char *data = (char*)malloc(sz);  
              rdsz = bufferevent_read(ev_buf, data, sz);  
              client->cur_size = (int)rdsz;  
              client->data = data;  
          } else {  
              // ������������  
              rdsz = bufferevent_read(ev_buf, client->data+client->cur_size, client->total_len-client->cur_size);  
              client->cur_size += (int)rdsz;  
          }  
  
          if (client->cur_size >= client->total_len) {  
              // �����(������)����  
              request_t  req;   // ��������ݽṹ(ͨ��protobuf����)  
              response_t res;   // ��Ӧ�����ݽṹ(ͨ��protobuf����)  
  
              // ���ô�������request���д���,���ѽ��д��response��  
              req.ParseFromArray((const void*)client->data, client->total_len);  
              process_func(req, res);  
                
              // д��Ӧ  
              string output;  
              res.SerializeToString(&output);  
              int status = bufferevent_write(ev_buf, output.c_str(), output.length());  
          }  
      }  
  
      void buffer_on_write(struct bufferevent *ev_buf, void *opqaue)  
      {  
          // ��������  
  
          client_t *client = (client_t*)opaque;  
          if (client->data && strlen(client->data) != 0)  
              free(client->data);  
          client->data = NULL;  
          client->total_len = 0;  
          client->cur_size = 0;  
      }  
  
      void buffer_on_error(struct bufferevent *ev_buf, short what, void *opqaue)  
      {  
          // ����������Ϣ  
          division_client *client = (division_client*)opaque;  
          struct sockaddr_in client_addr;  
          socklen_t len = sizeof(client_addr);  
          if (getpeername(client->sock_fd, (struct sockaddr*)&client_addr, &len) == 0)  
              LOG_INFO(��Client(%s:%u) connection closed or Error occured �� %d\n��,   
                      inet_ntoa(client_addr.sin_addr),   
                      ntohs(client_addr.sin_port),  
                      what);  
          else  
              LOG_INFO(��Client(unknown) connection closed or Error occured �� %d\n��, what);  
            
          if (client) {  
              if (client->data)  
                  free(client->data);  
              free(client);  
          }  
      }  