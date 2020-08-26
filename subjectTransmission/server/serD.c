

///////////////////////////DB 모듈///////////////////////
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

char * change_word(char *string, char *old_word, char *new_word)  
{  
    char *token;  
    char temp[30];  
  
    memset(temp, 0, sizeof(temp));  
    token = strtok(string, " ");  
  
    while(token != NULL)  
    {  
        if(0 == strcmp(token, old_word))  
        {  
            strcat(temp, new_word);  
        }  
        else  
        {  
            strcat(temp, token);  
        }  
        strcat(temp, " ");  
        token = strtok(NULL, " ");  
    }  
    temp[strlen(temp)-1] = 0;  
      
    return temp;  
}  

char* readFile(FILE* filename, int num)  //num은 읽어올 줄의 번호를 입력하는 곳 소켓안에 반복문 돌릴때 int num, num++같이 쓸 것
{
   int countline = 0;
   char text[100] = { 0 };
   char* textadd = malloc(sizeof(char) * 100);
	printf("들어옴?\n"); 

   if (filename == NULL)
   {
      perror("error(open)");
      exit(1);
   }

   while (1) { //파일끝이 아니라면 반복
         fgets(text, 100, filename);    //한줄 읽고
	 if(feof(filename)){
		printf("파일이 끝났습니다.\n");
		printf("---------------------------\n\n");
		memset(text, '\0', 100);
		break;
	 }
         if (num == countline)
	 {     
		printf("countline: %d\n\n", countline);                //라인수가 원하는줄이면
                for (int i = 0; i < 100; i++) { //줄 뒤에 붙은 개행문자 삭
                        if (text[i] == '\n'){
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



FILE* deleteFile(FILE* filename, char* msg) {
    char text[100] = { 0 };
    FILE* newfile;

    if (filename == NULL)
    {
        perror("error(open)");
        exit(1);
    }

    newfile = fopen("filefile.txt", "w+");

    while (fgets(text, 100, filename) != NULL) // 파일의 끝이 아니라면 최대 80칸짜리 한줄(과목명) 읽어 배열text에 저장
    {
        //위에서 읽어온 과목의 일정파일을 찾기
        if (strcmp(msg, text) != 0) {
            fputs(text, newfile);
        }
    }

    fclose(newfile);
    return newfile;
}
//
FILE* openreadFile (char* file_name) 
{
   FILE* fp = fopen(file_name, "r");

   //fp = fopen(file_name, "rt");   //해당학번파일을 읽는다.
    if(fp == NULL)
    {
        perror("error(open)");
        exit(1);
    }   
    return fp;
}

//고침
FILE* openwriteFile(char* file_name) 
{
   FILE* fp= fopen(file_name, "a+");

   //char filetitle[80] = { 0 };
   //fp = fopen(file_name, "a+");   //해당학번파일을 열기+추가모드로 연다는 파일 파라미터.
if(fp == NULL)
{
    perror("error(open)");
    exit(1);
}
   return fp;   //반환

}
FILE* openwriteonlyFile(char* file_name) 
{
   FILE* fp= fopen(file_name, "w");

   //char filetitle[80] = { 0 };
   //fp = fopen(file_name, "a+");   //해당학번파일을 열기+추가모드로 연다는 파일 파라미터.
if(fp == NULL)
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
	if (result == 0){printf("닫힘");}
	else {printf("안 닫힘");}
}
//찐 끝
