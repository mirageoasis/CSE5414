1. kafka 서버와 브로커를 킨다.

2. 현재 폴더에서 mvn clean package 명령어를 실행한다.

3. java -jar target/kafka_example-1.0.jar 라는 명령어를 실행한다.

4. 된다 ^오^

ps : 서버의 주소를 바꾸고 싶다면 Helloworld.java 에 있는 35번째 줄에서

static String SERVER_LOC 변수의 값을 바꾸면 된다. ex) "localhost:9092"

또한 kafka.properties에서 bootstrap.servers=localhost:9092 해당 주소의 값을 바꾼다.