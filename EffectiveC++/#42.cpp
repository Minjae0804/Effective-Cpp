#if 0

// #42. typename의 두 가지 의미를 제대로 파악하라


//
{
	template <class T> class Widget;
	template <typename T> class Widget;
	/*
	* 위 두 템플릿 선언은 동등하다.
	* 그러나 언제까지나 class와 typename이 동등하지는 않다.
	*/

	template<typename C>
	void print2nd(const C& container) {
		if (container.size() >= 2) {
			C::const_iterator iter(container.begin());

			++iter;
			int value = *iter; 
			std::cout << value;
		}
	}
	/*
	* 이 함수는 컴파일조차 되지 않는다.
	* 이 함수에서 중요한 것은 iter와 value인데, 이 둘에 대해서 자세히 살펴보자.
	* 
	* iter의 타입은 C::const_iterator인데, 이는 템플릿 매개변수인 C에 의해 달라진다.
	* 템플릿 내 이름 중 이렇게 템플릿 매개변수에 종속된 것을 의존 이름(dependent name)이라고 한다.
	* 의존 이름이 어떠한 클래스 내부에 중첩되어 있는 경우, 이를 중첩 의존 이름(nested dependent name)이라고 한다.
	* C::const_iterator는 중첩 의존 타입 이름(nested dependent type name)이다. 타입을 참조하는 중첩 이름이기 때문이다.
	* 
	* print2nd 함수에서 사용되는 또 하나의 지역변수인 value는 int이다.
	* int는 템플릿 매개변수가 어떻게 되든 상관 없는 타입이므로, 비의존 이름이라고 부른다.
	* 
	* 코드 내부에 중첩 의존 이름이 존재할 경우 컴파일러가 구문분석 시 애로사항이 발생할 수 있다.
	*/
	template<typename C>
	void print2nd(const C& container) {
		C::const_iterator* x;
		...;
	}
	/*
	* 언뜻 보면 C::const_iterator에 대한 포인터인 지역 변수로서 x를 선언하고 있는 것처럼 보이나, 그건 C::const_iterator이 타입이라는 것을 알고 있을 경우에만 그렇다.
	* 만약 C::const_iterator가 타입이 아니라면?
	* 우연히 const_iterator가 아닌 정적 데이터 멤버가 C에 들어 있을 경우에는?
	* 
	* 이 경우 위 코드는 지역 변수를 선언한 것이 아니라 C::const_iterator와 x를 피연산자로 받는 곱셈 연산이 된다.
	* C의 정체가 무엇인지 컴파일러에게 알려주지 않으면 C++은 모호성을 해결하기 위해 어떤 규칙을 하나 사용한다.
	* 이 규칙에 따르면 파서는 템플릿 내부에서 중첩 의존 이름을 만나면 사용자가 타입이라고 알려주지 않는 한 그것이 타입이 아니라고 가정하게 되어 있다.
	* 즉, 중첩 의존 이름은 기본적으로 타입이 아닌 것으로 해석된다.
	* 
	* 이를 해결하는 방법은, 당연하게도 타입이라고 명시해주면 된다.
	*/
	template<typename C>
	void print2nd(const C& container) {
		if (container.size() >= 2) {
			// C가 타입임을 명시
			typename C::const_iterator iter(container.begin());
			...;
		}
	}
	/*
	* typename 키워드는 중첩 의존 이름만을 식별하는 데에 사용하여야 한다. 그 외의 이름은 typename을 가져서는 안 된다.
	* 예로 어떤 컨테이너와 그 컨테이너 내의 반복자를 한꺼번에 받아들이는 함수 템플릿을 다음과 같이 만들었을 때 그러지 말라는 것이다.
	*/
	template<typename C>
	// typename을 사용하면 안 됨
	void print2nd(const C& container) {
		if (container.size() >= 2) {
			// typename을 사용해야 함
			typename C::const_iterator iter(container.begin());
			...;
		}
	}

	/*
	* typename은 중첩 의존 타입 이름 앞에 붙여 주어야 한다는 규칙에는 예외가 하나 있다.
	* 이 예외는 중첩 의존 타입 이름이 기본 클래스의 리스트에 있거나 멤버 초기화 리스트 내의 기본 클래스 식별자로서 존재하는 경우이다.
	*/
	template<typename T>
	// 상속되는 기본 클래스 리스트. typename을 사용하면 안 된다.
	class Derived : public Base<T>::Nested {
	public:
		// 멤버 초기화 리스트에 있는 기본 클래스 식별자. typename을 사용하면 안 된다.
		explicit Derived(int x) : Base<T>::Nested(x) {
			// 중첩 의존 타입 이름이며 기본 클래스 리스트에도 없고, 멤버 초기화 리스트의 기본 클래스 식별자도 아님. typename을 사용하면 된다.
			typename Base<T>::Nested(x);
			...;
		}
	};
	/*
	* 위의 경우에는 typename을 사용하면 안 된다.
	*/

	template<typename iterT>
	void workWithIterator(iterT iter) {
		typename std::iterator_traits<iterT>::value_type temp(*iter);
		...;
	}
	/*
	* std::iterator_traits<iterT>::value_type은 C++ 표준의 특성정보이다. 즉 iterT 타입 객체로 가리키는 대상의 타입이다.
	* 이 문장은 iterT 객체가 가리키는 것과 같은 타입의 지역변수를 선언한 후, iter가 가리키는 객체로 해당 temp를 초기화하는 문장이다.
	* 만약 iterT가 std::iterator<int>일 경우 temp는 int가 된다.
	* 
	* 아무튼, 여기에서 std::iterator_traits<iterT>::value_type은 중첩 의존 타입 이름이므로 이 이름 앞에는 typename을 사용해야 한다.
	*/

	/*
	* 참고로 특성정보 클래스에 속하는 value_type 등의 멤버 식별자는 해당 멤버의 식별자와 같게 하는 것이 관례이다.
	* 따라서, std::iterator_traits<iterT>::value_type을 typedef로 별칭을 선언하는 경우 typedef로 정의하는 지역 이름을 대개 다음과 같이 짓는다.
	*/
	template<typename iterT>
	void workWithIterator(iterT iter) {
		typedef std::iterator_traits<iterT>::value_type value_type;
		value_type temp(*iter);
		...;
	}
}
/*
* 결론.
* 1. 템플릿 매개변수를 선언할 때 class 및 typename은 서로 바꾸어 써도 무방하다.
* 2. 중첩 의존 타입 이름을 식별하는 용도에는 반드시 typename을 사용한다. 
*    단, 중첩 의존 이름이 기본 클래스에 존재하거나 멤버 초기화 리스트 내 기본 클래스 식별자로 있는 경우에는 예외이다.
*/

#endif