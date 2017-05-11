#include <stdlib.h>
#include <stdio.h>
#include <zmq.h>
#include <iostream>
using namespace std;

int main(int argc, char * argv[])
{
	void * ctx = zmq_ctx_new();
	// create req socket, act as client
	void * req_sock = zmq_socket(ctx, ZMQ_REQ);
	char ip[16] = "127.0.0.1";
	unsigned int port = 5600;
	char buf[256] = {0};
	sprintf(buf,"tcp://%s:%u",ip,port);
	int rc = zmq_connect(req_sock, buf);
	if (rc == -1)
	{
		printf("error, zmq connect failed.\n");
		return 0;
	}
	zmq_sleep(3);

	while(1)
	{
		//send message from req_socket
		char content[256] = "";
		cin >> content;
		
		zmq_sleep(3);
		zmq_msg_t msg;
		zmq_msg_init_size(&msg,sizeof(content));
		memcpy(zmq_msg_data(&msg),content,sizeof(content));
		rc = zmq_msg_send(&msg,req_sock,0);
		if(rc == -1)
		{
			printf("error, zmq send failed.\n");
			return 0;
		}
	    zmq_msg_close(&msg);
		//printf("send massage succeed.\n");

		zmq_sleep(3);

		//receive reply from rep socket
		zmq_msg_t msg3;
		zmq_msg_init(&msg3);
		rc = zmq_msg_recv(&msg3,req_sock,0);
		if(rc != 128)
		{
			printf("error,zmq recv failed.\n");
			return 0;
		}
		char * content3 = (char*)zmq_msg_data(&msg3);
		printf("Results :%s.\n",content3);
		zmq_msg_close(&msg3);
	}
	zmq_close(req_sock);
	zmq_ctx_term(ctx);

	return 0;
}
