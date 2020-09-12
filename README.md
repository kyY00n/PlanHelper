# Plan Helper

: 학교 일정과 개인 일정을 한 번에 볼 수 있는 플래너로 두 가지 모드(학생, 교수)가 있다.

      1. Plan Helper Auth(서버, 클라이언트 프로그램): 인증 기능 구현

      2. Plan Helper SubjectTransmission(서버, 클라이언트): 교수가 과목일정 업로드 후 학생이 접속하면 일정 자동 업데이트 기능 구현

~~1, 2 동시 구현 실패~~

------
### 개발 스택
​	Linux, vim, gcc, make, C

------

### 주요 기능

​		일정 추가/삭제/수정 기능

​		당일 일정 알림

​		학생 mode: 과목 일정 자동 업데이트, 과목 업데이트 알림

​		교수 mode: 과목 일정 변경 자동 업로드

------

### 기능 트리

![functionTree](./functionTree.png)

------

### Usage

- PlanHelper - Authenticity

  ```
  @linux~/PlanHelper/authentication/server$ ./a.out [port number]
  ```

  ```
  @linux~/PlanHelper/authentication/client$ ./client [same port number]
  ```



- PlanHelper - Subject Schedule Transmission

  ```
  @linux~/PlanHelper/subjectTransmission/server$ ./a.out [port number]
  ```

  ```
  @linux~/PlanHelper/subjectTransmission/clientF$ ./c10 [same port number]
  ```

  -----
  
  ### R&R
  안소현: Server authenticity module & File IO api
  
  안홍은: Client program
  
  윤가영: Server-Client Socket module & api
  
