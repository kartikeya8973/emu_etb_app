//Author - Kartikeya Arvind Yadav
//Date   - 27/01/2022
//TEST ETB APPLICATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include <pjsua-lib/pjsua.h>
#include <signal.h>

////For hiding the debugging printf statements
//#ifdef DEBUG
//#  define D(x) x
//#else
//#  define D(x)
//#endif

//Global Variables

info_t info_active; //for active information
info_t info_recv;   //for information recevied from client
info_t info_prev;   //for saving information of previous configuration
ch_info_t ch_info;

//ctrl+c signal handler
void handle_sigint(int sig)
{
	static pj_thread_desc s_desc;
	static pj_thread_t *s_thread;
	int status = pj_thread_register("handle_sigint", s_desc, &s_thread);
	if(status != PJ_SUCCESS){
		printf("\n\n\n\n\nThread failed to register\n\n\n\n\n");
	}
	else
		printf("\n\n\n\n\nThread registered successfully\n\n\n\n\n");
	pjsua_destroy();

	//Turning off LEDs when program in closed forcefully
	unexport_Led();
	exit(1);
}

int main(int argc, char* argv[])
{

	signal(SIGINT, handle_sigint);

// Exporting LEDs
	export_Led();

//  Calling function for creation and/or evalution of Database
	db_func();

//	reading values from database
	read_from_db();

//	Thread for calling button
	pthread_t call_button;
	if(pthread_create(&call_button, NULL, (void *)call_button_thread, NULL) != 0){
		perror("Call Button failed...\n");
		exit(1);
		}
	else
	printf("Press Call Button...\n");

//	Calling thread for making call
	pthread_t call;
	pthread_create(&call, NULL, (void *)pjsip_call_thread, NULL);

//	TCP server thread
	pthread_t tcp_server;
	if(pthread_create(&tcp_server, NULL, (void *)tcp_server_thread, NULL) != 0){
		perror("TCP server cannot be initialized...\n");
		exit(1);
	}
	else
	printf("TCP server created created successfully...\n");

	pthread_exit(NULL);



//

//	 Server no 2
//		if(pthread_create(&tcp_server, NULL, (void *)tcp_server_thread_2, NULL) != 0)
//		{
//			perror("TCP server cannot be initialized...\n");
//			exit(1);
//		}
//		else
//			printf("TCP server created created successfully...\n");

	return 0;
}



