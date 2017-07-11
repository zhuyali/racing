#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <termios.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include <curses.h>

#define MIDROW LINES/2
#define MIDCOL COLS/2
#define HEIGHT DOWNDGE-TOPDGE+1
#define LEFTDGE MIDCOL-20
#define RIGHTDGE MIDCOL+20
#define TOPDGE 1
#define DOWNDGE LINES-2

void drawCrash();
void drawBound();
void login();
void drawWin();
void drawLose();
void drawHinder(int,int);
void reDrawHinder(int*,int*);
void drawCar(int,int);
void reDrawCar(int*,int*,int);
int connectToServer(char* ,char*);
void sigResponse(int);
int set_ticker(int);
void drawPrompt(char *warning);
void reDrawPrompt(int tempGrade);

int width;
int x[30];
int y[30];
int tcp_socket;
int *px,*py;
int grade;
int carX,carY;
int *xCar,*yCar;
char username[20];

main(int ac,char *av[]){
	struct termios buf,save;
	char input;
	tcgetattr(0,&buf);
	save=buf;	
	int j;
	int index=0;
	px=x;
	py=y;
	if(ac< 3){
		printf("USE:CMD ip_addr port_num\n");
		exit(0);
	}
	tcp_socket=connectToServer(av[1],av[2]);
	initscr();
	width=RIGHTDGE;
	width-=LEFTDGE;
	width++;
	printf("%d,%d,%d\n",RIGHTDGE,LEFTDGE,width);
	buf.c_lflag &= ~ECHO;
	buf.c_lflag &= ~ICANON;
	login();
	getstr(username);
	write(tcp_socket,username,20);
	fflush(stdout);
	move(MIDROW+2,MIDCOL-12);
	addstr("About to enter...");
	refresh();
	sleep(3);
	beep();
	drawBound();
	drawPrompt("Loading...");
	carX=DOWNDGE-1;
	carY=MIDCOL;
	xCar=&carX;
	yCar=&carY;
	drawCar(carX,carY);
	int temp;
	temp=DOWNDGE;
	temp=temp-3;
	for(j=TOPDGE;j<=temp;j++){
		srand((unsigned)time(NULL));
		x[index]=j;
		int random=rand()%(width-2);
		if(random>=0)
			y[index]=LEFTDGE+1+random;
		else
			y[index]=LEFTDGE+1-random;
		drawHinder(x[index],y[index]);
		index++;
		sleep(1);
	}
	drawPrompt("Running...");
	signal(SIGALRM,sigResponse);
	if(set_ticker(900)== -1)
		perror("set_ticker");
	else{
		while(scanf("%c",&input)> 0){
			if(input== 'a' || input== 'A'){
				if((*yCar)>(LEFTDGE+2)){
					reDrawCar(xCar,yCar,-1);	
				}
			}
			else if(input== 'd'||input== 'D'){
				if((*yCar)<(RIGHTDGE-2)){
					reDrawCar(xCar,yCar,+1);
				}
			}
			else if(input== 'q'||input== 'Q')
				break;
			else
				continue;
		}
	}
//	getch();
	endwin();
}

void login(){
	clear();
	int i,j;
	char *msg= "Please input your name:";
	int len=24;
	move(MIDROW-2,MIDCOL-(len/2));
	addstr(msg);
	refresh();
	for(i= MIDROW-1;i<=MIDROW+1;){
		for(j= MIDCOL-(len/2)+1;j<= MIDCOL+(len/2);j++){
			move(i,j);
			addstr("-");
			refresh();
		}
		i=i+2;
	}
	move(MIDROW,MIDCOL-(len/2));
	addstr("|");
	refresh();
	move(MIDROW,MIDCOL+(len/2)+1);
	addstr("|");
	refresh();
	move(MIDROW,MIDCOL-(len/2)+2);
}

void reDrawPrompt(int tempGrade){
	move(TOPDGE+3,RIGHTDGE+3);
	addstr("           ");
	move(TOPDGE+3,RIGHTDGE+7);
	char *tgrade;
	sprintf(tgrade,"%d",tempGrade);
	addstr(tgrade);
	move(LINES-1,COLS-1);
	refresh();
}
void drawPrompt(char *warning){
	move(TOPDGE+1,RIGHTDGE+2);
	addstr("Tips:");
	int i,j;
	int len=strlen(warning);
	refresh();
	for(i= TOPDGE+2;i<=TOPDGE+4;){
		for(j= RIGHTDGE+3;j<= RIGHTDGE+13;j++){
			move(i,j);
			addstr("-");
			refresh();
		}
		i=i+2;
	}
	move(TOPDGE+3,RIGHTDGE+2);
	addstr("|");
	refresh();
	move(TOPDGE+3,RIGHTDGE+14);
	addstr("|");
	refresh();
	int midY;
	midY=RIGHTDGE;
	midY+=9;
	midY-=(len/2);
	move(TOPDGE+3,midY);
	addstr(warning);
	move(LINES-1,COLS-1);
	refresh();
}

void drawBound(){
	clear();
	int i= LEFTDGE;
	int j= TOPDGE;

	for(i= LEFTDGE;i<= RIGHTDGE;){
		for(j=TOPDGE;j<= DOWNDGE;j++){
			move(j,i);
			if(j%2== 1)
				standout();
			addstr(" ");
			if(j%2== 1)
				standend();
			refresh();
		}
		i+= RIGHTDGE;
		i-=LEFTDGE;
	}
	move(LINES-1,COLS-1);
}

void drawHinder(int x,int y){
	move(x,y);
	standout();
	addstr(" ");
	standend();
	move(LINES-1,COLS-1);
	refresh();
}

void reDrawHinder(int *x,int *y){
	int i=DOWNDGE;
	i=i-2;
	if((*x)== i){
		move(*x,*y);
		addstr(" ");
		(*x)=TOPDGE;
		srand((unsigned)time(NULL));
		int temp;
		temp=rand()%(width-2);
		if(temp>=0)
			(*y)=LEFTDGE+1+temp;
		else
			(*y)=LEFTDGE+1-temp;
		move(*x,*y);
		standout();
		addstr(" ");
		standend();	
		move(LINES-1,COLS-1);
		refresh();
		return;
	}
	move(*x,*y);
	addstr(" ");
	(*x)++;
	move(*x,*y);
	standout();
	addstr(" ");
	standend();
	move(LINES-1,COLS-1);
	refresh();
}

int set_ticker(int n_msecs){
	struct itimerval new_timeset;
	long n_sec,n_usecs;

	n_sec=n_msecs/1000;
	n_usecs=(n_msecs%1000)*1000L;

	new_timeset.it_interval.tv_sec=n_sec;
	new_timeset.it_interval.tv_usec=n_usecs;

	new_timeset.it_value.tv_sec=n_sec;
	new_timeset.it_value.tv_usec=n_usecs;

	return setitimer(ITIMER_REAL,&new_timeset,NULL);
}


void sigResponse(int signum){
	int i;
	int temp=HEIGHT;
	temp=HEIGHT-3;
	if(grade>=25&&grade<=55)
		set_ticker(600);	
	else if(grade>55&&grade<=90)
		set_ticker(300);
	else if(grade>90)
		set_ticker(220);	
	//else if(grade>150)
	//	set_ticker(55);
	for(i=0;i<temp;i++){
		if(*(px+i)>= (*xCar-2)&&*(py+i)<=((*yCar)+1)&&*(py+i)>=(*yCar-1)){
			drawCrash();
		}
		reDrawHinder((px+i),(py+i));
	}
	grade++;
}

void drawCrash(){
	int i,j;
	char buf[512];
	drawPrompt("Stopping...");
	char *msg= "Your score is:";
	int len=16;
	move(MIDROW-2,MIDCOL-(len/2));
	addstr(msg);
	refresh();
	for(i= MIDROW-1;i<=MIDROW+1;){
		for(j= MIDCOL-(len/2)+1;j<= MIDCOL+(len/2);j++){
			move(i,j);
			addstr("-");
			refresh();
		}
		i=i+2;
	}
	move(MIDROW,MIDCOL-(len/2));
	addstr("|");
	refresh();
	move(MIDROW,MIDCOL+(len/2)+1);
	addstr("|");
	refresh();
	move(MIDROW,MIDCOL-1);
	char *strGrade;
	sprintf(strGrade,"%d",grade);
	addstr(strGrade);
	move(LINES-1,COLS-1);
	write(tcp_socket,strGrade,5);
	read(tcp_socket,buf,sizeof(buf));
	fflush(stdout);
	close(tcp_socket);
	char output[50]="Winner is ";
	strcat(output,buf);
	move(MIDROW+2,MIDCOL-(len/2));
	if(strcmp(username,buf)== 0){
		//addstr("You win!");
		drawWin();
	}else{
		addstr(output);
		drawLose();
	}
	move(LINES-1,COLS-1);
	refresh();
	sleep(3);
	endwin();
	exit(0);
}

void drawWin(){
	move(MIDROW+3,MIDCOL-22);
	addstr("#   #  #####  #   #       #           #  #####  #   #");
	move(MIDROW+4,MIDCOL-22);
	addstr(" # #   #   #  #   #        #    #    #     #    ##  #");
	move(MIDROW+5,MIDCOL-22);
	addstr("  #    #   #  #   #         #   #   #      #    # # #");
	move(MIDROW+6,MIDCOL-22);
	addstr("  #    #   #  #   #          # # # #       #    #  ##");
	move(MIDROW+7,MIDCOL-22);
	addstr("  #    #####  #####           #   #      #####  #   #");
}

void drawLose(){
	move(MIDROW+3,MIDCOL-22);
	addstr("#   #  #####  #   #       #      #####  #####  #####");
	move(MIDROW+4,MIDCOL-22);
	addstr(" # #   #   #  #   #       #      #   #  #      #");
	move(MIDROW+5,MIDCOL-22);
	addstr("  #    #   #  #   #       #      #   #  #####  #####");
	move(MIDROW+6,MIDCOL-22);
	addstr("  #    #   #  #   #       #      #   #      #  #");
	move(MIDROW+7,MIDCOL-22);
	addstr("  #    #####  #####       #####  #####  #####  #####");
}

void drawCar(int x,int y){
	move(x-1,y-1);
	addstr("o o");
	move(x,y);
	standout();
	addstr(" ");
	standend();
	move(x+1,y-1);
	addstr("o o");
	move(LINES-1,COLS-1);
	refresh();
}

void reDrawCar(int* x,int* y,int dir){
	move((*x)-1,(*y)-1);
	addstr("   ");
	move(*x,*y);
	addstr(" ");
	move(*x+1,*y-1);
	addstr("   ");
	move(*x-1,*y-1+dir);
	addstr("o o");
	(*y)=(*y)+dir;
	move(*x,(*y));
	standout();
	addstr(" ");
	standend();
	move(*x+1,*y-1);
	addstr("o o");
	move(LINES-1,COLS-1);
	refresh();
}

int connectToServer(char *ip_addr,char* port_num){

	int tcp_socket;
	struct sockaddr_in addbuf;

	if((tcp_socket= socket(AF_INET,SOCK_STREAM,0))== -1){
		printf("cannot socket\n");
		exit(1);
	}

	addbuf.sin_family= AF_INET;
	addbuf.sin_port= htons(atoi(port_num));
	addbuf.sin_addr.s_addr= inet_addr(ip_addr);

	if((connect(tcp_socket,(const struct sockaddr*)&addbuf,sizeof(struct sockaddr_in)))== -1
	  ){
		printf("cannot connect\n");
		exit(1);
	}
	return tcp_socket;
}
