#if 0

// #32. public 상속 모형은 반드시 is-a를 따르도록 하라


// 
{
	/*
	* public 상속은 is-a를 의미한다.
	* 이를 절대 잊지 말라.
	*/

	/*
	* 클래스 D를 클래스 B로부터 퍼블릭 상속으로 파생시키는 것은, D 타입으로 만들어진 모든 객체는 B 타입의 객체이기도 하다.
	* 그러나 그 반대가 될 수는 없다.
	* 
	* 즉 B는 D보다 더 일반적인 개념이며 D는 B보다 더 특수한 개념을 나타낸다는 것이며, D는 B가 될 수 있다는 것이다.
	* 물론 그 반대는 안 된다.
	*/
	class Person {...};
	class Student : public Person { ... };
	/*
	* 모든 학생은 사람이지만, 모든 사람은 학생이 아니다.
	* 
	* Person 타입의 인자를 기대하는 함수는 Student 객체도 받을 수 있다.
	*/
	Person p;
	Student s;
	void eat(const Person& p);
	void study(const Student& s);

	eat(p);
	eat(s);

	study(s);
	// 에러
	study(p);
	/*
	* 참고로 이건 퍼블릭 상속에서만 통한다. 프라이빗 상속은 안 된다.
	* 
	* 퍼블릭 상속은 즉 is-a라는 건 꽤 직관적이나 이로 인해 판단이 잘못되는 경우도 있다.
	*/
	class Bird{
	public:
		virtual void fly();
		...;
	};

	class Penguin : public Bird {
		...;
	};
	/*
	* 위의 경우에는 느낌이 좀 이상하다. 해당 상속계통에 의하면 펭귄은 날 수 있어야 한다.
	* 이는 Bird 클래스가 새 그 자체가 아니라, 날 수 있는 새를 일반화한 것이기 때문에 그렇다.
	* 
	* 따라서 아래처럼 바꾸면 해결된다.
	*/
	class Bird {...};

	class FlyingBird : public Bird {
	public:
		virtual void fly();
		...;
	};

	class Penguin : public Bird {
		...;
	};
	/*
	* 다만 이것이 어떠한 설계라도 최적이라는 것은 아니다.
	* 어떠한 시스템에서는 굳이 날 수 있는 새와 날지 못하는 새를 구분하지 않아도 될 수 있기 때문이다.
	* 
	* 무릇 최고의 설계는 제작하려는 소프트웨어의 시스템이 기대하는 바에 따라 달라진다.
	* 오늘날에도 그렇고, 미래에도 그렇다.
	* 
	* 잠깐, 이 문제에 대해서 아래와 같은 해결 방안도 고려할 수 있을 것이다.
	*/
	void error(const std::string& msg);
	class Penguin : public Bird {
	public:
		virtual void fly() { error("펭귄은못날아요"); }
	};
	/*
	* 이 코드는 펭귄은 날 수 없음을 말하지 않는다. 펭귄이 날 수 있으나 날려고 하면 에러가 난다는 것을 말하고 있다.
	* 이 차이는 에러의 발생시점에 있다. 전자의 경우 컴파일러가 발생시킬 수 있지만, 후자의 경우 실행 시점에서만 발생할 수 있다.
	* 
	* 그렇다면 이런 건 어떨까?
	*/
	class Bird { ... };
	class Penguin : public Bird {...};

	Penguin p;
	// 에러 발생
	p.fly();
	/*
	* 이건 런타임 에러를 발생시키는 위의 코드와 다른 모습이다.
	* 이 방법을 사용하면 컴파일러는 컴파일 단계에서 에러를 바로 발생시킨다.
	*/

	/*
	* 정사각형 클래스는 직사각형 클래스에서 상속을 받아야 할까?
	* 
	* 학창 시절에는 정사각형이 직사각형의 일종이라 배웠으므로 당연 그래야 할 것처럼 보인다.
	* 실제로는 그렇지 않다는 것이 애석할 뿐이다.
	*/
	class Rectangle {
	public:
		virtual void setHeight(int newHeight);
		virtual void setWidth(int newWidth);
		virtual int height() const;
		virtual int width() const;
	};

	void makeBigger(Rectangle& r) {
		int oldHeight = r.height();
		r.setWidth(r.width + 10);
		// r의 height는 해당 함수 내에서 불변함을 단정 
		assert(r.height == oldHeight);
	}
	/*
	* 여기서 위의 단정문은 실패하지 않는다. height를 절대 건드리지 않으므로.
	*/
	class Square : public Rectangle {...};
	Square s;

	assert(s.width() == s.height());
	makeBigger(s);
	assert(s.width() == s.height());
	/*
	* 이 경우에도 물론, 두 번째 단정문이 실패하면 안 된다.
	* 
	* 그런데 뭔가 이상하다. 
	* 1. makeBigger() 호출 전 s의 세로길이는 가로와 같아야 한다.
	* 2. makeBigger() 함수 실행 중 s의 가로는 변하나 세로는 변하면 안 된다.
	* 3. makeBigger() 복귀 후 s의 세로는 가로와 여전히 같아야 한다.
	* 
	* 
	* 이렇게 논리적 정합성이 터지기 때문에 정사각형은 직사각형이 될 수 없다.
	* 직사각형의 모든 논리는 정사각형에도 동일하게 적용되어야 하지만, 그렇지 못하기 때문이다.
	*/
}
/*
* 결론.
* 1. public 상속은 즉 is-a이다. 여기에서는 기본 클래스에 적용되는 모든 것이 파생 클래스에 적용되어야 한다. 모든 파생 클래스 객체는 기본 클래스 객체의 일종이기 때문이다.
*/

#endif