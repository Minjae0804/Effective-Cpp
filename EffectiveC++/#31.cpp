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
	*/



}
/*
* 결론.
* 1. ...
*/

#endif