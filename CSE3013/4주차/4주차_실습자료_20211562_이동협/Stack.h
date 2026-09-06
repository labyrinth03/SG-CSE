#include "LinkedList.h"


//1. 템플릿 클래스로 확장해야함
//2. Stack형식으로 Delete 함수 재정의해야함
//주의: first, current_size는 class의 멤버 변수이기 때문에 this 포인터를 사용하여 가져와야함

//LinkedList class를 상속받음

template <class T>
class Stack : public LinkedList<T>{
	public:
		bool Delete (T &element){
			//first가 0이면 false반환
			if(this->first == 0){
				return false;
			}

			// LinkedList와 달리 Stack은 current가 가리키는 곳을 삭제
			Node<T> *current = this->first; // first에서 작업 시작.
			this->first = current->link; //first에 있던 내용이 삭제되므로 first에 다음 노드 연결
			Node<T> *previous = 0;
			
			
			element = current->data; // element에 current에 존재하는 data 저장.
			delete current;          // current 메모리 해제.
			delete previous;         // previous 메모리 해제.
			this->current_size--;    // 데이터의 개수 -1.
			return true;
		}
};
