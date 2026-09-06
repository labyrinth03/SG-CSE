make 후 ./myShell 입력 시 실행되며 
phase1에서 실행하던 명령어들이 pipeline으로 연결되어 입력 시 
pipeline 좌측의 stdout 값이 pipeline 우측의 명령어의 stdin으로 간주되어 실행됩니다.
pipeline이 2개 이상인 경우를 고려하여 재귀적으로 호출될 수 있게 하였고 정상적으로 동작합니다.