#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#define __NR_alcall 437
long alcall(int cmd, char* buf){
	return syscall(__NR_alcall, cmd, buf);
}
int main(int argc, char* argv[])
{
	int cmd;
	char buf[4096];
	cmd=0;
	alcall(cmd, buf);
	printf("ok! rum dmesg | grep alcall in terminal\n");
	return 0;
}
