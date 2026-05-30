#if 0
#include <iostream>
#include <string>
#include <list>

// #4. 객체를 사용하기 전에 반드시 해당 객체를 초기화하자


// 객체의 초기화
{
	class PhoneNumber { ... };

	class ABEntry {
		std::string theName;
		std::string theAddress;
		std::list<PhoneNumber> thePhones;
		int numTimesConsulted;
	public:
		ABEntry(
			const std::string& name,
			const std::string& address,
			const std::list<PhoneNumber>& phones
		);
	};

	ABEntry::ABEntry(
		const std::string& name,
		const std::string& address,
		const std::list<PhoneNumber>& phones
		) {
			theName = name;
			theAddress = address;
			thePhones = phones;
			numTimesConsulted = 0;
	};
	/* 
	* 어떤 객체이든 해당 객체의 데이터 멤버는 생성자 본문이 실행되기 전에 초기화되어야 한다. 
	* 객체 타입 멤버는 생성자 본문 실행 전에 초기화된다. (단 원시 타입 멤버는 생성자 실행 전 초기화된다는 것이라는 보장이 없다.)
	* 즉 위 생성자 실행 전에 멤버들은 이미 초기화를 완료한 상태이며, 생성자에서는 초기화된 멤버에 값을 대입하는 역할을 한다.
	*/

	ABEntry::ABEntry(
		const std::string& name,
		const std::string& address,
		const std::list<PhoneNumber>& phones
		) : theName(name), theAddress(address), thePhones(phones), numTimesConsulted(0) {}
	/*
	* 생성자 실행 전 멤버를 초기화하기 위해서는 위와 같이 멤버 초기화 리스트를 사용한다.
	* 이 방법은 생성자 실행 전 선제 초기화를 생성자 자체에서 진행하므로 훨씬 효율적이다.
	*/

	ABEntry::ABEntry() : theName(), theAddress(), thePhones(), numTimesConsulted(0) {}
	/*
	* 매개변수 없는 생성자를 위와 같이 정의할 수도 있다. 
	* 이는 코드 유지보수 측면에서 도움이 된다.
	* 다만 멤버가 상수나 참조일 경우 멤버 초기화 리스트를 통한 초기화가 필수인데, 상수와 참조는 대입이 불가능하기 때문이다.
	*/

	/*
	* 객체를 구성하는 데이터의 초기화 순서는 다음과 같다.
	* 1. 기본 클래스는 파생 클래스보다 먼저 초기화된다.
	* 2. 클래스 데이터 멤버는 그들이 선언된 순서대로 초기화된다.
	* 
	* 단, 비지역 정적 객체의 초기화 순서는 개별번역단위에서 정해진다.
	*/
}


// 비지역 정적 객체의 초기화 순서는 개별 번역 단위에서 정해진다
{
	/*
	* 정적 객체란 생명주기가 생성 이후 프로그램 종료 시까지인 객체를 의미한다.
	* 다음과 같은 객체는 정적 객체이다.
	* 1. 전역 객체
	* 2. 네임스페이스 유효범위에서 정의된 객체
	* 3. 클래스 내부에서 static으로 정의된 객체
	* 4. 함수 내에서 static으로 정의된 객체
	* 5. 파일 유효범위에서 static으로 정의된 객체
	* 
	* 이 때 4번, "함수 내에서 static으로 정의된 객체"의 경우 지역 정적 객체라 불리고, 나머지는 비지역 정적 객체라고 불린다.
	* 지역 정적 객체는 생명주기가 정적 객체와 같으나 스코프가 정의된 함수에 한정된다.
	*/

	/*
	* 번역 단위는 컴파일을 통해 단일 목적 파일을 생성하는 데에 바탕이 되는 소스코드를 의미한다.
	* 보통 단일 소스 파일이 되며, #include하는 파일까지 합쳐 단일 번역 단위가 된다.
	*/

	/*
	* 앞에서 비지역 정적 객체의 초기화 순서가 개별 번역 단위에서 정해진다고 했다.
	* 때문에 별도 컴파일된 소스파일이 2개 이상이며 각 소스파일에 비지역 정적 객체가 1개 이상 존재할 경우 초기화 순서를 예측할 수 없다.
	* 또한 번역 단위 A에 포함된 비정적 객체의 초기화 시 해당 객체의 초기화에 번역 단위 B에 포함된 정적 객체를 사용할 경우, 해당 정적 객체가 초기화되어 있음이 보장되지 않는다.
	*/

	// 번역 단위 A
	class FileSystem {
		...
	public:
		...
		std::size_t numDisks() const;
	};
	// extern: 파일 내 해당 객체의 실체는 존재하지 않으나 다른 파일에 존재함을 컴파일러에게 전달 
	extern FileSystem tfs;

	// 번역 단위 B
	class Directory {
	public:
		Directory(params);
		...
	};

	Directory::Directory(params) {
		...
		std::size_t disks = tfs.numDisks();
		...
	}

	Directory tempDir(params);
	/*
	* 위와 같은 경우 tfs가 tempDir보다 먼저 초기화되지 않으면 tempDir의 생성자는 초기화되지 않은 tfs를 사용하려 할 수 있다.
	* 그렇다면 tfs가 tempDir보다 먼저 초기화되도록 강제해야 하는데, 이는 불가능하다.
	* 
	* 문제를 근본적으로 해결할 수 없으므로 설계에 변화를 주어 문제를 예방한다. 해당 설계는 다음과 같다.
	* 1. 비지역 정적 객체를 하나씩 맡는 함수를 준비하고, 각 객체를 해당 함수에 넣는다. 
	* 2. 함수 내에서 이들을 정적 객체로 선언하고, 해당 함수들은 이것들에 대한 정적 객체를 반환하게 한다.
	* 3. 사용자는 비지역 정적 객체를 직접 참조하지 않고, 함수를 호출하는 것으로 정적 객체를 사용한다.
	* 
	* 이 경우 비지역 정적 객체가 지역 정적 객체로 바뀐다.
	* 지역 정적 객체의 경우 초기화 시점을 예측할 수 없는 비지역 정적 객체와 달리, 해당 정적 객체를 담는 함수가 호출되어 변수 선언에 도달하는 시점에 초기화된다.
	* 따라서 정적 객체의 초기화 시점을 예측할 수 있게 된다.
	*/

	// 번역 단위 A
	class FileSystem {...};

	// 정적 객체를 객체 참조를 반환하는 함수로 대신함 1
	FileSystem& tfs() {
		static FileSystem fs;
		return fs;
	}

	// 번역 단위 B
	class Directory {...};
	Directory::Directory(params) {
		...
		// tfs 객체 자체에 접근하는 대신, tfs의 참조(를 리턴하는 함수)를 통해 접근
		std::size_t disks = tfs().numDisks();
		...
	}
	// 정적 객체를 객체 참조를 반환하는 함수로 대신함 2
	Directory& tempDir() {
		static Directory td;
		return td;
	}
}

/*
* 결론.
* 1. 기본제공 타입 객체는 직접 초기화해야 한다.
* 2. 생성자에서는 멤버 초기화 리스트를 주로 사용하라. 초기화 리스트에 데이터 멤버를 나열할 때에는 클래스에 각 데이터 멤버가 선언된 순서와 같게 나열하라.
* 3. 여러 번역 단위에 존재하는 비지역 정적 객체들의 초기화 순서 문제는 회피하여 설계하라. 지역 정적 객체를 사용하는 방향으로 바꿔라.
*/

#endif