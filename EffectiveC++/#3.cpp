#if 0
#include <iostream>
#include <vector>
#include <string>

// 상수의 종류
{
	// 데이터
	char greeting[] = "Hello";

	// 비상수 포인터 / 비상수 데이터
	char* p = greeting;

	// 비상수 포인터 / 상수 데이터
	const char* p = greeting;

	// 상수 포인터 / 비상수 데이터
	char* const p = greeting;

	// 상수 포인터 / 상수 데이터
	const char* const p = greeting;

	/* 
	* const는 우측의 선언을 수식한다. 
	* 1. const가 포인터 변수 식별자의 바로 왼쪽에 위치할 경우 const는 변수의 포인터를 상수화한다.
	* 2. const가 데이터 타입의 바로 왼쪽에 위치할 경우 const는 그 포인터 변수의 포인터가 가리키는 데이터를 상수화한다.
	*/
}



// 상수 반복자(이터레이터)
{
	// 벡터
	const std::vector<int> vec = { 1,2,3,4,5,6,7,8,9,10,11,12 };

	// 상수로 선언된 일반 이터레이터
	const std::vector<int>::iterator iter = vec.begin();
	*iter = 10;
	++iter;			// 불가능 
	/*
	* 상수로 선언된 일반 이터레이터는 T* const처럼 동작한다.
	* 때문에 iter는 가리키는 대상 자체는 변경할 수 있으나 포인터는 변경할 수 없다.
	*/

	// 상수 이터레이터
	std::vector<int>::const_iterator cIter = vec.begin();
	++cIter;
	*cIter = 10;	// 불가능
	/*
	* 상수 이터레이터는 const t*처럼 동작한다.
	* 때문에 cIter는 가리키는 대상을 변경할 수 없으나 포인터는 변경할 수 있다.
	*/
}



// 상수 멤버 함수 오버로딩
{
	class TextBlock{
		std::string text;
	public:
		//상수객체용 operator[]
		const char& operator[](std::size_t position) const { return text[position]; }
		//비상수객체용 operator[]
		char& operator[](std::size_t position) { return text[position]; }

		/*
		* 오버로딩 판정에 const는 포함되지 않는다.
		* 때문에 상수 멤버 함수는 오버로딩이 가능하다.
		*/
	}


	// 비상수 멤버 호출
	TextBlock tb("hello");
	std::cout << tb[0];

	const TextBlock ctb("world");
	// 상수 멤버 호출
	std::cout << ctb[0];


	// 상수 객체가 사용되는 예제
	void print(const TextBlock& ctb) {
		std::cout << ctb[0];
		...
	}


	// 비상수 객체를 읽음
	std::cout << tb[0];
	// 비상수 객체에 값을 덮어씌움
	tb[0] = 'x';

	// 상수 객체를 읽음
	std::cout << ctb[0];
	// 상수 객체에 값을 덮어씌움(불가능)
	ctb[0] = 'x';


	// 값에 의한 반환
	class TextBlock {
		...
	public:
		...
		// 정상
		char& operator[](std::size_t position) { return text[position]; }
		// 컴파일 에러
		char operator[](std::size_t position) { return text[position]; }

		/*
		* 연산자 오버로딩 시 반환값은 참조여야 한다.
		* C++의 함수는 값에 의한 반환, 즉 결과 원본이 아닌 결과의 사본을 반환한다.
		* 때문에 참조를 사용하지 않으면 해당 연산자로 인해 수정되는 것이 text 원본이 아닌 text의 사본이 된다. 
		* 즉, 아무 일도 일어나지 않는다.
		*/
	}
}



// 물리적 상수성과 논리적 상수성
{
	/*
	* 물리적 상수성은 멤버함수가 자신이 속한 객체의 어떠한 멤버변수도 건드리지 않음을 나타내는 성질이다. 
	* 비트 단위조차 수정을 금하므로 비트수준 상수성이라고 하기도 한다.
	* C++에서는 기본적으로 비트 수준 상수성을 상수성으로 정의한다.
	*/

	class CTextBlock {
		char* pText;
	public:
		...
		// 상식적으로 상수성을 만족하지 않지만 물리적 상수성을 만족하는 경우
		char& operator[](std::size_t position) const { return pText[position]; }
	};
	/*
	* 위 예시에서 operator[]는 pText 자체를 수정할 수는 없지만 pText가 가리키는 데이터는 수정할 수 있다.
	* 때문에 물리적 상수성은 만족하지만 상식적으로 상수성을 만족하지는 않는다.
	*/

	// 상수 객체를 선언하고, 해당 객체의 operator[]를 호출하여 cctb의 내부 데이터에 대한 포인터를 pc에 저장
	const CTextBlock cctb("Hello");
	char* pc = &cctb[0];

	// 이러면 cctb가 가리키는 값을 변경할 수 있다. 즉 상식적인 상수성을 만족하지 못한다.
	*pc = 'J';


	/*
	* 논리적 상수성은 멤버함수가 자신이 속한 객체의 멤버변수를 건드릴 수는 있되 작동에 영향을 주지 않음을 나타내는 성질이다.
	*/

	class CTextBlock {
		char* pText;
		std::size_t textLen;
		bool lenIsValid;
	public:
		...
		std::size_t length() const;
	};

	// 물리적 상수성을 만족하지 않는 상수 멤버 변수
	std::size_t CTextBlock::length() const {
		if (!lenIsValid) {
			textLen = std::strLen(pText);
			lenIsValid = true;
		}
		return textLen;
	}
	/*
	* 물리적 상수성을 상수성으로 따르는 한 해당 상수 멤버 함수는 자신이 속한 객체의 멤버인 textLen과 lenIsValid를 수정할 수 없다.
	* 때문에 이 경우 멤버변수에 mutable을 선언하여 물리적 상수성에서 벗어나게 해야 한다.
	*/

	class CTextBlock {
		char* pText;
		// 멤버변수를 mutable로 선언하여 상수 멤버 함수 내에서도 수정 가능하게 한다.
		mutable std::size_t textLen;
		mutable bool lenIsValid;
	public:
		...
			std::size_t length() const;
	};

	std::size_t CTextBlock::length() const {
		if (!lenIsValid) {
			textLen = std::strLen(pText);
			lenIsValid = true;
		}
		return textLen;
	}
	/*
	* 상수 멤버 함수가 논리적 상수성을 따르게 하기 위해서는 사용자가 상수성을 객체 수준에서 설계하여야 한다.
	* mutable 키워드는 그 설계 도구 중 하나이다.
	*/
}



// const 캐스팅
{
	class TextBlock {
		std::string text;
	public:
		...
		const char& operator(std::size_t position) const {
			// 경계검사
			// 접근 데이터 로깅
			// 자료 무결성 검증
			return text[position];
		}
		char& operator(std::size_t position) {
			// 경계검사
			// 접근 데이터 로깅
			// 자료 무결성 검증
			return text[position];
		}
	};
	/*
	* 상수 멤버 함수의 존재로 인해 오버로딩이 필수이며, 이로 인해 코드 중복이 발생한다.
	* 상수 멤버 함수와 비상수 멤버 함수의 역할은 정확히 같으며, 다른 것은 const 뿐이다.
	* 때문에 이 경우 비상수 operator[]가 자신의 상수 버전을 호출하는 것으로 구현하여 안정성과 코드 중복 회피를 챙길 수 있다.
	*/


	class TextBlock {
		std::string text;
	public:
		...
		const char& operator[](std::size_t position) const {
			...
			return text[position];
		}
		char& operator[](std::size_t position) {
			return const_cast<char&> (
				static_cast<const TextBlock&>(*this)[position]
			);
		}
	};
	/*
	* 비상수 멤버 함수 operator[]는 다음과 같은 작업을 실행한다.
	* 
	* 1. static_cast<const TextBlock&>(*this): 비상수 객체인 자신을 상수 객체로 정적 캐스팅한다. 
	*      이 때 템플릿 타입에 const가 붙지 않으면 탈출 없는 재귀가 발생한다. 
	* 2. [position]: 상수 객체로서 상수 멤버 함수 operator[]를 호출하고 리턴값을 받는다. 이 때 리턴값의 타입은 const char&이다.
	* 3. return const_cast<char&>(...): 상수 operator[]의 리턴값의 타입인 const char&에서 const를 제거하여 char&로 만들고, 그 값을 최종 리턴한다.
	*/
}

/*
* 결론.
* 1. const는 붙일 각이 보이면 한 번씩 들이대보는 것이 코드 안정성에 이롭다.
* 2. 컴파일러는 물리적 상수성만 판단하므로 사용자가 논리적 상수성을 챙겨야 한다.
* 3. 상수 멤버 및 비상수 멤버 함수의 기능이 동일할 경우, 비상수 버전이 상수 버전을 호출하게 하여 코드 중복을 방지할 수 있다.
*/

#endif