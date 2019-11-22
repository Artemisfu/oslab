#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>
#define MAX_PRODUCT 3
#define PRODUCER 5
#define CONSUMER 4
#define NEED_PRODUCE 15
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty_sem = PTHREAD_COND_INITIALIZER;
pthread_cond_t full_sem = PTHREAD_COND_INITIALIZER;
time_t tm;
int in = 0;
int out = 0;
struct{
    int producer_id;
    int consumer_id;
    int product_id;
    int storage_positon;
    char * produce_time;
}typedef Output;
Output product[MAX_PRODUCT];
char* getTime(){
    time_t t;
    time(&t);
    struct tm *p =localtime(&t);
    printf("%02d:%02d:%02d ", p->tm_hour, p->tm_min, p->tm_sec);
    char*result = (char*)malloc(sizeof(char)*20);
    sprintf(result, "%02d:%02d:%02d ", p->tm_hour, p->tm_min, p->tm_sec);
    return result;
}
int findEmpty(){
    for(int i=0;i<MAX_PRODUCT;i++){
        if(product[i].producer_id==0){
            return i;
        }
    }
    return -1;
}
int findFull(){
    for(int i=0;i<MAX_PRODUCT;i++){
        if(product[i].producer_id!=0){
            return i;
        }
    }
    return -1;
}
void printProduct(){
    for(int i=0;i<MAX_PRODUCT;i++){
        if(product[i].producer_id==0){
            printf("□");
        }
        else{
            printf("■");
        }
    }
    printf("\n");
}
void producer(void *producer_id1){
    int producer_id = *((int*)producer_id1);
    while(1){
        if(in==NEED_PRODUCE){
            pthread_cond_signal(&empty_sem);
            break;
        }
        getTime();
        printf("生产者%d试图进入缓冲区\n",producer_id);
        pthread_mutex_lock(&mutex);
        getTime();
        printf("生产者%d进入缓冲区\n",producer_id);
        int i=findEmpty();
        if(i==-1){
            pthread_cond_signal(&full_sem);
            getTime();
            printf("生产者%d挂起\n", producer_id);
            pthread_cond_wait(&empty_sem, &mutex);
            i=findEmpty();
        }

        if(in==NEED_PRODUCE){
            getTime();
            printf("生产者%d离开缓冲区\n", producer_id);
            pthread_cond_signal(&empty_sem);
            pthread_mutex_unlock(&mutex);
            break;
        }
        printProduct();
        in += 1;
        product[i].produce_time = (char*)malloc(sizeof(char)*20);
        product[i].storage_positon = i;
        product[i].product_id = in;
        product[i].producer_id=producer_id;
        int use_time=rand()%100*400 + 10000;
        usleep(use_time);
        strcpy(product[i].produce_time, getTime());
        printf("生产者%d，产品%d，位置%d， %ds\n", producer_id, product[i].product_id,i, use_time/1000000);
        // printf("%s 生产者%d在第%d个位置生产了产品%d\n",timenow, producer_id, i+1, in);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&full_sem);
        getTime();
        printf("生产者%d离开缓冲区\n", producer_id);
        printProduct();
        // sem_post(&full_sem);
        sleep(1);
    }
}
void consumer(void* consumer_id1){
    int consumer_id = *(int*)consumer_id1;
    while(1){
        getTime();
        printf("消费者%d试图进入缓冲区\n", consumer_id);
        if(out==NEED_PRODUCE&&in==NEED_PRODUCE){
            pthread_cond_signal(&full_sem);
            break;
        }
        pthread_mutex_lock(&mutex);
        getTime();
        printf("消费者%d进入缓冲区\n", consumer_id);
        int i=findFull();
        if(i==-1){
            pthread_cond_signal(&empty_sem);
            getTime();
            printf("消费者%d挂起\n", consumer_id);
            pthread_cond_wait(&full_sem, &mutex);
            i=findFull();
        }
        if(out==NEED_PRODUCE&&in==NEED_PRODUCE){
            printf("消费者%d离开缓冲区\n", consumer_id);
            pthread_mutex_unlock(&mutex);
            pthread_cond_signal(&empty_sem);
            break;
        }
        printProduct();
        product[i].consumer_id = consumer_id;
        product[i].producer_id = 0;
        out += 1;
        int use_time=rand()%100*400 + 10000;
        usleep(use_time);
        getTime();
        printf("消费者%d，产品%d，位置%d, %ds\n",consumer_id, product[i].product_id,i, use_time/1000000);
        getTime();
        printf("消费者%d离开缓冲区\n", consumer_id);
        pthread_mutex_unlock(&mutex);
        printProduct();
        sleep(1);
    }
}
int main(){
    pthread_t producers[PRODUCER];
    pthread_t consumers[CONSUMER];
    for(int i=0;i<MAX_PRODUCT;i++){
        product[i].producer_id=0;
    }
    int max=PRODUCER>CONSUMER?PRODUCER:CONSUMER;
    for(int i=0;i<max;i++){
        if(i<PRODUCER){
            int * num = (int*)malloc(sizeof(int));
            *num = i+1;
            pthread_create(producers+i, NULL, (void *)producer, (void *)num);
        }
        if(i<CONSUMER){
            int * num = (int*)malloc(sizeof(int));
            *num = i+1;
            pthread_create(consumers+i, NULL, (void*)consumer, (void *)num);
        }
    }
    for(int i=0;i<max;i++){
        if(i<PRODUCER){
            pthread_join(producers[i], NULL);
        }
        if(i<CONSUMER){
            pthread_join(consumers[i], NULL);
        }
    }
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&full_sem);
    pthread_cond_destroy(&empty_sem);
    return 0;
}