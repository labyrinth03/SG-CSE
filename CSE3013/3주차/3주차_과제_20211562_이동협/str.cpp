#include <iostream>
#include <cstdlib>
#include <string.h>
#include "Str.h"
#include <stdlib.h>

using namespace std;

Str::Str(int leng)// leng 은 string의 길이.
{
	if(leng < 0){								// leng이 0보다 작게 입력된 경우
		cout << "String length error!" << endl;	// 에러 출력
	}
	else{
		str = new char[leng];					// leng 크기의 str 선언
		len = leng;								// leng의 값은 len에 저장
	}
}

Str::Str(char *neyong) // neyong은 초기화할 내용이 들어감.
{
	str = new char[strlen(neyong)];		//neyong의 길이만큼 str의 크기를 잡고 선언
	strcpy(str, neyong);   				// str에 neyong 복사
	len = strlen(neyong);				// len에 neyong의 길이 저장.
}

Str::~Str()// 소멸자. 
{
	delete []str;						// str 메모리 해제
}


int Str::length(void)// string의 길이를 리턴하는 함수. 
{
	return len;							//str의 길이인 len 리턴.
}

char* Str::contents(void)// string의 내용을 리턴하는 함수.
{
	return str;							//str 리턴.
}

int Str::compare(class Str& a)// a의 내용과 strcmp.
{
	return strcmp(str, a.contents()); //strcmp의 기능을 수행해야하므로 strcmp(str,a.contents())를 수행 후 해당 값 리턴
}

int Str::compare(char *a)// a의 내용과 strcmp.
{
	return strcmp(str, a);			//strcmp의 기능을 수행해야하므로 strcmp(str,a)를 수행 후 해당 값 리턴
}

void Str::operator=(char *a)// string a의 값을 대입
{
	if(len < strlen(a)){			// str의 메모리가 a의 길이보다 작은 경우를 대비
		delete []str;
		str = new char[strlen(a)];	// 기존 메모리 해제 후 a의 크기만큼 다시 할당
	}

	len = strlen(a);				//len에 a길이 저장
	strcpy(str, a);					//str에 a내용 복사
}

void Str::operator=(class Str& a)// Str a의 내용을 대입.
{
	if(len < a.length()){			// str의 메모리가 a의 길이보다 작은 경우를 대비
		delete []str;
		str = new char[a.length()];	// 기존 메모리 해제 후 a의 크기만큼 다시 할당
	}

	len = a.length();				//len에 a 길이 저장
	strcpy(str, a.contents());		//str에 a 내용 복사
}