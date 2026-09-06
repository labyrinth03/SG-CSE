Project 2 README

1. 데이터베이스 생성 방법

    1. MySQL Workbench 실행
    2. schema.sql 실행
    3. sample_data.sql 실행
    4. project2 데이터베이스 생성 확인

2. 프로그램 컴파일

    Windows 11에서 MSYS2 g++를 사용하는 경우

    g++ main.cpp -I"C:\Program Files\MySQL\MySQL Server 8.0\include" -L"C:\Program Files\MySQL\MySQL Server 8.0\lib" -lmysql -o main.exe

    Visual Studio Developer Command Prompt에서 MSVC를 사용하는 경우

    cl /EHsc main.cpp /I"C:\Program Files\MySQL\MySQL Server 8.0\include" /link /LIBPATH:"C:\Program Files\MySQL\MySQL Server 8.0\lib" libmysql.lib

3. 프로그램 실행

    MySQL C API DLL 경로가 PATH에 포함되어 있어야 함.

    PowerShell 예시:

    $env:PATH="C:\Program Files\MySQL\MySQL Server 8.0\lib;" + $env:PATH
    $env:MYSQL_HOST="127.0.0.1"
    $env:MYSQL_USER="admin"
    $env:MYSQL_PASSWORD="your_password"
    $env:MYSQL_DATABASE="project2"
    main.exe

    macOS 예시:

    export MYSQL_HOST=127.0.0.1
    export MYSQL_USER=admin
    export MYSQL_PASSWORD=your_password
    export MYSQL_DATABASE=project2
    ./main

    MYSQL_PASSWORD 또는 MYSQL_PWD가 설정되지 않으면 사용자 이름 admin에 비밀번호 1234로 접속을 시도.


