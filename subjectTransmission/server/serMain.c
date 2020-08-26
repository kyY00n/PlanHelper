#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>	
#include <semaphore.h>

#define BUF_SIZE 100
#define MAX_CLI 10
#define MAX_IP 30

//socket API & function
void* serverMenu(void*);
void sndPlan(int, char*);
void rcvPlan(int, char*);
void sndInt(int, int);
int rcvInt(int);
void sndMsg(int, char*);
char* rcvMsg(int);

//server_funcAC.c
extern char* decideMode(int);
extern char* CheckID(char*, char*);
extern char* CheckClient (char*);

//server_funcDB.c
extern char* readFile(FILE*, int);
extern void writeFile(FILE*, char*);
extern FILE* openreadFile (char*);
extern FILE* openwriteFile(char*);
extern void closeFile(FILE*);

int cliCount = 0;
int cliSock[MAX_CLI];
sem_t bin_sem1;

int main(int argc, char** argv)
{	
	printf("server program starts\n");

	int servSockFd, cliSockFd;
	struct sockaddr_in cliAddr, servAddr;
	int client_len;
	pthread_t thread;
	void* thread_return;
	int state1;
	int threadId;

	/*state1 = sem_init(&bin_sem1, 0, 1);

	  if (state1 != 0)
	  {
	  printf("Semaphotre 초기화 실패");
	  exit(0);
	  }
	 */

	if (argc != 2)
	{
		printf("Usage: ./server [port number]\n");
		printf("예: ./server 7777\n");
		exit(0);
	}

	//TCP소켓 생성
	if ((servSockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		exit(0);
	}

	//인터넷 주소 초기화
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(servSockFd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
	{
		perror("bind error");
		exit(0);
	}
	printf("bind completes\n");

	if (listen(servSockFd, 5) < 0)
	{
		perror("listen error");
		exit(0);
	}
	printf("listen completes\n\n");

	while (1)
	{
		printf("listen 후 while 들어왔습니다.\n");
		int cliAddrLen = sizeof(cliAddr);
		cliSockFd = accept(servSockFd, (struct sockaddr*)&cliAddr, &cliAddrLen);

		if(cliSockFd < 0)
		{
			perror("Socket accept error\n");
			exit(1);
		}

		printf("client Socket file descript Num: %d\n", cliSockFd);
		//Debug
		printf("new client connection\n");

		/*
		   sem_wait(&bin_sem1);
		   cliSock[cliCount++] = cliSockFd;
		   sem_post(&bin_sem1);
		 */

		//Debug
		//printf("세마포어 생성했습니다.\n");

		threadId = pthread_create(&thread, NULL, serverMenu, (void*)&cliSockFd);

		pthread_detach(threadId);

		if(threadId != 0)
		{
			perror("main: Thread creation failed\n");
			exit(EXIT_FAILURE);
		}

		printf("main: %d번째 스레드 생성!\n", cliCount+1);

	}		
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////서버 스레드 루틴 function///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* serverMenu (void* arg) 
{
	char* studmode = "STUDENT.txt";
	char* profmode = "PROFESSOR.txt";

	int cliSockFd = *(int*)arg;
	//	int cliMode = rcvInt(cliSockFd);
	///	printf("소켓으로 받은 사용자 선택 모드(1:교수 2:학생 3:종료): %d\n\n", cliMode); //debug

	//	char* mode = decideMode(cliMode);	
	//	printf("decideMode return value: \"%s\"\n", mode);
	//debug: mode = "PROFFESOR.txt" 로 저장됨


	printf("client: Student mode starts\n"); //debug

	/* char* idRcv = rcvMsg(cliSockFd);
	   printf("ID reception succeeds.(ID: %s)\n\n", idRcv); //debug

	   char* msg = CheckID(mode, idRcv);		///// msg: 학생아이디.txt or 없음
	   printf("ID authentication succeeds\n\n"); //debug

	   char* ACResult = CheckClient(idRcv);

	   printf("ACResult returns \"%s\"\n\n", ACResult); //debug

	   sndMsg(cliSockFd, ACResult);

	   printf("Authentication result deliver success.\n\n"); //debug

	   free(idRcv); */
	/* 
	   if (strcmp(msg,"없음")!=0) //학번이 리스트에 있으면 진행 //strcmp로 수정
	   {	

	   }
	   free(msg);
	   free(ACResult);
	   }
	   else { } //사용자 입력이 3일 때
	   free(mode); */

	FILE* fd_num;		////수강하는 과목명 읽으러 가는중
	int num = 0;

	while (1) //"1971078.txt" 한 줄씩 읽으면서 진행
	{   
		fd_num = openreadFile("1971078.txt");
		// fd_num = "1971078.txt"
		printf("num: %d\n", num);
		char* title = readFile(fd_num, num); //readFile은 파일이 끝나면 널문자열을 반환합니다."
		num++;
		if (title[0] == '\0')
		{
			sndMsg(cliSockFd, "끝");
			printf("사용자의 과목이 이제 없어요.\n\n");
			printf("끝이라고 보냈어요\n");
			break; 
		}

		printf("과목명: %s\n\n", title);	

		char subjname[80] = { 0 };	//위에서 읽은 과목명에 .txt붙이기
		sprintf(subjname, "%s.txt", title);
		/////이제 과목일정 읽어 보낸다.
		
		sndPlan(cliSockFd, subjname);
	}
	closeFile(fd_num);
                // fd_num = "1971078.txt"
		//char*title[1000]={0};

		//"1971078.txt" 한 줄씩 읽으면서 진행
                // fd_num = "1971078.txt"
		//char*title[1000]={0}; 
	/* char* title[1000]={0};
	char* subName;              
	 //printf("num: %d\n", num);
          	 for(int i=0;i<1000;i++)
		{
  			strcpy(subName,readFile(fd_num,i)); //1971078.txt i번째 줄 읽음
		
			if(strlen(subName)==0){
				break;}
			sprintf(title[i],"&s.txt",subName);
		}


	int n=sizeof(title); //저장된 포인터의 개수

	for(int i=0;i<n;i++){ //과목파일 보내기
		sndPlan(cliSockFd,title[i]);
	}*/
	printf("스레드 종료!: %d\n", cliSockFd);
	close(cliSockFd);
}
void sndPlan(int cliSockFd, char* fileName)
{
//	FILE* planFile = openreadFile(fileName);
	char plan[500];
	int lineNum = 0;

	while (1)
	{
		FILE* planFile = openreadFile(fileName);

		strcpy(plan, readFile(planFile, lineNum));

		printf("%s의 plan %d번째 줄: %s\n\n",fileName, lineNum+1, plan); //debug
		if(strlen(plan) == 0) break;
		write(cliSockFd, plan, 500);
		lineNum++;
		closeFile(planFile);
	}

}

void rcvPlan(int cliSockFd, char* fileName)
{
	FILE* planFile = openwriteFile(fileName);
	char plan[BUF_SIZE];
	int len = sizeof(strlen(plan));

	int state;

	while(1)
	{
		state = read(cliSockFd, plan, len);

		if(state == -1)
		{
			perror("rcvPlan, read() error");
			closeFile(planFile);
			exit(1);
		}
		else if(strcmp(plan, "끝") == 0) break;   
		else
		{
			writeFile(planFile, plan);
		}
	}
	closeFile(planFile);
	//sleep(10);

}

void sndInt(int cliSockFd, int inputNum)
{	
	int state; 

	char num[5];
	sprintf(num, "%d", inputNum);

	state = write(cliSockFd, num, sizeof(num));
	if(state == -1 )
	{
		perror("sndInt, transmission error");
		exit(1);
	}
}

int rcvInt(int cliSockFd)
{
	char num[BUF_SIZE];
	int state;

	sleep(1);
	state = read(cliSockFd, num, BUF_SIZE);

	if(state == -1)
	{
		perror("rcvInt, read() error");
		exit(1);
	}

	int chartoNum = atoi(num);
	return chartoNum;
}

void sndMsg(int cliSockFd, char* msg)
{
	char msgArray[50];
	int state;
	
	strcpy(msgArray, msg);	

	state = write(cliSockFd, msgArray, 50);

	if(state < 0)
	{
		perror("sndMsg, transmission error");
		exit(1);
	}
}

char* rcvMsg(int cliSockFd)
{
	char* msg = (char*) malloc(sizeof(char)*BUF_SIZE);
	int state;

	char msgArray[30];
	state = read(cliSockFd, msgArray, BUF_SIZE);

	if(state == -1)
	{
		perror("rcvMsg, read() error");
	}

	strcpy(msg, msgArray);

	return msg;
}
