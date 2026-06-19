#if 0

// #34. 인터페이스 상속과 구현 상속의 차이를 제대로 파악하고 구별하라


// 
{
	/*
	* 상속은 사실 두 가지로 나뉜다.
	* 하나는 함수 인터페이스의 상속, 다른 하나는 함수 구현의 상속이다.
	* 인터페이스 상속과 구현 상속의 차이는 함수 선언과 함수 정의의 차이와 맥을 같이 한다고 보면 된다.
	*/
	class Shape {
	public:
		virtual void draw() const = 0;
		virtual void error(const std::string& msg);
		int objectID() const;
	};

	class Rectangle : public Shape { ... };
	class Ellipce : public Shape { ... };

	/*
	* Shape는 순수가상함수인 draw에 의해 추상 클래스가 된다.
	* 때문에 Shape 클래스는 인스턴스 생성이 불가능하며, 이 클래스의 파생만 인스턴스 생성이 가능하다.
	* 
	* 하지만 Shape는 파생에 대해 절대적인 영향을 가한다. 그 이유가 무엇일까?
	*/

	/* 
	* 먼저, 멤버 함수 인터페이스는 항상 상속된다.
	* public 상속의 의미는 is-a이므로 기본 클래스에 해당하는 것들은 모두 파생 클래스에도 해당되어야 한다.
	* 따라서 어떠한 클래스에서 동작하는 함수는 해당 클래스의 파생에서도 동작하여야 옳다.
	* 
	* Shape 클래스에 존재하는 세 가지의 함수들은 선언된 방법도 각기 다르다. 그런데, 선언의 다름에 어떠한 뜻이 있을까?
	*/

	/*
	* 순수가상함수인 draw()의 경우부터 살펴보자.
	* 순수가상함수의 특징은 두 가지가 있다.
	* 1. 어떤 순수가상함수를 물려받은 구체 클래스는 해당 순수가상함수를 다시 선언해야 한다.
	* 2. 순수 가상 함수는 전형적으로 추상클래스 내부에서 정의를 지니지 않는다.
	* 
	* 이 두 특징을 모아보면 단일한 결론이 나온다.
	* 순수가상함수를 선언하는 목적은 파생 클래스에게 함수의 인터페이스만을 물려주려는 것이다.
	* 
	* 
	* 사실 순수가상함수에도 정의를 제공할 수 있다. 즉, 구현이 붙을 수 있다.
	* 다만 이 경우 반드시 클래스명을 한정자로 붙여 주어야 한다.
	* 참고로 이 부분은 비순수가상함수에 대한 기본구현을 더 안전히 제공하는 매커니즘으로도 쓸 수 있다.
	*/

	/*
	* 다음은 비순수(단순)가상함수인 error()이다. 
	* 비순수가상함수는 파생 클래스로 하여금 함수의 인터페이스를 상속하게 함은 같으나. 파생에서 오버라이드할 수 있는 구현부도 제공한다는 점이 다르다.
	* 즉, 비순수가상함수를 선언하는 목적은 파생 클래스에게 함수의 인터페이스 뿐 아니라 해당 함수의 기본구현도 물려주려는 것이다.
	* 
	* 알고 보면 비순수가상함수에거 함수 인터페이스와 기본 구현을 한꺼번에 지정토록 내버려두는 것은 위험할 수도 있다.
	* 다음의 예를 보자.
	*/
	class Airport { ... };
	class Airplane {
	public:
		virtual void fly(const Airport& destination);
		...;
	};

	void Airplane : fly(const Airport& destination) {
		대충 주어진 목적지로 비행기를 날리는 코드;
	}
	
	class ModelA : public Airplane { ... };
	class ModelB : public Airplane { ... };

	/*
	* 보다시피 Airplane::fly는 가상 함수로 선언되어 있다. 모든 비행기는 fly()를 지원해야 하는 것을 나타내야 하기 때문이다.
	* ModelA 및 ModelB에 대해 동일한 코드를 작성하는 것은 막아야 하므로, 기본적인 비행원리를 Airplane::fly() 함수의 본문으로 제공함으로서 이것을 각 파생이 물려받을 수 있게 했다.
	* 
	* 여기에서 신규 모델 클래스를 생성한다고 해 보자. 이 클래스는 비행 방식이 완전히 다르다.
	* 그런데, 만약 해당 클래스의 프로그래머들이 fly() 재정의를 잊어버린다면?
	* 이러면 기본 제공된 비순수가상함수의 구현을 그대로 가져올 것이다.
	* 
	* 작금의 문제는 Airplane::fly()가 기본 동작을 구현하여 지니고 있음이 아니라, 해당 클래스가 그 기본 구현을 원한다고 명시하지 않았음에도 자동으로 상속하게 된다는 것이다.
	* 다행히 기본 동작을 물려주는 것도 쉬우나 물려주지 않는 것도 쉽다.
	* 일종의 수법인데, 가상 함수의 인터페이스와 해당 가상함수의 기본 구현을 잇는 연결관계를 끊어버리는 것이다.
	*/
	class Airplane {
	public:
		virtual void fly(const Airport& destination) = 0;
		...;
	protected:
		void defaultFly(const Airport& description);
	};

	void Airplane::fly(const Airport& destination) {
		대충 주어진 목적지로 비행기를 날리는 코드;
	}
	/*
	* fly()가 순수가상함수로 변경되었는데, 해당 가상함수가 fly()의 인터페이스를 제공하는 역할을 맡게 된다.
	* 그리고 그 구현을 defalutFly()로 분리했다.
	* 기본 동작이 필요한 경우 fly() 함수 내에서 defaultFly()를 인라인 호출하기만 하면 된다.
	*/
	class ModelA : public Airplane {
	public:
		void fly(const Airport& destination) { defaultFly(destination); }
	};

	class ModelB : public Airplane {
	public:
		void fly(const Airport& destination) { defaultFly(destination); }
	};

	/*
	* 이 방법으로 프로그래머의 실수를 완전 막을 수는 없지만, 원래의 설계보다는 좀 더 믿고 쓸 수 있게 되었다.
	* defaultFly()는 protected 멤버인데, Airplane 및 그 클래스의 파생만 내부적으로 사용하는 구현 세부사항이므로 protected를 붙여도 된다.
	* 
	* 또 다른 중요사항은 Airplane::defaultFly()가 비가상이라는 것이다.
	* 이는 파생에서 해당 함수를 재정의하면 안 되기 때문이다.
	* 
	* 참고로 인터페이스와 기본 구현이 분리되면서 인터페이스 및 기본 구현 제공 함수를 별도 마련하지 않을 수도 있다.
	* 이건 순수가상함수가 구체 파생에서 재선언되어야 하되 자체적으로 순수가상함수의 구현을 구비하는 방법이다.
	*/
	class Airplane {
	public:
		virtual void fly(const Airport& destination) = 0;
		...;
	};

	void Airplane::fly(const Airport& destination) {
		대충 주어진 목적지로 비행기를 날리는 코드;
	}

	class ModelA : public Airplane {
	public:
		void fly(const Airport& destination) { Airplane::fly(destination); }
	};

	class ModelB : public Airplane {
	public:
		void fly(const Airport& destination) { Airplane::fly(destination); }
	};

	class ModelC : public Airport {
	public:
		void fly(const Airport& destination);
		...;
	};

	void ModelC::fly(const Airport& destination) {
		대충 주어진 목적지로 ModelC를 날리는 코드;
	}
	/*
	* 이전 설계와 거의 같다. 요컨대, fly()가 선언부와 정의부의 두 쪽으로 나뉜 것이다.
	* 선언부는 이 함수의 인터페이스를 지정하고, 정의부는 이 함수의 기본동작을 지정한다.
	* 그러나 fly와 defaultFly가 합쳐졌으므로 각 함수 양쪽에 다른 접근지정자를 붙이는 융통성은 사라졌다. 
	*/

	/*
	* 마지막으로 비가상 함수인 objectID()이다.
	* 
	* 멤버함수가 비가상이라는 것은, 해당 함수는 파생에서 다른 행동이 발생할 것으로 가정하지 않았다는 것이다.
	* 실제로 비가상 멤버함수는 클래스 파생에 상관없이 불변하는 동작을 지정할 때 사용한다.
	* 
	* 즉, 비가상 함수를 선언하는 목적은 파생 클래스가 함수 인터페이스와 더불어 해당 함수의 필수 구현을 물려받게 하는 것이다.
	* 따라서, Shape::objectID는 클래스 파생에 상관없는 불변동작과 같으므로, 파생 클래스에서 재정의할 수 있는 것이 절대 아니다.
	*/

	/*
	* 각 함수의 선언문이 지니는 차이를 통해 해당 클래스의 파생이 물려받을 것을 정밀히 지정할 수 있다.
	* 각 선언문 형식만큼 뜻하는 바도 제각각이므로, 멤버함수 선언은 굉장히 신중해야 한다.
	* 
	* 멤버함수 선언 시 흔히 발견되는 결정적인 실수 두 가지는 다음과 같다.
	* 
	* 1. 모든 멤버 함수를 비가상으로 선언하는 것.
	*    이렇게 하면 파생 클래스를 만들더라도 기본 클래스 동작을 특별히 할 수 있는 여지가 사라진다.
	*    기본 클래스로 사용되는 클래스는 거의 반드시 가상 함수를 지니고 있다.
	* 
	* 2. 모든 멤버 함수를 가상으로 선언하는 것.
	*    뭐 인터페이스 클래스라는 좋은 예는 있겠으나, 분명히 파생에서 재정의가 불가해야 하는 함수도 있다.
	*    클래스 파생에 상관없는 불변동작을 지녀야 한다면 확실하게 그리 하라.
	*/
}
/*
* 결론.
* 1. 인터페이스 상속은 구현 상속과 다르다. 퍼블릭 상속에서 파생 클래스는 항상 기본 클래스의 인터페이스를 모두 물려받는다.
* 2. 순수가상함수는 인터페이스 상속만을 허용한다.
* 3. 비순수가상함수는 인터페이스 상속과 더불어 기본 구현 상속도 가능케 한다.
* 4. 비가상함수는 인터페이스 상속에 더불어 필수 구현의 상속을 강제한다.
*/

#endif