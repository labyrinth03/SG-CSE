#!/bin/bash

echo 'Working directory' #/sogang/under/cse20131251/shell
read dirname #directory 이름을 입력을 저장

if [ -n "$dirname" ]; then # 입력이 존재하는 경우
        if [ ! -d "$dirname" ]; then # 입력한 디렉토리가 존재하지 않는 경우
                echo "There's no directory $dirname"
                exit 0 # 디렉토리가 존재하지 않음을 알리고 프로그램 종료
        fi
        cd $dirname # 해당 디렉토리로 이동
else # 입력이 없는 경우
        dirname=$(pwd) # 현재 디렉토리의 주소를 이용
fi


for dir in *
do
        if [ -f $dir ]; then # 파일 대상으로만 대소문자 변경 실행
                newname=`echo $dir | tr "[a-z] [A-Z]" "[A-Z] [a-z]"`
                #변수 newname은 dir의  대소문자를tr을 이용해서 바꾼것
                mv $dir $newname
                #$dir을 newname으로 바꾼다.
        fi
done
