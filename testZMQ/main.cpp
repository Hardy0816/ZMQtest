#include <stdlib.h>
#include <stdio.h>
#include <zmq.h>


int main(int argc, char * argv[])
{
	void * ctx = zmq_ctx_new();
	// create req socket, act as client
	void * req_sock = zmq_socket(ctx, ZMQ_REQ);
	// create rep socket, act as server
	void * rep_sock = zmq_socket(ctx, ZMQ_REP);
	
	char ip[16] = "127.0.0.1";
	unsigned int port = 5600;
	
	char buf[256] = {0};
	sprintf(buf, "tcp://%s:%u", ip, port);

	// bind rep socket to the specified ip & port
	int rc = zmq_bind(rep_sock, buf);
	if (rc == -1)
	{
		printf("error, zmq bind failed.\n");
		return 0;
	}
	// connect req socket to the specified ip & port
	rc = zmq_connect(req_sock, buf);
	if (rc == -1)
	{
		printf("error, zmq connect failed.\n");
		return 0;
	}
	// send message to rep
	char content[256] = "hello, this is SunShengSheng from req_socket.";
	zmq_msg_t msg;
	zmq_msg_init_size(&msg, sizeof(content));

	memcpy(zmq_msg_data(&msg), content, sizeof(content));

	rc = zmq_msg_send(&msg, req_sock, 0);
	if (rc == -1)
	{
		printf("error, zmq send failed.\n");
		return 0;
	}
	zmq_msg_close(&msg);
	// recv message from req
	zmq_msg_t msg1;
	zmq_msg_init(&msg1);
	rc = zmq_msg_recv(&msg1, rep_sock, 0);
	if (rc == -1)
	{
		printf("error, zmq recv failed.\n");
		return 0;
	}

	char * content1 = (char*)zmq_msg_data(&msg1);

	printf("received message from req socket, the content is: %s.\n", content1);

	zmq_msg_close(&msg1);
	// reply req socket with new message
	char content2[128] = "hello, this is DuanXuyang from rep_socket.";
	zmq_msg_t msg2;
	zmq_msg_init_size(&msg2, sizeof(content2));

	memcpy(zmq_msg_data(&msg2), content2, sizeof(content2));

	rc = zmq_msg_send(&msg2, rep_sock, 0);
	if (rc == -1)
	{
		printf("error, zmq send failed.\n");
		return 0;
	}
	zmq_msg_close(&msg2);
	// receive reply from rep socket
	zmq_msg_t msg3;
	zmq_msg_init(&msg3);

	rc = zmq_msg_recv(&msg3, req_sock, 0);
	if (rc != 128)
	{
		printf("error, zmq recv failed.\n");
		return 0;
	}
	char * content3 = (char*)zmq_msg_data(&msg3);
	printf("received message from rep socket, the content is: %s.\n", content3);

	zmq_msg_close(&msg3);
	// exit
	zmq_close(req_sock);
	zmq_close(rep_sock);

	zmq_ctx_term(ctx);

	return 0;
}




