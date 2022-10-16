make를 돌리면 
stockserver 와 multiclient 실행파일이 각각 나오게된다.

그러면 해당 실행파일을 실행한다.

처음에 ./stockserver <port 번호> 로 해당 파일을 하고

이후에 같이 돌릴 multiclient파일을 ./multiclient <host번호> <포트번호> <client 스레드 개수> <한 스레드당 접근개수> <읽어들이는 명령어를 담은 파일의 이름>
과 같은 형식으로 입력을 한다.

만약에 server의 thread개수를 변경하고 싶으면 stockserver.c 파일에 들어가서 NTHREADS라는 변수의 값을 변경하면 된다.

현재 과제는 master thread 모델만 구현하였음