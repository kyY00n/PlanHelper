#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define LINESIZE 257
#define BUFSIZE 257
#define SCHSIZE 30
#define DATESIZE 20
#define TIMESIZE 10

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
            printf("수정할 일정이 없습니다.\n");
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

                    printf("before sprintf");
                    sprintf(temp, "%s%s%s%s%s%s%s", newdate, " ", time, " <", sub, "> ", schedule);
                   // strcpy(temp, change_word(strp, datep, newdatep));
                    printf(temp);
                    break;

                case 2://시간바꾸기
                    printf("새로운 시간을 입력하세요\n");
                    printf("    (시간을 지정하지 않을 시 00:00 로 입력하세요.)\n");
                    scanf("%s", newtime);
                    getchar();
                    closeFile(fp);
                    printf("before sprintf");
                    //char* timep = &time;
                    //char* newtimep = &newtime;
                    //strcpy(temp, change_word(strp, timep, newtimep));
                    sprintf(temp, "%s%s%s%s%s%s%s", date, " ", newtime, " <", sub, "> ", schedule);
                    printf(temp);
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
            closeFile(fp1);*/
            while (1)
            {
                fp1 = openreadFile(file_name);//열고
                fp2 = openwriteFile("filefile.txt");
                //printf("5"); 
                 strcpy(buf, readFile(fp1, line_count2));
                printf(buf);
                if (feof(fp1)!=0)
                    {
                        break;
                    }
                if (line_count != line_count2)
                {
                    printf("beforewritenewFile");                   //같으면
                    writeFile(fp2, buf);
                    printf("afterwritenewFile");   
            
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
            printf("삭제할 일정이 없습니다.\n");
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
                printf(buf);
                if (feof(fp1)!=0)
                    {
                        break;
                    }
                if (line_count != line_count2)
                {
                    printf("beforewritenewFile");                   //같으면
                    writeFile(fp2, buf);
                    printf("afterwritenewFile");   
            
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
    int line_count = 1;
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

    printf("현재 날짜(시스템 날짜) 기준으로 검색하고 싶은 일수를 선택하세요\n");
    printf("    현재 날짜 기준으로 7일의 일정을 보고 싶은 경우 ex) 7\n");
    scanf("%d", &n);
    getchar();

    for (int i = 0; i < n; i++)
    {
        time_t t = time(NULL) + i * (24 * 60 * 60);
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
    }
}
void findSub(char* sub)
{
    FILE* fp;
    char buf[BUFSIZE] = { 0 };
    char* file_name = "all.txt";

    printf("<< %s >>\n", sub);
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
      {
         printf("%d. ", line_num);
             line_num++;
          puts(buf);
      }
            
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
    printf("Asd\n");
    fclose(fp);
    printf("Asd");
}

