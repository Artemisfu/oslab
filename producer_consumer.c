#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#define MAX_PRODUCT 15
#define PRODUCTOR 5
#define CONSUMER 3 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t empty_sem;
sem_t full_sem;
time_t tm;
int in = 0;
int out = 0;
int product[MAX_PRODUCT];
int findEmpty(int*product){
    for(int i=0;i<MAX_PRODUCT;i++){
        if(product[i]==0){
            return i;
        }
    }
    return -1;
}
int findFull(int*product){
    for(int i=0;i<MAX_PRODUCT;i++){
        if(product[i]!=0){
            return i;
        }
    }
    return -1;
}
void printProduct(int *product){
    for(int i=0;i<MAX_PRODUCT;i++){
        if(product[i]==0){
            printf("□");
        }
        else{
            printf("■");
        }
    }
    printf("\n");
}
void productor(int productor_id){
    while(1){
        printf("生产者%d试图进入缓冲区\n", productor_id);
        pthread_mutex_lock(&mutex);
        printf("生产者%d进入缓冲区\n", productor_id);
        sem_wait(&empty_sem);
        int i=findEmpty(product);
        if(i!=-1){
            printProduct(product);
            product[i]=productor_id;
            printf("生产者%d在第%d个位置生产了产品\n", productor_id, i);
            pthread_mutex_unlock(&mutex);
            printProduct(product);
            usleep(rand()%100*5000);
        }
        else{
            pthread_mutex_unlock(&mutex);
            sem_post(&full_sem);
        }
    }
}
void consumer(int consumer_id){
    while(1){
        printf("消费者%d试图进入缓冲区\n", consumer_id);
        pthread_mutex_lock(&mutex);
        printf("消费者%d进入缓冲区\n", consumer_id);
        sem_wait(&full_sem);
        int i=findFull(product);
        if(i!=-1){
            printProduct(product);
            product[i]=0;
            printf("消费者%d在第%d个位置消费了产品\n", consumer_id, i);
            pthread_mutex_unlock(&mutex);
            printProduct(product);
            usleep(rand()%100*5000);
        }
        else{
            pthread_mutex_unlock(&mutex);
            sem_post(&empty_sem);
        }
    }
}
int main(){
    // pthread_mutex_init(&mutex);
    sem_init(&empty_sem,1,MAX_PRODUCT);
    sem_init(&full_sem,1,0);
    for(int i=0;i<MAX_PRODUCT;i++){
        product[i]=0;
    }
    for(int i=0;i<PRODUCTOR;i++){
        pthread_t td;
        pthread_create(&td, NULL, (void *)&productor, i+1);
    }
    for(int i=0;i<CONSUMER;i++){
        pthread_t td;
        pthread_create(&td, NULL, (void*)&consumer, i+1);
    }
    sleep(30);
    return 0;
}