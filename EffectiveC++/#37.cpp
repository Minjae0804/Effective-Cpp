#if 0

// #37. 어떤 함수에 대해서도 상속받은 기본 매개변수값은 절대 재정의하지 마라


// 
{
	/*
	* C++에서 상속받을 수 있는 함수의 종류는 아무리 발버둥쳐도 두 가지, 즉 가상 함수와 비가상 함수뿐이다.
	* 그런데, 이들 중 비가상 함수는 언제라도 재정의하면 안 되는 함수이므로 지금부터 하는 이야기는 기본 매개변수값을 지니는 가상함수를 상속하는 경우라고 해도 별 문제가 없을 것이다.
	* 
	* 이번 항목의 전제는, 가상함수는 동적 바인딩되나 기본 매개변수는 정적 바인딩된다는 것이다.
	*/

	/*
	* 객체의 정적 타입은 프로그램 소스 내에 선언문을 통해 해당 객체가 지니게 되는 타입이다.
	*/
	class Shape {
		enum shapeColor { Red, Green, Blue };
		virtual void draw(ShapeColor color = Red) const = 0;
		...;
	};

	class Rectangle : public Shape {
	public:
		// 기본 매개변수 변경
		virtual void draw(ShapeColor color = Green) const;
		...;
	};

	class Circle : public Shape {
	public:
		virtual void draw(ShapeColor color) const;
		...;
	};

	// 정적 타입 = Shape*
	Shape* ps;
	Shape pr = new Rectangle;
	Shape pc = new Circle;
	/*
	* 여기서 ps, pr, pc는 모두 Shape에 대한 포인터로 선언되어 있으므로 각각의 정적 타입도 이 타입이다.
	* 단, 그렇다고 하여 이들이 진짜로 가리키는 대상이 달라지는 것은 아니다. 그저 정적 타입이 Shape*일 뿐이다.
	*/

	/*
	* 객체의 동적 타입은 현재 그 객체가 진짜로 무엇이냐에 따라 결정되는 타입이다. 
	* 다시 말해, 해당 객체가 어떻게 동작할 것인가를 가리키는 타입이 동적 타입이다.
	* 
	* 위의 경우를 보면 pr은 Rectangle*, pc는 Circle*이다.
	* ps는 아직 아무 객체도 참조하고 있지 않으므로 동적 타입이 없다.
	* 
	* 동적 타입은 프로그램 실행 도중 변경될 수 있다.
	* 대개 대입문을 통해 변경된다.
	*/
	ps = pc;
	ps = pr;
	/*
	* 가상 함수는 동적 바인딩된다. 즉, 가상 함수의 호출이 일어난 객체의 동적 타입에 따라 어떤 가상함수가 호출될지 결정된다.
	*/
	pc->draw(shape::Red);
	pr->draw(Shape::Red);	
	/*
	* 문제는 기본 매개변수 값이 설정된 가상 함수의 경우이다.
	* 가상함수는 동적으로 바인딩되어 있으나 기본 매개변수는 정적 바인딩되어 있기 때문이다.
	* 즉 파생 클래스에 정의된 가상 함수를 호출하면서 기본 클래스에 정의된 기본 매개변수 값을 사용하게 된다는 이야기이다.
	*/

	// Rectangle::draw(Shape::Red)를 호출
	pr->draw();

	/*
	* 이 경우 pr의 동적 타입이 Rectangle*이므로 호출되는 가상 함수는 Rectangle의 그것이나, pr의 정적 타입은 Shape*이므로 호출되는 가상함수의 기본 매개변수값을 Shape에서 가져오기 된다.
	* 
	* 이렇게 동작하는 데에는 런타임 효율의 문제가 있다. 
	* 만약 함수의 기본 매개변수가 동적 바인딩될 경우 프로그램 실행 중 가상 함수의 기본 매개변수값을 결정할 방법을 컴파일러 측에서 마련해야 한다.
	* 현재의 매커니즘은 속도 유지 및 구현 간편성에 무게를 더 두어 결정한 결과이다.
	* 
	* 그런데, 기본 클래스 및 파생 클래스의 사용자에게 기본 매개변수 값을 똑같이 제공해보려고 한다면 어떻게 될까?
	*/
	class Shape {
	public:
		enum ShapeColor { Red, Green, Blue };
		virtual void draw(ShapeColor color = Red) const = 0;
		...;
	};
	
	class Rectangle : public Shape {
	public:
		virtual void draw(ShapeColor color = Red) const;
		...;
	};
	/*
	* 당연하지만 코드 중복이다. 더해, 의존성까지 걸려 있다. 
	* Shape 클래스에서 기본 매개변수가 변경될 경우 해당 값을 반복하고 있는 파생은 모두 그 값을 변경해야 한다.
	* 
	* 원하는 대로 가상함수가 동작하는 것이 힘글 경우, 다른 설계 방법을 사용하는 것이 더 현명하다.
	* 이 경우 NVI를 쓰면 된다. 이 방법은 파생에서 재정의할 수 있는 가상함수를 private로 두고, 해당 가상함수를 호출하는 public 비가상 함수를 기본 클래스에 두는 것이다. 
	* 여기에서 이 방법을 응용한다면, 비가상이 기본 매개변수를 지정할 수 있다.
	*/
	class Shape {
		virtual void doDraw(ShapeColor color) const = 0;
	public:
		enum shapeColor { Red, Green, Blue };
		virtual void draw(ShapeColor color = Red) const { doDraw(color); }
		...;
	};

	class Rectangle : public Shape {
		virtual void doDraw(ShapeColor color) const;
	public:
		// 기본 매개변수 변경
		virtual void draw(ShapeColor color = Green) const;
		...;
	};
	/*
	* 비가상 함수는 파생에서 재정의되면 안 되므로, 위와 같이 설계하면 함수의 color 매개변수에 대한 기본값을 깔끔하게 Red로 고정시킬 수 있다.
	*/
}
/*
* 결론.
* 1. 상속받은 기본 매개변수는 절대 재정의하면 안 된다. 기본 매개변수는 정적 바인딩되는 반면 가상 함수는 동적 바인딩되기 때문이다.
*/

#endif