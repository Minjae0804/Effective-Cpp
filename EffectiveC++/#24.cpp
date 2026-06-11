#if 0

// #24. 타입 변환이 모든 매개변수에 대해 적용되어야 한다면 비멤버 함수를 선언하라


// 
{
	/*
	* 클래스에서 암시적 타입 변환을 지원하는 것은 일반적으로 좋은 생각이 아니다.
	* 물론 이 규칙에도 예외가 존재하는데, 가장 대표적인 것이 숫자 타입을 생성할 때이다.
	* 
	* 예를 들어 유리수를 나타내는 클래스를 만들고 있다면, 정수에서 유리수로의 암시적 변환은 허용하자고 판단하더라도 논리상 어색하지는 않다.
	*/
	class Rational {
		...;
	public:
		Rational(int numerator = 0, int denominator = 1);
		// 분자 및 분모에 대한 접근용 함수
		int numerator() const;		
		int denominator() const;
	};
	/*
	* 유리수를 나타내는 클래스이니만큼 덧셈이나 곱셈 등의 수치연산을 지원하는 게 좋을 것 같은데, 이걸 어떻게 구현하는 것이 좋을까?
	* 일단 유리수의 곱셈은 Rational 클래스 자체와 관련이 있으므로 operator*는 Rational 클래스 내부에 구현하는 게 자연스러울 것 같다.
	*/
	class Rational {
	public:
		const Rational operator*(const Rational& rhs) const;
	};
	/*
	* 이렇게 설계해두면 유리수 곱셈을 쉽게 할 수 있다.
	*/
	Rational oneEighth(1, 8);
	Rational oneHalf(1, 2);
	Rational Result = oneHalf * oneEighth;
	result = result * oneEighth;
	/*
	* 그러나, 여기서 혼합형 수치 연산도 지원하면 어떨까?
	* 즉, Rational을 int같은 것과도 곱한다면?
	*/

	result = oneHalf * 2;	// 정상
	result = 2 * oneHalf;	// 에러

	/*
	* 왜 두 번째 것은 에러가 발생할까?
	* 이 두 예제를 함수 형태로 바꾸어 써 보면 알 수 있다.
	*/
	result = oneHalf.operator*(2);
	result = 2.operator*(oneHalf);

	/*
	* 두 번째 예제에서 2는 정수 데이터일 뿐이므로 호출할 수 있는 operator*()같은 것이 있을 리가 없다.
	* 컴파일러가 2의 operator*을 2의 멤버에서 찾지 못했으므로 호출할 수 있는 비멤버 버전의 operator*도 찾아본다.
	*/
	result = operator*(2, oneHalf);
	/*
	* 그러나 작금의 예제에서는 int와 Rational을 매개변수로 받는 비멤버 버전 operator*이 없으므로 컴파일 에러가 발생한다.
	* 그럼 첫 번째 예제는 왜 성공했을까?
	* 
	* 첫 번째 예제는 Rational의 operator*이 2를 정상적으로 처리했다. 하지만 Rational::operator*는 Rational을 매개변수로 받는다. 왜 에러가 발생하지 않았을까?
	* 암시적 타입 변환에 그 이유가 존재한다.
	* 
	* 컴파일러는 매개변수로 Rational을 요구하는 operator*에 int가 넘겨졌다는 사실을 인지하였으나, 이 int를 Rational 클래스의 생성자에 주어 호출할 경우 Rational로 둔갑시킬 수 있다는 사실 또한 인지하고 있다.
	* 따라서, 결론적으로 다음과 같은 코드가 수행된 결과가 된다.
	*/
	const Rational temp(2);
	result = oneHalf * temp;
	/*
	* 물론 컴파일러가 이렇게 동작한 이유는 explicit으로 선언되지 않은 생성자가 존재하기 때문이다. 
	* Rational 생성자가 만약 명시호출 생성자였을 경우 다음의 코드 중 어느 쪽도 컴파일되지 않는다.
	*/
	result = oneHalf * 2;
	result = 2 * oneHalf;
	/*
	* 명시호출 생성자를 쓰면 최소한 두 문장의 동작은 일관되게 유지하겠으나 혼합형 수치연산에 대한 지원이 불가능해진다.
	* 그렇다면 동작도 일관되게 유지하고 혼합셩 수치연산도 지원하도록 하게 하려면 어떻게 해야 할까?
	* 
	* 암시적 타입 변환에 대해 매개변수가 적용되기 위해서는 매개변수 리스트에 존재해야 한다는 것이다.
	* 즉 호출되는 멤버함수를 지니는 객체에 해당하는 암시적 매개변수에는 암시적 변환이 적용되지 않는다.
	* 두 번째 문장이 컴파일되지 않는 이유는 이것에 기반한다. 전자의 경우 매개변수 리스트에 존재하는 매개변수가 쓰이나, 후자의 경우 그렇지 않기 때문이다.
	* 
	* 따라서, operator*을 비멤버 함수로 만들어 컴파일러 쪽에서 모든 인자에 대해 암시적 타입 변환을 수행하도록 하게 하는 것이다.
	*/
	class Rational {
		...;
	};

	const Rational operator*(const Rational& lhs, const Rational& rhs) {
		return Rational(lhs.numerator() * rhs.numerator(), lhs.denominator() * rhs.denominator());
	}

	Rational oneFourth(1, 4);
	Rational result;

	result = oneFourth * 2;
	result = 2 * oneFourth;
	/*
	* 여기에서 궁금증 하나. operator*는 Rational의 프렌드 함수로 두어도 될까?
	* 해당 예제의 경우에서는 아니다. operator*는 완전히 Rational의 public 인터페이스만을 써서 구현할 수 있기 때문이다.
	* 
	* 프렌드 함수는 가능하면 피해야 한다. 
	* 물론 반드시 프렌드 함수를 사용해야 하는 경우가 존재하기는 한다만, 멤버 함수면 안 된다는 말은 프렌드 함수를 사용하여야 한다는 말이 절대 아니라는 사실은 불변한다.
	*/
}
/*
* 결론.
* 1. 어떤 함수에 들어가는 모든 매개변수에 대해 차입 변환을 하여야 할 필요가 있다면 해당 함수는 비멤버여야 한다.
*/

#endif