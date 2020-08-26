#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>

#define BUF_SIZE 500
#define LINESIZE 500
#define DATESIZE 20
#define BUFSIZE 257
#define TIMESIZE 10

void sndPlan(int, char*);
void rcvPlan(int, char*);
void sndInt(int, int);
int rcvInt(int);
void sndMsg(int, char*);
char* rcvMsg(int);
void notice2();
void notice1();
void makeAll();

extern char* readFile(FILE*, int);
extern FILE* openreadFile(char*);
extern FILE* openwriteFile(char*);
extern void writeFile(FILE*, char*);
extern void closeFile(FILE*);

extern void add(char*, char*);
extern void edit(char*);
extern void delet(char*);
extern void findDay(char*);
extern void findSys(char*);
extern void findSub(char*);
extern void sort(char*);
extern void makeAll();
extern void rmFile(char*);

int client_len;
int cliSockFd;

struct sockaddr_in clientaddr;

char buff_rcv[BUF_SIZE + 1];
char buff_snd[BUF_SIZE + 1];
void professor()
{
	printf(" 교수모드를 시작합니다.\n");
	printf("==========================\n");
	int resultInt;
	char id[10];
	char* result;

	//교수 인증번호 입력
	printf("인증번호를 입력하세요: ");
	scanf("%s", id);

	printf("==========================\n"); //debug
	//인증번호를 서버로 보냄 이건.txt붙이면 안돼 비교하는 거라서
	sndMsg(cliSockFd, id);
	printf("waiting acception…\n\n");
	sleep(1);

	//////////인증결과받는 부분 시작
	result = rcvMsg(cliSockFd);
	printf("%s\n\n", result);         ///result는 == (server의)ACResult

	while (1)
	{
		if (strcmp(result, "일치하지 않는 아이디입니다.")==0) break;

		int ans1;
		int ans2;
		char* file_name;
		char filename[20];
		char* sub;
		
		printf("============================\n");
		printf(" 접근할 메뉴를 선택하세요\n");
		printf("    1. 개인 일정 \n");
		printf("    2. 과목 일정 \n");
		printf("    3. 종료 \n\n");

		scanf("%d", &ans1);
		getchar();
		sndInt(cliSockFd, ans1);                                        //소켓에 메뉴뭐 골랐는>지 알려주는거

		if (ans1 == 1)      //개인일정 선택
		{	
                        printf("============================\n");
			printf(" 실행할 작업을 선택하세요\n");
			printf("    1. 개인 일정 추가\n");
			printf("    2. 개인 일정 수정\n");
			printf("    3. 개인 일정 삭제\n");
			printf("    4. 개인 일정 검색(하루)\n");
			printf("    5. 개인 일정 보기(n일)\n");

			scanf("%d", &ans2);
			getchar();

			file_name = "개인일정.txt";
			sub = "개인일정";
			switch (ans2)
			{
				case 1:
					add(file_name, sub);
					break;
				case 2:
					edit(file_name);
					break;
				case 3:
					delet(file_name);
					break;
				case 4:
					findDay(file_name);
					break;
				case 5:
					findSys(file_name);
					break;
				default:
					printf("잘못된 입력입니다.\n");
					printf("다시 입력해주세요.\n");
					break;
			}
		}
		else if (ans1 == 2)         //과목일정 선택
		{
			printf("과목명을 입력하세요>> ");
			scanf("%s", filename);               //과목명입력
			getchar();
			sndMsg(cliSockFd, filename);         //소켓이 과목이름만전달
			resultInt = rcvInt(cliSockFd);   //교수가 과목접근할 수 있는지 인증값


			if (resultInt == 0) {
				printf("확인되지 않은 아이디입니다.\n\n");
				exit(1);
			}

			else {
				sprintf(file_name, "%s.txt", filename); 
				printf("%s과목일정을 불러옵니다....\n\n", filename);
				rcvPlan(cliSockFd, file_name);                  //파일>받아  오기

				printf("실행할 작업을 선택하세요\n");
				printf("    1. 과목 일정 추가\n");
				printf("    2. 과목 일정 수정\n");
				printf("    3. 과목 일정 삭제\n");
				printf("    4. 과목 일정 보기(하루)\n");
				printf("    5. 과목 일정 검색(n일)\n");

				scanf("%d", &ans2);
				getchar();

				switch (ans2) //교수님이 선택한 기능
				{
					case 1:
						add(file_name, sub);
						break;
					case 2:
						edit(file_name);
						break;
					case 3:
						delet(file_name);
						break;
					case 4:
						findDay(file_name);
						break;
					case 5:
						findSys(file_name);
						break;
					default:
						printf("잘못된 입력입니다.\n");
						printf("다시 입력해주세요.\n");
						break;
				}
			}

		}
		else if (ans1 == 3)
		{
			printf("교수 다이어리를 종료합니다.\n\n");
			sndPlan(cliSockFd, file_name);     ///////파일 업로드   여기 소솟ㅁㄴ소소켓
			rmFile(file_name); //파일 삭제
			exit(0);
		}
		else
		{
			printf("잘못된 입력입니다.\n");
			printf("다시 입력해주세요.\n");
		}
	}
        printf("  서비스 종료됩니다.\n");
	printf("==========================\n");
}


void student()
{
        printf("============================\n");
	printf(" 학생모드를 시작합니다.\n\n");
	int resultInt;
	char* result;
	char id[10];
	//학생인증번호 입력
	printf("학번을 입력하세요: ");
	scanf("%s", id);
	sndMsg(cliSockFd, id);               //학번보내는거
	//////////인증결과받기
	result = rcvMsg(cliSockFd);
	if(strcmp(result, "인증되었습니다.") != 0)
	{
		printf("존재하지 않는 학번입니다.\n\n");
		exit(0);
	}
	printf(result);         ///result는 == (server의)ACResult
	free(result);

        printf("============================\n");
	printf("수강과목일정을 불러옵니다...\n\n");
	rcvPlan(cliSockFd, "subject.txt");

	printf("before makeAll\n");
	makeAll();//all.txt만들기
	printf("after makeAll before sort\n");
	sort("all.txt");
	printf("after sort  before notice1\n");
	notice1();//알림(추가, 삭제)이랑 subjectOld.txt 지우고 subject.txt 를 subjectOld.txt로 이름바꾸기
	printf("after notice1 before notice2\n");
	notice2();//알림(오늘날짜)
	printf("after notice2\n");
	while (1)
	{
		int ans1;
		int ans2;
		char* file_name;
		char* sub;
		printf("==============================\n");
		printf("접근할 메뉴를 선택하세요\n");
		printf("------------------------------\n");
		printf("    1. 개인 일정 \n");
		printf("    2. 과목 일정 보기\n");
		printf("    3. 모든 일정 보기(하루)\n");
		printf("    4. 모든 일정 보기(n일)\n");
		printf("    5. 종료\n");
		printf("------------------------------\n");
		printf(">>>>");

		scanf("%d", &ans1);
		getchar();

		if (ans1 == 1)
		{
			printf("============================\n");
			printf("실행할 작업을 선택하세요\n"); 
			printf("----------------------------\n");                  
			printf("    1. 개인 일정 추가\n");
			printf("    2. 개인 일정 수정\n");
			printf("    3. 개인 일정 삭제\n");
			printf("    4. 개인 일정 검색(하루)\n");
			printf("    5. 개인 일정 보기(일주일)\n");
			printf("-----------------------------\n");
			scanf("%d", &ans2);
			getchar();

			file_name = "개인일정.txt";
			sub = "개인일정";

			switch (ans2)
			{
				case 1:
					add(file_name, sub);
					sort(file_name);
					break;
				case 2:
					edit(file_name);
					sort(file_name);
					break;
				case 3:
					delet(file_name);
					sort(file_name);
					break;
				case 4:
					findDay(file_name);
					break;
				case 5:
					findSys(file_name);
					break;
				default:
					printf("잘못된 입력입니다.\n");
					printf("다시 입력해주세요.\n");
					break;
			}
		}
		else if (ans1 == 2)
		{
			printf("----------------------------\n");
			printf("과목 이름을 입력하세요!\n");
			printf(">>>>");
			gets(sub);

			findSub(sub);
		}
		else if (ans1 == 3)
		{
			file_name = "all.txt";
			findDay(file_name);
		}
		else if (ans1 == 4)
		{
			file_name = "all.txt";
			findSys(file_name);
		}
		else if (ans1 == 5)
		{
			printf("학생 모드를 종료합니다. \n\n");
			exit(0);
		}
		else
		{
			printf("잘못된 입력입니다.\n");
			printf("다시 입력해주세요\n");
		}

	}
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("Usage: ./client [port]\n");
		printf("예: ./client 4444\n");
		exit(0);
	}

	cliSockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (cliSockFd <= 0)
	{
		perror("소켓생성 오류");
		exit(0);
	}

	clientaddr.sin_family = AF_INET;
	clientaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	clientaddr.sin_port = htons(atoi(argv[1]));
	client_len = sizeof(clientaddr);

	if (connect(cliSockFd, (struct sockaddr*) & clientaddr, client_len) < 0)
	{
		perror("Connect error");
		exit(0);
	}
	system("clear");
	printf("**********PLAN HELPER***********\n");
	printf("플랜 핼퍼가 실행되었습니다.\n");
	while (1)
	{
		printf("모드를 선택해주세요!.\n");
		printf("    1. 교수\n");
		printf("    2. 학생\n");
		printf("    3. 종료\n");
		printf(">>>>");

		char modeInput[5];
		int mode;

		char y;
		//fgets(modeInput, sizeof(modeInput), stdin);
		//mode = atoi(modeInput);
		scanf("%d", &mode);

		if (mode == 1)
		{
			sndInt(cliSockFd, mode);                   /////////////////////여기 소켓!!!!!!!!!!!!!!!!!1
			professor();
			break;
		}
		else if (mode == 2)
		{
			sndInt(cliSockFd, mode);                   /////////////////////여기 소켓!!!!!!!!!!!!!!!!!1
			student();
			break;
		}
		else if (mode == 3)
		{
			sndInt(cliSockFd, mode);

			while (1)
			{
				printf("Plan Helper를 종료하시겠습니까? (y/n):");
				scanf("%c", &y);
				if (y == 'y')
				{
					printf("다음에 다시 만나요! - plan helper\n");
					exit(0);
				}
				else if (y == 'n')
				{
					printf("프로그램을 계속합니다!\n\n");
					break;
				}
				else
				{
					printf("잘못된 입력입니다.\n");
					printf("다시 입력해주세요\n\n");
				}
			}
		}
		else
		{
			printf("잘못된 입력입니다.\n");
			printf("다시 입력해주세요\n");
		}
	}

	close(cliSockFd);
	exit(0);
}


void rcvPlan(int cliSockFd, char* filename) {
	printf("rcvPlan() 실행함\n"); //debug
	char buffforFile[1025];

	memset(buffforFile, 0, sizeof(buffforFile));

	FILE* fp = fopen(filename, "wb");
	int tot = 0; int b;
	if (fp != NULL)
	{
		printf("소켓으로 문장 전송 받기 직전\n");
		sleep(1);
		while ((b = read(cliSockFd, buffforFile, 1024)) > 0)
		{
			printf("Received 1 line byte: %d\n", b);
			printf("전송받은 일정: \"%s\"\n", buffforFile);
			printf("bufforFile strlen:%d\n", strlen(buffforFile));

			if(strcmp(buffforFile, "끝") == 0|| strlen(buffforFile) == 0)
			{
				printf("끝이당\n");
				break;
			}
			printf("recv while에 들어오긴 했니?\n");
			fwrite(buffforFile, 1, b, fp);

			memset(buffforFile, 0, 1025);
			printf("초기화 후 바이트: %d, %s\n\n", sizeof(buffforFile), buffforFile);
		}

		printf("%s\n", buffforFile);
		if (b < 0)
			perror("Receiving");

		fclose(fp);
	}
	else
	{
		perror("File");
	}
	printf("rcvPlan 종료\n"); 
}

void sndPlan(int cliSockFd, char* filename) {

	int b;
	char buffforFile[1024];
	char sendbuffer[100];

	FILE* fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("File");
		return; //고침
	}

	while ((b = fread(sendbuffer, 1, sizeof(sendbuffer), fp)) > 0) {
		send(cliSockFd, sendbuffer, b, 0);
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
	printf("소켓보낼 문자열:%s\n", msgarr);
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



////찐막 메이크올 메인에넣어주세용
void  makeAll()
{
	FILE* fp1 = NULL;
	FILE* fp2 = NULL;
	FILE* fp3 = NULL;
	FILE* fp=NULL;
	char line[LINESIZE];
	if ((fp1 = fopen("개인일정.txt", "r")) == NULL)
	{
		if((fp=fopen("개인일정.txt","w"))==NULL)
		{
			perror("file open error1/n");
			exit(1);
		}
		//fprintf(fp, "첫 실행");
		//perror("file open error1\n");
		//exit(1);
		fclose(fp);
		if((fp1=fopen("개인일정.txt","r"))==NULL)
		{
			perror("file open error1/n");
			exit(1);
		}

	}
	if ((fp2 = fopen("subject.txt", "r")) == NULL)
	{
		perror("file open error2\n");
		exit(1);
	}
	if ((fp3 = fopen("all.txt", "w")) == NULL)
	{
		perror("file open error3\n");
		exit(1);
	}
	while (1)
	{
		fgets(line, LINESIZE, fp1);
		if (feof(fp1)!=0)
			break;
		fputs(line, fp3);
	}
	while (1)
	{
		fgets(line, LINESIZE, fp2);
		if (feof(fp2)!=0)
			break;
		fputs(line, fp3);
	}
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
}

void notice1(){
	FILE* fp1 = NULL;
	FILE* fp2 = NULL;
	FILE* fp3 = NULL;
	FILE* fp = NULL;
	char line1[LINESIZE];
	char line2[LINESIZE];
	char line3[LINESIZE];
	if ((fp1 = fopen("subject.txt", "r")) == NULL)
	{
		perror("noyice1()\nfile open error1\n");
		exit(1);
	}
	if ((fp2 = fopen("subjectOld.txt", "r")) == NULL)
	{
		if ((fp = fopen("subjectOld.txt", "w")) == NULL) {

			perror("noyice1()\nfile open error2\n");
			exit(1);
		}

		fclose(fp);
		if ((fp2 = fopen("subjectOld.txt", "r")) == NULL)
		{
			perror("noyice1()\nfile open error2\n");
			exit(1);
		}

	}
	if ((fp3 = fopen("notice.txt", "w")) == NULL)
	{
		perror("notice1()file open error3\n");
		exit(1);
	}
	fputs("<추가된 일정>\n", fp3);
	while (fgets(line2, LINESIZE, fp2) != NULL)
	{
		while (fgets(line1, LINESIZE, fp1) != NULL)
		{
			char* p = strstr(line2, line1);
			if (p = NULL)
			{
				break;
			}
			fputs(line1, fp3);
		}
	}
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	if ((fp3 = fopen("notice.txt", "r")) == NULL)
	{
		perror("file open error3\n");
		exit(1);
	}
	while (fgets(line3, LINESIZE, fp3) != NULL)
	{
		printf(line3);
	}
	fclose(fp3);
	rmFile("subjectOld.txt");
	rename("subject.txt", "subjectOld.txt");
}

void notice2()
{
	FILE* fp;
	char buf[BUFSIZE] = { 0 };
	char date[DATESIZE];
	int n = 0;
	char *file_name="all.txt";
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char year[5];
	char month[3];
	char day[3];
	sprintf(year, "%d", tm.tm_year + 1900);
	if (tm.tm_mon + 1 < 10)
		sprintf(month, "%s%d", "0", tm.tm_mon + 1);
	else
		sprintf(month, "%d", tm.tm_mon + 1);
	if (tm.tm_mday < 10)
		sprintf(day, "%s%d", "0", tm.tm_mday);
	else
		sprintf(day, "%d", tm.tm_mday);
	sprintf(date, "%s%s%s%s%s", year, "/", month, "/", day);


	printf("<<%s의 일정 >>\n", date);

	int line_num = 1;
	int line_count = 0;
	while (1)
	{
		fp = openreadFile(file_name);
		strcpy(buf, readFile(fp, line_count)); //한줄읽어서 저장
		if (feof(fp) != 0)//파일끝이면
		{
			break;
		}
		if (strstr(buf, date) != NULL)//if (strncmp(date, buf, 10) == 0)
		{
			printf("%d. ", line_num);
			puts(buf);
			line_num++;
		}
		line_count++;
		closeFile(fp);
	}

	if (line_num == 1)
	{
		printf("해당 날짜에 일정이 없습니다.\n\n");
	}



}//6/3 만듬
