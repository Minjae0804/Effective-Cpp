#if 0

// #21. 함수에서 객체를 반환하여야 할 경우 참조를 반환하려 하지 마라


// 
{
	class Rational {
		int n, d;
		friend const Rational operator*(const Rational& lhs, const Rational& rhs);
	public:
		Rational(int numerator = 0, int denominator = 1);
	};
	/*
	* 해당 클래스의 operator*는 곱셈 결과를 값으로 반환하도록 설계되어 있다.
	* 그런데, 과연 여기에 비용을 들여야 할까?
	* 다시 말해, operator*의 반환이 반드시 Rational, 즉 값 반환이 되어야 할까? 참조가 되어서는 안 될까?
	* 
	* 값이 아닌 참조를 반환할 수 있으면 비용 부담은 확실히 없을 테지만, 참조는 근본적으로 존재하는 객체에 대한 별명이다.
	* operator*에서 참조를 반환한다면, 그 참조는 당연히 존재하는 객체를 가리켜야 한다.
	* 
	* 그렇다면 반환될 객체는 어디에 있을까?
	* 함수 수준에서 신규 객체를 생성하는 방법은 두 가지뿐이다.
	* 첫 번째는 스택에 생성하는 것이고, 두 번째는 힙에 생성하는 것이다.
	* 
	* 스택에 생성하는 경우는 지역 변수로 정의하면 된다.
	*/

	// 먼저 말하건데 이건 목적을 상실한 코드다.
	const Rational& operator*(const Rational& lhs, const Rational& rhs) {
		Rational result(lhs.n * rhs.n, lhs.d * rhs.d);
		return *result;
	}

	/*
	* 사소한 문제가 있다면, 지역 객체의 경우 스코프 이탈 시 소멸자가 실행되며 사라진다는 점이다.
	* 때문에 반환된 참조는 결과 객체...였던 메모리 공간을 가리키게 된다.
	* 당연하겠지만, 이것은 댕글링 포인터가 되어 온갖 에러의 주범이 된다.
	* 
	* 그럼, 힙에 생성하는 경우는 어떻게 될까?
	*/

	// 이러면 안 된다.
	const Rational& operator*(const Rational& lhs, const Rational& rhs) {
		Rational *result = new Rational(lhs.n * rhs.n, lhs.d * rhs.d);
		return *result;
	}

	/* 
	* 생성자가 호출되기는 매한가지이기도 하나, 다른 중요한 문제가 있다.
	* 함수 탈출 이후에는 힙의 객체를 해제할 방법이 사라진다. 
	*/
	Rational w, x, y, z;
	w = x * y * z;
	/*
	* 여기에서는 한 문장 내에서 operator* 호출이 2회 발생하므로 new와 더불어 delete도 2회 호출되어야 한다.
	* 그러나 사용자는 delete를 할 수 있는 방법이 하나도 없다. 반환되는 참조 뒤의 숨겨진 포인터에 대해 접근할 방법이 공중분해되었기 때문이다.
	* 
	* 지금까지의 두 코드는 문제 하나를 공유하고 있었다. 바로 생성자 호출이다.
	* 참조 반환의 목적은 생성자를 호출하지 않음으로서 메모리 자원을 절약함에 있는데, 두 방법 모두 결론적으로 생성자를 호출한다.
	* 
	* 그럼 마지막으로 생성자를 호출하지 않는 방법, Rational 객체를 정적 객체로 함수 내에 정의하고 그 참조를 반환하는 식으로 사용한다면?
	*/
	const Rational& operator*(const Rational& lhs, const Rational& rhs) {
		static Rational result;
		result = ...;
		return *result;
	}
	/*
	* 정적 객체를 사용하는 설계가 항상 그러하듯 이 설계는 스레드 안전성에 문제가 있다.
	* 하지만 그것보다 더 확실한 문제가 있다. 
	*/
	bool operator==(const Rational& lhs, const Rational& rhs);
	Rational a, b, c, d;
	...
	if ((a * b) == (c * d)) ...;
	else ...;
	/*
	* 정적 객체를 사용하면 위의 (a * b) == (c * d)가 항상 true로 판정된다.
	* 모든 동류 정적 객체가 단일한 주소를 공유하기 때문이다.
	* 간단히 말해서, 위의 경우 operator==은 operator* 내부 정적 객체의 값과 operator* 내부 정적 객체의 값을 비교한다.
	* 
	* 신규 객체를 반환해야 하는 함수를 작성하는 방법은 간단하다.
	* 신규 객체를 반환하게 하면 된다.
	*/
	inline const Rational operator*(const Rational& lhs, const Rational& rhs) {
		return Rational(lhs.n * rhs.n, lhs.d * rhs.d);
	}
	/*
	* 이 함수는 호출 시 반환값을 생성하고 소멸시키는 비용이 발생한다.
	* 하지만 이는 적절한 비용이다.
	* 심지어 현대 C++에서는 RVO(반환 값 최적화) 덕분에 이 사본 생성 비용마저 0에 가깝다.
	* 
	* 
	* 가장 중요한 것은 어떠한 선택을 하든 올바른 동작이 이루어지게 하는 것이다.
	* 그것을 가장 먼저 고민하라.
	*/
}
/*
* 결론.
* 1. 지역 스택 객체에 대한 포인터나 참조 반환, 힙에 할당된 객체에 대한 참조 반환, 지역 정적 객체에 대한 포인터나 참조 반환은 그러한 객체가 2개 이상 필요해질 가능성이 있을 경우 하지 마라.
*/

#endif