#if 0

// #52. 위치지정 new를 작성한다면 위치지정 delete도 같이 준비하라


// 
{
	Widget* pw = new Widget\;
	/*
	* 위 라인에서는 함수 두 개가 호출된다.
	* 먼저 operator new가 호출되고, 그 뒤에 Widget의 기본 생성자가 호출된다.
	* 
	* 여기에서 operator new는 완수되었지만 Widget의 기본 생성자에서 예외가 던져졌을 경우 new에서 발생한 메모리 할당을 취소해야 한다.
	* 이는 C++ 런타임 시스템에서 수행하게 된다.
	* 
	* C++ 런타임 시스템이 여기에서 해야 하는 것은, 호출한 new와 짝이 되는 delete를 호출하는 것이다.
	* 하지만 이게 정상적으로 수행되기 위해서는 어떠한 delete를 호출해야 하는지를 런타임 시스템이 제대로 이해하고 있어야만 한다.
	* 다만 기본 operator new는 기본형 operator delete와 짝을 맞추기 때문에 일반적으로 대수인 것은 아니다.
	* 
	* 그런데, 기본형이 아닌 operator new를 사용하기 시작하면 런타임 시스템이 어떤 delete를 짝지어야 하는지 혼란하게 된다.
	* 이 때 비기본형은 다른 매개변수를 추가로 받는 operator new를 의미한다.
	* 
	* 예로 어떠한 클래스에 대해 전용으로 사용되는 operator new를 만들고 있는데 메모리 할당 정보를 로그로 기록할 ostream을 지정받는 꼴로 만든다고 가정하자.
	* 그리고 클래스 전용 operator delete는 기본형으로 만든다고 하자.
	*/
	class Widget {
	public:
		static void* operator new(std::size_t size, std::ostream& logStream) throw(std::bad_alloc);
		static void operator delete(void* pMemory size_t size) throw();
	};
	/*
	* 물론 이 설계에는 문제가 있다. 일단 이는 조금 뒤에서 다뤄보도록 하자.
	* 
	* operator new는 기본형과 달리 파라미터를 추가로 받는 형태로도 선언 가능하다.
	* 이러한 형태의 함수를 위치지정(placement) new라고 한다.
	* 위의 new는 위치지정 new라고 할 수 있다.
	* 
	* 위치지정 new는 그냥 추가 파라미터를 받는 new이므로 다양한 것이 있을 수 있다만, 그 중 특출나게 유용한 것이 있다.
	* 바로 객체를 생성시킬 메모리 위치를 나타내는 포인터를 매개변수로 받는 new이다.
	*/
	void* operator new(std::size_t, void* pMemory) throw();
	/*
	* 참고로 이 형태의 위치지정 new는 STL에도 포함되어 있다.
	* 다양한 STL 객체나 템플릿에도 사용되고 있으며, 위치지정 new라는 용어의 원조이기도 하다.
	* 
	* 아무튼, Widget 클래스의 설계는 문제가 있다고 했는데 이 클래스는 메모리 누출을 유발할 수 있다.
	*/

	Widget* pw = new (std::cerr) Widget;
	/*
	* 위 코드에서는 operator new를 호출할 때 cerr을 ostream 인자로 넘기는데, 이 때 Widget 생성자에서 예외가 발생하면 메모리가 누출된다.
	* 런타임 시스템은 할당 구문에서 예외 발생 시 new와 짝을 짓는 delete를 찾아 호출하는데, 위의 경우 ostream& 타입 파라미터를 추가로 받으므로 동일 시그니처를 지닌 delete도 존재해야 한다.
	*/
	void operator delete(void*, std::ostream&) throw();
	/*
	* 매개변수를 추가로 받는다는 부분에 대해서는 위치지정 new와 동일하므로 이러한 delete를 위치지정 delete라고 한다.
	* 그러나 현재 Widget에는 operator delete의 위치지정 버전이 마련되어 있지 않으므로, 런타임 시스템 쪽에서는 위치지정 new로 할당한 메모리를 어떻게 족쳐야 할지 혼란할 수밖에 없다.
	* 결론적으로, 아무것도 하지 않는다. 고로 메모리가 그대로 누출된다.
	* 
	* 다시 말해, 추가 매개변수를 취하는 operator new가 있을 때 그것과 같은 추가 매개변수를 받는 operator delete가 짝을 이루지 않을 경우
	* new를 포함하는 구문에서 예외가 발생하더라도 delete가 호출되지 않는다.
	*/
	class Widget {
	public:
		static void* operator new(std::size_t size, std::ostream& logStream) throw(std::bad_alloc);
		static void operator delete(void* pMemory size_t size) throw();

		static void operator delete(void* pMemory, std::ostream& logStream) throw();
	};

	Widget* pw = new (std::cerr) Widget;
	/*
	* 이러면 new를 사용하는 Widget 생성 라인의 생성자 부분에서 예외가 터지더라도 위치지정 new와 짝이 되는 위치지정 delete가 자동으로 호출된다.
	* 
	* 그런데, 위 구문에서 예외가 발생하지 않고 성공적으로 구문이 실행되었다면?
	*/
	delete pw;
	/*
	* 나중에 사용자가 해당 객체를 위와 같이 delete할 때 사용자가 지정한 위치지정 delete가 아니라 기본형의 operator delete가 호출된다.
	* 위치지정 delete가 호출되는 경우는, 위치지정 new의 호출을 동반하는 라인 내부의 생성자에서 예외가 발생할 때 뿐이다.
	* 
	* 때문에 어떠한 위치지정 new와 연관된 모든 메모리 누출을 사전에 막기 위해서는 표준 operator delete를 기본으로 마련하여야 하며, 그와 함께 위치지정 new와 같은 시그니처의 위치지정 delete도 마련해야 한다.
	* 
	* 단 절대 빼먹으면 안 되는 것이 하나 있는데, 외부 스코므에 있는 어떠한 함수의 이름과 클래스 멤버함수의 이름이 동일할 경우, 외부 스코프 함수는 이름만 같아도 가려진다.
	* 때문에 사용자 자신이 사용할 수 있다고 생각하는 다른 new들을 클래스 전용 new가 가리지 않도록 각별히 신경써야 한다.
	*/
	class Base {
	public:
		static void* operator new(std::size_t size, std::ostream& logStream) throw(std::bad_alloc);
	};

	// 에러
	Base* pb = new Base;
	Base* pb = new (std::cerr) Base;
	/*
	* 이러한 기본 클래스를 상속하는 파생은 operator new는 물론 자신이 상속받은 기본의 operator new까지 가린다.
	*/
	class Derived : public Base {
	public:
		static void* operator new(std::size_t size) throw(std::bad_alloc);
	};

	// 에러
	Derived* pb = new (std::clog) Derived;
	Derived* pd = new Derived;
	/*
	* 다만 메모리 할당 함수를 작성하는 것만 신경쓴다면 굳이 억지로 볼 필요는 없다.
	* 기본적으로 C++이 전역 스코프에서 제공하는 operator new의 형태는 다음의 세 가지가 표준이라는 것만 기억하면 된다.
	*/
	void* operator new(std::size_t) throw(std::bad_alloc);
	void* operator new(std::size_t, void*) throw();
	void* operator new(std::size_t, const std::nothrow_t&) throw();
	/*
	* 어떠한 형대이든 operator new가 클래스 내부에 선언되는 순간 표준 형태들은 전부 가려진다.
	* 사용자가 이 표준 형태들을 사용하지 못하게 하는 것이 목적이 아니라면 나름 넣어 준 사용자 정의 operator new 외 표준 형태들도 접근 가능하게 만들어라.
	* 물론 delete도 만들어주는 것을 잊으면 안 된다.
	* 
	* 참고로 이를 쉽게 하는 방법은, 그냥 기본 클래스를 하나 만들고 그 내부에 new 및 delete의 기본형태를 전부 넣어두면 된다.
	*/
	class StandardNewDeleteForms {
	public:
		void* operator new(std::size_t size) throw(std::bad_alloc) { return ::operator new(size); }
		void* operator delete(void* pMemory) throw() { return ::operator delete(pMemory); }

		void* operator new(std::size_t size, void* ptr) throw() { return ::operator new(size, ptr); }
		void* operator delete(void* pMemory, void* ptr) throw() { return ::operator delete(pMemory, ptr); }

		void* operator new(std::size_t size, const std::nothrow_t& nt) throw() { return ::operator new(size, nt); }
		void* operator delete(void* pMemory, const std::nothrow_t& nt) throw() { return ::operator delete(pMemory); }
	};
	/*
	* 표준 형태에 덧붙여 사용자 정의 형태를 추가하고 싶을 경우 해당 기본 클래스를 축으로 넓혀가면 된다.
	* 상속과 using 선언을 사용하여 표준 형태를 파생으로 끌어와 외부에서 사용 가능하게 한 뒤, 사용자 정의 형태를 선언하면 된다.
	*/
	class Widget : public StandardNewDeleteForms {
	public:
		using StandardNewDeleteForms::operator new;
		using StandardNewDeleteForms::operator delete;

		static void* operator new(std::size_t size, std::ostream& logStream) throw(std::bad_alloc);
		void* operator delete(void* pMemory, std::ostream& logStream) throw();
	};
}
/*
* 결론.
* 1. operator new의 위치지정 버전을 만들 때에는 해당 함수와 짝을 이루는 위치지정 버전 delete도 만들어라.
*	 이걸 안 하면 찾기도 힘든데다 간헐적으로 발생하는 메모리 누출이 생긴다.
* 2. new 및 delete의 위치지정 버전을 선언할 경우 실수로 표준 버전이 가려지지 않게 하라.
*/

#endif