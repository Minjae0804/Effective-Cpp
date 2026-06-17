#if 0

// #31. 파일 사이의 컴파일 의존성을 최대로 줄여라


// 
{
	/*
	* C++의 클래스 정의는 클래스 인터페이스만 지정하는 것이 아니라, 구현 세부사항까지 지정한다.
	*/
	class Person {
		// 구현 세부사항들
		std::string theName;
		Date theBirthDate;
		Address theAddress;
	public:
		Person(const std::string& name, const Date& birthday, const Address& addr);
		std::string name() const;
		std::string birthDate() const;
		std::string address() const;
		...;
	};
	/*
	* 위 코드들만 가지고 Person 클래스는 컴파일될 수 있을 리가 없다.
	* 구현 세부사항에 속하는 데이터 멤버들, 즉 string, Date, Address가 어떻게 정의되었는지가 명시되어야 한다.
	* 따라서 이들이 정의된 정보를 가져와야 하고, 이 때 쓰는 것이 #include 지시자이다.
	*/
#include <string>
#include "date.h"
#include "address.h"
	/*
	* #include는 Person을 정의한 파일과 헤더 파일들을 컴파일 의존성으로 엮는다.
	* 이러면 헤더 파일 중 하나라도 바뀌거나 이들과 엮인 헤더가 수정되기라도 하는 순간 Person 클래스를 정의한 모든 파일들이 리컴파일되어야 한다.
	* 
	* 그럼 어째서 C++ 표준은 구현 세부사항이 클래스 정의문 내부로 들어가는 것을 용인하였을까?
	* 그러니까, 구현 세부사항을 구현 정의와 분리하면 안 되겠느냐는 것이다.
	*/
	class Date;
	class Address;

	class Person {...};
	/*
	* 물론 위 코드는 컴파일되지 않는다. 
	* 
	* 전방 선언의 문제는 컴파일러가 컴파일 도중 객체들의 크기를 전부 알아야 함에 있다.
	*/
	int main() {
		int x;
		Person p(params);
	}
	/*
	* 컴파일러는 x의 정의문을 만나면 일단 int 하나를 담을 만한 충분한 공간을 할당해야 함을 이해하고 있다.
	* 여기까지는 아무 문제가 없지만, p는 문제가 된다. Person 하나를 담을 크기의 공간을 할당해야 한다는 것은 컴파일러도 잘 알고 있는데, 문제는 그 크기를 모르기 때문이다.
	* 때문에 해당 코드를 컴파일하는 시점에서 구현 세부사항이 반드시 존재해야 한다.
	* 
	* 자바 등에서는 객체 정의 시 컴파일러가 해당 객체의 포인터를 담을 공간만 할당한다.
	* 그러니까, 대충 다음과 같이 구현된다.
	*/
	int main() {
		int x;
		Person* p;
	}
	/*
	* 그리고 실제로 이러한 코드도 적법한 C++ 코드이다.
	* 
	* 참고로 이렇게 하는 좋은 방법이 있다.
	* 주어진 클래스를 두 클래스로 쪼개고, 한 쪽은 인터페이스만, 다른 한 쪽은 인터페이스의 구현을 맡도록 하는 것이다.
	*/
#include <string>
#include <memory>

	class PersonImpl;
	class Date;
	class Address;

	class Person {
		// 구현 클래스 객체에 대한 포인터
		std::tr1::shared_ptr<PersonImpl> pImpl;
	public:
		Person(const std::string& name, const Date& birthday, const Address& addr);
		std::string name() const;
		std::string birthDate() const;
		std::string address() const;
	};
	/*
	* 이 클래스를 보면 주 클래스에 존재하는 데이터 멤버는 구현 클래스에 대한 포인터가 전부이다.
	* 이러한 패턴을 pimpl 관용구(pointer to implementation)라고 하며, 이 패턴의 포인터 변수명을 보통 pImpl로 정한다.
	* 
	* 이 경우 주 클래스가 세부사항과 갈라지게 된다. 
	* 구현 클래스 부분은 마음대로 고칠 수 있고, person 쪽에서는 리컴파일 필요성이 줄어든다.
	* 게다가 Person의 구현을 알 수가 없으므로 구현 세부사항을 물어대는 코드가 발생할 여지가 사라진다.
	* 
	* 이렇게 인터페이스와 구현을 둘로 나누는 것의 핵심은 정의부에 대한 의존성(dependency of definitions)을 선언부에 대한 의존성(dependency of declarations)으로 변환하는 것에 있다.
	* 즉, 헤더 파일을 정의할 때에는 실용적으로 의미를 가지는 한 자체적으로 완성되도록 하며, 그러기 힘들다면 다른 파일에 대해 의존성을 지니게 하도록 하되 정의부가 아닌 선언부에만 의존성을 지니게 하면 된다.
	* 이외의 나머지 전략은 이것을 축으로 한다.
	* 
	* 1. 객체 참조자 및 포인터로 충분한 경우 객체를 직접 사용하지 마라.
	*    단 타입의 객체를 정의할 때에는 해당 타입의 정의가 준비되어 있어야 한다.
	* 
	* 2. 가능하다면 클래스 정의 대신 클래스 선언에 최대한 의존케 하라.
	*    어떤 클래스를 사용하는 함수를 선언할 때에는 해당 클래스의 정의가 필요 없다.
	*    심지어 해당 클래스 객체를 값으로 전달하거나 반환할 경우에도 클래스 정의가 필요하지 않다.
	*/
	class Date;

	Date today();
	void clearAppointments(Date d);
	/*
	*    이건 왜 가능할까?
	*    누군가 이 함수를 호출할 경우 호출 전에 Date의 정의가 파악되어야 하는데, 이 함수가 호출될 것인지에 대한 여부가 확실치 않기 때문이다.
	*    때문에 컴파일러는 그냥 함수 선언이 되어있는 헤더가 아니라, 함수 호출이 발생하는 사용자의 소스 파일에 해당 클래스 정의를 전가한다.
	*    이러면 실제로 쓰지 않을 타입 정의에 대해 사용자가 의존성을 끌어오는 쓸모없는 짓을 막을 수 있다.
	* 
	* 3. 선언부와 정의부에 대해 별도 헤더 파일을 제공한다.
	*    클래스를 분리하는 아이디어가 실용성을 지니기 위해서는 그것들이 파일 단위로 나뉘어 있어야 한다.
	*    물론, 이 클래스들은 항상 짝으로 사용되어야 한다.
	* 
	* 참고로 pimpl을 사용하는 Person 같은 클래스를 핸들 클래스라고 한다.
	* 핸들 클래스에서 어떠한 함수를 호출하게 되어 있을 경우, 해당 핸들 클래스에 대응되는 구현 클래스 쪽으로 그 함수 호출을 전달하여 구현 클래스가 실제 작업을 수행하게 하라.
	*/
#include "Person.h"
#include "PersonImpl.h"

	Person::Person(const std::string& name, const Date& birthday, const Address& addr) 
		: pImpl(new PesronImpl(name, birthday, addr)) {}
	std::string Person::name() const { return pImpl->name(); }
	/*
	* 이 경우 핸들 클래스는 클래스 자체의 역할을 다한다. 단지 구현을 분리하기 전과 동작이 다를 뿐이다.
	* 
	* 핸들 클래스 외에 다른 방법도 있다. Person을 특수 형태의 추상 기본 클래스로 정의하는 것이다. 이 때 이 추상 기본 클래스를 인터페이스 클래스라고 한다.
	* 어떠한 기능을 나타내는 인터페이스를 추상 기본 클래스로 마련해 놓고, 이 클래스로부터 파생 클래스를 만들 수 있게 하자는 아이디어이다.
	* 인터페이스 클래스는 파생이 목적이므로 데이터 멤버가 존재하지 않고 생성자도 없으며, 단일한 가상 소멸자와 인터페이스를 구성하는 순수가상함수만 존재한다.
	* 
	* 인터페이스 클래스는 자바나 닷넷의 인터페이스와 흡사하나, C++은 자바나 닷넷과 달리 인터페이스 클래스에 제약을 걸지 않는다.
	* 일례로 자바 및 닷넷의 인터페이스는 데이터 멤버나 함수 구현이 언어 차원에서 금지되어 있지만, C++은 가능하다.
	*/
	class Person {
	public:
		virtual ~Person();
		virtual std::string name() const = 0;
		virtual std::string birthDate() const = 0;
		virtual std::string address() const = 0;
		...;
	};
	/*
	* 해당 클래스를 코드에 적용하려면 Person에 대한 포인터나 참조자로 프로그래밍하는 방법밖에 없다.
	* 또한 인터페이스 클래스의 인터페이스가 수정되지 않는 한 사용자는 리컴파일할 필요가 없다.
	* 
	* 또한 인터페이스 클래스를 사용하기 위해서는 객체 생성 수단이 1개 이상은 존재해야 한다.
	* 이 문제는 보통 팩토리 함수(또는 가상 생성자)를 통해 해결한다.
	* 팩토리 함수는 주어진 인터페이스 클래스의 인터페이스를 지원하는 객체를 동적 할당하고, 해당 포인터를 반환한다.
	* 보통 이러한 함수는 클래스 내부에 정적 멤버로 선언된다.
	*/
	class Person {
	public:
		...;
		static std::tr1::shared_ptr<Person> create(const std::string& name, const Date& birthday, const Address& addr);
	};
	/*
	* 이러한 경우 사용자 측에서는 다음과 같이 사용한다.
	*/
	std::string name;
	Date dateOfBirth;
	Address address;
	...;
	static std::tr1::shared_ptr<Person> pp(Person::create(name, birthday, addr));
	...;
	std::cout << pp->name() << " was born on " << pp->birthdDate() << " and now lives at " << pp->address();
	/*
	* 물론 해당 인터페이스 클래스의 인터페이스를 지원하는 구체 클래스가 어디엔가는 정의되어야 할 것이며, 실제로 실행되는 생성자가 호출되어야 함은 당연하다.
	* 실제로 이 부분은 다른 파일에서 이루어질 것이다.
	* 일례로 Person 클래스로부터 파생된 RealPerson이라는 구체 클래스는 자신이 상속받은 가상함수에 대한 구현부를 제공하는 식으로 정의되었을 것이다.
	*/
	class RealPerson :public Person {
		std::string theName;
		Date theBirthDate;
		Address theAddress;
	public:
		RealPerson(const std::string& name, const Date& birthday, const Address& addr)
			: theName(name), theBirthDate(birthday), theAddress(addr) {}
		virtual ~RealPerson() {}
		std::string name() const;
		std::string birthDate() const;
		std::string address() const;
	};

	std::tr1::shared_ptr<Person> Person::create(const std::string& name, const Date& birthday, const Address& addr) {
		return std::tr1::shared_ptr<Person>(new RealPerson(name, birthday, addr));
	}
	/*
	* 인터페이스 클래스를 구현하는 용도로 가장 많이 쓰이는 매커니즘이 두 가지 있는데, 일단 위의 경우가 그 중 하나이다. 
	* 즉 인터페이스 클래스로부터 인터페이스 명세를 상속하게 한 후, 해당 인터페이스에 포함된 함수를 구현하는 것이다.
	* 다른 하나는 다중 상속을 사용하는 것이다.
	*/

	/*
	* 결론적으로 핸들 클래스와 인터페이스 클래스는 구현부로부터 인터페이스를 분리함으로서 파일들 내의 컴파일 의존성을 완화시킨다.
	* 물론 단점도 존재한다. 먼저 실행시간 비용이 증가하고, 객체당 필요한 공간이 늘어난다.
	* 
	* 핸들 클래스의 경우 데이터 멤버 접근을 위해 포인터를 타야 하는데, 이 자체가 비용이 된다.
	* 또한 객체 하나씩을 저장하는 데에 필요한 메모리 크기에 구현부 포인터 크기가 더해진다.
	* 마지막으로 구현부 포인터가 동적할당된 구현부 객체를 가리키도록 해당 구현부 포인터의 초기화가 발생해야 한다.
	* 결국 동적할당에 발생하는 연산 오버헤드에, 메모리 고갈 예외를 마주할 가능성도 높아진다.
	* 
	* 인터페이스 클래스의 경우 호출되는 함수가 전부 가상 함수이기 때문에, 함수 호출 시마다 가상 테이블 점프에 따르는 비용이 발생한다.
	* 게다가 인터페이스 클래스에서 파생된 객체는 죄다 가상 테이블 포인터를 지녀야 하므로 가상 함수의 단점을 그대로 받게 된다.
	* 
	* 마지막으로 핸들 클래스와 인터페이스 클래스가 동시에 지니는 약점도 있는데, 인라인 적용이 매우 어렵다.
	* 인라인이 되기 위해서는 함수 본문이 대개 헤더에 두어야 하는데, 컴파일 의존성을 떨어트리는 설계는 이와 상극이기에 그렇다.
	* 
	* 
	* 물론 이런 단점이 있답시고 이러한 기법을 절대 쓰지 않는다는 것만큼 멍청한 짓거리도 없다.
	* 일단 개발 도중에는 위와 같은 기법을 사용하고, 출시 시점에서 비용 등을 고려하여 결합도를 증가시켜야 할 필요를 고민하라.
	*/
}
/*
* 결론.
* 1. 컴파일 의존성을 최소화하는 핵심은 정의 대신 선언에 의존케 하는 것이다. 이는 핸들 클래스나 인터페이스 클래스로 구현 가능하다.
* 2. 라이브러리 헤더는 그 자체로 모든 것을 갖추어야 하며 선언부만 지니는 형태여야 한다. 
*/

#endif