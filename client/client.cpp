#include <stdlib.h>
#include <stdio.h>
#include <zmq.h>
#include <iostream>
#include <windows.h>
#include "..\server\msgid.h"
#include "..\server\defines.h"

using namespace std;

volatile bool g_stop = false;


DWORD WINAPI ThreadProc(void * req_sock)
{
	zmq_msg_t msg;
	zmq_msg_init(&msg);
    while(!g_stop)
	{
		//receive reply from rep sockett		
		int rc = zmq_msg_recv(&msg,req_sock,0);
		if(rc == -1)
		{// something wrong
			printf("%s\n", zmq_strerror(zmq_errno()));
			break;
		}
		int size = zmq_msg_size(&msg);
		if (size < sizeof(SERVER_HEADER))
		{
			printf("error: incorrect size of data received!\n");
			continue;
		}
		SERVER_HEADER * header = (SERVER_HEADER*)zmq_msg_data(&msg);
		char * data = NULL;
		if (size > sizeof(SERVER_HEADER)) // 判断size大于sizeof(SERVER_HEADER), 说明有具体内容
		{
			data = (char*)(zmq_msg_data(&msg)) + sizeof(SERVER_HEADER);
			printf("%s\n", data);
		}
		
	}
	zmq_msg_close(&msg);
    return 0;
}
int main(int argc, char * argv[])
{
	void * ctx = zmq_ctx_new();
	// create req socket, act as client
	void * req_sock = zmq_socket(ctx, ZMQ_CLIENT);
	char ip[16] = "127.0.0.1";
	unsigned int port = 5555;
	char buf[256] = {0};
	sprintf(buf,"tcp://%s:%u",ip,port);
	int rc = zmq_connect(req_sock, buf);
	if (rc == -1)
	{
		printf("error, zmq connect failed.\n");
		return 0;
	}
	//zmq_sleep(3);


	SERVER_HEADER header = {0};
	header.client_id = atoi(argv[1]);

	// send login message
	header.msg_id = LOGIN_ID;
	header.msg_len = sizeof(SERVER_HEADER);
	header.version = CLIENT_VERSION;
	

	zmq_msg_t msg;
	zmq_msg_init_size(&msg, sizeof(SERVER_HEADER));
	memcpy(zmq_msg_data(&msg),&header, sizeof(header));
	rc = zmq_msg_send(&msg,req_sock,0);	
	
	//reate thread to receive data from other client
    HANDLE hThread1 = CreateThread(NULL, 0, ThreadProc, req_sock, 0, NULL); 

    while(1)
	{
		//send message from req_socket
		char content[256] = "";
		cin.getline(content, sizeof(content) - 1);
		// 需要一个退出的方法
		if(content[0] == 'q')
			break;

		zmq_sleep(3);
		zmq_msg_t msg;
		zmq_msg_init_size(&msg,sizeof(content)+sizeof(SERVER_HEADER));
		header.msg_id = MESSAGE_ID;
		header.msg_len = sizeof(content)+sizeof(SERVER_HEADER);
		memcpy(zmq_msg_data(&msg), &header, sizeof(SERVER_HEADER));
		memcpy((char*)zmq_msg_data(&msg)+sizeof(SERVER_HEADER), content, sizeof(content));
		rc = zmq_msg_send(&msg,req_sock,0);
		if(rc == -1)
		{
			printf("error, zmq send failed.\n");
			break;
		}
	    zmq_msg_close(&msg);
		//printf("send massage succeed.\n");
		zmq_sleep(3);
	}
	// 关闭线程
	g_stop = true;
	WaitForSingleObject(hThread1, 5000);
    CloseHandle(hThread1);

	zmq_close(req_sock);
	zmq_ctx_term(ctx);

	return 0;
}


