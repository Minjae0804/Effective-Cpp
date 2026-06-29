#if 0

// #43. 템플릿으로 생성된 기본 클래스 내부 이름에 접근하는 방법을 알아라


// 
{
	class CompanyA {
	public:
		...;
		void sendCleartext(const std::string msg);
		void sendEncrypted(const std::string msg);
		...;
	};

	class CompanyB {
	public:
		...;
		void sendCleartext(const std::string msg);
		void sendEncrypted(const std::string msg);
		...;
	};

	class MsgInfo { ... };

	template<typename Company> 
	class MsgSender {
	public:
		...;
		void sendClear(const MsgInfo& info) {
			std::string msg;
			Company c;
			c.sendCleartext(msg);
		}

		void sendSecret(const MsgInfo& info){ ... }
	};
	/*
	* 여기에서, 메시지를 보낼 때마다 관련 정보를 로그로 남기고 싶다.
	* 파생 클래스를 사용하면 이 기능을 쉽게 붙일 수 있을 것 같다.
	*/
	template<typename Company>
	class LoggingMsgSender : public MsgSender<Company> {
	public:
		void sendClearMsg(const MsgInfo& info) {
			메시지 전송 전 정보를 로그에 기록;
			sendClear(info);
			메시지 전송 후 정보를 로그에 기록;
		}
	};

	/*
	* 여기에서 잘 보면 파생의 메시지 전송 함수의 이름이 기본 클래스의 그것과 다른 것을 알 수 있다.
	* 물론 이는 잘 된 설계이다.
	* 기본 클래스로부터 물려받은 이름을 파생에서 가리는 문제는 물론, 상속받은 비가상을 재정의하는 문제도 해결한 것이기 때문이다.
	* 
	* 하지만 이 코드는 컴파일되지 않는다.
	* sendClear()가 존재하지 않는 것이 그 이유이다.
	* 
	* sendClear()이 왜 존재하지 않는지에 대해서는, 컴파일러가 LoggingMsgSender 클래스 템플릿의 정의와 마주칠 때 컴파일러는 해당 클래스가 어디에서 파생되었는지 모르기 때문이다.
	* MsgSender<Company>라는 것은 맞지만, Company는 템플릿 파라미터이고 이것은 LoggingMsgSender가 인스턴스로 생성될 때까지 무엇이 될 지 알 수 없다.
	* Company가 정확히 무엇인지 모르는 상황에서는 MsgSender<Company>가 어떠한 형태인지 알 방법이 없다.
	*/

	class CompanyZ {
	public:
		...;
		void sendEncrypted(const std::string& msg);
		...;
	};
	/*
	* 조금 전에 본 일반형 MsgSender 템플릿은 그대로 이 클래스에 쓰기 애매하다.
	* 해당 템플릿은 CompanyZ 객체의 설계 철항과 맞지 않는 sendClear 함수를 제공하기 때문이다.
	* 이를 바로잡기 위해 CompanyZ를 위한 MsgSender의 특수화 버전을 생성할 수 있다.
	*/
	template<>
	class MsgSender<CompanyZ> {
	public:
		...;
		void sendSecret(const MsgInfo& info) { ... }
	};

	/*
	* 여기에서 template<>는 이것이 템플릿도, 클래스도 아님을 나타낸다.
	* 정확히는 이 코드는 MsgSender 템플릿을 템플릿 매개변수가 CompanyZ일 때 사용 가능하도록 특수화한 것이다.
	* 
	* 특히 이것은 완전 템플릿 특수화(total template specialization)이라고 한다.
	* 이는 해당 템플릿이 어떠한 타입에 대해 특수화되었고, 이 템플릿의 파라미터들이 전부 구체적인 타입으로 정해진 상태라는 것이다.
	* 따라서 일반 타입 파라미터가 CompanyZ로 정의된 이상 이 템플릿의 매개변수로는 다른 것이 올 수 없게 된다는 것이다.
	*/
	template<typename Company>
	class LoggingMsgSender : public MsgSender<Company> {
	public:
		void sendClearMsg(const MsgInfo& info) {
			메시지 전송 전 정보를 로그에 기록;
			// Company == CompanyZ일 경우 이 함수는 존재하지 않는다.
			sendClear(info);
			메시지 전송 후 정보를 로그에 기록;
		}
	};
	/*
	* 주석에도 나왔듯 기본 클래스가 MsgSender<CompanyZ>일 경우 이 코드는 말이 되지 않는다.
	* MsgSender<CompanyZ>에는 sendClear()가 없기 때문이다.
	* 때문에, 위와 같은 함수 호출은 C++에서 받아주지 않는다.
	* 즉 C++ 컴파일러는 템플릿으로 만들어진 기본 클래스를 뒤져서 상속된 이름을 찾는 것을 거부한다.
	* 
	* 이 문제를 해결하기 위해서는 이 동작이 발생하지 않게 해야 한다.
	* 이에는 세 가지 방법이 있다.
	*/
	/*
	* 1. 기본 클래스 함수에 대한 호출문 앞에 this->를 붙인다.
	*	 이러면 sendClear가 상속되는 것으로 가정된다.
	*/
	template<typename Company>
	class LoggingMsgSender : public MsgSender<Company> {
	public:
		void sendClearMsg(const MsgInfo& info) {
			메시지 전송 전 정보를 로그에 기록;
			this->sendClear(info);
			메시지 전송 후 정보를 로그에 기록;
		}
	};

	/*
	* 2. using 선언을 사용한다.
	*/
	template<typename Company>
	class LoggingMsgSender : public MsgSender<Company> {
	public:
		// 컴파일러에게 sendClear()가 기본 클래스에 존재한다고 가정하게 한다.
		using MsgSender<Company>::SendClear;
		void sendClearMsg(const MsgInfo& info) {
			메시지 전송 전 정보를 로그에 기록;
			sendClear(info);
			메시지 전송 후 정보를 로그에 기록;
		}
	};

	/*
	* 3. 호출할 함수가 기본 클래스의 함수임을 명시적으로 지정한다.
	*    다만 이 방법은 추천되지 않는데, 호출되는 함수가 가상함수일 경우 이런 식으로 명시적 한정을 했을 때 가상함수 바인딩이 무시되기 때문이다.
	*/
	template<typename Company>
	class LoggingMsgSender : public MsgSender<Company> {
	public:
		void sendClearMsg(const MsgInfo& info) {
			메시지 전송 전 정보를 로그에 기록;
			// Company == CompanyZ일 경우 이 함수는 존재하지 않는다.
			MsgSender<Company>::sendClear(info);
			메시지 전송 후 정보를 로그에 기록;
		}
	};

	/*
	* 이름에 대한 가시성을 조작한다는 부분에 대해서는 세 가지의 방법 전부 동작 원리가 같다.
	* 기본 클래스 템플릿이 이후 어떻게 특수화되더라도 원래의 일반형 템플릿에서 제공하는 인터페이스를 그대로 제공할 것이라고 컴파일러에게 약속하는 것이다.
	* 
	* 이러한 약속은 파생 클래스 템플릿을 컴파일러가 구문분석하는 데에 반드시 필요하나, 이것이 구현과 반대되는 경우 컴파일 과정에서 반드시 에러가 발생한다.
	*/
	LoggindMsgSender<CompanyZ> zMsgSender;
	MsgInfo msgData;
	...;
	zMsgSender.sendClearMsg(msgData);
	/*
	* sendClearMsg() 호출은 컴파일되지 않는다.
	* 이 부분에서는 기본 클래스가 MsgSender<CompanyZ>d라는 것을 컴파일러가 알고 있는데다가 sendClearMsg()가 호출하려 하는 sendClear()는 MsgSender<CompanyZ>에 없다는 것도 알고 있기 때문이다.
	*/

	/*
	* 본질적으로 기본 클래스의 멤버에 대한 참조가 무효한지를 컴파일러가 진단하는 과정이 미리 들어가느냐 나중에 들어가느냐가 중요하다.
	* 여기에서 C++은 이른 진단을 선호하는 정책으로 결정한 것이다.
	*/
}
/*
* 결론.
* 1. 파생 클래스 템플릿에서 기본 클래스 템플릿의 이름을 참조할 때에는 this->를 접두사로 붙이거나 기본 클래스 한정문을 명시하는 것으로 해결하라.
*/

#endif