#include <stdlib.h>
#include <stdio.h>
#include <zmq.h>
#include <iostream>
#include <string>
#include <map>
#include "msgid.h"
#include "defines.h"

using namespace std;


void print_usage()
{
	printf("Usage: server.exe ip port\n");
	printf("\tip:	the ip address for the server socket.\n");
	printf("\tport: the port number for the server socket.\n");
}

int main(int argc, char * argv[])
{
	void * ctx = zmq_ctx_new();
	// create server socket
	void * server = zmq_socket(ctx, ZMQ_SERVER);
	// parse args
	char ip[16] = "127.0.0.1";
	unsigned int port = 5555;
	if (argc == 3)
	{
		printf("%s\n", argv[0]);
		strcpy(ip, argv[1]);
		port = atoi(argv[2]);
	}
	else
	{
		print_usage();
		return 0;
	}	
	// bind ip
	char buf[256] = {0};
	sprintf(buf,"tcp://%s:%u",ip,port);
	zmq_bind(server, buf);


	zmq_msg_t msg;
	zmq_msg_init(&msg);
	zmq_msg_t msg1;
	zmq_msg_init(&msg1);
	int rc = 0;

	// client map of client_id and client address
	map<int, unsigned int>	client_map;
	
	while(true)
	{
		rc = zmq_msg_recv(&msg, server, 0);
		if(rc < 0)
		{
			printf("error: data received failed!\n");
			continue;
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
			data = (char*)(zmq_msg_data(&msg)) + sizeof(SERVER_HEADER);

		switch(header->msg_id)
		{
		case LOGIN_ID:
			{
				client_map[header->client_id] = zmq_msg_routing_id(&msg);
				printf("success client login, the ID is:%d\n",header->client_id);
				break;
			}
		case MESSAGE_ID:
			{
				printf("success received data from client ID:%d, message: %s\n",
					header->client_id, data == NULL ? "NULL" : data);
				if(data[0]=='q') // 客户退出 删除客户信息
				{
					printf("success client logout, the ID is:%d\n",header->client_id);
					client_map.erase(client_map.find(header->client_id)); //通过关键字删除
					continue;
				}

				if(data[0] == 'c') //收到客户查询需求,发送在线客户信息给客户
				{
					char client_id_content[256];
					for ( map<int, unsigned int>::const_iterator iterc = client_map.begin();
						iterc != client_map.end(); iterc++)
					{
						//if (header->client_id != iter->first) // 排除发送消息的client
						{		
							sprintf(client_id_content,"Online client: %d",iterc->first);
							zmq_msg_t msgc;
							zmq_msg_init_size(&msgc,zmq_msg_size(&msg));
							memcpy(zmq_msg_data(&msgc), &header, sizeof(SERVER_HEADER));
							memcpy((char*)zmq_msg_data(&msgc)+sizeof(SERVER_HEADER),client_id_content, zmq_msg_size(&msg)-sizeof(SERVER_HEADER));
							zmq_msg_set_routing_id(&msgc,client_map.find(header->client_id)->second);
							rc = zmq_msg_send(&msgc, server, 0);
							if(rc == -1)
							{
								printf("error, zmq send failed.\n");
								break;
							}
							zmq_msg_close(&msgc);
						}
					}
					continue;

				}
				for (map<int, unsigned int>::const_iterator iter = client_map.begin();
					iter != client_map.end(); iter++)
				{
					if (header->client_id != iter->first) // 排除发送消息的client
					{						
						zmq_msg_t msg1;
						zmq_msg_init_size(&msg1,zmq_msg_size(&msg));
						memcpy(zmq_msg_data(&msg1), &header, sizeof(SERVER_HEADER));
						memcpy((char*)zmq_msg_data(&msg1)+sizeof(SERVER_HEADER), data, zmq_msg_size(&msg)-sizeof(SERVER_HEADER));
						zmq_msg_set_routing_id(&msg1, iter->second);

						printf("send %s to client id:%d\n",data,iter->first);
                        zmq_msg_send(&msg1, server, 0);
                        zmq_msg_close(&msg1);
					}
				}
				break;
			}
		default:
			printf("unrecognized msgid recieved, msg id is: %d\n", header->msg_id);
			break;
		}
	}
	zmq_msg_close(&msg);
	zmq_close(server);
	zmq_ctx_term(ctx);

	//// create req socket, act as server
	//void * rep_sock = zmq_socket(ctx, ZMQ_REP);
	//char ip[16] = "127.0.0.1";
	//unsigned int port = 5600;
	//char buf[256] = {0};
	//sprintf(buf,"tcp://%s:%u",ip,port);
	//// bind rep socket to the specified ip & port
	//int rc = zmq_bind(rep_sock, buf);
	//if (rc == -1)
	//{
	//	printf("error, zmq bind failed.\n");
	//	return 0;
	//}

	//while(1)
	//{
	//	zmq_msg_t msg1;
	//	zmq_msg_init(&msg1);
	//	rc = zmq_msg_recv(&msg1, rep_sock,0);
	//	if(rc == -1)
	//	{
	//		printf("error, zmq recv failed.\n");
	//		return 0;
	//	}
	//	char * content1 = (char*)zmq_msg_data(&msg1);
	//	printf("Client :%s.\n",content1);
	//	zmq_msg_close(&msg1);

	//	//calculate
	//	int result=0;
	//	result = math_simple(content1);

 //       char  content2[128] = "";
	//	itoa(result,content2,10);
	//	zmq_msg_t msg2;
	//    zmq_msg_init_size(&msg2, sizeof(content2));
	//	
	//	memcpy(zmq_msg_data(&msg2),content2,sizeof(content2));
	//	rc = zmq_msg_send(&msg2,rep_sock,0);
	//	if(rc == -1)
	//	{
	//		printf("error, zmq send failed.\n");
	//		return 0;
	//	}
	//	zmq_msg_close(&msg2);
	//}
	//zmq_close(rep_sock);
	//zmq_ctx_term(ctx);
	return 0;
}
