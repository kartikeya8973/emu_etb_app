/*
 * test_thread.c
 *
 *  Created on: Feb 23, 2022
 *      Author: Kartikeya
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *test_thread(void){
	//Prints value on button press
	for(int i = 0;i < 5 ; i++){
	sleep(2);
	printf("Button pressed using thread %d\n", i);
	}
	return 0;
	return NULL;
}
