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


DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	void * ctx = zmq_ctx_new();
	// create req socket, act as server
	void * rep_sock = zmq_socket(ctx, ZMQ_REP);
	char ip[16] = "127.0.0.1";
	unsigned int port = 5600;
	char buf[256] = {0};
	sprintf(buf,"tcp://%s:%u",ip,port);
	// bind rep socket to the specified ip & port
	int rc = zmq_bind(rep_sock, buf);
	if (rc == -1)
	{
		printf("error, zmq bind failed.\n");
		return 0;
	}
	int result=0;
	while(1)
	{
	    zmq_msg_t msg1;
	    zmq_msg_init(&msg1);
		// recieve from the client
		rc = zmq_msg_recv(&msg1, rep_sock,0);
		if(rc == -1)
		{
			printf("error, zmq recv failed.\n");
			return 0;
		}
		char *content1 = (char*)zmq_msg_data(&msg1);
		if(content1 == "q")
			return 0;
		printf("Client :%s.\n",content1);
		zmq_msg_close(&msg1);
		//calculate
		result = math_simple(content1);
		char  content2[128] = "";
		itoa(result,content2,10);
		zmq_msg_t msg2;
	    zmq_msg_init_size(&msg2,sizeof(content2));
		//send result to client
		memcpy(zmq_msg_data(&msg2),content2,sizeof(content2));
		rc = zmq_msg_send(&msg2,rep_sock,0);
		if(rc == -1)
		{
			printf("error, zmq send failed.\n");
			return 0;
		}   
		zmq_msg_close(&msg2);
	}
	zmq_close(rep_sock);
	zmq_ctx_term(ctx);
	return 0;
}

int main(int argc, char * argv[])
{
	int a = 0;
	HANDLE hThread1 = CreateThread(NULL,0,ThreadProc, &a, 0, NULL);
	WaitForSingleObject(hThread1, INFINITE);
	CloseHandle(hThread1);
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
