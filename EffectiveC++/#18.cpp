#if 0

// #18. 인터페이스 설계는 제대로 쓰기에는 쉽게, 잘못 쓰기에는 어렵게 하라


//
{
	/*
	* 제대로 쓰기 쉽고 잘못 쓰기 어려운 인터페이스를 개발하기 위해서는 먼저 사용자가 저지를 만한 실수의 종류를 머리에 넣어두고 있어야 한다.
	*/
	class Date {
	public:
		Date(int month, int date, int year);
		...
	};
	/*
	* 딱히 문제는 없어 보이지만, 여기에는 사용자가 오류를 저지를 만한 지점이 최소 2군데 존재한다.
	* 첫 번째는 매개변수 전달 순서가 잘못될 여지가 있다는 것이다.
	*/
	Date d(30, 3, 1995);	// 월과 일의 순서가 바뀌었다.
	/*
	* 두 번째는 월과 일에 해당하는 숫자가 이상한 숫자일 수 있다는 것이다.
	*/
	Date d(2, 29, 1995);	// 1995년의 2월은 28일이 마지막이다.
	/*
	* 이는 신규 타입으로 인터페이스를 강화하여 해결할 수 있다.
	* 사실 이상한 코드가 컴파일되는 경우로부터 사용자를 지키는 것이 바로 타입 시스템이다.
	* 
	* 위와 같은 경우를 대비해 일/월/년을 구분하는 간단한 래퍼 타입을 만들고, 이를 Date 생성자에 둘 수 있다.
	*/
	struct Day	{ explicit Day(int d)	: val(d); int val; };
	struct Month{ explicit Month(int m) : val(m); int val; };
	struct Year { explicit Year(int y)  : val(y); int val; };

	class Date {
	public:
		Date(const Month& m, const Day& d, const Year& y);
		...
	};

	Date d(30, 3, 1995);					// 타입이 틀림
	Date d(Day(30), Month(3), Year(1995));	// 순서가 틀림
	Date d(Month(3), Day(30), Year(1995));
	/*
	* 물론 Day 및 Month, Year에 데이터를 이것저것 숨겨 자체적으로 기능하는 클래스로 만들면 단순 구조체보다는 나을지도 모른다.
	* 그러나 위와 같이 타입을 적절히 준비하기만 해도 인터페이스 사용 에러를 방지하는 데에는 손색이 없다.
	* 
	* 적절한 타입만 준비되어 있다면 각 타입의 값에 제약을 가하더라도 괜찮은 경우가 생긴다.
	* 예를 들어 Month가 지닐 수 있는 적절한 범위는 1~12이므로 이를 제약으로 활용할 수 있다.
	* 
	* 한 가지 방법으로 월 표시 값을 나타내는 enum을 넣는 방법이 있는데, enum은 타입 안전성이 그리 좋지 못하다. 때로는 int처럼 사용될 수도 있기도 하다.
	* 타입 안전성이 문제될 것 같다면 유효 Month 집합을 미리 정의해 두어도 괜찮다.
	*/
	class Month {
		// Month 값이 신규 생성되지 못하도록 명시호출 생성자를 private로 둔다.
		explicit Month(int m);
	public:
		// 유효한 Month 값을 반환하는 함수들
		static Month Jan() { return Month(1); }
		static Month Feb() { return Month(2); }
		...
		static Month Dec() { return Month(12); }
	};
	/*
	* 특정 월을 나타내는 데에 왜 함수를 사용하느냐에 대한 물음은 #4를 참고하면 된다.
	* 다시 말하지만 비지역 정적 객체의 초기화의 위험성은 절대 무시할 수 있는 것이 아니다.
	* 
	* 예상되는 사용자 실수를 방지하는 다른 방법으로는, 어떠한 타입이 제약을 부여하여 해당 타입을 통해 할 수 있는 일들을 묶어버리는 것이 있다.
	* 제약 부여로 아주 흔히 사용되는 방법은 상수로 선언하는 것이 있다.
	* 
	* 사실, 특정한 이유가 없다면 사용자 정의 타입은 기본 제공 타입처럼 동작하게 하는 것이 이롭다.
	* int 정도의 타입의 사용법은 사용자들이 모를 리가 없으므로, 사용자 정의 타입도 그리 동작하게 하여 직관성과 일관성을 확보하는 것이다.
	* 제대로 쓰기 좋은 인터페이스를 만드는 요인 중 일관성만큼 중요한 것이 별로 없으며, 쓰기 나쁜 인터페이스를 만드는 요인 중 비일관성만큼 큰 것이 별로 없다.
	* STL 컨테이너의 인터페이스는 전반적으로 일관성을 지니며, 이로 인해 사용에 큰 부담이 없다.
	* 
	* 사용자 측에서 뭔가를 외워야 제대로 쓸 수 있는 인터페이스는 잘못 쓰기 쉽다. 사용법을 언제라도 잊어버릴 수 있으므로.
	* #13의 팩토리 함수를 예로 들자면, 해당 함수는 Investment 클래스 계통에 해당하는 어떤 객체를 동적할당하고 해당 객체의 포인터를 반환한다.
	*/
	Investment* createInvestment();
	/*
	* 해당 함수를 사용할 때에는 자원 누출을 피하기 위해 createInvestment에서 얻어낸 포인터를 나중에라도 삭제해야 한다.
	* 그러나 이 점 때문에 사용자는 포인터 삭제를 잊거나 동일 포인터에 대해 delete를 두 번 이상 적용할 수 있다.
	* 
	* 때문에 #13에서는 해당 함수의 반환을 스마트 포인터에 저장하여 포인터 삭제의 책임을 이임하는 방식을 쓸 수 있다.
	* 그러나, 사용자가 이 스마트 포인터를 사용하는 것조차 잊어버린다면 어떻게 해야 할까?
	* 이를 위해, 해당 함수가 스마트 포인터 자체를 반환하게 할 수 있다.
	*/
	std::tr1::shared_ptr<Investment> createInvestment();
	/*
	* 이렇게 하면 해당 함수의 반환은 해당 객체를 저장한 스마트 포인터가 된다.
	* 사용자가 객체 메모리 해제를 잊었을 때 스마트 포인터가 메모리 해제를 함으로서 편의성이 높아진다.
	*/

	/*
	* 아니면, createInvestment를 통해 얻은 Investment*를 직접 삭제하게 하지 않고 getRidOfInvestment라는 함수를 준비하여 해당 함수에 넘기게 하는 것은 어떨까?
	* 이 방법은 겉으로는 깔끔해 보이지만, 이러한 인터페이스는 사용자의 실수를 하나 더 유발하게 된다. 자원 해제 매커니즘을 잘못 사용하는 경우가 생기기 때문이다.
	* createInvestment를 살짝 고쳐, getRidOfInvestment가 삭제자로 묶인 shared_ptr을 반환토록 구현하는 것이 훨씬 낫다.
	* 
	* shared_ptr에는 두 개의 인자를 받는 생성자가 있다. 첫 번째 인자는 스마트 포인터로 관리할 실제 포인터, 두 번째 인자는 참조 카운트가 0이 될 때 호출될 삭제자이다.
	* 따라서 shared_ptr이 널 포인터를 물게 함과 동시에 삭제자로 getRidOfInvestment를 지니게 하는 방법으로 다음과 같은 방법을 생각할 수 있다.
	*/
	std::tr1::shared_ptr<Investment> pInv(0, getRidOfInvestment);
	/*
	* 물론 이는 제대로 작성된 코드가 아니다. shared_ptr의 해당 생성자의 첫 인자는 포인터여야 하지만, 0은 포인터가 아니므로.
	* 물론 0은 포인터로 캐스팅이 가능하지만, 지금의 경우는 이것만으로는 부족하다. shared_ptr이 요구하는 포인터는 Investment* 타입 포인터이기 때문이다.
	* 때문에 캐스팅을 통해 해결한다.
	*/
	std::tr1::shared_ptr<Investment> pInv(static_cast<Investment*>(0), getRidOfInvestment);
	/*
	* 이것이 createInvestment()에서 getRidOfInvestment를 삭제자로 지니는 shared_ptr을 반환하도록 구현하는 방법이다.
	*/
	std::tr1::shared_ptr<Investment> createInvestment() {
		std::tr1::shared_ptr<Investment> retVal(static_cast<Investment*>(0), getRidOfInvestment);
		// retVal은 실제 객체를 가리키도록 한다.
		retVal = ...;
		return retVal;
	}
	/*
	* 다만, 이 코드는 그리 좋은 코드가 아니다!
	* retVal로 관리할 실제 객체가 스마트 포인터의 생성보다 앞설 수 있다면 retVal의 포인터를 null로 초기화하고 나중에 대입하는 것보다 바로 생성자에 넘겨버리는 게 훨씬 낫다.
	* 
	* shared_ptr은 좋은 특징이 하나 있는데, 포인터별 삭제자를 자동 사용하여 사용자의 다른 하나의 실수를 방지한다는 점이다. 
	* 이 다른 하나의 실수는 '교차 DLL 문제'라 불리는데, 객체 생성 시 DLL의 new를 사용했을 때 해당 객체 삭제 시 해당 DLL이 아닌 다른 DLL의 delete를 사용하는 경우이다.
	* 이 경우 거의 반드시 런타임 에러가 발생하는데, shared_ptr은 이를 방지할 수 있다. 해당 클래스의 기본 삭제자는 shared_ptr이 생성된 DLL과 동일한 DLL에서 delete를 사용하도록 되어 있기 때문이다.
	*/
	class Stock : public Investment {...};

	// createInvestment()가 반환하는 shared_ptr는 교차 DLL 문제를 걱정하지 않아도 된다.
	std::tr1::shared_ptr<Investment> createInvestment() { return std::tr1::shared_ptr<Investment>(new Stock); }
}
/*
* 결론.
* 1. 좋은 인터페이스를 만들려면 제대로 쓰기 쉽게, 잘못 쓰기 어렵게 하라.
* 2. 사용자가 인터페이스를 제대로 쓰게 하는 방법으로는 인터페이스 간 일관성 확보, 기본제공 타입과의 동작 호환성 유지가 있다.
* 3. 사용자의 실수 방지를 위해서는 신규 타입 생성, 타입에 대한 연산 제한, 객체의 값에 대한 제약, 자원 관리 책임을 사용자에게 두지 않기 등이 있다.
* 4. shared_ptr은 사용자 정의 삭제자를 지원한다. 하여 교차 DLL 문제를 방지하며, 뮤텍스 등을 자동으로 잠금 해제하는 등에 사용할 수 있다.
*/

#endif