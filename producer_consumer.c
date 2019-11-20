#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#define MAX_PRODUCT 15
#define PRODUCTOR 5
#define CONSUMER 3
pthread_mutex_t mutex;
sem_t empty_sem;
sem_t full_sem;
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
int findNotEmpty(int*product){
    for(int i=0;i<MAX_PRODUCT;i++){
        if(product[i]!=0){
            return i;
        }
    }
    return -1;
}
void *productor(){

    sem_wait(empty_sem);
    usleep(rand(100)*50000);
    int i=findEmpty(product);
    if(i!=-1){
        product[i]=1;
    }
    
    
}
void *consumer(){

}
int main(){
    pthread_mutex_init(&mutex);
    sem_init(&empty_sem,1,MAX_PRODUCT);
    sem_init(&full_sem,1,0);

}