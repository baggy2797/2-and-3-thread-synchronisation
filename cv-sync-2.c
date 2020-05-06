/* On-demand 3-thread synchronization */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // Compile with "gcc cv-sync-2.c -o cv-sync-2 -lpthread"

int thisThread = 1;

int totalCount = 0;
int completedCount = 0;
int thread1Count = 0;
int t1CompletedCount = 0;
int thread2Count = 0;
int t2CompletedCount = 0;
int thread3Count = 0;
int t3CompletedCount = 0;

int flag = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t condition1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition3 = PTHREAD_COND_INITIALIZER;
 
void * thr_exec(void *n) { 
while(totalCount > 0) {
	pthread_mutex_lock(&mutex);
	int *param = (int*) n;
	
	if(thisThread != *param){
		if(*param == 1){
			pthread_cond_wait(&condition1, &mutex);
		}
		else if (*param == 2){
			pthread_cond_wait(&condition2, &mutex);
		}
		else if (*param == 3){
			pthread_cond_wait(&condition3, &mutex);
		}
		
	}

	if(flag){
		goto end;
	}

	if (thisThread == 1){
		for(int i=0; i<thread1Count; i++){
			completedCount++, t1CompletedCount++, totalCount--;
			printf("%2d: IN T1, TID = %ld\n", completedCount, pthread_self());
			if(totalCount == 0){
				flag = 1;
				break;
			}
		}
	}
	else if(thisThread == 2){
		for(int i=0; i<thread2Count; i++){
			completedCount++, t2CompletedCount++, totalCount--;
			printf("%2d: IN T2, TID = %ld\n", completedCount, pthread_self());
			if(totalCount == 0){
				flag = 1;
				break;
			}
		}
	}
	else if(thisThread == 3){
		for(int i=0; i<thread3Count; i++){
			completedCount++, t3CompletedCount++, totalCount--;
			printf("%2d: IN T3, TID = %ld\n", completedCount, pthread_self());
			if(totalCount == 0){
				flag = 1;
				break;
			}
		}
	}
	
	end:
	if (thisThread == 3){
		thisThread = 1;
		pthread_cond_signal(&condition1);
	}
	else if(thisThread == 1){
		thisThread = 2;
		pthread_cond_signal(&condition2);
	}
	else if(thisThread == 2){
		thisThread = 3;
		pthread_cond_signal(&condition3);
	}
	pthread_mutex_unlock(&mutex); 

	if(flag){
		pthread_exit(NULL);
		return NULL;
	}
} 
return NULL; 
}

int main(int argc, char * argv[]) {
	if (argc != 5 || (atoi(argv[2]) == 0 && atoi(argv[3]) == 0 && atoi(argv[4]) == 0)) {
		return 0;
	}

    pthread_t threadid1, threadid2, threadid3;
    
	int threadNum1 = 1;
	int threadNum2 = 2;
	int threadNum3 = 3; 

	totalCount = atoi(argv[1]);
	thread1Count = atoi(argv[2]);
	thread2Count = atoi(argv[3]);
	thread3Count = atoi(argv[4]);

	pthread_create(&threadid1, NULL, thr_exec, (void *)&threadNum1);
	pthread_create(&threadid2, NULL, thr_exec, (void *)&threadNum2);
	pthread_create(&threadid3, NULL, thr_exec, (void *)&threadNum3);

	pthread_join(threadid1, NULL);
	pthread_join(threadid2, NULL);
	pthread_join(threadid3, NULL);

	printf("In main thread: T1 printed %d times. T2 printed %d times. T3 printed %d times.\n", t1CompletedCount, t2CompletedCount, t3CompletedCount);

    return 0;
}

