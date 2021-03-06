#include <stdlib.h>
#include <stdio.h>
#include <zmq.h>
#include <iostream>
#include <string>
#include <math.h>
#include <Windows.h>
#define INFINITE            0xFFFFFFFF  
using namespace std;

int math_simple(char *str);

struct ThreadParam
{
	void * ctx;
	char host[16];
	int port;
	volatile bool stop;
};
HANDLE g_hMutex = NULL; 


DWORD WINAPI ThreadProc(void *param)
{
	ThreadParam* thread_param = (ThreadParam*) param;
	// create req socket, act as server
	void* rep_sock = zmq_socket(thread_param->ctx, ZMQ_REP);
	//char ip[16] = Par.;
	//unsigned int port = 5600;
	char buf[256] = {0};
	sprintf(buf,"tcp://%s:%u",thread_param->host,thread_param->port);
	
	//ask for g_hMutex
	WaitForSingleObject(g_hMutex, INFINITE);
	
	// bind rep socket to the specified ip & port
	int rc = zmq_bind(rep_sock, buf);
	if (rc == -1)
	{
		printf("error, zmq bind failed.\n");
		zmq_close(rep_sock);
	    zmq_ctx_term(thread_param->ctx);
		return 0;
	}
	int result=0;

	zmq_msg_t msg1;
	zmq_msg_init(&msg1);
	char  content2[128] = "";
	while(!thread_param->stop)
	{
		// recieve from the client
		rc = zmq_msg_recv(&msg1, rep_sock,0);
		if(rc == -1)
		{
			printf("error, zmq recv failed.\n");
			break;
		}
		char* content1 = (char*)zmq_msg_data(&msg1);
		if(zmq_msg_size > 0 && *content1 == 'q')
			break;
		else continue;
		printf("Client :%s.\n",content1);
		zmq_msg_close(&msg1);
		//calculate
		result = math_simple(content1);
		itoa(result,content2,10);
		zmq_msg_t msg2;
	    zmq_msg_init_size(&msg2,sizeof(content2));
		//send result to client
		memcpy(zmq_msg_data(&msg2),content2,sizeof(content2));
		rc = zmq_msg_send(&msg2,rep_sock,0);
		if(rc == -1)
		{
			printf("error, zmq send failed.\n");
			break;
		}   
		zmq_msg_close(&msg2);
	}
	zmq_msg_close(&msg1);
	zmq_close(rep_sock);
	zmq_ctx_term(thread_param->ctx);

	//release Mutex
	ReleaseMutex(g_hMutex);
	
	return 0;
}

int main(int argc, char * argv[])
{
	//create Mutex
	g_hMutex = CreateMutex(NULL, FALSE, NULL);
	ThreadParam thread_param = {NULL, "127.0.0.1", 5600, false};
	thread_param.ctx = zmq_ctx_new();
	if(thread_param.ctx == NULL)
	{
		fprintf(stderr, "failed to create zmq context, reason: %s.\n", zmq_strerror(zmq_errno()));
		return -1;
	}
	HANDLE hThread1 = CreateThread(NULL,0,ThreadProc, &thread_param, 0, NULL);

	while (true)
	{
		//ask for Mutex		
		fprintf(stdout, "enter q to exit.\n");
		WaitForSingleObject(g_hMutex, INFINITE);
		char cmd = getchar();
		if (cmd == 'q' || cmd == 'Q')
		{
			// stop log thread
			thread_param.stop = true;
			break;
		}
	// release mutex
	ReleaseMutex(g_hMutex);
	}

	CloseHandle(hThread1);
	// the last one thing to do is terminate context
	//zmq_ctx_term(thread_param.ctx);
	return 0;
}

int math_simple(char str[])
{
	int a=0, b=0, c=0, i = 0,number=0;
	int length = strlen(str);
	for(i = 0 ; i <length; i++)
	{
		if(str[length-i] == '+'||str[length-i] == '-'||str[length-i] == '*'||str[length-i] == '/')
		{
			break;
		}
	}
	a = int(str[0])-48;
	for(int j = 1;j<length - i;j++)
	{
		a = a*10 + int(str[j])-48;
	}

	b = int(str[length - i + 1])-48;
	for(int j = 1; j<i - 1 ;j++)
	{
		b = b*10 + int(str[length -i+ j+1])-48;
	}

	switch(str[length-i])
	{
	case '+' :
		c = a + b;
		break;
	case '-' :
		c = a - b;
		break;
	case '*' :
		c = a * b;
		break;
	case '/' :
		c = a / b;
		break;
	}

	return c;
}
