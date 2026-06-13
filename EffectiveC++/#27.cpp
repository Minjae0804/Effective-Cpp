#if 0

// #27. 캐스팅은 가능한 한 피하라


// 
{
	/*
	* C++은 어떤 일이 있어도 타입 에러가 발생하지 않는 것을 원칙으로 한다.
	* 때문에 이론적으로 C++ 프로그램은 일단 컴파일이 끝나면 어떠한 객체에 대해서도 불완전 연산이나 이상한 연산을 수행하여 하지 않는다.
	* 
	* 하지만 C++에는 이 단단한 타입 시스템을 가볍게 터트려 버리는 괴물이 있는데, 바로 캐스트(cast), 즉 형변환이다.
	* 캐스트는 온갖 에러의 근원이며, 굉장히 조심히 써야 하는 기능이다.
	*/
	
	/*
	* 캐스팅 문법은 세 가지로 나뉜다.
	*/ 
	
	// 1. C 스타일 캐스트
	(T)expression
	;

	// 2. 함수 방식 캐스트
	T(expression)
	;
	/*
	* 3. C++ 스타일 캐스트
	* C++ 스타일 캐스트는 좀 특별하다. 이들은 단순한 형변환이 아니라 각자 독자적인 역할이 존재한다.
	*/
	const_cast<T>(expression)
	;
	dynamic_cast<T>(expression)
	;
	reinterpret_cast<T>(expression)
	;
	static_cast<T>(expression)
	;
	/*
	* 1. const_cast는 객체의 상수성이나 휘발성을 제거하는 용도로 사용한다.
	* 
	* 2. dynamic_cast는 안전한 다운캐스팅을 할 때 사용한다. 
	*    즉 주어진 객체가 어떠한 클래스 상속 계통에 속한 특정 타입인지 아닌지를 결정하는 작업에 사용한다.
	*    참고로 이 캐스트는 런타임 비용이 신경쓰일 정도로 높다.
	* 
	* 3. reinterpret_cast는 포인터를 int로 변환하는 등의 하부 수준 캐스트를 할 때 사용한다. 
	*    이것의 적용 결과는 구현환경에 의존적이다. 즉 이식성이 존재하지 않는다.
	*	 이러한 캐스트는 하부 수준 코드 외에는 절대 존재하지 않아야 한다.
	* 
	* 4. static_cast는 암시적 변환을 강제 진행할 때 사용한다. 
	*    흔히 이루어지는 타입 변환을 거꾸로 수행하는 용도(void*를 일반 타입 포인터로 변환한다거나, 기본 클래스의 포인터를 파생 클래스의 포인터로 바꾸는 등)로도 사용된다.
	*    물론 상수 객체를 비상수 객체로 캐스팅하는 데에 사용할 수는 안 된다.
	* 
	* 구형 스타일 캐스트는 아직 적법히 쓰이나, 지금은 C++ 스타일 캐스트를 사용하는 것이 더 바람직하다. 그 이유는 다음과 같다.
	* 1. 가독성이 증가한다.
	* 2. 캐스트 사용 목적을 명확히 지정하므로 컴파일러 측에서 적법한 사용 여부를 진단할 수 있다.
	*    즉 상수성을 제거하려 한 부분에서 const_cast 대신 다른 신형 스타일 캐스트를 실수로 사용했을 경우 코드 자체가 컴파일되지 않는다.
	*/

	/*
	* 캐스팅은 어떤 타입을 다른 타입으로 처리하라고 컴파일러에게 알리는 것뿐이 아니다.
	* 일단 타입 변환이 존재한다면 런타임에 실행되는 코드가 생성되는 경우가 적지 않다.
	*/
	int x, y;
	...;
	// x를 y로 나눔. 단, 부동소수점 나눗셈을 사용
	double d = static_cast<double>(x) / y;
	/*
	* int 타입 x를 double 타입으로 캐스팅하는 부분에서 코드가 생성된다. 
	* 그 이유는 대부분의 컴퓨터 아키텍처에서 int의 표현구조와 double의 표현구조가 아예 다르기 때문이다. 
	*/
	class Base {...};
	class Derived : public Base {...};
	Derived d;
	// Derived* -> Base* 암시적 변환
	Base* pb = &d;
	/*
	* 위의 경우에서 두 포인터의 값이 같지 않은 경우가 가끔 존재한다. 
	* 이럴 경우 포인터의 오프셋을 Derived*에 적용하여 실제의 Base* 포인터 값을 구한다.
	* 이 동작은 런타임에 발생한다.
	* 
	* 즉, 객체 하나가 지닐 수 있는 주소는 2개 이상이 될 수 있다.
	* C++에서는 다중상속이 발생할 경우 이런 현상이 항상 생기나, 단일상속인 경우에도 위와 같은 일이 발생하는 경우가 더러 있다.
	* 
	* 즉, C++에서는 데이터가 어떤 식으로 메모리에 존재할 것이라는 섣부른 가정을 피해야 한다. 
	* 또한 이러한 가정에 기반한 캐스팅은 절대 통하지 않는다.
	*/

	/*
	* 캐스팅을 쓰면 코드의 가독성이 떨어진다. 실제로 틀린 코드라도 보기에는 멀쩡한 코드로 보이는 경우가 많아진다.
	*/
	class Window {
		...;
	public:
		virtual void onResize() { ... };
		...;
	};

	class SpecialWindow : public Window {
		...;
	public:
		virtual void onResize() {
			// *this를 Window로 캐스팅하고 그것에 대해 onResize()를 호출. 동작하지 않는다.
			static_cast<Window>(*this).onResize();
			...;
		}
	};
	/*
	* 위의 경우 SpecialWindow를 통해 호출되는 onResize()는 Window::onResize()이다.
	* 문제는 실행 시 함수 호출이 이루어지는 객체가 현재 객체가 아니다!
	* 
	* 해당 코드에서는 캐스팅이 발생하면서 *this의 기본 클래스 부분에 대한 사본이 암시적으로 생성되는데, 현재의 onResize()는 바로 이 임시 객체에서 호출된다.
	* 즉, 위의 코드는 현재 객체에 대해 Window::onResize()를 호출하지 않고 지나간다.
	* 이 때 Window::onResize()가 객체를 수정하는 로직을 지니고 있을 경우 현재 객체가 아니라 현재 객체의 사본에 수정이 적용된다.
	* 물론 SpecialWindow::onResize()에서 객체 수정 시에는 현재 객체가 수정된다.
	* 
	* 이 문제를 해결하기 위해서는 캐스팅을 하지 말아야 한다.
	* 그냥 현재 객체에 대해 onResize()의 기본 클래스 버전을 호출하도록 하면 된다.
	*/

	class SpecialWindow : public Window {
		...;
	public:
		virtual void onResize() {
			// *this에서 Window::onResize()를 호출
			Window::onResize();
			...;
		}
	};

	/*
	* dynamic_cast는 상당수 구현환경에서 상당히 느리게 동작하도록 구현되어 있다. 
	* 대표적으로 MetroWerks CodeWarriors의 C++ 컴파일러의 경우 클래스 이름에 대한 문자열 비교 연산에 기반을 두어 dynamic_cast가 구현되어 있다.
	* 즉, 깊이가 4인 단일상속계통에 속한 어떠한 객체에 대해 이 연산자를 적용할 때 해당 구현환경에서는 클래스 이름을 비교하기 위해 strcmp가 최대 4회까지 호출될 수 있다.
	* 상속 깊이가 더 깊거나, 다중 상속을 사용하는 경우 비용이 더욱 커진다.
	* 아무튼, 수행 성능이 중요시되는 코드라면 dynamic_cast를 조심히 사용하여야 한다.
	* 
	* dynamic_cast가 분명 유용히 보이는 때는 있다.
	* 예로 파생 클래스 객체임이 분명한 것이 있어 파생 클래스의 함수를 호출하고 싶은데, 해당 객체를 조작할 수 있는 수단이 기본 클래스의 포인터밖에 없는 경우가 적지 않다.
	* 이러한 경우를 회피하는 방법은 일반적으로 두 가지가 있다.
	* 
	* 1. 파생 클래스 객체에 대한 포인터나 스마트 포인터를 컨테이너에 담아서 각 객체를 기본 클래스 인터페이스로 조작할 필요를 아예 없애기.
	*    이를테면 지금까지 봐 왔던 Window 및 SpecialWindow 상속계통에서 블링크 기능을 SpecialWindow 객체만 지원하게 되어 있을 경우 다음과 같이 작성할 수 있다.
	*/
	
	// 잘못된 예시. dynamic_cast를 사용하고 있음
	{
		class Window { ... };

		class SpecialWindow : public Window {
			...;
		public:
			void blink();
			...;
		};

		typedef std::vector<std::tr1::shared_ptr<Window>> VPW;
		VPW winPtrs;

		...;

		for (VPW::iterator iter = winPtrs.begin(); iter != winptrs.end(); ++iter) {
			if (SpecialWindow * psw == dynamic_cast<SpecialWindow*>(iter->get())) psw->blink();
		}
	}

	// 옳은 예시.
	{
		typedef std::vector<std::tr1::shared_ptr<Window>> VPSW;
		VPSW winPtrs;

		...;

		for (VPSW::iterator iter = winPtrs.begin(); iter != winptrs.end(); ++iter) {
			(*iter)->blink();
		}
	}

	/*
	* 다만 이 방법의 경우 Window에서 파생될 수 있는 모든 클래스 객체에 대한 포인터를 동일 객체에 저장할 수 없다.
	* 이 경우 다른 타입의 포인터를 담기 위해서는 타입 안정성을 지니는 컨테이너 여러 개가 필요하다.
	* 
	* 2. Window의 파생을 전부 기본 클래스 인터페이스를 통해 조작할 수 있는 방법은, 원하는 조작을 가상함수 집합으로 정리하여 기본 클래스에 정의하면 된다. 
	*/
	class Window {
		...;
	public:
		// 가상함수
		virtual void blink() {};
		...;
	};

	class SpecialWindow : public Window {
		...;
	public:
		// 구현
		virtual void blink() {...}
	};

	// Window에서 파생된 모든 타입의 객체에 대한 포인터들을 담는 컨테이너
	typedef std::vector<std::tr1::shared_ptr<Window>> VPW;
	VPW winPtrs;

	...;

	for (VPSW::iterator iter = winPtrs.begin(); iter != winptrs.end(); ++iter) {
		(*iter)->blink();
	}

	/*
	* 둘 중 어느 방법도 모든 상황에 적용하기에는 불가능하다만, 상당히 많은 상황에서 dynamic_cast를 사용하는 방법 대신 쓸 수 있다.
	* 
	* 정말 피해야 하는 설계는 cascading dynamic_cast라고 불리는 구조이다.
	*/
	class Window { ... };
	class SpecialWindow1 : public Window { ... };
	class SpecialWindow2 : public Window { ... };
	class SpecialWindow3 : public Window { ... };

	typedef std::vector<std::tr1::shared_ptr<Window>> VPW;
	VPW winPtrs;

	for (VPSW::iterator iter = winPtrs.begin(); iter != winptrs.end(); ++iter) {
		if		(SpecialWindow1 * psw1 == dynamic_cast<SpecialWindow1*>(iter->get())) { ... }
		else if (SpecialWindow2 * psw2 == dynamic_cast<SpecialWindow2*>(iter->get())) { ... }
		else if (SpecialWindow3 * psw3 == dynamic_cast<SpecialWindow3*>(iter->get())) { ... }
	}
	/*
	* 위와 같은 코드는 Window 클래스 계통이 변경되었을 때 수정 고려대상이 되므로, 이러한 코드는 절대 사용하지 말아야 하며 보면 즉각 수정하여야 한다.
	*/

	/*
	* 정말 잘 작성된 C++ 코드는 캐스팅을 거의 사용하지 않는다. 다만 캐스팅이 필요한 상황에서도 사용하지 않는 것 또한 문제가 된다.
	* 캐스팅은 최대한 격리하여야 한다. 즉 캐스팅을 해야 하는 코드를 내부 함수에 몰아넣고 해당 함수의 구현에서는 호출하는 외부에서 알 수 없도록 인터페이스로 가리면 된다.
	*/
}
/*
* 결론.
* 1. 캐스팅은 최대한 피하라. 특히 수행 성능이 민감한 코드에서 dynamic_cast는 빼는 것을 최선으로 고려하라.
*    설계 중에 캐스팅이 필요해진 경우 캐스팅을 사용하지 않는 다른 방법을 시도해 보라.
* 2. 캐스팅이 어쩔 수 없이 필요해진 경우 함수 내에 숨길 수 있도록 하라. 사용자가 캐스팅을 사용하지 않을 수 있다.
* 3. 구형 C 스타일 캐스트보다 C++ 스타일 캐스트를 선호하라.
*/

#endif