#if 0

// #9. 객체 생성 및 소멸 과정 중에는 절대 가상 함수를 호출하지 말라


//
{
	class Transaction {
	public:
		Transaction();
		virtual void logTransaction() const = 0;
		...
	};

	Transaction::Transaction() {
		...
		logTransaction();
	}

	class BuyTransaction : public Transaction {
	public:
		virtual void logTransaction() const;
		...
	};

	class SellTransaction : public Transaction {
	public:
		virtual void logTransaction() const;
	};

	BuyTransaction b;

	/*
	* 이 때 BuyTransaction 생성자를 호출할 경우 Transaction의 생성자가 먼저 호출된다.
	* 생성자 호출 시 해당 클래스의 상속계통 최상위 클래스의 생성자부터 아래로 내려가며 실행되기 때문이다.
	* 
	* 이 때 Transaction 생성자의 마지막에는 가상 함수인 logTransaction()을 호출하는데, 이 경우 BuyTransaction의 logTransaction()이 아닌 Transaction의 logTransaction()이 실행되게 된다. 
	* 기본 클래스의 생성자가 실행되는 동안 가상함수는 절대 파생 클래스로 내려가지 않는다. 대신 객체 자신이 기본 클래스 타입인 것처럼 동작한다. 
	* 
	* 상속계통 최상위에서 생성자 호출이 내려오는 과정에서 각 클래스의 생성자가 실행되는 동안에는 생성한 객체 타입이 무엇이든 실행 중인 생성자의 클래스 타입으로 판정된다.
	* 이는 소멸자도 비슷하다. 소멸자는 생성자와 반대로 현재 클래스에서 올라가며 실행되는데, 각 소멸자의 실행 시점에는 소멸 객체 타입이 실행 소멸자의 클래스 타입이 된다.
	* 
	* 이렇게 진행되는 이유는 상위 클래스에 실행 과정 중에는 하위 클래스가 초기화되지 않은 상태이기 때문이다. 
	* 때문에 기본 클래스의 생성 과정에서는 가상함수의 동작(적절한 파생 클래스의 구현함수 호출)이 발생하지 않는다.
	*/

	class Transaction {
		// 비가상 함수에서 가상함수 호출
		void init() { logTransaction(); }

	public:
		Transaction() { init(); };
		virtual void logTransaction() const = 0;
		...
	};
	/*
	* 이 경우 코드 빌드에서 아무 문제도 발생하지 않으나 실행 시 바로 종료된다. 생성자의 logTransaction()이 순수가상함수이기 때문이다.
	* 
	* 이러한 문제는 생성/소멸중인 객체에 대해 생성자/소멸자에서 가상함수를 호출하는 코드를 철저히 솎아내고, 생성자와 소멸자가 호출하는 모든 함수가 동일 제약을 따르도록 강제하는 것으로 해결한다.
	* 대표적인 것은 logTransaction()을 Transaction 클래스의 비가상 멤버 함수로 전환하고, 이후 파생 클래스의 생성자들로 하여금 필요 로그 정보를 Transaction 생성자로 넘겨야 한다는 규칙을 만드는 것이다.
	*/

	class Transaction {
	public:
		// 로그 정보를 인자로 받음
		explicit Transaction(const std::string& logInfo);
		// 비가상으로 전환
		void logTransaction(const std::string& logInfo) const;
		...
	};

	Transaction::Transaction(const std::string& logInfo) {
		...
		// 비가상 함수 호출
		logTransaction(logInfo);
	}

	class BuyTransaction : public Transaction {
		static std::string createLogString(parameters);
	public:
		// 로그 정보를 기본 클래스의 생성자로 넘김
		BuyTransaction(parameter) : Transaction(createLogString(parameters)) {...}
		...
	};

	/*
	* 기본 클래스 생성자가 실행될 때에는 가상함수를 호출하더라도 파생 클래스의 함수를 사용할 수 없다.
	* 따라서 필요한 초기화 정보를 파생 클래스에서 기본 클래스 생성자로 던져줌으로서 필요 정보를 부모가 받을 수 있다.
	* 
	* 여기에서 기본 클래스 생성자로 넘길 값을 생성하는 createLogString()은 정적 멤버이므로 생성되지 않은 BuyTransaction의 미초기화된 데이터 멤버를 건드릴 염려가 없다.
	*/
}
/*
* 결론.
* 1. 생성자 또는 소멸자에서 가상함수를 호출하면 안 된다.
*/

#endif