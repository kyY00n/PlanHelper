/////////////////////////AC 모듈/////////////////////////
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BUF_SIZE 100

extern char* readFile(FILE*, int);
extern FILE* openreadFile(char*);
extern FILE* openwriteFile(char*);
extern void closeFile(FILE*);

char* decideMode(int mode)
{
    char* file_name = malloc(sizeof(char) * BUF_SIZE);

    if (mode == 1)                                  //교수라는 식별번호가 들>어오면
        file_name = "PROFESSOR";
    else if (mode == 2)                             //학생이라는 식별번호가 >들어오면
        file_name = "STUDENT";
    else if (mode == 3)
        printf("client 연결 종료");
    else
    {
        printf("클라이언트 입력 오류");
        exit(0);                
    }

    return file_name;
}

char* CheckID(char* modetxt, char* id)
{
    char buf[100] = { 0 };
    printf("ID checking starts...\n"); //debug

    sprintf(buf, "%s.txt", modetxt);
    FILE* idfile;

    char* idlist = NULL;
    char result[20];
    int num = 0;
    int same = 0;                                                  
    //이건 아이디가 list에 없었으면 반복문끝날떄 0으로 끝나서 결과 저장하게 함

        while (1)
        {
            idfile = openreadFile(buf);
            idlist = readFile(idfile, num);
            num++;
            if (feof(idfile)) { break; }
            if (strcmp(idlist, id) == 0)
            {
                sprintf(result, "%s.txt", id);          //만약 동일 아이>디 있으면 결과 저장하고 반복문 탈출
                same++;
                break;
            }
        }
    if (same == 0) { sprintf(result, "%s", "없음"); }
    char* putresult = malloc(sizeof(char) * 20); ;
    strcpy(putresult, result);

    return putresult;
}


char* CheckClient(char* msg)
{
    char* resultmention = malloc(sizeof(char) * 80); ;
    char mention1[80] = { 0 };
    char mention[80] = { 0 };
    sprintf(mention, "%s", msg);
    ///접근가능 과목명 읽으러 가는중
    if (strcmp(mention, "없음") == 0) /////인증 안된거
    {
        sprintf(mention1, "%s", "일치하지 않는 아이디입니다.");
    }
    else
    {
        sprintf(mention1, "%s", "인증되었습니다.");
    }
    strcpy(resultmention, mention1);
    printf("%s: checkclienet\n", resultmention);
    return resultmention;
}

