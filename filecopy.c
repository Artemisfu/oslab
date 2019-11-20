#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#define PERLEN 100000
int main(int argc, char*argv[])
{
	char* infile=argv[1];
	char* outfile=argv[2];
	FILE* in,* out;
	if(!(in=fopen(infile, "r"))){
		printf("Can't open infile!");
	}
	if(!(out=fopen(outfile, "w"))){
		printf("Can't open infile!");
	}
	int x;
	char p[PERLEN];
	do{
		x=fread(p,1, PERLEN,in);
		fwrite(p,1,x,out);
	}
	while(x==PERLEN);
	return 0;
}
	
