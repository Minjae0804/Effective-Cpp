#if 0

// #46. 타입 변환이 바람직할 경우에는 비멤버 함수를 클래스 템플릿 내부에 정의해 두어라


//
{
	template<typename T>
	class Rational {
	public:
		Rational(const T& numerator = 0, const T& denominator = 1);

		const T numerator() const;
		const T denominator() const;
		...;
	};

	template<typename T>
	const Rational<T> operator*(const Rational<T>& lhs, const Rational<T>& rhs) { ... }
	/*
	* 모든 매개변수에 대해 암시적 타입 변환이 되게 하기 위해서는 비멤버 함수밖에 방법이 없다.
	* #24에서도 그랬듯이 혼합형 수치 연산은 여전히 필요하므로 위와 같은 코드는 여전히 컴파일되어야 하지만, 놀랍게도 아래 코드는 컴파일되지 않는다.
	*/
	Rational<int> oneHalf(1, 2);
	Rational<int> result = oneHalf * 2;		// 컴파일 에러
	/*
	* 이유는 컴파일러가 해당 시점에 어떠한 함수를 호출하려고 하는지에 대해 알지 못하기 때문이다. 
	* 그저 operatot*라는 템플릿으로부터 인스턴스화할 함수를 결정하기 위해 온갖 계산을 동원하게 된다.
	* 
	* 이 시점에서 컴파일러가 확실하게 아는 것은, Rational<T> 타입 매개변수를 두 개 받아들이는 operator*를 인스턴스로 만들어야 한다는 것 뿐이다.
	* 문제는 그 T가 무엇인지에 대해 알아야 하는데, 컴파일러는 그걸 알아낼 수 있는 능력이 없다는 것이다.
	* 
	* 컴파일러는 T가 무엇인지를 알아내기 위해 operator* 호출에 넘겨진 인자의 모든 타입을 파악한다. 위 예제의 경우 Rational<int> 및 int이다.
	* oneHalf 쪽은 첫 번째 매개변수가 Rational<T>로 선언되어 있고 operator*에 넘겨진 첫 인자도 그것이므로 T는 int로 직관적으로 파악할 수 있다. 
	* 다만 2 쪽은 타입 유추가 어려운데, operator*의 두 번째 매개변수는 Rational<T>이지만 인수는 int형이기 때문이다.
	* 
	* 이 때 컴파일러는 어떻게 해야 T가 무엇인지를 알아낼 수 있을까?
	* Rational<int>에는 explicit으로 선언되지 않은 생성자가 들어 있으므로 컴파일러가 해당 생성자를 통해 int를 Rational<int>로 유추할 수 있지 않을까?
	* 
	* 하지만 컴파일러는 그렇게 하지 못한다. 중요한 내용인데, 템플릿 인자 추론 과정에서는 암시적 타입 변환이 고려되지 않기 때문이다.
	*/

	/*
	* 클래스 템플릿 내부에 프렌드 함수를 삽입하면 함수 템플릿으로서의 성격을 주지 않고 특정한 함수 하나를 나타낼 수 있다.
	* 즉, Rational<T> 클래스에 대해 operator*를 프렌드로 선언하는 것이 가능하다는 것이다.
	* 
	* 클래스 템플릿은 템플릿 인자 추론에 좌우되지 않는다. 템플릿 인자 추론은 함수 템플릿에 대해서만 적용되는 이야기이기 때문이다.
	* 때문에 T의 정확한 정보는 Rational<T> 클ㄷ래스가 인스턴스화될 당시에 바로 알 수 있다.
	* 하여, 호출 시의 정황에 맞는 operator* 함수를 프렌드로 선언하는 데에 별 문제가 발생하지 않는다.
	*/
	template<typename T>
	class Rational {
	public:
		...;
		friend const Rational operator*(const Rational& lhs, const Rational& rhs);
	};

	template<typename T>
	const Rational<T> operator*(const Rational<T>& lhs, const Rational<T>& rhs) { ... }
	/*
	* oneHalf 객체가 Rational<int>로 선언될 경우 Rational<int> 클래스가 인스턴스로 만들어진다.
	* 이 때 그 과정의 일부로서 Rational<int> 타입 매개변수를 받는 프렌드 함수인 operator*도 자동으로 선언된다.
	* 
	* 이전과 달리 이 경우는 함수 템플릿이 아닌 함수가 선언된 것이므로 컴파일러는 해당 호출에 대해 암시적 변환 함수 규칙을 적용할 수 있게 된다.
	* 다만, 이 코드는 링킹이 되지 않는다. 이 부분에 대해서는 조금 뒤에 적는다.
	*/

	/*
	* 클래스 템플릿 내부에서는 템플릿의 이름을 제외한 것을 해당 템플릿 및 파라미터의 줄임으로 사용할 수 있다.
	* 즉, Rational<T> 내부에서는 Rational이라고만 사용해도 된다.
	* 
	* 위의 코드가 이를 적용한 것이며, 이 문법을 적용하지 않은 것은 아래와 같다.
	*/
	template<typename T>
	class Rational {
	public:
		...;
		friend const Rational<T> operator*(const Rational<T>& lhs, const Rational<T>& rhs);
	};

	/*
	* 해당 코드가 링킹되지 않는 이유는 Rational 내부에서 선언만 되어 있고 정의가 되어 있지 않기 때문이다. 
	* 이를 가장 간단히 해결하려면 operator*의 본문을 선언부에 붙이면 된다.
	*/
	template<typename T>
	class Rational {
	public:
		...;
		friend const Rational operator*(const Rational& lhs, const Rational& rhs) {
			return Rational(lhs.numerator() * rhs.numerator(), lhs.denominator() * rhs.denominator());
		}
	};

	/*
	* 이 항목에서 사용한 friend 함수에 대해서는 재미있는 사항이 하나 있는데, friend 권한은 클래스의 public이 아닌 부분에 접근하는 것과 별 상관이 없다.
	* 모든 인자에 대해 타입 변환이 가능하도록 만들기 위해 비멤버 함수가 필요하고, 호출 시의 상황에 맞는 함수를 자동으로 인스턴스하기 위해서는 해당 비멤버 함수를 클래스 내부에 선언해야 했으며, 그 유일한 방법이 friend 함수였을 뿐이다.
	*/

	/*
	* 클래스 내부에 정의된 함수는 암시적으로 인라인 선언된다. 이는 지금의 operator* 또한 예외가 아니다.
	* 클래스 외부에서 정의된 도우미 함수만 호출하는 방식으로 operator*를 구현하면 이러한 암시적 인라인의 영향을 최소화할 수 있다.
	*/

	/*
	* Rational이 템플릿이라는 사실에 대해 조금만 더 생각하면, 도우미 함수의 경우도 대개 템플릿일 것이라는 사실이 나온다.
	* 그러므로 Rational을 정의하는 헤더에 존재하는 코드는 다음과 같은 형태임을 유추할 수 있다.
	*/
	template<typename T> class Rational;
	template<typename T>
	const Rational<T> doMultiply(const Rational& lhs, const Rational& rhs);

	template<typename T>
	class Rational {
	public:
		...;
		friend const Rational operator*(const Rational& lhs, const Rational& rhs) { return doMultiply(lhs, rhs); }
	};

	/*
	* 대다수의 컴파일러에서 템플릿 정의를 헤더에 전부 넣을 것을 사실상 강요하므로, deMultiply()도 헤더에 정의해야 할 것이다.
	*/
	template<typename T>
	const Rational<T> doMultiply(const Rational& lhs, const Rational& rhs) {
		return Rational<T>(lhs.numerator() * rhs.numerator(), lhs.denominator() * rhs.denominator());
	}
	/*
	* 물론 doMultiply()는 템플릿으로서 혼합형 곱셈을 지원하지 못하나, 지원할 필요가 없기도 하다.
	* 해당 템플릿을 사용하는 것은 operator*밖에 없을 것인데, operator*가 이미 혼합형 연산을 지원하고 있기 때문이다. 
	*/
}
/*
* 결론.
* 1. 모든 매개변수에 대해 암시적 타입 변환을 지원하는 템플릿과 관계가 있는 함수를 제공하는 클래스 템플릿을 정의하겠다면, 클래스 템플릿 내부에 프렌드 함수로 정의하라.
*/

#endif