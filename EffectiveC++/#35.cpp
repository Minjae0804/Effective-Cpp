#if 0

// #35. 가상 함수의 대용도 생각하라


// 
{
	/*
	* 게임 캐릭터를 클래스로 설계한다고 가정해보자.
	* healthValue()는 해당 캐릭터의 체력이 얼마나 남았는지를 나타낸다.
	*/
	class GameCharacter {
	public:
		virtual int healthValue() const;
	};
	/*
	* healthValue()는 순수가상함수로 선언되지 않은 것으로 보아, 체력치를 계산하는 기본 알고리즘으로 보인다.
	* 당연한 설계다. 이와 동시에, 이것은 약점이 된다. 너무 당연한 설계이므로 다른 방법이 잘 떠오르지 않는 탓이다.
	* 그렇다면 다른 설계는 없을까? 이 항목에서는 그 방법들에 대해 논하고자 한다.
	*/

	/*
	* 1. 비가상 인터페이스 관용구를 통한 템플릿 메서드 패턴
	* 먼저 가상 함수는 반드시 private 멤버로 두어야 한다는 이론부터 시작해보자.
	* 이 이론을 따르는 사람들이 제안하는 더 괜찮은 설계는 healthValue()를 public 멤버 그대로 두되 비가상으로 두고, 내부적으로는 실제 동작을 맡은 private 가상함수를 호출하는 식으로 하는 것이다.
	*/
	class GameCharacter {
		virtual int doHealthValue() const { ... };
	public:
		virtual int healthValue() const {
			...;
			int retVal = doHealthValue();
			...;
			return retVal;
		}
	};
	/*
	* 사용자로서 하여금 public 비가상 멤버함수를 통해 private 가상함수를 간접호출하게 만드는 방법으로, 비가상 함수 인터페이스(non-virtual interface, NVI) 관용구라고 불린다.
	* NVI 관용구의 이점은 가상 함수 호출 전과 호출 후, 그 상태를 제거하는 작업이 비가상 함수를 통해 간접적으로 보장된다는 것이다.
	* 예로 뮤텍스 잠금을 걸고 푼던가, 로그 정보를 생성한다던가... 
	* 만약 사용자 측에서 가상함수를 직접 호출토록 두었을 경우 지금처럼 사전/사후 동작을 끼워넣을 방법이 없었을 것이다.
	* 
	* 그렇다면 NVI 관용구 사용 시 private 가상함수를 파생에서 오버라이딩할 수 있을까? 
	* 이는 설계상 전혀 문제가 없다. 
	* 가상 함수를 재정의하는 것은 어떤 동작을 어떻게 구현할 것인가를 지정하는 것이며, 가상함수 재정의는 해당 동작이 수행될 시점을 지정하는 것이다.
	* 하여, 이 둘은 관심사 자체가 겹치지 않는다.
	* 
	* NVI 관용구에서는 파생의 가상함수 재정의를 허용한다.
	* 따라서 어떤 기능을 어떻게 구현할지를 조정하는 권한은 파생이 지니게 되지만, 함수를 언제 호출할지를 결정하는 것은 기본 클래스만의 고유 권한이다.
	* 
	* 사실 NVI에서 가상함수는 엄격히 private일 필요가 없다. 
	* 어떤 클래스 계통의 경우 파생 클래스에서 재정의되는 가상함수가 기본클래스의 대응함수를 호출할 것을 예상하고 설계된 것도 있는데, 이런 경우 적법한 함수호출이 되기 위해서는 해당 함수가 protected여야 하기 때문이다.
	*/

	/*
	* 2. 함수 포인터로 구현한 전략 패턴
	* NVI는 public 가상함수를 대체할 수 있는 꽤 괜찮은 방법이지만, 클래스 설계 관점에서 본다면 눈속임이나 다름없다. 
	* 어쨌는 게임 캐릭터의 체력을 계산하는 데에 가상함수를 사용하는 것은 여전하기 때문이다.
	* 
	* 조금 더 극적으로 간다면, 캐릭터의 체력치를 계산하는 작업은 캐릭터 타입과 별개로 놓는 편이 더 맞을 것이다.
	* 다시 말해, 체력치 계산이 구태여 어떤 캐릭터의 일부일 필요는 없다는 것이다.
	* 예로 각 캐릭터의 생성자에 체력치 계산용 함수 포인터를 넘기게 하고, 해당 함수를 호출하여 실제 계산을 수행토록 하면 어떨까?
	*/
	class GameCharacter;
	int defaultHealthCalc(const GameCharacter& gc);

	class GameCharacter {
		HealthCalcFunc healthFunc;
	public:
		typedef int (*HealthCalcFunc) (const GameCharacter&);
		explicit GameCharacter(HealthCalcFunc hcf = defaultHealthCalc) : healthFunc(hcf) {}
		int healthValue() const { return healthFunc(*this); }
	};
	/*
	* 이 방법은 전략 패턴의 단순한 응용예제이다. NVI와 비교하여 꽤 재미있는 융통성을 지니고 있다.
	* 
	* 먼저 동일 캐릭터 타입으로 생성된 객체들도 체력치 계산함수를 각각 다르게 가질 수 있다.
	*/
	class EvilBadGuy : public GameCharacter {
	public:
		explicit EvilBadGuy(HealthCalcFunc hcf = defaultHealthCalc) : healthFunc(hcf) { ... }
		...;
	};

	int loseHealthQuickly(const GameCharacter&);
	int loseHealthSlowly(const GameCharacter&);

	EvilBadGuy edg1(loseHealthQuickly);
	EvilBadGuy edg2(loseHealthSlowly);
	/*
	* 두 번째로 게임이 실행되는 도중 특정 캐릭터에 대한 체력치 계산함수를 바꿀 수 있다.
	* 
	* 물론 단점도 존재한다. 체력치 계산 함수가 GameCharacter 클래스 계통의 멤버함수가 아니라는 점은, 체력치가 계산되는 대상 객체의 비공개 데이터는 이 함수로 접근하지 못한다는 의미도 된다.
	* 일례로 defaultHealthCalc()는 EvilBadGuy의 public 멤버 이외를 건드릴 수 없다.
	* 다만  이 부분은 클래스 내부 기능을 해당 클래스 바깥에 있는 동등한 기능으로 대체하려고 하면 언제든지 생기는 고민거리이다.
	*/

	/*
	* 3. tr1::function으로 구현한 전략 패턴
	* 체력치 계산은 함수뿐 아니라 펑터도 할 수 있는데, 왜 굳이 함수를 써야 할까?
	* 만약 반드시 함수를 써야 한다면, 어째서 멤버 함수는 안 될까?
	* 사실 반환도 그렇다. 굳이 int를 반환해야 할까? int로 변환 가능한 임의 타입은 안 될까?
	* 
	* tr1::function 객체가 함수 포인터를 대체하게 하면 된다.
	* tr1::function 계열 객체는 함수호출성 개체를 지닐 수 있으며, 이 개체들은 주어진 시점에서 예상되는 시그니처와 호환되는 시그니처를 가지고 있다.
	*/
	class GameCharacter;
	int defaultHealthCalc(const GameCharacter& gc);

	class GameCharacter {
		HealthCalcFunc healthFunc;
	public:
		typedef std::tr1::function<int(const GameCharacter&)> HealthCalcFunc;
		explicit GameCharacter(HealthCalcFunc hcf = defaultHealthCalc) : healthFunc(hcf) {}
		int healthValue() const { return healthFunc(*this); }
	};
	/*
	* 보다시피 HealthCalcFunc는 tr1::function 템플릿을 인스턴스화한 것에 대한 별칭이다. 즉, 이 타입은 일반화된 함수 포인터 타입처럼 동작한다. 
	* int(const GameCharacter&)는 const GamcCharacter에 대한 참조를 받고, int를 반환하는 함수라는 의미이다.
	* 이렇게 정의된 tr1::function 타입 객체는 앞으로 대상 시그니처와 호환되는 함수호출성 개체를 어떤 것이라도 가질 수 있다.
	* 여기서 호환된다는 것은, 함수호출성 개체의 매개변수 타입이 const GameCharacter&로 암시적 변환이 가능한 타입이며, 반환 타입도 암시적으로 int로 변환될 수 있다는 의미이다.
	* 
	* 함수 포인터를 사용한 전략 패턴과 비교하면 이 설계도 크게 다른 것은 없다.
	* 다른 것이 있다면, GameCharacter가 이제는 tr1::function 객체, 그러니까 좀 더 일반화된 함수 포인터를 물게 된다는 것이다.
	* 크게 변한 것은 아니지만, 사용자 측에서는 체력치 계산 함수 지정에 있어서 선택지가 조금 더 늘어났다.
	*/
	short clacHealth(const GameCharacter&);

	struct HealthCalculator {
		int operator() (const GameCharacter&) const { ... }
	};

	class GameLevel {
	public:
		float health(const GameCharacter&) const;
	};

	class EvilBadGuy : public GameCharacter {
		...;
	};

	class EyeCandyCharacter : public GameCharacter {
		...;
	};

	EvilBadGuy dbg1(calcHealth);
	EyeCandyCharacter ecc1(HealthCalculator());

	GameLevel currentLevel;
	...;
	EvilBadGuy ebg2(std::tr1::bind(&GameLevel::health, currentLevel, _1));
	/*
	* 위의 정의문이 말하는 바는 ebg2의 체력치를 계산하기 위해 GameLevel의 health를 사용하라는 것이다.
	* GameLevel::health()는 매개변수를 하나만 받는 것으로 되어 있지만 실제로는 두 개를 받는다. GameLevel 객체 하나를 암시적으로 받기 때문이다.
	* 그러나 GameCharacter 객체에 사용되는 체력치 계산 함수가 받는 매개변수는 체력치가 계산되는 GameCharacter 객체 하나뿐이다.
	* 
	* 만일 ebg2의 체력치 계산에 GameLevel::health 함수를 사용하려 한다면 매개변수 두 개를 받는 함수를 매개변수를 하나만 받는 것으로 바꾸어야 할 것이다.
	* 그러나 ebg2의 체력치 계산에 사용할 GameLevel 객체로서 currentLevel만을 사용할 것이므로 GameLevel::health()가 호출될 때마다 currentLevel이 사용되도록 묶어준 것이다.
	* bind는 이 묶기 작업을 맡는다. 즉, ebg2의 체력치 계산 함수는 항상 currentLevel만을 GameLevel 객체로 사용한다고 지정한 것이다.
	* 
	* 다양한 세부사항이 더 있는데, _1은 ebg2에 대해 currentLevel과 묶인 GameLevel::Health 함수를 호출할 때 넘기는 첫 번째 자리의 매개변수를 의미한다는 것도 있다.
	* 아무튼 중요한 건 함수 포인터 대신 tr1::function을 사용하여 사용자가 게임캐릭터의 체력치를 계산할 때 시그니처가 호환되는 함수호출성 개체는 어떤 것도 원하는 대로 구사할 수 있도록 융통성을 늘려준 것이다.
	*/

	/*
	* 4. 고전적인 전략 패턴
	* 고전적인 전략 패턴으로 구현한다면, 체력치 계산 함수를 나타내는 클래스 계통을 아예 따로 만들고 실제 체력치 계산 함수는 이 클래스 계통의 가상 멤버 함수로 만드는 것이다.
	*/
	class GameCharacter;

	class HealthCalcFunc {
	public:
		virtual int calc(const GameCharacter& gc) const { ... }
	};

	HealthCalcFunc defaultHealthCalc;

	class GameCharacter {
		HealthCalcFunc* pHealthCalc;
	public:
		explicit GameCharacter(HealthCalcFunc *phcf = &defaultHealthCalc) : pHealthCalc(phcp) {}
		int healthValue() const { return pHealthCalc->calc(*this); }
	};

	/*
	* 요약.
	* 핵심은, 어떤 문제를 해결하기 위한 설계를 찾을 때 가상함수를 대신하는 방법들도 고려해 보라는 것이다.
	* 
	* 위에서 다룬 것들을 간단히 정리해 보자.
	* 1. NVI 관용구: 공개되지 않은 가상함수를 비가상 public 멤버함수로 감싸서 호출하는 템플릿 메서드 패턴의 한 형태.
	* 2. 가상함수를 함수 포인터 데이터 멤버로 대체: 전략 패턴의 핵심만을 보여주는 형태.
	* 3. 가상함수를 tr1::function 데이터 멤버로 대체: 이 또한 전략 패턴의 한 형태.
	* 4. 한쪽 클래스 계통에 속해 있는 가상함수를 다른 쪽 계통에 속해 있는 가상함수로 대체: 전략 패턴의 전통적인 구현 형태.
	* 
	* 물론 이 네 가지가 전부는 아니다만, 가상 함수를 대체할 수 있는 것이 꽤 있음을 말하는 용도로는 충분할 것이다.
	* 
	* 객체 지향 설계의 여정은 길다. 그저 앞만 보고 가지 말고, 주변을 둘러보며 견문을 쌓아라.
	* 이 또한 나중에는 식견이 되어 쓰일 것이다.
	*/
}
/*
* 결론.
* 1. 가상 함수 대신 사용할 수 있는 다은 방법으로는 NVI 관용구 및 전략패턴을 들 수 있다. 이 중 NVI 관용구는 그 자체가 템플릿 메서드 패턴의 한 예이다.
* 2. 객체에 필요한 기능을 멤버함수로부터 클래스 외부 비멤버 함수로 옮기면 해당 비멤버 함수는 그 클래스의 public 멤버가 아닌 것들에 접근할 수 없다는 단점이 있다.
* 3. tr1::function 객체는 일반화된 함수포인터처럼 동작한다. 이 객체는 주어진 대상 시그니처와 호환되는 모든 함수호출성 개체를 지원한다.
*/

#endif