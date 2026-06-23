#if 0

// #40. 다중 상속은 심사숙고하여 사용하라


// 
{
	/*
	* 다중 상속(multiple inheritance, MI)에 대해 중요한 사실 중 하나는, 둘 이상의 기본 클래스로부터 동일한 이름을 물려받을 가능성이 생겨버린다는 것이다.
	* 즉, 다중 상속 자체만으로 모호성이 생긴다.
	*/
	class BorrowableItem {
	public:
		void checkOut();
		...;
	};

	class ElectronicGadget {
		bool checkOut() const;
		...;
	};

	class MP3Player: public BorrowableItem, public ElectronicGadget { ... };
	MP3Player mp;

	// 모호!
	mp.checkOut();
	
	/*
	* 여기에서 눈여겨보아야 할 사실은, 이 두 checkOut() 중 접근 가능한 것이 명확한데도 모호성이 발생한다는 것이다.
	* 이는 중복된 함수 호출 중 하나를 골라내는 C++의 규칙을 따른 결과이다.
	* 
	* 어떤 함수가 접근 가능한 함수인지를 알아보기 전에, C++ 컴파일러는 이 규칙을 써서 주어진 호출에 대해 최적 일치하는 함수인지를 먼저 확인한다.
	* 즉, 최적 일치 함수를 먼저 찾은 후에야 함수의 접근가능성을 판단한다는 것이다.
	* 
	* 지금의 두 함수의 경우 C++ 규칙에 의한 일치도가 동일하므로 최적 일치 함수가 결정되지 않는다.
	* 따라서 접근가능성은 고려조차 되지 않는다.
	* 
	* 때문에 모호성을 해소하기 위해서는 접근할 함수를 직접 지정하여야 한다.
	*/
	mp.BorrowableItem::checkOut();

	/*
	* 다중 상속의 의미는 그저 둘 이상의 클래스로부터 상속을 받는 것일 뿐이나, 이 MI는 상위 단계의 기본 클래스를 여러 개 가지는 클래스 계통에서 심심치 않게 눈에 띈다.
	* 이러한 구조의 상속계통에서는 죽음의 MI 마름모꼴(deadly MI Diamond)라 불리는, 좋지 않은 구조가 발생할 수 있다.
	*/
	class File { ... };
	class InputFile: public File { ... };
	class OutputFile: public File { ... };
	class IOFile: public InputFile, public OutputFile { ... };
	/*
	* File 클래스 내부에 fileName이라는 데이터 멤버가 있을 때 IOFile 내에는 해당 필드가 몇 개가 될까?
	* 결과적으로 보면 InputFile과 OutputFile에서 그 필드들의 사본을 하나씩 받으므로 두 개가 될 법도 하고, 중복되지 않게 한 개가 될 법도 하다.
	* 
	* C++은 이 두 경우를 다 지원한다.
	* 기본적으로 데이터 멤버를 중복 생성하고, 중복 생성을 원한 것이 아니었다면 해당 데이터 멤버를 지닌 클래스를 가상 기본 클래스로 변환하는 것으로 해결한다.
	* 더 자세히 말하자면, 가상 기본 클래스로 삼을 클래스에 직접 연결된 파생 클래스에서 가상 상속을 사용하게 한다.
	*/
	class File { ... };
	class InputFile : virtual public File { ... };
	class OutputFile : virtual public File { ... };
	class IOFile : public InputFile, public OutputFile { ... };
	/*
	* 정확한 동작 관점에서 보면 public 상속은 항상 가상 상속이어야 하는 것이 맞다...만, 정확성 외의 측면도 고려하면 문제가 있다.
	* 상속되는 데이터 멤버의 중복 생성을 막기 위해 컴파일러는 기본 클래스 부분을 별도 메모리로 빼고 해당 메모리 공간의 주소를 남긴다.
	* 이로 인해 가상 상속을 사용하는 클래스로 생성된 객체는 그렇지 않은 경우보다 크기가 더 크다.
	* 게다가 간접참조로 인한 오버헤드 때문에 기본 클래스의 멤버에 접근하는 속도도 늦다.
	* 즉, 가상 상속은 비용이 크다.
	* 
	* 이것으로 비용 지불을 끝냈으면 좋았을 텐데, 다른 쪽으로도 비용이 발생한다.
	* 가상 기본 클래스의 초기화에 관련된 규칙은 비가상 기본 클래스의 초기화 규칙보다 훨씬 복잡한데다 직관성도 더 떨어진다.
	* 대부분의 경우 가상 상속이 되어 있는 클래스 계통에서는 파생 클래스들로 인해 가상 기본 클래스 부분을 초기화할 일이 생기게 된다.
	* 이 때 들어가는 초기화 규칙은 다음과 같다.
	* 1. 초기화가 필요한 가상 기본 클래스로부터 클래스가 파생된 경우, 해당 파생 클래스는 가상 기본 클래스와의 상속계통상 거리에 상괸없이 가상 기본 클래스의 존재를 염두에 두어야 한다.
	* 2. 기존 클래스 상속계통에 파생 클래스를 새로 추가할 때에도 해당 파생 클래스는 가상 기본 클래스의 초기화를 떠맡아야 한다.
	* 
	* 가상 기본 클래스의 사용 방법은 굉장히 간단하다. 쓰지 않으면 된다.
	* 다만 써야 한다면 가상 기본 클래스에는 데이터 멤버를 들이지 마라.
	*/

	class IPerson {
	public:
		virtual ~IPerson();
		virtual std::string name() const = 0;
		virtual std::string birthDate() const = 0;
	};
	/*
	* IPerson을 사용하려면 IPerson 포인터 및 참조를 통해 프로그래밍을 해야 한다.
	* 추상 클래스로 인스턴스 생성은 할 수 없으므로.
	* 
	* 하여, 조작 가능한 IPerson 객체를 생성하기 위해 IPerson의 사용자는 팩토리 함수를 사용하여 IPerson의 구체 파생 클래스를 인스턴스로 생성한다.
	*/
	std::tr1::shared_ptr<IPerson> makePerson(DatabaseID personIdentifier);

	DatabaseID askUserForDatabaseID();
	DatabaseID id(askUserForDatabaseID());

	std::tr1::shared_ptr<IPerson> pp(makePerson(id));
	/*
	* 그런데, makePerson()은 어떻게 해당 함수가 반환할 포인터로 가리킬 객체를 새로 생성할 수 있을까?
	* makePerson()이 인스턴스로 생성할 수 있는 구체 클래스가 IPerson으로부터 파생되어 있어야 할 텐데 말이다.
	* 
	* 해당 구체 클래스의 이름이 CPerson이라고 가정하자.
	* 구체 클래스가 원래 그렇든 CPerson은 IPerson으로부터 물려받은 순수가상함수에 대한 구현을 제공해야 한다.
	* 그런데, 예전에 준비해 두었던 데이터베이스 전담 클래스인 PersonInfo가 눈에 띈다.
	*/
	class PersonInfo {
		virtual const char* valueDelimOpen() const;
		virtual const char* valueDelimClose() const;
	public:
		explicit PersonInfo(DatabaseID pid);
		virtual ~PersonInfo();
		virtual const char* theNmae() const;
		virtual const char* theBirthDate() const;
		...;
	};
	/*
	* PersonInfo 클래스는 데이터베이스 필드를 다양한 서식으로 출력할 수 있는 기능이 있다는 것을 알아냈다.
	* 이 기능을 사용하면 각 필드값의 시작과 끝을 임의 문자열로 구분하여 출력할 수 있다.
	* 기본적으로 출력욕 필드값의 시작과 끝에 붙는 구분자가 대괄호로 미리 정해져 있다. 예로 필드값이 sally-law sally라면 다음과 같이 서식화된다. 
	*/
	[sally-law sally]
	/*
	* PersonInfo의 사용자가 전부 대괄호를 구분자로 쓰고 싶어하지는 않을 것이므로, 사용자가 원하는 시작 구분자와 끝 구분자를 파생 클래스에서 지정할 수 있도록 각 구분자 함수를 가상 함수로 한다.
	* 그리고 PersonInfo 클래스의 다른 멤버 함수들은 이 가상함수들을 통해 사용하는 필드값에 적절한 구분자를 붙이도록 구현한다.
	*/
	const char* PersonInfo::valueDelimOpen() const { return "["; }
	const char* PersonInfo::valueDelimClose() const { return "]"; }
	const char* PersonInfo::theName() const {
		static char value[Max_Formatted_Field_Value_Length];
		std::strcpy(value, valueDelimOpen());

		// 대충 value 문자열에 해당 객체의 name 필드를 붙이는 코드

		std::strcat(value, valueDelimClose());
		return value;
	}
	/*
	* theName()은 valueDelimOpen()을 호출하여 시작 구분자를 생성하고, name값 자체를 생성한 두; valueDelimClose()을 호출하도록 구현되어 있다.
	* 이 때 valueDelimOpen()과 valueDelimClose()는 가상 함수이므로 theName()의 반환값은 PersonInfo에만 좌우되는 것이 아니라 PersonInfo로부터 파생된 클래스에도 좌우된다.
	* 
	* CPerson을 구현하는 사람의 입장에서 이러한 점은 매우 좋다. 
	* name()과 birthDate()의 반환값에는 구분자가 붙으면 안 되기 때문이다.
	* 
	* CPerson과 PersonInfo간 관계는, PersonInfo 클래스는 CPerson을 구현하기 편하게 만들어주는 함수를 지니고 있을 뿐이다.
	* 이는 is-implemented-in-terms-of 관계이며, 이 경우 객체 합성 또는 private 상속으로 구현할 수 있다.
	* 대부분의 경우 객체합성을 사용하는 것이 좋으나, 가상 함수를 반드시 재정의해야 한다면 private 상속을 사용해야 한다.
	* 
	* 이 경우에는 CPerson 클래스에서 반드시 valueDelimOpen()과 valueDelimClose()가 재정의되어야 하므로 private 상속을 사용하여야 한다.
	* 물론 객체 합성과 public 상속을 조합하는 것만으로도 해결 가능하나, 여기에서는 private 상속을 사용해 보자.
	* 
	* 그에 더해, CPerson 클래스는 IPerson 인터페이스도 구현해야 하므로 이를 위해서는 Public 상속이 필요하다.
	* 따라서 다중 상속을 사용할 수 있다.
	*/
	class IPerson {
	public:
		virtual ~IPerson();
		virtual std::string name() const = 0;
		virtual std::string birthDate() const = 0;
	};

	class DatabaseID { ... };

	class PersonInfo {
		virtual const char* valueDelimOpen() const;
		virtual const char* valueDelimClose() const;
	public:
		explicit PersonInfo(DatabaseID pid);
		virtual ~PersonInfo();
		virtual const char* theNmae() const;
		virtual const char* theBirthDate() const;
		...;
	};

	class CPerson : public IPerson, private PersonInfo {
		const char* valueDelimOpen() const { return ""; }
		const char* valueDelimClose() const { return ""; }
	public:
		explicit CPerson(DatabaseID pid) : PersonInfo(pid) {}
		virtual std::string name() const { return PersonInfo::theName(); }
		virtual std::string birthDate() const { return PersonInfo::theBirthDate(); }
	};


	/*
	* 다중 상속은 대단한 것이 아니다. 그저 객체 지향 기법으로 소프트웨어를 개발하는 데에 사용되는 도구 중 하나일 뿐이다.
	* 단일 상속과 비교하여 사용하기에도 복잡하고 이해하기에도 더 복잡한 것이 사실이므로, MI 설계와 동등한 효과를 내는 SI 설계가 존재한다면 그것을 사용하는 게 더 좋다.
	* 정말 웬만한 경우, MI를 대체할 수 있는 SI 설계가 존재한다. 물론 가장 명료하며 유지보수성도 좋고 가장 적합한 방법이 MI인 경우도 존재한다만.
	* 
	* 중요한 건 다중 상속을 할 때 성급한 설계는 아닌지 확실하게 확인하라는 것이다.
	*/
}
/*
* 결론.
* 1. 다중 상속은 단일 상속보다 확실하게 복잡하다. 신규 모호성 문제를 발생시킬 뿐만 아니라, 가상 상속이 필요해지는 경우도 있다.
* 2. 가상 상속을 사용하면 비용이 늘어나며, 초기화 및 대입연산의 복잡도가 커진다. 따라서 가상기본클래스에는 데이터를 두지 않는 것이 현실적으로 가장 실용적이다.
* 3. 다중상속을 적법히 사용할 수 있는 경우가 있다. 대표적으로 인터페이스 클래스로부터 public 상속을 시킴과 동시에 구현을 돕는 클래스로부터 private 상속을 시키는 것이다.
*/ 
#endif