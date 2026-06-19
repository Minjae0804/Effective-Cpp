#if 0

// #33. 상속된 식별자를 숨기는 것은 피하라


// 
{
	/*
	* 사실 상속이라는 말을 달고 시작했지만 상속과는 별 관계가 없다.
	* 진짜 관계있는 것은 스코프다.
	*/
	int x;

	void someFunc() {
		double x;
		std::cin >> x;
	}
	/*
	* 값을 읽어 x에 넣는 위 문장에서 실제로 참조하는 x는 지역 변수이다.
	* 유효 스코프에 존재하는 식별자가 외부 스코프에 존재하는 식별자을 가리기 때문이다.
	*
	* 컴파일러가 someFunc의 유효범위 내부에서 x라는 식별자를 만날 경우 일단 해당 컴파일러는 처리 중인 스코프, 즉 로컬 스코프를 뒤져서 동일한 식별자가 존재하는지 찾는다.
	* 위의 경우 x가 로컬 스코프에 존재하므로 컴파일러는 이외 스코프를 탐색하지 않는다.
	* 타입이 다르긴 하다만 그것은 중요 사안이 아니다.
	*
	* 상속으로 넘어가, 기본 클래스에 속해 있는 것을 파생 클래스 멤버 함수 내부에서 참조하는 문장이 있을 경우, 컴파일러는 참조대상을 바로 찾을 수 있다.
	* 기본 클래스에 선언된 것은 파생 클래스가 모두 물려받기 때문이다.
	* 어떻게 이런 일이 일어나느냐 하면, 파생 클래스의 스코프가 기본 클래스의 스코프 내부에 중첩되어 존재하기 때문이다.
	*/
	class Base {
		int x;
	public:
		virtual void mf1() = 0;
		virtual void mf2();
		void mf3();
	};
	class Derived : public Base {
	public:
		virtual void mf1();
		void mf4();
	};
	/*
	* mf4()가 파생 클래스에서 다음과 같이 구현되어 있다고 가정하자.
	*/
	void Derived::mf4() {
		mf2();
	}
	/*
	* 컴파일러는 해당 함수 내부를 차례로 읽어 가다가 mf2라는 식별자를 발견하는데, 이 mf2가 어디에 존재하는 식별자인지를 먼저 파악한다.
	* 이 파악을 위해 컴파일러는 mf2라는 식별자가 붙은 것의 선언문이 들어 있는 스코프를 탐색한다. 
	* 먼저 로컬 스코프를 탐색하고, 그것을 감싸는 스코프를 탐색해간다.
	* 결론적으로 Base에서 찾게 되고, 여기에서 찾지 못하면 그것을 둘러싸는 네임스페이스에서 탐색 후 마지막으로 전역에서 탐색하게 된다.
	*/
	class Base {
		int x;
	public:
		virtual void mf1() = 0;
		virtual void mf1(int);
		virtual void mf2();
		void mf3();
		void mf3(double);
	};

	class Derived : public Base {
	public:
		virtual void mf1();
		void mf3();
		void mf4();
	};
	/*
	* 이 경우 식별자 탐색 시점에서 mf1()과 mf3()은 Derived가 Base로부터 상속한 것이 아니게 된다.
	*/
	Derived d;
	int x;

	d.mf1();
	// 에러
	d.mf1(x);

	d.mf2();

	d.mf3();
	// 에러
	d.mf3(x);
	/*
	* 때문에 위의 경우 상속한 멤버함수를 오버로딩까지 파생 클래스에서 가려버리므로 에러가 발생한다.
	* 즉, 기본 클래스와 파생 클래스에 있는 식별자가 동일한 함수들이 받아들이는 매개변수 타입이 같건 말건 별 상관이 없다.
	* 심지어 함수들이 가상인지 비가상인지조차 상관없다.
	* 
	* 물론 이렇게 동작하는 이유는 존재한다.
	* 어떠한 라이브러리나 응용프로그램 프레임워크를 이용하여 파생 클래스를 생성할 때, 멀리 떨어져 있는 기본 클래스로부터 오버로드 버전을 상속시키는 경우를 막는다.
	* 뭐 그 기능을 쓰지 않는 경우가 태반이니 문제지만.
	* 
	* 가려진 식별자는 using 선언을 통해 가져올 수 있다.
	*/
	class Base {
		int x;
	public:
		virtual void mf1() = 0;
		virtual void mf1(int);
		virtual void mf2();
		void mf3();
		void mf3(double);
	};

	class Derived : public Base {
	public:
		using Base::mf1;
		using Base::mf3;

		virtual void mf1();
		void mf3();
		void mf4();
	};

	Derived d;
	int x;

	d.mf1();
	d.mf1(x);

	d.mf2();

	d.mf3();
	d.mf3(x);
	/*
	* 이를 통해 위 코드는 정상적으로 작동한다.
	* 즉 기본 클래스로부터 상속을 받으려 하는데 오버로딩한 함수가 해당 클래스에 존재하며 이 함수 중 몇 가지만 오버라이딩하고 싶을 경우 각 식별자에 대해 using 선언을 붙여야 한다.
	* 
	* 기본 클래스가 지니는 함수를 전부 상속하고 싶은 것이 아닌 경우도 있기는 하다. 물론, 이 경우와 public 상속은 떼어두어야 한다.
	* 기본 클래스와 파생 클래스 사이의 is-a 관계가 붕괴하기 때문이다.
	* 
	* 하지만 private 상속을 쓰면 이 경우가 말이 될 수 있다.
	* 일례로 Derived가 Base로부터 private 상속이 이루어졌다고 가정하고, Derived가 상속했으면 하는 mf1()은 매개변수가 없는 버전 하나만이라고 하자.
	* 이 경우 using 선언으로 해결할 수가 없다. using 선언을 하면 해당 식별자에 해당하는 것들이 모두 파생으로 내려가기 때문이다.
	* 
	* 이 경우 전달 함수(forwarding function)을 정의하여 해결할 수 있다.
	*/
	class Base {
	public:
		virtual void mf1() = 0;
		virtual void mf1(int);
	};

	class Derived : private Base {
	public:
		// 전달 함수
		virtual void mf1() { Base::mf1(); }
	};

	Derived d;
	int x;

	d.mf1();
	// 에러
	d.mf1(x);
}
/*
* 결론.
* 1. 파생 클래스의 식별자는 기본 클래스의 식별자를 가린다. public 상속에서는 이러한 현상이 바람직하지 않다.
* 2. 가려진 식별자를 다시 볼 수 있게 하는 방법을 using 선언이나 전달 함수를 사용할 수 있다.
*/

#endif