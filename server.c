#include <sys/types.h>      
#include <fcntl.h>
#include <pthread.h>    
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct score_struct
{	
	char name[20];
	char grade[5];
	//int index;
};
struct score_struct score[20];
void *handle_msg(void*);
int cmp(const void*a,const void*b);
void setup(pthread_attr_t*);
int arrIndex;

main(int ac, char *av[]){
	int tcp_socket;
	struct sockaddr_in addbuf;
	int n;
	int fd;
	int *fdptr;
	pthread_t worker[20];
	pthread_attr_t attr;

	if(ac< 2){
		printf("USE:CMD port_number\n");
		exit(1);
	}
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(tcp_socket == -1){
		perror("cannot socket");
		exit(1);
	}
	addbuf.sin_family= AF_INET;
	addbuf.sin_port= htons(atoi(av[1]));
	addbuf.sin_addr.s_addr=INADDR_ANY;

	if(bind(tcp_socket,(const struct sockaddr*)&addbuf,sizeof(struct sockaddr_in))== -1){
		perror("cannot bind");
		exit(1);
	}

	if(listen(tcp_socket,1)== -1){
		printf("cannot listen\n");
		exit(1);
	}
	setup(&attr);
	int tempIndex=0;
	while(1){
		fd= accept(tcp_socket,NULL,0);
		fdptr=malloc(sizeof(int));
		*fdptr=fd;
		pthread_create(worker+arrIndex,&attr,handle_msg,fdptr);	
		usleep(300);
		arrIndex++;
	}
}

void setup(pthread_attr_t *attrp){
	pthread_attr_init(attrp);
	pthread_attr_setdetachstate(attrp,PTHREAD_CREATE_DETACHED);
}

void* handle_msg(void *fdptr){
	int fd;
	int tempIndex=arrIndex;
	char temp[50];
	fd=*(int *)fdptr;
	free(fdptr);
	int i;
	
	read(fd,score[tempIndex].name,20);
	printf("got client's name: %s",score[tempIndex].name);
	read(fd,score[tempIndex].grade,5);
	printf("got client's score:%s",score[tempIndex].grade);
	qsort(score,20,sizeof(struct score_struct),cmp);	
	memset(temp,0,sizeof(temp));
	strcat(temp,score[0].name);
	//printf("\ntemp:%s\n",temp);
	fflush(stdout);
	write(fd,temp,50);	
	close(fd);
	return NULL;
}

int cmp(const void*a,const void*b){
	struct score_struct *c=(struct score_struct*)a;
	struct score_struct *d=(struct score_struct*)b;
	return (atoi(d->grade))-(atoi(c->grade));
}
