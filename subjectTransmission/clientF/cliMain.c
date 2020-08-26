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

#define BUFSIZE 257
#define SCHSIZE 30
#define DATESIZE 20
#define TIMESIZE 10

#define BUF_SIZE 500
#define LINESIZE 500

void sndPlan(int, char*);
void rcvPlan(int, char*);
void sndInt(int, int);
int rcvInt(int);
void sndMsg(int, char*);
char* rcvMsg(int);
void notice2();
void notice1();

char* readFile(FILE*, int);
FILE* openreadFile (char*);
FILE* openwriteFile(char*);
void writeFile(FILE*, char*);
void closeFile(FILE*);

void add(char*, char*);
void edit(char*, char*);
void delet(char*, char*);
void findDay(char*);
void findSys(char*);
void findSub(char*);
void sort(char*);
void makeAll();
void rmFile(char*);

int client_len;
int cliSockFd;

struct sockaddr_in clientaddr;

char buff_rcv[BUF_SIZE + 1];
char buff_snd[BUF_SIZE + 1];
void professor()
{
	printf("\n\n");
	printf("교수모드를 시작합니다.\n\n");
	printf("*---*---*---*---*---*---*\n");
	int resultInt;
	char id[10];
	char* result;

	//교수 인증번호 입력
	printf("인증번호를 입력하세요: ");
	printf(">>>");
	scanf("%s", id);

	//인증번호를 서버로 보냄
	//write(cliSockFd, id, BUF_SIZE);
	sndMsg(cliSockFd, id);                      
	printf("waiting acception...\n");

	sleep(1);

	//////////인증결과받는 부분 시작
	result = rcvMsg(cliSockFd);
	printf("%s\n", result);         ///result는 == (server의)ACResult

	free(result);
	//strcmp넣기

	while (1)
	{
		int ans1;
		int ans2;
		char file_name[20];
		char sub[20];

		printf("=========================\n");
		printf("접근할 메뉴를 선택하세요\n");
		printf("    1. 개인 일정 \n");
		printf("    2. 과목 일정 \n");
		printf("    3. 종료 \n\n");

		scanf("%d", &ans1);
		getchar();
		sndInt(cliSockFd, ans1);					//소켓에 메뉴뭐 골랐는지 알려주는거

		if (ans1 == 1)      //개인일정 선택
		{	
			printf("=========================\n");
			printf("실행할 작업을 선택하세요\n");
			printf("    1. 개인 일정 추가\n");
			printf("    2. 개인 일정 수정\n");
			printf("    3. 개인 일정 삭제\n");
			printf("    4. 개인 일정 검색(하루)\n");
			printf("    5. 개인 일정 보기(n일)\n");

			scanf("%d", &ans2);
			getchar();

			strcpy(file_name, "개인일정.txt");
			strcpy(sub, "개인일정");

			switch (ans2)
			{
				case 1:
					add(file_name, sub);
					sort(file_name);
					break;
				case 2:
					edit(file_name,sub);
					sort(file_name);
					break;
				case 3:
					delet(file_name,sub);
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
		else if (ans1 == 2)         //과목일정 선택
		{
			printf("\n=====================\n");
			printf("과목명을 입력하세요: ");
			scanf("%s", sub);               //과목명입력
			sprintf(file_name, "%s.txt", sub);  //과목.txt
			sndMsg(cliSockFd, file_name);         //소켓이 과목.txt전달

			resultInt = rcvInt(cliSockFd);   //교수가 과목접근할 수 있는지 인증값
			//파일 받기  
			if (resultInt == 0) {
				printf("확인되지 않은 아이디입니다.\n");
				printf("Plan Helper를 종료합니다^_^!\n");
				exit(0);
			}

			else {
				rcvPlan(cliSockFd, file_name);			//파일받아오기
				printf("==========================\n");
				printf("실행할 작업을 선택하세요\n");
				printf("    1. 과목 일정 추가\n");
				printf("    2. 과목 일정 수정\n");
				printf("    3. 과목 일정 삭제\n");
				printf("    4. 과목 일정 보기(하루)\n");
				printf("    5. 과목 일정 검색(n일)\n");
				printf(">>>>");
				scanf("%d", &ans2);
				getchar();

				switch (ans2) //교수님이 선택한 기능
				{
					case 1:
						add(file_name, sub);
						break;
					case 2:
						edit(file_name,sub);
						break;
					case 3:
						delet(file_name,sub);
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
			printf("\n");
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
}
void student()
{	
	printf("\n");
	printf("학생모드를 시작합니다!\n\n");
	printf("*---*---*---*---*---*---*\n");

	////////파일받기
	rcvPlan(cliSockFd, "subject.txt");    
	
	makeAll();//all.txt만들기
	sort("all.txt");
	notice1();//알림(추가, 삭제)이랑 subjectOld.txt 지우고 subject.txt 를 subjectOld.txt로 이름바꾸기 
	printf("---------------------------\n");
	notice2();//알림(오늘날짜)
	while (1)
	{
		int ans1;
		int ans2;
		char* file_name;
		char* sub;
		printf("==============================\n");
		printf("접근할 메뉴를 선택하세요\n");
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
			printf("==========================\n");
			printf("실행할 작업을 선택하세요\n");
			printf("    1. 개인 일정 추가\n");
			printf("    2. 개인 일정 수정\n");
			printf("    3. 개인 일정 삭제\n");
			printf("    4. 개인 일정 검색(하루)\n");
			printf("    5. 개인 일정 보기(일주일)\n");
			printf(">>>>");
			scanf("%d", &ans2);
			getchar();

			file_name = "개인일정.txt";
			sub = "개인일정";

			switch (ans2)
			{
				case 1:
					add(file_name, sub);
					sort(file_name);
					makeAll();
					break;
				case 2:
					edit(file_name, sub);
					sort(file_name);
					makeAll();
					break;
				case 3:
					delet(file_name, sub);
					sort(file_name);
					makeAll();
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
			printf("=========================\n");
			printf("과목 이름을 입력하세요!\n");
			printf(">>>>");
			char subb[30];
			scanf("%s",subb);
			printf("=========================\n");
			findSub(subb);

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
			printf("\n");
			printf(" *학생 다이어리를 종료합니다.*\n");
			printf("\n*-------*-------*-------*-------*\n");
			rmFile("subjectOld.txt");
			rename("subject.txt", "subjectOld.txt");
			exit(0);
		}
		else
		{
			printf("잘못된 입력입니다.\n");
			printf("다시 입력해주세요.\n");
			printf("=========================\n");
		}

	}
}

int main(int argc, char** argv)
{
	system("clear");
	printf("------------------------------------\n");
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
	printf("안녕하세요\n");
	printf("PLAN HELPER를 시작합니다!\n");
	printf("*-----*-----*-----*-----*-----*-----*\n");
	printf("*\n");
	printf("*\n");
	while (1)
	{
		printf("교수/학생 모드를 선택하세요.\n");
		printf("    1. 교수\n");
		printf("    2. 학생\n");
		printf("    3. 종료\n");
		printf(">>> ");
		char* modeInput; 
		int mode;

		char y;
		fgets(modeInput, sizeof(modeInput), stdin);
		mode = atoi(modeInput);

		//sndInt(cliSockFd, mode);

		//printf("디버깅: 모드 전송완료\n\n");

		if (mode == 1)
		{
			//sndInt(cliSockFd, mode);                   /////////////////////여기 소켓!!!!!!!!!!!!!!!!!1
			professor();
			break;
		}
		else if (mode == 2)
		{ 
			//sndInt(cliSockFd, mode);                   /////////////////////여기 소켓!!!!!!!!!!!!!!!!!1
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
					printf("종료합니다. - plan helper\n");
					exit(0);
				} 
				else if (y == 'n') 
				{
					printf("플랜헬퍼를  계속합니다!\n\n");
					break;
				}
				else
				{
					printf("잘못된 입력입니다.\n");
					printf("다시 입력해주세요.\n");
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

void sndPlan(int cliSockFd, char* fileName)
{
	FILE* planFile = openreadFile(fileName);
	char plan[500];
	int lineNum = 0;

	while (1)
	{
		strcpy(plan, readFile(planFile, lineNum));
		if (plan == NULL)
			break;
		write(cliSockFd, plan, BUF_SIZE);
		lineNum++;
	}
}

void rcvPlan(int cliSockFd, char* fileName)
{
	char plan[500];

	int state;
	FILE* planfile = openwriteFile(fileName);
	while(1)
	{
		state = read(cliSockFd, plan, 500);
		if(state == -1)
		{
			perror("rcvPlan, read() error");
			exit(1);
		}

		if(strcmp(plan, "끝") == 0) break;
		fprintf(planfile, "%s\n", plan);
	}
	closeFile(planfile);

}

void sndInt(int cliSockFd, int inputNum)
{	
	int state; 

	char num[5];
	printf(num, "%d", inputNum);

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
	int state;
	int len = sizeof(strlen(msg));

	state = write(cliSockFd, msg, len);
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

///노티스 1, 2 최종 메인에넣어주세용 근데 찐막은아닐듯 ..? 되면 걍 하고 이상하면 금방고침요(노티스2 오픈 사용)

void notice1(){
	FILE* fp1 = NULL;
	FILE* fp2 = NULL;
	FILE* fp3 = NULL;
	FILE* fp = NULL;
	char line1[LINESIZE];
	char line2[LINESIZE];
	char line3[LINESIZE];

	sort("subject.txt");
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
	fputs("<새로운 과목  일정>\n", fp3);
	while (fgets(line2, LINESIZE, fp2) != NULL)
	{
		while (fgets(line1, LINESIZE, fp1) != NULL)
		{
			char* p = strstr(line2, line1);
			if (p = NULL)
			{
				break;
			}
			printf(" ");
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
	//	rmFile("subjectOld.txt");
	//	rename("subject.txt", "subjectOld.txt");
}

void notice2()
{
	int fd;
	char buf[BUFSIZE] = { 0 };
	char date[DATESIZE];
	int n = 0;
	//    char* file_name="all.txt";
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


	printf("<DATE: %s>\n", date);
	printf("<<오늘의 일정 >>\n");

	int length = 0;
	int lengthLine = 0;
	int line_num = 1;
	int line_count = 0;
	char temp[100];

	mode_t mode;
	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	fd = open("all.txt", O_RDONLY, mode);
	if(fd == -1) { perror("ERROR"); return;}
	while (read(fd, temp, 1)>0)
	{
		if (temp[0] != '\n')
		{

			buf[length] = temp[0];
			length++;
		}
		else
		{
			buf[length] = '\0';
			length = 0;
			if (strncmp(date, buf, 10) == 0)
			{
				printf("%d. ", line_num);
				line_num++;
				puts(buf);
			}
		}

	}
	if (line_num == 1)
	{
		printf("해당 날짜에 일정이 없습니다.\n\n");
	}

}//6/3 만듬
char* readFile(FILE* filename, int num)  //num은 읽어올 줄의 번호를 입력하는 곳 소켓안에 반복문 돌릴때 int num, num++같이 쓸 것
{
	int countline = 0;
	char text[100] = { 0 };
	char* textadd = malloc(sizeof(char) * 100);

	if (filename == NULL)
	{
		perror("error(open)");
		exit(1);
	}

	while (1) { //파일끝이 아니라면 반복
		fgets(text, 100, filename);    //한줄 읽고

		if (feof(filename)) {
			break;
		}
		if (num == countline)
		{
			for (int i = 0; i < 100; i++) { //줄 뒤에 붙은 개행문자 삭
				if (text[i] == '\n') {
					text[i] = '\0';
					break;
				}
			}
			break;
		}
		countline++; //읽은 줄수 세기
		text[0] = '\0';        //초기화
	}
	sprintf(textadd, "%s", text);
	return textadd;
}
//
FILE* openreadFile(char* file_name)
{
	FILE* fp = fopen(file_name, "r+");

	//fp = fopen(file_name, "rt");   //해당학번파일을 읽는다.
	if (fp == NULL)
	{
		perror("error(open)");
		exit(1);
	}
	return fp;
}

//고침
FILE* openwriteFile(char* file_name)
{
	FILE* fp = fopen(file_name, "a+");
	if (fp == NULL)
	{
		perror("error(open)");
		exit(1);
	}
	return fp;   //반환

}
FILE* openwriteonlyFile(char* file_name)
{
	FILE* fp = fopen(file_name, "w");

	if (fp == NULL)
	{
		perror("error(open)");
		exit(1);
	}
	return fp;   //반환

}


//고침
void writeFile(FILE* filename, char* msg)
{

	if (filename == NULL)
	{
		perror("error(open)");
		exit(1);
	}

	int len = strlen(msg);

	fwrite(msg, len, 1, filename);
	fwrite("\n", 1, 1, filename);
}
//되는듯
void closeFile(FILE* filename)
{
	int result = fclose(filename);
}
//찐 끝


void rmFile(char* file_name)
{
	int result = remove(file_name);
	if (result == -1)
	{
		perror("ERROR(파일 삭제 실패)");
		exit(1);
	}
}
void add(char* file_name, char* sub)
{
	FILE* fp=NULL;
	char buf[BUFSIZE];
	char schedule[SCHSIZE];
	char date[DATESIZE];
	char time[TIMESIZE];

	fp = openwriteFile(file_name);
	// fp=fopen(file_name, "r+");
	int i = 0;

	printf("일정명을 입력하세요.: ");
	scanf("%s", schedule);
	getchar();

	printf("날짜를 입력하세요.\n");
	printf("    ex)2020/01/01\n");
	scanf("%s", date);
	getchar();

	printf("시간을 입력하세요.\n");
	printf("(시간을 지정하지 않을 시 00:00 로 입력하세요.)\n");
	scanf("%s", time);
	getchar();
	sprintf(buf, "%s%s%s%s%s%s%s", date, " ", time, " <", sub, "> ", schedule);
	writeFile(fp, buf);

	printf("추가한 일정: %s\n", buf);
	fclose(fp);
}
void edit(char* file_name,char*sub)
{
	FILE* fp=NULL;
	FILE* fp1 = NULL;
	FILE* fp2 = NULL;
	//FILE* fp3 = NULL;
	char buf[BUFSIZE] = { 0 };
	char date[DATESIZE] = { 0 };
	char time[TIMESIZE] = { 0 };
	char newdate[DATESIZE] = { 0 };
	char newtime[TIMESIZE] = { 0 };
	char schedule[SCHSIZE] = { 0 };
	int line_count = 0;
	char temp[257] = { 0 };
	int choice = 3;
	int line_count2 = 0;
	char str[257] = { 0 };
	char str2[257] = { 0 };




	printf("수정할 일정의 일정명을 입력하세요.\n");
	scanf("%s", schedule);
	getchar();

	printf("수정할 일정의 날짜를 입력하세요.\n");
	printf("    ex) 2020/01/01 \n");
	scanf("%s", date);
	getchar();


	while (1)
	{
		fp = openreadFile(file_name); //해당 일자 메모가 들어있는 파일 오픈(읽기)
		strcpy(str, readFile(fp, line_count));
		if (feof(fp) != 0)
		{
			//printf("수정할 일정이 없습니다.\n");
			break;
		}
		else if ((strstr(str, date) != NULL) && (strstr(str, schedule) != NULL))
		{
			printf("\n%s\n", str);

			sprintf(time, "%c%c%c%c%c", str[11], str[12], str[13], str[14], str[15]);//여기까지 ㅇㅋ
			//linecount 기억하기
			while (choice == 3)
			{
				printf("수정할 항목을 선택하세요\n");
				printf("    1. 날짜\n");
				printf("    2. 시간\n");
				scanf("%d", &choice);

				// char* strp = &str;

				switch (choice)
				{
					case 1://날짜바꾸기
						printf("새로운 날짜를 입력하세요\n");
						printf("    ex)2020/01/01\n");
						scanf("%s", newdate);
						getchar();
						closeFile(fp);
						//char* datep = &date;
						//char* newdatep = &newdate;

						//printf("before sprintf");
						sprintf(temp, "%s%s%s%s%s%s%s", newdate, " ", time, " <", sub, "> ", schedule);
						// strcpy(temp, change_word(strp, datep, newdatep));
						printf("바뀐 일정: %s\n",temp);
						break;

					case 2://시간바꾸기
						printf("새로운 시간을 입력하세요\n");
						printf("    (시간을 지정하지 않을 시 00:00 로 입력하세요.)\n");
						scanf("%s", newtime);
						getchar();
						closeFile(fp);
			//			printf("before sprintf");
						//char* timep = &time;
						//char* newtimep = &newtime;
						//strcpy(temp, change_word(strp, timep, newtimep));
						sprintf(temp, "%s%s%s%s%s%s%s", date, " ", newtime, " <", sub, "> ", schedule);
						printf("수정된 일정: %s\n", temp);
						break;

					default:
						printf("잘못된 입력입니다.\n");
						printf("다시 입력해주세요.\n");
						choice = 3;
						break;
				}
			}
			/* printf("1");
			   fp1 = openwriteFile(file_name);
			   printf("2");
			   writeFile(fp1, temp);
			   printf("3");
			   closeFile(fp1)/;*/
			while (1)
			{
				fp1 = openreadFile(file_name);//열고
				fp2 = openwriteFile("filefile.txt");
				//printf("5"); 
				strcpy(buf, readFile(fp1, line_count2));
			//	printf(buf);
				if (feof(fp1)!=0)
				{
					break;
				}
				if (line_count != line_count2)
				{
			//		printf("beforewritenewFile");                   //같으면
					writeFile(fp2, buf);
			//		printf("afterwritenewFile");

				}//if문 끝
				if(line_count==line_count2)
				{
					writeFile(fp2, temp);
				}
				line_count2++;
				closeFile(fp1);//닫아
				closeFile(fp2);
			}//while end
			rmFile(file_name);//삭제
			rename("filefile.txt", file_name);//새로만든거이름바꾸기
		}
		line_count++;
	}
}
void delet(char* file_name,char*sub)
{
	FILE* fp=NULL;
	FILE* fp1 = NULL;
	FILE* fp2 = NULL;
	//FILE* fp3 = NULL;
	char buf[BUFSIZE] = { 0 };
	char date[DATESIZE] = { 0 };
	char time[TIMESIZE] = { 0 };
	char newdate[DATESIZE] = { 0 };
	char newtime[TIMESIZE] = { 0 };
	char schedule[SCHSIZE] = { 0 };
	int line_count = 0;
	char temp[257] = { 0 };
	int choice = 3;
	int line_count2 = 0;
	char str[257] = { 0 };
	char str2[257] = { 0 };




	printf("삭제할 일정의 일정명을 입력하세요.\n");
	scanf("%s", schedule);
	getchar();

	printf("삭제할 일정의 날짜를 입력하세요.\n");
	printf("    ex) 2020/01/01 \n");
	scanf("%s", date);
	getchar();


	while (1)
	{
		fp = openreadFile(file_name); //해당 일자 메모가 들어있는 파일 오픈(읽기)
		strcpy(str, readFile(fp, line_count));
		if (feof(fp) != 0)
		{
			//printf("삭제할 일정이 없습니다.\n");
			break;
		}
		else if ((strstr(str, date) != NULL) && (strstr(str, schedule) != NULL))
		{
			printf("\n%s\n", str);

			sprintf(time, "%c%c%c%c%c", str[11], str[12], str[13], str[14], str[15]);//여기까지 ㅇㅋ
			//linecount 기억하기

			/* printf("1");
			   fp1 = openwriteFile(file_name);
			   printf("2");
			   writeFile(fp1, temp);
			   printf("3");
			   closeFile(fp1);*/
			while (1)
			{
				fp1 = openreadFile(file_name);//열고
				fp2 = openwriteFile("filefile.txt");
				//printf("5"); 
				strcpy(buf, readFile(fp1, line_count2));
			//	printf(buf);
				if (feof(fp1)!=0)
				{
					break;
				}
				if (line_count != line_count2)
				{
			//		printf("beforewritenewFile");                   //같으면
					writeFile(fp2, buf);
			//		printf("afterwritenewFile");

				}//if문 끝

				line_count2++;
				closeFile(fp1);//닫아
				closeFile(fp2);
			}//while end
			rmFile(file_name);//삭제
			rename("filefile.txt", file_name);//새로만든거이름바꾸기
		}
		line_count++;
	}
}
void findDay(char* file_name)
{
	FILE* fp;//= openreadFile(file_name);//읽기로 파일열기
	int line_num = 1;
	int line_count = 0;
	char buf[BUFSIZE] = { 0 };
	char date[DATESIZE];
	char text[257];

	printf("일정을 보고 싶은 날짜를 입력하세요.\n");
	printf("    ex) 2020/01/01\n");
	scanf("%s", date);
	getchar();
	printf("<<%s의 일정>>\n", date);//여기까지 됨


	while (1)
	{
		fp = openreadFile(file_name);//읽기로 파일열기
		strcpy(text, readFile(fp, line_count));
		strcpy(buf, text); //한줄읽어서 저장
		if ((feof(fp)) != 0) break;
		if (strstr(buf, date) != NULL)//…?
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
}
void findSys(char* file_name)
{
	FILE* fp;
	char buf[BUFSIZE] = { 0 };
	char date[DATESIZE];
	int n = 0;
	printf("\n");
	printf("<구간 일정 검색>\n");
	printf("오늘을  기준으로 검색하고 싶은 일수를 선택하세요\n");
	printf("    현재 날짜 기준으로 7일의 일정을 보고 싶은 경우 ex) 7\n");
	scanf("%d", &n);
	getchar();

	printf("****************");
	for (int i = 0; i < n; i++)
	{
		time_t t = time(NULL) + i * (24 * 60 * 60);
		struct tm tm = *localtime(&t);
		char year[4];
		char month[2];
		char day[2];
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


		printf("\n<<%s의 일정 >>\n", date);

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
	}
}
void findSub(char* sub)
{
	FILE* fp;
	char buf[BUFSIZE] = { 0 };
	char* file_name = "all.txt";
	printf("findsub start: ");
	printf("<<%s>>\n", sub);
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
		//strcpy(buf, readFile(fd, line_count)); //한줄읽어서 저장
		if (strstr(buf, sub) != NULL)
		{	printf("%d. ", line_num);
		line_num++;
		puts(buf);}
		line_count++;
		closeFile(fp);
	}
	if (line_num == 1)
	{
		printf("%s의 일정이 없습니다.\n\n", sub);
	}
	//      closeFile(fd);
}
//찐 끝 !
void sort(char* file_name)
{
	//debug: 파일 이름 잘 들어왔나 확인
	// printf("\"%s\"을 엽니다.\n\n", file_name);

	char* buf[100][LINESIZE] = { 0 };
	FILE* fp = fopen(file_name, "r");
	char temp[LINESIZE];

	if (fp == NULL)
	{
		perror("error(open)");
		exit(1);
	}

	int k = 0;
	while (1)
	{
		fgets(buf[k], LINESIZE, fp);

		if (feof(fp) != 0) break;
		k++;
	}

	fclose(fp);


	for (int i = 0; i < k; i++)
	{

		for (int j = 0; j < k - 1 - i; j++)
		{

			if (buf[i][0] == '\0') break;

			if (strcmp(buf[j], buf[j + 1]) > 0)
			{
				memset(temp, 0, LINESIZE);
				strncpy(temp, buf[j + 1], LINESIZE);
				strncpy(buf[j + 1], buf[j], LINESIZE);
				strncpy(buf[j], temp, LINESIZE);
			}
		}
	}


	fp = fopen(file_name, "w");
	if (fp == NULL)
	{
		perror("error(open)");
		exit(1);
	}
	else {
		//printf("홍은이공주님");

		for (int i = 0; i < k; i++)
		{
			int len = strlen(buf[i]);
			fwrite(buf[i], len, 1, fp);
		}
	}
	fclose(fp);
}

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
	while (fgets(line, LINESIZE,fp1)!=NULL)
	{
		//         fgets(line, LINESIZE, fp1);
		//       if (feof(fp1)!=0)
		//fputs(line, fp3);
		//             break;
		fputs(line, fp3);
	}
	while (fgets(line, LINESIZE, fp2)!=NULL)
	{
		// fgets(line, LINESIZE, fp2);
		//if (feof(fp2)!=0)
		//fputs(line, fp3);
		//      break;
		fputs(line, fp3);
	}
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
}

///노티스 1, 2 최종 메인에넣어주세용 근데 찐막은아닐듯 ..? 되면 걍 하고 이상하면 금방고침요(노티스2 오픈 사용)

