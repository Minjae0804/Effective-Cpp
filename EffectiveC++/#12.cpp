#if 0

// #12. 객체의 모든 부분을 빠짐없이 복사하라


//
{
	/*
	* 복사 생성자와 복사 대입 연산자는 객체 복사 함수(copying function)이라고도 불린다.
	* 객체 복사 함수는 사용자가 정의하지 않으면 컴파일러가 직접 생성하는데, 따라서 이를 직접 선언한다는 것은 기본 생성되는 객체 복사 함수의 동작이 의도한 것과 다름을 의미하게 된다.
	* 다만 객체 복사 함수를 직접 구현할 때 해당 함수가 잘못 구현되었을 경우 컴파일러는 이를 보통 에러로 보지 않는다.
	*/
	// 로그를 생성하는 함수
	void logCall(const std::string& funcName);
	
	// 날짜 정보를 위한 클래스
	class Date { ... };

	class Customer {
		std::string name;
		// 신규 멤버 변수 추가. 현재 클래스의 객체 복사 함수에는 반영되지 않음
		Date lastTransaction;
	public:
		...
		Customer(const Customer& rhs);
		Customer& operator=(const Customer& rhs);
		...
	};

	// rhs의 데이터를 복사
	Customer::Customer(const Customer& rhs) : name(rhs.name) {
		logCall("Customer copy constructor");
	}

	Customer& Customer::operator=(const Customer& rhs) {
		logCall("Customer copy assignment operator");
		// rhs의 데이터를 복사
		name = rhs.name;
		return *this;
	}
	/*
	* 이 경우 복사 함수의 동작은 완전 복사가 아닌 부분 복사(partial copy)가 된다.
	* 멤버 변수 중 name은 복사되나, lastTransaction은 복사하지 않는다.
	* 
	* 문제는 이것을 알려주는 컴파일러가 거의 없다. 
	* 때문에 멤버변수를 하나 넣었으면 각 객체 복사 함수를 일일이 최신화해줘야 한다. 생성자도 최신화해주고... 온갖 것을 전부 최신화해 주어야 한다.
	* 
	* 이 문제가 특히 악랄해지는 부분이 있는데, 바로 상속이다.
	*/
	// 파생 클래스
	class PriorityCustomer : public Customer {
		int priority;
	public:
		...
		PriorityCustomer(const PriorityCustomer& rhs);
		PriorityCustomer& operator=(const PriorityCustomer& rhs);
		...
	};

	PriorityCustomer::PriorityCustomer(const PriorityCustomer& rhs) : priority(rhs.priority) {
		logCall("PriorityCustomer copy constructor");
	}

	PriorityCustomer& PriorityCustomer:: operator=(const PriorityCustomer& rhs) {
		logCall("PriorityCustomer copy assignment operator");
		priority = rhs.priority;

		return *this;
	}
	/*
	* PriorityCustomer 클래스의 복사 함수는 PriorityCustomer에서 선언된 모든 데이터 멤버를 복사하고 있기는 하나, Customer 상속을 통해 받은 데이터 멤버는 복사하고 있지 않다.
	* PriorityCustomer의 복사 생성자에는 기본 클래스에 넘길 인자들도 명시되어 있지 않으므로 PriorityCustomer 객체의 Customer 부분은 기본 Customer 생성자에 의해 초기화된다.(있을 경우. 없으면 컴파일 에러가 발생한다.)
	* PriorityCustomer의 복사 대입 연산자는 기본 클래스의 데이터 멤버를 전혀 건드리지 않으므로 기본 클래스의 데이터 멤버 부분은 변경되지 않게 된다.
	* 
	* 아무튼, 파생 클래스에 대한 복사 함수를 직접 정의할 것이라면 기본 클래스 부분을 절대 빠트리지 않아야 한다. 
	* 다만 기본 클래스 부분의 데이터 멤버는 private일 가능성이 매우 높으므로 이것들을 직접 건드리기는 힘드므로, 기본 클래스의 복사 함수를 호출하면 된다.
	*/
	// 기본 클래스의 복사 생성자 호출
	PriorityCustomer::PriorityCustomer(const PriorityCustomer& rhs) : Customer(rhs), priority(rhs.priority) {
		logCall("PriorityCustomer copy constructor");
	}

	PriorityCustomer& PriorityCustomer:: operator=(const PriorityCustomer& rhs) {
		logCall("PriorityCustomer copy assignment operator");
		// 기본 클래스 부분 대입
		Customer::operator=(rhs);
		priority = rhs.priority;

		return *this;
	}
	/*
	* 즉 객체의 복사 함수를 작성할 때에는 두 가지를 항상 확인하라.
	* 1. 해당 클래스의 데이터가 모두 복사하는지
	* 2. 이 클래스가 상속한 기본 클래스의 복사 함수를 호출하는지
	* 
	* 복사 생성자와 복사 대입 연산자는 비슷하게 생기긴 했지만, 한 쪽을 사용하여 다른 쪽을 구현하려는 시도는 절대 하면 안 된다.
	* 애초에 말이 되지 않는 발상일 뿐더러, 특정 조건에서 객체의 데이터가 훼손될 수 있어 매우 위험하다.
	* 다만 겹치는 부분을 멤버 함수로 분리하고 각각에서 해당 함수를 호출하는 것은 가능하고 안전하며 검증된 방법이기도 하므로, 사용할 만 하다.
	*/
}
/*
* 결론.
* 1. 객체 복사 함수는 주어진 객체의 모든 데이터 멤버 및 모든 기본 클래스 부분을 복사하여야 한다.
* 2. 클래스의 복사 함수 두 개를 구현할 때 한 쪽을 이용하여 다른 쪽을 구현하려는 시도는 하지 마라. 
     겹치는 부분을 별도 멤버 함수로 분리하고 호출하게 하는 방법을 사용하라.
*/

#endif