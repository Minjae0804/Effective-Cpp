#if 0

// #49. new 처리자의 동작원리를 제대로 이해하라


// 
{
	/*
	* 사용자가 보낸 메모리 할당 요청을 operator new가 맞춰주지 못하는 경우(즉 메모리가 부족한 경우) operator new는 예외를 던지게 되어 있다. 
	* 이 예외를 던지기 전에 사용자 측에서 지정 가능한 에러 처리 함수를 우선 호출하도록 되어 있는데, 이 에러 처리 함수를 new 핸들러(new-handler, 할당에러처리자)라고 한다.
	* 이와 같은 메모리 고갈 상황을 처리할 함수를 사용자 측에서 지정 가능하도록 표준 라이브러리에는 set_new_handler라는 함수가 준비되어 있다. 이 함수는 <new>에 선언되어 있다.
	*/
	namespace std {
		typedef void(*new_handler) ();
		new_handler set_new_handler(new_handler p) throw();
	}
	/*
	* new 핸들러는 받는 것도 없고 반환하는 것도 없는 함수의 포인터에 대해 typedef를 걸어놓은 타입동의어이다.
	* 그리고 set_new_handler는 new_handler를 받고 new_handler를 반환하는 함수이다.
	* 
	* set_new_handler가 받아들이는 new_handler 타입 매개변수는 요구된 메모리를 operator new가 할당하지 못했을 때 operator new가 호출할 함수 포인터이다.
	* 반환값은 지금의 set_new_handler가 호출되기 바로 전까지 new 핸들러로 사용되고 있던 함수의 포인터이다.
	*/
	void outOfMem() {
		std::cerr << "Unable to satisfy request for memory\n";
		std::abort();
	}

	int main() {
		std::set_new_handler(outOfMem);
		int* pBigDataArray = new int(1000000000L);
		...;
	}
	/*
	* 만약 operator new가 1억 개의 정수 할당에 실패할 경우 outOfMem()이 호출되고, 이 함수는 에러 메시지를 출력하면서 프로그램을 강제 종료시킬 것이다.
	* 
	* 사용자가 명령한 만큼의 메모리를 할당해주지 못할 경우 operator new는 충분한 메모리를 찾아낼 때까지 new 핸들러를 되풀이하여 호출한다.
	* 이를 통해 호출되는 new 핸들러가 프로그램의 동작에 좋은 영향을 끼치도록 설계하려 한다면, new 핸들러는 다음 동작 중 하나 이상을 실행하여야 한다.
	* 
	* 1. 사용할 수 있는 메모리를 더 많이 확보한다.
	*	 operator new가 시도하는 이후의 메모리 확보가 성공할 수 있도록 하자는 전략.
	*	 여러 방법으로 구현될 수 있으나, 대표적으로 프로그램이 시작할 때 메모리 블록을 크게 하나 선언하고 new 핸들러가 가장 처음 실행될 때 해당 메모리 공간을 사용하게 하는 방법이 있다.
	* 
	* 2. 다른 new 핸들러를 설치한다.
	*	 현재의 new 핸들러가 더 이상 가용 메모리를 확보할 수 없다고 해도, 자신의 몫까지 해 줄 다른 new 핸들러의 존재를 알고 있을 가능성도 있다.
	*	 만약 그러할 경우, 현재의 new 핸들러는 제자리에서 다른 new 핸들러를 설치할 수 있다.
	*	 operator new 함수가 다시 new 핸들러를 호출할 때가 될 경우, 새로 설치된 new 핸들러가 호출된다.
	* 
	* 3. new 핸들러의 설치를 제거한다.
	*	 set_new_handler에 널 포인터를 넘긴다. 
	*	 new 핸들러가 설치된 것이 없을 경우 operator new는 메모리 할당이 실패했을 때 예외를 던지게 된다.
	* 
	* 4. 예외를 던진다.
	*	 bad_alloc이나 그것에서 파생된 타입의 예외를 던진다.
	*	 operator new에는 이쪽 종류의 에러를 받아 처리하는 부분이 존재하지 않으므로, 해당 예외는 메모리 할당을 요청한 원래 위치로 전파된다.
	* 
	* 5. 복귀하지 않는다.
	*	 보통 abort()나 exit()을 호출한다.
	*/

	/*
	* 할당된 객체의 클래스 타입에 따라 메모리 할당 실패에 대한 처리를 각각 다르게 하고 싶은 경우가 있다.
	*/
	class X {
	public:
		static void outOfMemory();
		...;
	};

	class Y {
	public:
		static void outOfMemory();
		...;
	};

	X* p1 = new X;
	Y* p2 = new Y;
	/*
	* C++에는 특정 클래스만을 위한 할당에러 처리자를 둘 수 있는 기능 같은 것이 없다. 다만, 사실 별 필요 없기도 하다. 직접 구현할 수 있기 때문이다.
	* 해당 클래스에서 자체 버전의 set_new_handler 및 operator new를 제공하도록 만들어 주면 된다.
	* 
	* 여기에서 클래스에서 제공하는 set_new_handler의 역할은 사용자로부터 그 클래스에 쓰기 위한 new 처리자를 받아내는 것이다.
	* 한편 클래스에서 제공하는 operator new 함수는 해당 클래스 객체를 담을 메모리가 할당되려고 할 때 전역 new 처리자 대신 클래스 버전의 new 처리자가 호출되도록 만드는 역할을 한다.
	* 
	* Widget 클래스에 대한 메모리 할당 실패를 직접 처리하고 싶을 경우, Widget 객체를 담을 만큼의 메모리를 operator new가 할당하지 못할 경우 호출될 new 처리자 함수를 어딘가에 간수해 둘 필요가 있다.
	* 때문에 이 new 처리자를 가리키는 new_handler 타입 정적 멤버 데이터를 선언한다.
	*/
	class Widget {
		static std::new_handler currendHandler;
	public:
		static std::new_handler set_new_handler(std::new_handler p) throw();
		static void* operator new(std::size_t size) throw(std::bad_alloc);
	};
	/*
	* 정적 클래스 멤버의 정의는 해당 클래스의 바깥에 존재해야 하므로 다음과 같이 정의해야 한다.
	*/
	std::new_handler Widget::currentHandler = 0;
	/*
	* Widget이 제공하는 set_new_handler는 자신에게 넘어온 포인터를 아무런 점검 없이 저장해 놓고, 바로 전에 저장했던 포인터를 아무 점검 어뵤이 반환하는 역할만 맡는다.
	* 표준 라이브러리의 set_new_handler가 하는 일과 완전히 같다.
	*/
	std::new_handler Widget::set_new_handler(std::new_handler p) throw() {
		std::new_handler oldHandler = currentHandler;
		currentHandler = p;
		return oldHander;
	}
	/*
	* 이제 마지막으로 Widget의 operator new가 할 일을 정의해야 한다.
	* 
	* 1. 표준 set_new_handler에 Widget의 new 핸들러를 넘겨 호출한다.
	*	 즉 전역 new 핸들러로서 Widget의 new 핸들러를 설치한다.
	* 
	* 2. 전역 operator new를 호출하여 실제 메모리 할당을 수행한다.
	*	 전역 operator new의 할당이 실패할 경우 이 함수는 Widget의 new 핸들러를 호출하게 된다.
	*	 만약 마지막까지 전역 operator new의 메모리 할당 시도가 실패할 경우 전역 operator new는 bad_alloc을 예외를 던진다.
	*	 이 경우 Widget의 operator new는 전역 new 핸들러를 원래의 것으로 되돌리고 이 예외를 전파해야 한다.
	* 
	*	 원래의 전역 new 핸들러를 실수 없이 되돌려놓을 수 있도록 Widget은 전역 new 핸들러를 자원으로 간주하고 처리한다. 
	*	 즉 자원 관리 객체를 통해 전역 new 핸들러를 관리하여 메모리 누수를 차단한다.
	* 
	* 3. 전역 operator new가 Widget객체 하나만큼의 메모리를 할당할 수 있으면 Widget의 operator new는 이렇게 할당된 메모리를 반환한다.
	*	 이와 동시에 전역 new 처리자를 관리하는 객체의 소멸자가 호출되면서 Widget의 operator new가 호출되기 전 사용되던 전역 new 처리자가 자동 복원된다.
	* 
	* 위를 C++로 풀어 보자. 일단 전역 new 처리자를 자원으로 삼는다고 했으므로, 우선 자원관리 클래스를 하나 준비한다. 
	* 해당 클래스는 객체 생성 중에 자원을 획득하고 객체 소멸 중 해당 자원을 해제하는 지극히 일반적인 RAII 연산 외엔 아무것도 지니지 않는다.
	*/
	class newHandlerHolder {
		std::new_handler handler;
		NewHandlerHolder(const NewHandlerHolder&);
		NewHandlerHolder& operator=(const NewHandlerHolder&);
	public:
		explicit NewHandlerHolder(std::new_handler nh) : handler(nh) {}
		~NewHandlerHolder() { std::set_new_handler(handler); }
	};
	/*
	* 어지간한 일들이 자원 관리 클래스 쪽으로 몰려갔으므로 Widget의 operator new는 정말 간단히 구현할 수 있다.
	*/
	void* Widget::operator new(Std::size_t size) throw(std::bad_alloc) {
		NewHandlerHolder h(std::set_new_handler(currentHandler));
		return ::operator new(size);
	}
	/*
	* Widget 클래스를 사용하는 쪽에서 new 핸들러 기능을 쓰려면 다음과 같이 하면 된다.
	*/
	void outOfMem();
	Widget::set_new_handler(outOfMem);
	Widget* pw1 = new Widget;
	std::string* ps = new std::string;
	Widget::set_new_handler(0);
	Widget* pw2 = new Widget;
	/*
	* 이렇게 자원 관리 객체를 통한 할당에러 처리를 구현하는 이런 방식의 코드는 어떤 클래스를 사용하더라도 똑같이 사용된다.
	* 때문에, 믹스인 양식의 기본 클래스를 사용하면 좋다.
	* 즉 다른 파생 클래스들이 한 가지의 특정 기능만을 물려받아갈 수 있도록 설계된 기본 클래스를 만들면 된다.
	* 
	* 그 다음엔 그렇게 만든 기본 클래스를 템플릿으로 변경한다.
	* 이렇게 하면 파생 클래스마다 클래스 데이터의 사본이 따로따로 존재하게 된다.
	* 
	* 이렇게 설계된 클래스 템플릿으로 얻을 수 있는 효과는 크게 두 가지이다. 
	* 먼저 기본 클래스 부분은 파생들이 가져야 하는 set_new_handler와 operator new를 물려준다.
	* 그리고 템플릿 부분은 각 파생이 currentHandler 데이터 멤버를 따로따로 가질 수 있게 한다.
	*/
	template<typename T>
	class NewHandlerSupport {
		static std::new_handler currentHandler;
	public:
		static std::new_handler set_new_handler(std::new_handler p) throw();
		static void* operator new(std::size_t size) throw(std::bad_alloc);
		...;
	};

	template<typename T>
	std::new_handler NewHandlerSupport<T>::set_new_handler(std::new_handler p) throw() {
		std::new_handler oldHandler = currentHandler;
		currentHandler = p;
		return oldHandler;
	}

	template<typename T>
	void* NewHandlerSupport<T> operator new(std::size_t size) throw(std::bad_alloc) {
		NewHandlerHolder h(std::new_handler(currentHandler));
		return ::operator new(size);
	}

	template<typename T>
	std::new_handler NewHandlerSupport<T>::currentHandler = 0;
	/*
	* 이렇게 만들어진 클래스 템플릿이 있으면 Widget 클래스에 set_new_handler 기능을 추가하는 것은 별로 어려워지지 않게 된다.
	* 그저 NewHandlerSupport<Widget>으로부터 상속만 받으면 끝이기 때문이다.
	*/
	class Widget : public NewHandlerSupport<Widget> { ... };
	/*
	* 사실 이 템플릿은 T를 사용할 이유가 전혀 없다. 
	* 필요한 것은 NewHandlerSupport로부터 파생된 각 클래스에 대한 NewHandlerSupport 객체의 서로 다른 사본 뿐이기 때문이다.
	* 당 템플릿의 매개변수인 T는 그저 파생들을 구분해주는 역할만 할 뿐이다.
	* 
	* 템플릿 매개변수로 Widget을 받아 만들어진 기본 클래스로부터 Widget이 파생된 것 또한 흥미로운 부분인데, 의외로 쓸 만한 기법이다.
	* 첫 인상처럼 그 이름 또한 기묘한 재귀 템플릿 패턴(Curiously Recurring Template Pattern, CRTP)라고 한다.
	*/

	/*
	* 1993년까지의 C++은 operator new가 메모리 할당이 불가할 때 널 포인터를 반환하도록 되어 있었다.
	* 이후 몇 년이 지난 후 bad_alloc 예외를 던지도록 명세가 바뀌게 된다.
	* 그러나 이미 동작하고 있던 많은 개발도구들이 널 포인터 기반 new 연산자를 쓰고 있었기 때문에 전통적인 할당 실패 시 널 반환으로 동작하는 대안 형태의 operator new도 같이 내놓았다.
	* 이러한 형태를 통틀어 예외불가(nothrow) 형태라고 한다.
	*/
	class Widget { ... };
	Widget* pw1 = new Widget;

	if (pw1 == 0) ...;
	Widget* pw2 = new (std::nothrow) Widget;

	if (pw2 == 0) ...;
	/*
	* 위에 나온 new (std::nothrow) Widget에서는 두 가지 동작이 이루어진다.
	* 1. operator new 함수의 예외불가 버전이 호출되어 Widget 객체를 담기 위한 메모리 할당을 시도한다.
	*	 만약 이 할당이 실패할 경우 operator new는 널 포인터를 반환한다.
	* 
	* 2. 만약 할당이 성공할 경우 Widget 생성자가 호출된다. 
	*	 이 때 중요한 점은, 이 생성자가 구현에 따라서 자체적인 new를 사용할 수도 있다.
	*	 이 new는 처음 실행되었던 예외불가 new로부터 아무런 제약을 받지 않는다.
	*	 때문에 해당 구문에서는 예외가 발생할 수 있으며, 그로 인해 예외가 전파될 수도 있다.
	* 
	* 즉, 예외불가 new는 사용된 당시에 호출되는 operator new에서만 예외가 발생하지 않는 것을 보장할 뿐이다.
	*/

	/*
	* 중요한 건 new 핸들러의 동작원리를 제대로 이해해야 한다는 것이다.
	* new 핸들러는 양쪽에서 모두 사용되기 때문이다.
	*/
}
/*
* 결론.
* 1. set_new_handler를 쓰면 메모리 할당 요청이 만족되지 못했을 때 호출되는 함수를 지정할 수 있다.
* 2. 예외불가 new는 영향력이 제한되어 있다. 이는 예외불가 new를 사용하는 메모리 할당 자체에만 적용되기 때문이다.
*	 이후 호출되는 생성자에서는 얼마든지 예외를 던질 수 있다.
*/

#endif