#if 0

// #20. 값에 의한 전달보다는 상수객체 참조에 의한 전달을 택하는 것이 대개 낫다


// 
{
	/*
	* C++의 함수는 기본적으로 값에 의한 호출(call by Value)을 지원한다.
	* 값에 의한 호출은 인자로 주어진 객체의 사본을 생성하여 함수 내부에서 사용하는 방식이다.
	* 특별히 다른 방식을 지정하지 않는 한 함수 매개변수는 실제 인자의 사본을 통해 초기화되며, 어떤 함수를 초기화한 쪽은 그 함수가 반환한 값의 사본을 돌려받는다.
	* 
	* 이 때 이 사본을 생성하는 것은 복사 생성자이다.
	* 때문에 값에 의한 호출은 고비용의 연산이 되기도 한다.
	*/
	class Person {
		std::string name;
		std::string address;
	public:
		Person();
		virtual ~Person();
	};

	class Student : public Person {
		std::string schoolName;
		std::string schoolAddress;
	public:
		Student();
		~Student();
	};

	// Student를 값으로 전달받는 함수
	bool validateStudent(Student s);

	Student plato;
	bool platoIsOk = validateStudent(plato);

	/*
	* 이 함수가 호출될 경우 plato로부터 s를 초기화하기 위해 Student의 복사 생성자가 호출된다.
	* s는 validateStudent()에서 복귀할 때 소멸된다.
	* 따라서 해당 함수의 매개변수 전달 비용은 Student의 복사 생성자 호출 1회, 소멸자 호출 1회이다.
	* 
	* 다만 Student 객체에는 String 객체 두 개가 멤버로 들어 있고, Person을 상속하였으므로 Person의 생성자도 호출되어야 하고, Person의 멤버도 존재한다.
	* 결론적으로 함수 호출 한 번에 Student 생성자 1회, Person 생성자 1회, 각 String 멤버 생성자가 4회 호출되어 최종적으로 생성자가 6회 호출된다.
	* 물론 함수에서 복귀할 때 소멸자도 동일한 횟수로 호출된다. 즉 함수 호출 한 번에 생성자 호출 6회 + 소멸자 호출 6회가 발생한다.
	*/

	/*
	* 다행히 함수를 호출할 때마다 위와 같은 비용이 발생하지 않게 할 수 있는 방법이 있다.
	* 바로 상수객체에 대한 참조로 전달되게 하는 것이다.
	*/
	bool validateStudent(const Student& s);
	/*
	* 이렇게 하면 신규 생성되는 객체도 없고, 당연히 생성자 및 소멸자 호출도 없으므로 자원이 절약된다.
	* 여기에서 중요한 것은 매개변수에 박힌 const이다.
	* 
	* 원본 함수의 경우 값에 의한 전달이므로 함수 내부에서 사본을 사용한다. 하지만 참조에 의한 호출은 객체의 포인터를 반환한다.
	* 이 둘의 가장 큰 차이점은 함수 내부에서 원본을 수정할 수 있느냐이다.
	* 값에 의한 호출은 내부에서 사본을 사용하므로 당연히 원본 수정이 불가능하다. 하지만, 참조에 의한 호출의 경우 객체의 포인터를 쓰기 때문에 원본이 수정될 수 있다.
	* 때문에 위의 경우 매개변수에 const를 붙여 함수 내부에서 파라미터로 전달받는 객체의 수정을 방지한다. 
	*/

	/*
	* 참조에 의한 호출을 통해 파라미터가 전달되는 경우 복사손실 문제(slicing problem)이 없어지는 장점도 있다.
	* 파생 클래스 객체가 기본 클래스 객체로서 전달될 때, 이것이 값에 의한 호출을 통해 전달될 경우 기본 클래스의 복사 생성자가 호출된다. 
	* 파생 클래스 객체의 생성자가 호출되지 않으므로 함수 내부에서 사용되는 사본은 파생 클래스로서의 역할을 하지 못한다. 
	*/
	class Window {
		...
	public:
		std::string name() const;
		virtual void display const;
	};

	class WindowWithScrollBars : public Window {
	public:
		...
		virtual coid display() const;
	};

	// 값에 의한 전달
	void printNameAndDisplay(Window w) {
		std::cout << w.name();
		w.display();
	}

	WindowWithScrollBars wwsb;
	printNameAndDisplay(wwsb);
	/*
	* printNameAndDisplay()를 호출하면 매개변수 w가 호출되기는 하나 Window 객체로 생성된다.
	* 즉, wwsb가 WindowWithScrollBars로 동작할 수 있는 모든 부분이 복사로 생성되지 않는다.
	* 
	* 복사손실 문제에서 벗어나기 위해서는, w를 상수객체에 대한 참조로 호출하면 된다.
	*/
	void printNameAndDisplay(const Window& w) {
		std::cout << w.name;
		w.display();
	}

	/*
	* 참조는 보통 포인터를 사용하여 구현된다. 즉 참조 전달은 즉 포인터의 전달과 같다.
	* 이렇게 따졌을 때, 전달하는 객체의 타입이 기본 제공 타입인 경우 참조보다는 값으로 넘기는 경우가 보통 더 효율적이다.
	* 이는 STL의 이터레이터와 펑터에도 마찬가지이다. 
	* 참고로 반복자와 펑터 구현 시에는 반드시 복사 효율을 높이고, 복사손실 문제에 노출되지 않게 하는 것이 필수이다.
	* 
	* 다만 기본제공 타입의 크기가 작음에 착안하여 객체라도 크기가 작을 경우 값에 의한 전달은 괜찮다고 생각할 수 있다.
	* 하지만 다음과 같은 문제가 있으므로 추천되지 않는다.
	* 1. 크기가 작다고 복사 생성자 호출이 저비용은 아니다.
	* 2. 수행 성능 문제 발생 여지가 있다.
	* 3. 사용자 정의 타입의 크기는 언제든 변화에 노출되어 있다.
	*/
}
/*
* 결론.
* 1. 값에 의한 호출보다는 상수객체 참조에 의한 호출을 선호하라. 대부분의 경우 효율적이며, 복사손실 문제까지 방지한다.
* 2. 다만 기본제공 타입, STL 이터레이터, 펑터에 대해서는 적절하지 않다. 이들의 경우 값에 의한 호출이 더 적절하다.
*/

#endif