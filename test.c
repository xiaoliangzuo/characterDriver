#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
	int fb = open("/dev/globaldev", O_RDWR);
	//memset(buf, '\0', sizeof(buf));
	pid_t pid = 0;
	
	ssize_t count = -1;
	pid = fork();
	if(pid == 0){
		sleep(2);
		char buf[100];
//		int fb = open("/dev/globaldev1", O_RDWR);
//		count = read(fb, buf, 5);
//		 if(count == -1) printf("read1 error\n");
		
		printf("pos of son: %d\n",lseek(fb, 0, SEEK_CUR));
		strcpy(buf, "hello world,i am thfweffe son");
		count = write(fb, buf, strlen(buf));

		if(count == -1)
		{
			printf("write error\n");
			return -1;
		}
		 lseek(fb, 0, SEEK_SET);
	memset(buf, '\0', 100);
		count = read(fb, buf, 40);
		if(count == -1) printf("read error\n");
		printf("儿子: %s\n", buf);
		printf("fb is : %d\n 儿子进程结束\n\n", fb);
		getchar();
		close(fb);
	}
	else{
		char buf[100];
//		int fb = open("/dev/globaldev", O_RDWR);
		strcpy(buf, "hellobababadwfffffff");
		printf("buf :%s\n", buf);
		count = write(fb, buf, strlen(buf));
		if(count == -1)
		{
			printf("write error\n");
			return -1;
		}
		lseek(fb, 0, SEEK_SET);
	    memset(buf, '\0', 100);
		count = read(fb, buf, 20);
		printf("\n%d\n",buf[0]);
		if(count == -1 || count == 0) printf("read error\n");
		printf("baba %s\n", buf);
		printf("fb is : %d\n 爸爸进程结束\n", fb);
		getchar();
		close(fb);			
	}


//	memset(buf, '\0', sizeof(buf));
	//count = read(fb, buf, 5);
 //	scanf("%d", &count);	
//	close(fb);
}
