#if 0

// #38. has-a 혹은 is-implemented-in-terms-of를 모형화할 때에는 객체 합성을 사용하라


// 
{
	/*
	* 합성(composition)이란 어떤 타입의 객체들이 그와 다른 타입의 객체들을 포함하고 있을 경우 성립하는 해당 타입들 사이의 관계를 의미한다.
	* 포함된 객체들을 모아 이들을 포함한 다른 객체를 합성한다는 의미로, 다음과 같은 경우이다.
	*/
	class Address { ... };
	class PhoneNumber { ... };

	class Person {
		std::string name;
		Address addr;
		PhoneNumber voiceNumber;
		PhoneNumber faxNumber;
	};
	/*
	* 예제를 보면 알겠지만, Person 객체는 string, Address, PhoneNumber 객체로 이루어져 있다.
	* 이를 의미하는 다른 용어들도 많은데, 대표적으로 layering, containment, aggregation, embedding 등이 있다.
	* 
	* #32에서 public 상속의 의미는 is-a라 공부한 적이 있는데, 객체 합성 역시 의미를 지니고 있다. *
	* 실제로는 2개의 뜻을 지닐 수 있는데, has-a(~는 ~을 가짐)을 의미할 수도 있고 is-emplemented-in-terms-of(~는 ~를 사용하여 구현됨)을 의미할 수도 있다.
	* 
	* 이렇게 뜻이 2가지인 이유는, 소프트웨어 개발에서 개발자가 대하는 영역(domain)이 두 가지이기 때문이다.
	* 
	* 객체는 크게 2가지로 나뉜다.
	* 1. 응용 영역(application domain): 일상생활에서 볼 수 있는 사물을 본뜬 것들. 사람, 이동수단, 비디오 프레임 등.
	* 2. 구현 영역(implementation domain): 시스템 구현만을 위해 존재하는 인공물. 버퍼, 뮤텍스, 탐색 트리 등.
	* 
	* 객체 합성이 응용 영역 객체간 발생할 경우 그것은 has-a 관계이다.
	* 객체 합성이 구현 영역 객체간 발생할 경우 그것은 is-implemented-in-terms-of 관계이다.
	* 
	* 위 예제에서 Person 클래스가 나타내는 관계는 has-a 관계이다.
	* 하나의 Person 객체는 이름, 주소, 음성전화, 팩스전화 번호를 지니고 있다.
	* 사람이 이름의 일종이라거나, 사람이 주소의 일종이라 말하는 건 아무래도 부자연스러우므로, is-a와 has-a를 헷갈릴 일은 별로 없다.
	* 문제는 has-a와 is-emplemented-in-terms-of의 차이인데, 이게 꽤 어렵다.
	*/

	/*
	* 예로, 객체로 구성된 작은 집합(set), 정확히 말하자면 중복 원소가 없는 집합체를 나타내고 저장 공간도 적게 차지하는 클래스 템플릿이 하나 필요하다고 가정하자.
	* 그런데 set 템플릿은 원소 1개당 포인터 3개의 오버헤드가 걸리도록 구현되어 있어서, 사용자 지정 템플릿을 연결 리스트를 활용하여 만들려고 한다.
	* 
	* 이번 구현의 포인트는 Set 템플릿을 만들되 list에서 파생된 형태부터 시작하도록 만든다는 것이다.
	* 다시 말해, set<T>는 list<T>로부터 상속을 받는다.
	* 어찌 되었건 실제로 set 객체는 list의 일종이 된다.
	*/
	template<typename T>
	class Set : public std::list<T>{...};
	/*
	* D와 B 사이에 is-a 관계가 성립하면 B에서 참인 것들이 전부 D에서도 참이어야 한다.
	* 그러나 list 객체는 중복 원소를 지닐 수 있는 컨테이너이므로, 3051이라는 값이 list<int>에 두 번 삽입되면 이 리스트는 3051의 사본 2개를 품게 된다. 
	* 하지만 Set 객체는 중복을 허용하지 않으므로, is-a 관계가 성립하지 않는다.
	* 
	* 두 클래스 간 관계가 is-a가 될 수 없으므로, public 상속은 지금의 관계를 모형화하는 데에 맞지 않는다. 
	* 올바른 설계는 Set 객체가 list 객체를 사용하여 구현되는(is-implemented-in-terms-of) 형태의 설계이다.
	*/
	template<typename T>
	class Set {
		std::list<T> rep;
	public:
		bool member(const T& item) const;
		void insert(const T& item);
		void remove(const T& item);

		std::size_t size() const;
	};
	/*
	* Set의 멤버 함수는 list에서 이미 제공하는 기능 및 STL의 다른 구성요소를 사용하여 만들면 되므로, 실제 구현은 매우 간단하다.
	*/
	template<typename T>
	bool Set<T>::member(const T& item) const { return std::find(rep.begin(), rep.end(), item) != rep.end(); }

	template<typename T>
	void set<T>::insert(const T& item) { if (!member(item)) reop.push_back(item); }

	template<typename T>
	void set<T>::remove(const T& item) { 
		typename std::list<T>::iterator if = std::find(rep.begin(), rep.end(), item);
		if (it != rep.end()) rep.erase(it);
	}

	template<typename T>
	std::size_t Set<T>::size() const { return rep.size(); }
}
/*
* 결론.
* 1. 객체 합성의 의미는 public 상속이 지니는 의미와 완전히 다르다.
* 2. 응용 영역에서 객체 합성의 의미는 has-a이다. 구현 영역에서는 is-implemented-in-terms-of이다.
*/

#endif