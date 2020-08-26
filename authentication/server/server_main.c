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
#include <netdb.h>
#include <errno.h>

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
extern char* CheckClient(char*);

//server_funcDB.c
extern char* readFile(FILE*, int);
extern void writeFile(FILE*, char*);
extern FILE* openreadFile(char*);
extern FILE* openwriteFile(char*);
extern void closeFile(FILE*);

int cliCount = 0;
int cliSock[MAX_CLI];
sem_t bin_sem1;

int main(int argc, char** argv)
{
    system("clear");
    printf("***********PLAN HELPER's SERVER***********\n\n");
   
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

    if (bind(servSockFd, (struct sockaddr*) & servAddr, sizeof(servAddr)) < 0)
    {
        perror("bind error");
        exit(0);
    }
    if (listen(servSockFd, 5) < 0)
    {
        perror("listen error");
        exit(0);
    }

    while (1)
    {
        int cliAddrLen = sizeof(cliAddr);
        cliSockFd = accept(servSockFd, (struct sockaddr*) & cliAddr, &cliAddrLen);
	
        if (cliSockFd < 0)
        {
            perror("Socket accept error\n");
            exit(1);
        }
	printf("new client connection\n");
        printf("client Socket file descript Num: %d\n", cliSockFd);
        //Debug
        /*
                   sem_wait(&bin_sem1);
                   cliSock[cliCount++] = cliSockFd;
                   sem_post(&bin_sem1);
                   printf("세마포어 생성했습니다.\n");
                 */

        threadId = pthread_create(&thread, NULL, serverMenu, (void*)&cliSockFd);

        pthread_detach(threadId);

        if (threadId != 0)
        {
            perror("main: Thread creation failed\n");
            exit(EXIT_FAILURE);
        }

        printf("main: %d번째 스레드 생성!\n", cliCount + 1);
        printf("==================================\n\n");

	cliCount++;
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////서버 스레드 루틴 function////////////////////////////////

void* serverMenu(void* arg)
{
    int cliSockFd = *(int*)arg;
sleep(1);
    int cliMode = rcvInt(cliSockFd);
    printf("========================================-\n");
    printf(" 사용자 선택 모드(1:교수 2:학생 3:종료): %d\n\n", cliMode); //debug
    char* mode = decideMode(cliMode);
    printf("현재 사용자는 %s입니다.\n", mode);
    //debug: mode = "PROFFESOR"혹은 "STUDENT" 로 저장됨
    sleep(1);
    //서버의 교수메뉴 시작
    if (cliMode == 1)
    {
        printf("교수ver PLAN HELPER 시작합니다.\n\n"); //debug

        char* idRcv;
        while (1)
        {
            idRcv = rcvMsg(cliSockFd);
            if (strlen(idRcv) >= 3) break;
        }
        char* check = CheckID(mode, idRcv);

        ///// 저장되는 값은 교수x의아이디.txt or 없음
        printf("ID 인증이완료되었습니다.\n"); //debug

        char* ACResult = CheckClient(check);

        printf("ACResult : \"%s\"\n\n", ACResult); //debug

        sndMsg(cliSockFd, ACResult);

        while (1) //★
        {
            int menuu = rcvInt(cliSockFd);
            printf("%d\n", menuu);
            if (menuu == 3)
                break;
            while (menuu != 2)
            {
                menuu = rcvInt(cliSockFd);
                if (menuu == 1) break;
            }
	    
            if (strcmp(check, "없음") != 0 && menuu == 2) //아이디가 리스트에 있으면과목인증 진행
            {
                char subjname[BUF_SIZE];
                char* subjnameP;
                subjname[0] = '\0';

                subjnameP = rcvMsg(cliSockFd); //접근하려는 과목명을 받아서
                strcpy(subjname, subjnameP);   //쓰기 쉽게 배열에 저장

                ////접근가능 과목명 읽으러 가는중 교수아이디.txt를 열거야
                sprintf(subjname, "%s.txt", subjname);  //디버깅문항 과목이름.txt
                int num = 0;                            //linenumber
                int same = 0;                           //과목 인증 여부
                char* title;
                FILE* subj_list;
                while (1) //교수가 접근할 수 있는 과목리스트
                {
                    subj_list = openreadFile(check);
                    printf("num: %d\n", num);           //디버깅 문항 과목수,인덱스
                    title = readFile(subj_list, num); //과목이름 한줄씩읽기

		    if(title[0] == '\0'){
			break;
		    }

                    sprintf(title, "%s.txt", title);                                          //디버깅 문항

                    printf("클라이언트 입력받은 subjname: %s\n", subjname); //디버깅문항
                    printf("파일에서 읽은 title: %s\n\n", title);           //디버깅문항

                    if (strcmp(title, subjname) == 0) //교수의 입력과목과 리스트안의 과목이름이 동일하면
                    {
                        printf("subjname: %s\n", subjname); //서버에 확인
                        printf("title: %s\n\n", title);     //하도록 출략하고
                        same++;                             //인증됬다는 표시를 하고
                        break;                              //반복문 탈출
                    }
                    num++;
                }
                sndInt(cliSockFd, same);
                //숫자로 된 인증결과 0이면 실패 아니면 성공
                closeFile(subj_list);
                if (same != 0)
                {
                    sndPlan(cliSockFd, subjname);
                    printf("플랜전송완료");

                    rcvPlan(cliSockFd, subjname);
                    free(subjname);
                    free(title);
                }
            }
            free(idRcv);
        }
    printf("서비스 종료됩니다.\n");
 
   }
    ///// 학생mode 시작
    else if (cliMode == 2)
    {
	printf("학생ver PLAN HEPLER 시작합니다.\n\n");
        char* idRcv;
        while (1)
        {
            idRcv = rcvMsg(cliSockFd);
            if (strlen(idRcv) >= 3)
                break;
        }
        char* check = CheckID(mode, idRcv); 
                              ///// msg: 학생아이디.txt or 없음
        printf("ID 인증이 왼료되었습니다.\n"); //debug
        char* ACResult = CheckClient(check);

        printf("ACResult : \"%s\"\n\n", ACResult); //debug
        sndMsg(cliSockFd, ACResult);

        free(idRcv);

        if (strcmp(check, "없음") != 0) //학번이 리스트에 있으면 진행 //strcmp로 수정
        {
            FILE* fd_num; ////수강하는 과목명 읽으러 가는중
            int num = 0;
            while (1)
            {
                fd_num = openreadFile(check); //학번.txt을 열음
                printf("num: %d번\n", num);
                char* title = readFile(fd_num, num); //수강하는 과목명
                num++;
                if (title == NULL)
                {
                    break;
                }
                printf("과목명: %s\n", title);
                char subjname[80] = { 0 }; //위에서 읽은 과목명에 .txt붙이기
                sprintf(subjname, "%s.txt", title);
                /////이제 과목일정 읽어 보낸다.
                sndPlan(cliSockFd, subjname);
            }
            closeFile(fd_num);
        }
	printf("서비스 종료됩니다.\n");
        free(ACResult);
        free(check);
    }
	printf("스레드 종료, cliSockFd: %d\n", cliSockFd);
        close(cliSockFd);
	printf("=================================\n");

}

//////////////////소켓

void rcvPlan(int cliSockFd, char* filename) {

    char buffforFile[1025];

    memset(buffforFile, '0', sizeof(buffforFile));
    while (1)
    {
        FILE* fp = fopen(filename, "wb");
        int tot = 0; int b;
        if (fp != NULL)
        {
            while ((b = recv(cliSockFd, buffforFile, 1024, 0)) > 0)
            {
                tot += b;
                fwrite(buffforFile, 1, b, fp);
            }

            printf("Received byte: %d\n", tot);
            if (b < 0)
                perror("Receiving");

            fclose(fp);
        }
        else
        {
            perror("File");
        }
    }

}

void sndPlan(int cliSockFd, char* filename) {

    int b;
    char buffforFile[1024];
    char sendbuffer[100];

    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("File");
        return;
    }

    while (1)
    {
	b = fgets(sendbuffer, sizeof(sendbuffer), fp);
        if(feof(fp)!=0) break;
	printf("전송할 문자열: %s\n", sendbuffer);
	sleep(1);
        send(cliSockFd, sendbuffer, b, 0);
	memset(sendbuffer, 0, sizeof(sendbuffer));
    }

    fclose(fp);

}


void sndInt(int cliSockFd, int inputNum)
{
    int state;
    char num[5];
    sprintf(num, "%d", inputNum);
    state = write(cliSockFd, num, sizeof(num));
    if (state == -1)
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
    if (state == -1)
    {
        perror("rcvInt, read() error");
        exit(1);
    }

    int chartoNum = atoi(num);
    return chartoNum;
}
void sndMsg(int cliSockFd, char* msg)
{
    int state;
    char msgarr[500];
    strcpy(msgarr, msg);
    state = write(cliSockFd, msgarr, BUF_SIZE);
    if (state < 0)
    {
        perror("sndMsg, transmission error");
        exit(1);
    }
}
char* rcvMsg(int cliSockFd)
{
    char* msg = (char*)malloc(sizeof(char) * BUF_SIZE);
    int state;
    char msgmsg[BUF_SIZE];
    sleep(1);
    state = read(cliSockFd, msgmsg, BUF_SIZE);
    if (state == -1)
    {
        perror("rcvMsg, read() error");
    }
    strcpy(msg, msgmsg);
    return msg;
}
