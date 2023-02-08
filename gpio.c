/*
 * gpio.c
 *
 *  Created on: Feb 16, 2022
 *      Author: Kartikeya
 */


#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/select.h>
#include<sys/time.h>
#include<fcntl.h>
#include<errno.h>
#include<linux/input.h>
#include <linux/uinput.h>
#include "common.h"
#include <pjsua-lib/pjsua.h>

//This flag is used to call the ui_make_call() function in legacy_etb_main() func
// call flag == 0 -> call func disabled
// call flag == 1 -> call func enabled
int call_flag;
//This flag would disable the calling button for the duration of the call
// button_flag == 0 -> user button disabled
// button_flag == 1 -> user button enabled
int button_flag;

int *call_button_thread(int argc, char *argv[])
{

	{
		//		int ledn;
		//		char* tmp;
		int keys_fd;
		struct input_event t;
		argv[1] = "/dev/input/event1";
		keys_fd = open(argv[1], O_RDONLY);
		if (keys_fd <= 0)
		{
			printf ("open %s device error!\n",argv[1]);
			return 0;
		}

		//Setting flag to high for first button press after application starts
		button_flag = 1;

		while(1)
		{

			if (read(keys_fd, &t, sizeof(t)) == sizeof(t))
			{

				if (t.type == EV_KEY)
					if (t.value == 0 && button_flag == 1)
					{
						printf("**********************\nBUTTON PRESSED CALLING\n**********************\n");

						call_flag = 1;

						if(call_flag == 1)
						{
							/*--------------------------------------------*/
							//CASE 2
//							pthread_create(&blinkgreenLed, NULL, (void *)greenblink, NULL);
							/*--------------------------------------------*/
						}
						//			        printf ("key %d %s\n", t.code,(t.value) ? "Pressed" : "Released");
						//			    	pthread_t press;
						//			    	pthread_create(&press, NULL, (void *)test_thread, NULL);

						//			    	if(t.code==KEY_ESC)
						//			    	break;
					}
				if(t.value == 0 && button_flag == 0){
					printf("BUTTON IS DISABLED FOR THE DURATION OF THE CALL...\n");
				}
			}
		}
		close (keys_fd);
		return 0;

	}
}

//RED LED - ON
//(CASE 1 - IDLE / AVAILABLE)
void redLed()
{
	system("echo 28 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio28/direction");
	system("echo 1 > /sys/class/gpio/gpio28/value");
}

//GREEN LED - BLINKING
//(CASE 2 - CALL PLACED RINGING)
int *greenblink()
{
	system("echo 0 > /sys/class/gpio/gpio28/value");
	system("echo 29 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio29/direction");
	while(1){
		sleep(1);
		system("echo 1 > /sys/class/gpio/gpio29/value");
		sleep(1);
		system("echo 0 > /sys/class/gpio/gpio29/value");
	}

	return 0;
}

//GREEN LED - ON
//(CASE 3 - CALL PROGRESS)
void greenLed()
{
	system("echo 29 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio29/direction");
	system("echo 0 > /sys/class/gpio/gpio29/value");
	system("echo 1 > /sys/class/gpio/gpio29/value");
}

//RED LED - ON
//(CASE 4 - CALL TERMINATED)
void redTerm()
{
	system("echo 0 > /sys/class/gpio/gpio29/value"); //GREEN OFF
	system("echo 1 > /sys/class/gpio/gpio28/value"); // RED ON
}

int *blueblink()
{
	system("echo 116 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio116/direction");
	while(1){
		sleep(1);
		system("echo 1 > /sys/class/gpio/gpio116/value");
		sleep(1);
		system("echo 0 > /sys/class/gpio/gpio116/value");
	}
	return 0;
}


void export_Led()
{

	system("echo 28 > /sys/class/gpio/export");
	system("echo 29 > /sys/class/gpio/export");
	system("echo 116 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio28/direction");
	system("echo out > /sys/class/gpio/gpio29/direction");
	system("echo out > /sys/class/gpio/gpio116/direction");
	system("echo 0 > /sys/class/gpio/gpio28/value");
	system("echo 0 > /sys/class/gpio/gpio29/value");
	system("echo 0 > /sys/class/gpio/gpio116/value");

}

void unexport_Led()
{
	system("echo 0 > /sys/class/gpio/gpio28/value");
	system("echo 28 > /sys/class/gpio/unexport");
	system("echo 0 > /sys/class/gpio/gpio29/value");
	system("echo 29 > /sys/class/gpio/unexport");
	system("echo 0 > /sys/class/gpio/gpio116/value");
	system("echo 116 > /sys/class/gpio/unexport");
}

//pj_thread_t *call_button_thread (int argc, char *argv[])
//{
////	int ledn;
////	char* tmp;
//	int keys_fd;
//	struct input_event t;
//	argv[1] = "/dev/input/event2";
//
//	keys_fd = open(argv[1], O_RDONLY);
//	if (keys_fd <= 0)
//	{
//        printf ("open %s device error!\n",argv[1]);
//	    return 0;
//	}
//
//	printf("Hit any key on board ......\n");
//
//    while(1)
//	{
//        if (read(keys_fd, &t, sizeof(t)) == sizeof(t))
//	    {
//            if (t.type == EV_KEY)
//		    if (t.value == 0 || t.value == 1)
//		    {
//		        printf ("key %d %s\n", t.code,(t.value) ? "Pressed" : "Released");
//		        if(t.code==KEY_ESC)
//		            break;
//		    }
//	    }
//    }
//
//    close (keys_fd);
//    return 0;
//
//}

