#if 0

// #17. new로 생성한 객체를 스마트 포인터에 저장하는 코드는 별도의 한 문장으로 만들라


//
{
	// 처리 우선순위를 알려주는 함수
	int priority();
	// 동적 할당한 Widget 객체에 대해 어떠한 우선순위에 따라 처리를 적용하는 함수
	void processWidget(std::tr1::shared_ptr<Widget> pw, int priority);
	
	processWidget(new Widget, priority());
	/*
	* 이 코드는 컴파일되지 않는다.
	* 포인터를 받는 shared_ptr의 생성자는 explicit으로 선언되어 있으므로, new Widget으로 생성된 포인터가 shared_ptr 타입 객체로 암시적 변환이 이루어질 리 없기 때문이다.
	*/

	processWidget(std::tr1::shared_ptr<Widget>(new Widget), priority);
	/*
	* 반면 위의 경우는 컴파일된다.
	* 그러나 위의 경우는 자원이 누출될 가능성이 존재한다. 어째서일까?
	* 
	* 컴파일러는 processWidget 호출을 생성하기 위해 우선 이 함수의 매개변수로 넘겨질 인자를 평가한다. 
	* 여기에서 두 번째 인자는 priority()의 호출이지만, 첫 번째 인자는 두 부분으로 나뉘어 있다.
	* 1. new Widget을 실행하는 부분.
	* 2. tr1::shared_ptr 생성자를 호출하는 부분.
	* 
	* 때문에 processWidget 함수 호출이 이루어지기 전 컴파일러는 다음의 세 가지 연산을 위한 코드를 생성하여야 한다.
	* 1. priority 호출
	* 2. new Widget 실행
	* 3. shared_ptr 생성자 호출
	* 
	* 
	* 문제는 이 각각의 연산이 실행되는 순서가 컴파일러마다 다르다.
	* new Widget은 shared_ptr 생성자 실행 전에 호출되어야 하고 이는 대부분 지켜지겠으나, priority() 호출은 순서가 미정의되어 있다.
	* 
	* 만약 컴파일러가 priority 호출을 두 번째로 하는 것으로 정했다면, 연산 순서는 다음과 같을 것이다.
	* 1. new Widget 실행
	* 2. priority 호출
	* 3. shared_ptr 생성자 호출
	* 
	* 이 경우 priority() 호출에서 예외가 발생하면 new Widget으로 생성된 포인터가 유실된다. shared_ptr에 저장되기도 전에 예외가 발생했으므로.
	* 즉 processWidget() 호출 중 자원이 누출될 가능성이 존재하는 이유는, 자원 생성 시점과 그 자원이 자원 관리 객체에 저장되는 사이에 예외가 발생할 수 있기 때문이다.
	* 
	* 이러한 문제를 피하는 방법은 간단하다. Widget을 생성하여 스마트 포인터에 저장하는 코드를 별도 문장 하나로 정하고, 해당 스마트 포인터를 processWidget()에 넘기는 것이다.
	*/

	// new로 생성한 객체를 스마트 포인터에 담는 과정을 단일 문장으로 뺀다.
	std::tr1::shared_ptr<Widget> pw(new Widget);
	processWidget(pw, priority());

	/*
	* 동일 문장 내 연산들보다는 문장과 문장 사이에 있는 연산들이 컴파일러의 조정을 받을 여지가 적으므로, 위의 코드는 자원 누출 가능성이 적다.
	*/
}
/*
* 결론.
* 1. new로 생성한 객체를 스마트 포인터로 넣는 코드는 별도의 한 문장으로 빼라. 이것이 안 되어 있으면 예외 발생 시 디버깅하기 힘든 자원 누출이 발생할 수 있다.
*/

#endif