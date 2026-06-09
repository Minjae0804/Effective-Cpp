#if 0

// #5. C++이 은근슬쩍 만들어 호출해버리는 함수들에 촉각을 세우자


//
{
	/*
	* C++ 클래스에서 생성자, 복사 생성자, 복사 대입 연산자, 소멸자는 직접 선언하지 않을 경우 자동 생성된다.
	* 이 때 생성되는 함수는 모두 기본 형태를 지니며, 접근지정자가 public이고 inline 함수이다.
	*/

	class Empty {
	public:
		// 기본 생성자
		Empty() {...}
		// 복사 생성자
		Empty(const Empty& rhs) {...}
		// 소멸자
		~Empty() {...}
		// 복사 대입 연산자
		Empty& operator=(const Empty& rhs) {...}
	};
	/*
	* 해당 함수들은 다음과 같은 코드가 존재할 때만 자동 생성된다.
	*/

	// 생성자 및 소멸자
	Empty e1;

	// 복사 생성자
	Empty e2(e1);

	// 복사 대입 연산자
	e2 = e1;

	/*
	* 컴파일러가 생성한 복사 생성자 및 복사 대입 연산자는 원본 객체의 비정적 데이터를 사본 객체로 복사한다.
	*/

	template<typename T>
	class NamedObject {
		std::string nameValue;
		T objectValue;
	public:
		NamedObject(const char* name, const T& value);
		NamedObject(const std::string& name, const T& value);
		...
	};
	/*
	* 위와 같이 생성자가 이미 선언되어 있을 경우 컴파일러는 기본 생성자를 생성하지 않는다. 
	* 즉, 생성자 인자가 반드시 필요한 클래스를 만들었을 경우 컴파일러는 인자를 받지 않는 생성자를 굳이 만들지 않는다.
	* 
	* 단 복사 생성자나 복사대입연산자의 경우 NamedObject에 선언되어 있지 않으므로 필요 시 컴파일러가 직접 생성한다. 
	* 때문에 복사 생성자의 용례는 다음과 같이 발생한다.
	*/

	NamedObject<int> no1("Smallest Prime Number", 2);
	// 복사 생성자 호출
	NamedObject<int> no2(no1);
	

	/*
	* 컴파일러에 의해 생성된 복사 생성자는 no1.nameValue와 no1.ObjectValue의 값으로 no2.nameValue와 no2.objectValue를 초기화해야 한다.
	* 이 때 nameValue는 string인데, 표준 string 타입은 자체적인 복사 생성자를 지니므로 no1.nameValue의 초기화는 string의 복사 생성자에 no1.nameValue를 인자로 넘겨 호출함으로서 이루어진다.
	* 한편, objectValue는 원시 타입 int이므로 복사는 비트 복사로 끝난다.
	* 
	* 다만 복사대입연산자의 동작이 적절하기 위해서는 최종결과코드가 적법해야 하며 이치에 맞아야 한다.
	* 둘 중 아무것도 만족하지 못하면 컴파일러는 복사 대입 연산자의 자동생성을 거부한다.
	*/

	template<class T>
	class NamedObject {
		std::string& nameValue;
		const T objectValue;
	public:
		NamedObject(std::string& name, const t& value);
		...
	};

	std::string newDog("Persephone");
	std::string oldDog("Satch");

	NamedObject<int> p(newDog, 2);
	NamedObject<int> s(oldDog, 36);

	p = s;
	/*
	* 이 경우 대입연산 전 p.nameValue와 s.nameValue는 string 객체를 참조하고 있는데, 이 때 대입 연산을 하려고 하면 컴파일 에러가 발생한다.
	* p.nameValue는 참조인데, 참조는 초기화 후 수정이 불가능하기 때문에, 복사 대입 연산자의 작동을 정의할 수 없기 때문이다.
	* 때문에 참조나 상수를 멤버로 지니는 클래스의 경우 복사 대입 연산자를 직접 정의해야 한다. 
	* 
	* 추가로 복사 대입 연산자가 private로 선언된 클래스의 자식 클래스는 암시적 복사 대입 연산자를 가질 수 없다. 이 또한 컴파일 에러가 발생한다.
	*/
}

/*
* 결론.
* 1. 컴파일러는 경우에 따라 생성자, 복사 생성자, 소멸자, 복사 대입 연산자를 암시적으로 생성할 수 있다.
*/

#endif