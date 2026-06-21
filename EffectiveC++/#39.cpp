#if 0

// #39. private 상속은 심사숙고하라


// 
{
	class Person{ ... };

	// private 상속
	class Student : private Person { ... };

	void eat(const Person& p);
	void study(const Student& s);

	Person p;
	Student s;

	eat(p);
	// 에러
	eat(s);
	/*
	* private 상속은 public 상속과 달리 파생 클래스를 기본 클래스로 변환하지 않는다.
	* 또한, 기본으로부터 물려받은 멤버는 파생에서 모두 private가 된다. 원래 접근지정자가 무엇이었든 상관없이.
	* 
	* 즉, private 상속의 의미는 is-implemented-in-terms-of이다.
	* 기본 클래스 B로부터 private 상속을 통해 D를 파생시킨 것은 B 클래스에서 쓸 수 있는 기능들 몇 개를 활용할 목적으로 한 행동이다.
	* B와 D 간 어떠한 개념적 관계가 있어서 한 행동이 아니라는 것이다.
	* 
	* 즉, private 상속은 그 자체로 구현 기법 중 하나다. 상속으로 구현만 물려받을 수 있고, 인터페이스는 받지 못한다.
	* D가 B로부터 private 상속을 받으면, 이것은 그냥 D가 B를 사용하여 구현되는 것이다.
	* private 상속은 소프트웨어 설계에서는 아무 의미도 가지지 않고, 소프트웨어 구현 중에만 의미를 가진다.
	*/

	/*
	* 그런데, private 상속의 의미가 is-implemented-in-terms-of라는 사실은 다소 헷갈리는 부분이 있다.
	* 객체 합성도 is-implemented-in-terms-of이기 때문이다. 그러면, 어떠한 선택 기준을 사용해야 할까?
	* 일단 가능한 한 객체 합성을 사용하고, 꼭 해야 할 경우 private 상속을 사용하면 된다.
	* 
	* private 상속이 필수적인 경우가 있는데, 비공개 멤버를 접근할 때나 가상 함수를 재정의할 때가 이 경우에 속한다.
	*/
	
	/*
	* 일례로 Widget 객체를 사용하는 응용프로그램을 하나 개발하고 있다고 가정하자.
	* Widget 객체가 어떻게 사용되는지를 더 이해해야 할 필요가 있어, 각 멤버함수가 몇 번이나 호출되는지를 추적하기 위해 해당 클래스를 직접 수정하기로 한다.
	* 이를 위해 아래의 타이머 클래스를 사용한다.
	*/
	class Timer {
	public:
		explicit Timer(int tickFrequency);
		virtual void onTick() const;
		...
	};
	/*
	* Timer 객체는 반복적으로 시간을 경과시킬 주기를 우리가 정해줄 수 있고, 일정 시간이 경과할 때마다 가상함수를 호출하도록 되어 있다.
	* 이 가상 함수를 재정의하면 Widget 객체의 현재 상태를 점검할 수 있을 것이다.
	* 
	* 이렇게 하기 위해서는 Widget 클래스에서 Timer의 가상함수를 재정의할 수 있어야 하므로 해당 클래스에서 상속을 받아야 한다.
	* 그러나 지금 경우에서 public 상속은 적절하지 않다. Widget이 Timer의 일종은 아니기 때문이다.
	* 게다가 Widget 객체의 사용자는 해당 객체를 사용하여 onTick()을 호출하면 안 된다. 이 함수는 개념적으로 Widget 객체의 일종으로 볼 수 없기 때문이다.
	* 
	* 때문에 private 상속을 쓴다.
	*/
	class Widget : private Timer {
		virtual void onTick() const;
		...;
	};
	/*
	* 이것만 놓고 보면 흠잡을 곳 없는 설계이나, 여기에서 꼭 private 상속을 써야 하느냐에 대해서는 생각할 필요가 있다. 
	* 대신 객체 합성을 사용한다면 충분히 그렇게 할 수 있기 때문이다.
	*/
	class Widget {
		class WidgetTimer : public Timer {
		public:
			virtual void onTick() const;
			...;
		};
		WidgetTimer timer;
	};
	/*
	* 현실적으로는 private 상속 대신 public 상속에 객체 합성하는 식이 더 자주 사용되긴 한다.
	* 그 이유는 두 가지이다.
	* 
	* 1. Widget 클래스를 설계하는 데에 있어 파생은 가능케 하되 파생에서 onTick()을 재정의할 수 없도록 설계상 제어하고 싶을 때 유용하다.
	*    Widget을 Timer에서 상속시킨 구조라면 이게 불가능하다. 심지어 상속을 private로 해도 안 된다.
	*    그러나, Timer로부터 상속을 받은 클래스가 Widget 클래스의 private에 존재하면 Widget의 파생은 WidgetTimer에 접근할 수 없다.
	* 
	* 2. Widget의 컴파일 의존성을 최소화하고 싶을 때 유용하다. 
	*    Widget이 Timer에서 파생된 경우 Widget이 컴파일될 때 Timer의 정의도 끌어올 수 있어야 하므로 Widget 정의부에서 Timer.h 같은 헤더를 include해야 할 수도 있다.
	*    지금의 설계의 경우 Widget 객체에 대한 포인터만 지니도록 해 두면 WidgetTimer 클래스를 선언하는 것만으로고 컴파일 의존성을 회피할 수 있다.
	*/

	/*
	* 앞에서 private 상속이 필수인 경우에 대해 기본 클래스의 private에 파생이 접근해야 하거나 가상 함수를 1개 이상 재정의해야 할 경우가 주된 용도라고 했다.
	* 여기에 더해, 객체 합성보다 private 상속을 선호할 수밖에 없도록 하는 만약의 경우도 있다. 
	* 
	* 비정적 데이터 멤버가 없는 클래스, 즉 가상 함수도 하나도 없어야 하며 가상 기본 클래스도 없는 클래스를 공백 클래스(empty class)라고 한다.
	* 이러한 클래스는 상식적으로 차지하는 메모리 공간이 존재하면 안 된다.
	* 하지만 기술적인 문제로 인해, 독립 구조(freestanding)의 객체는 반드시 크기가 1 이상이어야 한다는 조건이 있다. 때문에,
	*/
	class Empty();
	class HoldAnInt {
		int x;
		Empty e;
	};
	/*
	* sizeof(HoldsAnInt) > sizeof(int)가 된다. 즉, Empty 타입 데이터 멤버가 메모리 공간을 가진다.
	* 독립 구조 객체의 크기가 0인 경우를 방지하기 위해 컴파일러가 이러한 공백 객체에 char 하나를 끼워넣기 때문이다.
	* 그러나 바이트 정렬이 필요하다고 판단될 경우 컴파일러는 HoldsAnInt 등의 클래스에 바이트 패딩 과정을 추가할 수도 있어서, HoldsAnInt 객체의 크기는 char 하나의 크기보다 커진다.
	* 
	* 그런데, 이 제약은 파생 클래스 객체의 기본 클래스 부분에는 적용되지 않는다. 이 때의 기본 클래스 부분은 독립구조가 아니기 때문이다.
	* Empty 타입 객체를 데이터 멤버로 두자 않고 Empty로부터 상속을 시킬 경우
	*/
	class HoldsAnInt : private Empty {
		int x;
	};
	/*
	* sizeof(HoldsAnInt) == sizeof(int)가 참이 된다.
	* 이 공간 절약 기법은 공백 기본 클래스 최적화(Empty Base Optimization)이라고 알려져 있다.
	* 참고로 EBO는 일반적으로 단일상속 하에서만 적용된다.
	* 
	* 실무적인 입장에서 공백 클래스는 진짜로 비어있는 것은 아니다. 
	* 비정적 데이터는 가지지 않지만 typedef, enum, 정적 데이터 멤버, 비가상 함수까지 지니는 경우가 있다.
	* STL에는 이러한 클래스가 많은데, unary_function과 binary_function이 그 예이다.
	* 이들은 사용자 정의 함수 객체를 만들 때 상속시킬 기본 클래스로 자주 사용된다.
	*/

	/*
	* 하지만 물론 아무것도 없는 클래스를 사용하는 건 드문 일이므로, 가능한 경우 객체 합성을 사용하는 게 더 낫다.
	* 
	* private 상속이 적법한 설계전략일 가능성이 가장 높은 경우는 아래와 같다.
	* 1. 아무리 봐도 is-a 관계로 이어질 것 같지 않은 두 클래스를 사용해야 하며,
	* 2. 이 둘 사이에서 한 쪽 클래스가 다른 쪽 클래스의 protected 멤버에 접근해야 하거나 다른 쪽 클래스의 가상함수를 재정의할 필요가 있을 경우.
	* 
	* 물론 이것도 객체 합성으로 해결할 수 있다. 물론 좀 복잡한 구현이 되겠지만.
	* 그러므로 일단 private 상속을 섣불리 쓰지 말고 다른 방안을 충분히 모색한 뒤, private 상속이 최선이라고 판단될 때 사용하라.
	*/


}
/*
* 결론.
* 1. private 상속은 is-implemented-in-terms-of이다. 
*    대개 객체 합성과 비교하여 사용되는 경우가 많지는 않으나 파생에서 기본 클래스의 protected 멤버에 접근해야 하거나 혹은 상속받은 가상 함수를 재정의해야 할 경우 의미가 있다.
* 2. 객체 합성과 달리 private 상속은 공백 기본 클래스 최적화(EBO)를 활성화할 수 있다.
*/

#endif