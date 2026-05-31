#if 0

// #6. 컴파일러가 만들어낸 함수가 필요 없으면 확실하게 해당 함수의 사용을 금하라


//
{
	/*
	* 컴파일러가 생성하는 모든 함수는 public이다.
	* 생성을 막고 싶은 기본 생성될 수 있는 함수를 private로 선제 선언해두면 기본 생성 함수의 생성을 막음과 동시에 외부 호출도 차단할 수 있다.
	* 더불어 해당 함수들을 선언해두고 정의하지 않으면 내부에서의 의도치 않은 호출도 방지할 수 있다.
	*/
	class HomeForSale {
		...
		HomeForSale(const HomeForSale&);
		HomeForSale& operator=(const HomeForSale&);
	public:
		...
	};

	/*
	* 더불어, 복사 생성자와 복사 대입 연산자를 private로 선언하고 별도의 부모 클래스에 박아둔 뒤 목적 클래스를 해당 클래스의 자식으로 두는 방법을 사용할 수 있다. 
	* 이는 5항목에서 설명했듯 복사 대입 연산자가 private로 선언된 클래스의 자식 클래스는 암시적 복사 대입 연산자를 가질 수 없기 때문에 가능하다.
	* 이 때 부모 클래스는 복사 방지의 책임만을 가지게 된다.
	*/
	class Uncopyable {
		Uncopyable(const Uncopyable&);
		Uncopyable& operator=(const Uncopyable&);
	protected:
		Uncopyable() {}
		~Uncopyable() {}
	};

	class HomeForSale : private Uncopyable {
		...
	};
}

/*
* 결론.
* 1. 컴파일러에서 자동으로 제공하는 함수 생성을 막으려면 대응되는 멤버함수를 private로 선언 후 구현하지 말라. 
*    또는 별도의 클래스를 잡고 해당 클래스에서 자동 생성을 막고 싶은 멤버함수를 private로 선언한 뒤 자식으로 대상 클래스를 두는 것도 한 방법이다.
*/

#endif