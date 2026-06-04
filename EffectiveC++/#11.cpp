#if 0

// #11. operator=에서는 자기대입에 대한 처리가 빠지지 않도록 하라


//
{
	/*
	* 자기대입이란 객체가 자기 자신에 대해 대입연산자를 적용하는 것을 의미한다.
	*/
	class Widget {...};

	Widget w;
	...
	// 자기대입
	w = w;

	// i == j일 경우 자기대입
	a[i] = a[j];

	// 가리키는 대상이 같은 경우 자기대입
	*px = *py;

	/*
	* 이러한 자기대입이 발생하는 이유는 여러 곳에서 하나의 객체를 참조하는 상태, 즉 중복참조 때문이다.
	* 동일 타입으로 생성된 객체 여러 개를 참조 또는 포인터로 잡고 동작하는 코드를 작성할 경우 동일 객체가 사용될 가능성을 고려하여야 한다.
	*/

	class Base {...};
	class Derived : public Base {...};

	// 이 때 rb와 pd는 동일 객체일 가능성이 존재한다.
	void doSomething(const Base& rb, Derived* pd);

	/*
	* #13 및 #14에서는 자원 관리 용도로 객체 생성을 권장한다는 내용을 다루는데, 이 때 조심하여야 하는 것이 바로 대입 연산자이다.
	*/

	class Bitmap {...};
	class Widget {
		// 힙에 할당한 객체를 관리하는 포인터
		Bitmap* pb;
	};

	// 안전하지 않게 구현된 operator=
	Widget& Widget::operator=(const Widget& rhs) {
		// 현재의 비트맵 사용을 중지하고 rhs의 비트맵 사용을 강제
		delete pb;
		pb = new Bitmap(*rhs.pb);

		return *this;
	}

	/*
	* 여기에서 찾을 수 있는 자기 참조 문제는 operator= 내부에서 *this와 rhs가 동일 객체일 가능성이 존재한다는 것이다. 
	* *this와 rhs가 동일 객체일 경우 delete가 *this뿐 아니라 rhs의 비트맵에까지 적용된다.
	* 따라서, operator=가 종료되는 시점에는 Widget 객체는 자신의 포인터 멤버를 통해 물고 있던 객체가 삭제된 상태가 된다.
	* 
	* 전통적으로는 operator=의 실행 시작 부분에서 일치성 검사를 진행하여 자기대입을 점검하여 에러를 방지하는 방법이 있었다.
	*/
	Widget& Widget::operator=(const Widget& rhs) {
		// 자기대입일 경우 바로 탈출
		if (this == &rhs) return *this;

		delete pb;
		pb = new Bitmap(*rhs.pb);

		return *this;
	}
	/*
	* 그러나 이전 버전의 operator=은 자기대입에 안전하지 못할 뿐더러 예외에도 안전하지 않다.
	* 예외 안정성에 대해서는 이 코드 또한 문제가 존재하는데, 특히 new Bitmap(...)의 경우가 특히 그렇다.
	* 여기에서 예외가 발생할 경우 Widget 객체의 멤버 pb는 댕글링 포인터가 되어 온갖 에러와 예외의 주범이 된다.
	* 
	* operator=를 예외 안전하게 구현하면 대개 자기대입에도 안전한 코드가 된다. 즉 예외 안정성에만 집중하면 자기대입 문제는 무시해도 보통 괜찮다.
	* 사실 대부분의 경우 코드 순서만 잘 바꿔도 예외 안전한 코드가 되는데, 이 경우도 마찬가지이다.
	*/
	Widget& Widget::operator=(const Widget& rhs) {
		// 원래의 pb를 다른 곳에 기억
		Bitmap* pOrig = pb;
		// pb가 *pb의 사본을 가리키게 함
		pb = new Bitmap(*rhs.pb);
		// 원래의 pb를 삭제
		delete pOrig;

		return *this;
	}
	/*
	* 이 코드의 경우 new Bitmap 부분에서 예외가 발생하더라도 pb는 변경되지 않은 상태가 유지되므로 예외 안전하다.
	* 
	* 이외에도 예외 안정성과 자기대입 안정성을 동시에 지니는 operator=를 구현하는 방법으로 copy and swap이라는 방법이 있다.
	*/
	class Widget {
		...
		// *this의 데이터와 rhs의 데이터를 스왑
		void swap(Widget& rhs);
		...
	};

	Widget& Widget::operator=(const Widget& rhs) {
		// rhs의 데이터에 대해 사본을 생성
		Widget temp(rhs);
		// *this의 데이터를 그 사본의 것과 스왑
		swap(temp);

		return *this;
	}
	/*
	* 이 코드는 C++가 지니는 두 가지 특성을 이용하여 조금 다르게 구현할 수도 있다.
	* 첫 번째는 클래스의 복사 대입 연산자는 인자를 값으로서 취하도록 선언이 가능하다는 점이며,
	* 두 번째는 값에 의한 전달을 수행하면 전달 대상의 사본을 생성하여 활용한다는 점이다.
	*/
	// 함수 실행 시 rhs의 사본이 생성되어 내부로 전달된다.
	Widget& Widget::operator=(Widget rhs) {
		// *this의 데이터를 사본의 데이터와 스왑
		swap(temp);

		return *this;
	}
}
/*
* 결론.
* 1. operator=를 구현할 때 어떤 객체가 그 자신에 대입되는 경우를 제대로 처리하도록 구현하라.
* 2. 두 개 이상의 객체에 대해 동작하는 함수가 존재할 경우 해당 함수에 넘겨지는 객체가 실제로 동일 객체일 경우에 대해 정확히 동작하는지 확인하라.
*/

#endif