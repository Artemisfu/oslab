#include <stdio.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#define BUFF_SIZE 1024*1024
#define PATH_SIZE 32*1024
#define MAX_COUNT 350

pthread_t wait[MAX_COUNT];
int count = 0;
struct{
    char *from;
    char *to;
} typedef Args;
int is_dir(char*path){
    struct stat s_buf;
    stat(path, &s_buf);
    return (S_ISDIR(s_buf.st_mode));
}
int is_dot(char*path){
    return (strcmp(path,".")==0 || strcmp(path,"..")==0);
}
int is_slashed(char* path){
    return path[strlen(path) - 1] == '/';
}
void copy_file(void*p){
    char*from = (char*)malloc(PATH_SIZE);
    char*to = (char*)malloc(PATH_SIZE);
    char*buff = (char*)malloc(BUFF_SIZE);
    // strcpy(from, args[0]);
    Args * a = (Args*) p;
    strcpy(from, a->from);

    // strcpy(to, args[1]);
    strcpy(to, a->to);
    // printf("from %s to %s\n", from ,to);
    FILE * f1 = fopen(from, "r");
    if(!f1) {
        printf("In Flie error: %s\n", from);
        return;
    }
    FILE * f2 = fopen(to, "aw");

    if(!f2){
        printf("out Flie error: %s\n", to);
        fclose(f1);
        return;
    }
    int len = ftell(f1);
    int read;
    do{
        read = fread(buff, 1, BUFF_SIZE, f1);
        fwrite(buff, 1, read, f2);
    }while(read == BUFF_SIZE);
    fclose(f1);
    fclose(f2);
    free(buff);
    free(from);
    free(to);
    free(a->from);
    free(a->to);
    return;
}

void copy_dir(char*from, char*to){
    // printf("from: %s to: %s", from, to);
    if(!is_dir(from)){
        // pthread_t t1;
        Args * a = (Args*)malloc(sizeof(char*)*2);
        a->from = (char*)malloc(PATH_SIZE);
        a->to = (char*)malloc(PATH_SIZE);
        strcpy(a->from, from);
        strcpy(a->to, to);
        // int err = pthread_create(&t1,NULL,(void*)copy_file,(void*)a);
        int err = pthread_create(&wait[count++],NULL,(void*)copy_file,(void*)a);
        // pthread_join(t1, NULL);

        if(count==MAX_COUNT){
            for(int i = 0; i < MAX_COUNT; i++){
                pthread_join(wait[i], NULL);
            }
            count = 0;
        }
        return;
    }
    if(access(to, F_OK) == 0){
        printf("目录已存在！\n");
        return;
    }
    if(mkdir(to, 0777)!=0){
        printf("创建目录失败！\n");
        return;
    }
    char* base_from = (char*)malloc(PATH_SIZE);
    strcpy(base_from, from);
    char* base_to = (char*)malloc(PATH_SIZE);
    strcpy(base_to, to);
    DIR*dir = opendir(base_from);
    if(dir==NULL){
        printf("can not open!: %s\n", from);
        return;
    }
    struct dirent *file;

    while((file=readdir(dir))!=NULL){
        // printf("file:%s\n", file->d_name);
        if(is_dot(file->d_name)){
            continue;
        }
        char* file_from = (char*)malloc(PATH_SIZE);
        strcpy(file_from, base_from);
        // printf("from before slash:%s\n", file_from);
        if(!is_slashed(file_from)){
            strcat(file_from, "/");
        }
        strcat(file_from, file->d_name);
        char* file_to = (char*)malloc(PATH_SIZE);
        strcpy(file_to, base_to);
        // printf("to before slash:%s\n", file_to);
        if(!is_slashed(file_to)){
            strcat(file_to, "/");
        }
        strcat(file_to, file->d_name);
        copy_dir(file_from, file_to);
        free(file_from);
        free(file_to);
    }
    free(base_from);
    free(base_to);
    closedir(dir);
}
int main(int argc, char**args){
    copy_dir(args[1], args[2]);
    for(int i = 0; i < count; i++){
        pthread_join(wait[i], NULL);
    }
    return 0;
}