#if 0

// #45. 호환되는 모든 타입을 받아들이는 데에는 멤버 함수 템플릿을 사용하라


// 
{
	/*
	* 스마트 포인터는 일반 포인터처럼 동작하지만, 자원의 자동 해제 등 일반 포인터에는 없는 편리기능을 지니는 객체를 의미한다.
	* 대표적으로 RAII 객체가 있고, 이터레이터처럼 ++로 다음 요소로 이동하는 객체도 스마트 포인터로 판정한다.
	* 
	* 포인터에도 스마트 포인터로 대신할 수 없는 특징이 있는데, 그 중 하나는 암시적 변환을 지원한다는 것이다.
	* 파생 클래스 포인터는 암시적으로 기본 클래스 포인터로 변환되고, 비상수 객체에 대한 포인터는 상수객체에 대한 포인터로의 암시적 변환이 가능한 등이다.
	* 예로 아래와 같은 세 수준으로 구성된 클래스 계통이 주어질 경우, 몇 가지의 타입 변환이 가능하다.
	*/
	class Top{...};
	class Middle : public Top{...};
	class Bottom : public Middle{...};

	Top* pt1 = new Middle;
	Top* pt2 = new Bottom;
	const Top* pct2 = pt1;
	/*
	* 이런 식의 캐스팅을 사용자 정의 스마트 포인터를 사용하여 흉내내기 위해서는 매우 까다롭다.
	*/
	template<typename T>
	class SmartPtr {
	public:
		explicit SmartPtr(T* realPtr);
		...;
	};
	SmartPtr<Top> pt1 = SmartPtr<Middle>(new Middle);
	SmartPtr<Top> pt2 = SmartPtr<Bottom>(new Bottom);
	SmartPtr<const Top> = pct2 = pt1;
	/*
	* 동일 템플릿으로 생성된 다른 인스턴스간에는 어떠한 관계도 존재하지 않으므로, 컴파일러의 눈에 비치는 SmartPtr<Middle>과 SmartPtr<Top>은 완전 별개의 클래스이다.
	* 즉, 클래스 간 어떠한 변환이 가능하게 하고 싶다면 변환이 되도록 직접 프로그램을 만들어야 한다는 것이다.
	* 
	* 생성자 함수를 직접 만드는 것으로는 우리에게 필요한 모든 생성자를 만들어낼 수 없다.
	* 위 클래스 계통에서는 SmartPtr<Middle>이나 Smartptr<Bottom>으로부터 SmartPtr<Top>을 생성할 수 있으나 나중에 클래스 계통이 더 확장되는 경우 확장된 다른 스마트 포인터 타입으로부터 SmartPtr<Top> 객체를 만들 방법도 마련되어야 한다.
	* 
	* 중요한 것은 이후 클래스 상속계통 확장에서 정의될 생성자인데, 앞으로 생성될 상속계통 클래스에서 생성자가 잘 동작하게 하기 위해서는 템플릿을 쓰면 된다.
	* 생성자를 만들어내는 생성자를 사용하는 것이다.
	* 이 생성자 템플릿은 멤버 함수 템플릿(Member Function Template, 멤버 템플릿이라고도 한다.)의 예이다. 이는 어떠한 클래스의 멤버 함수를 찍어내는 템플릿을 의미한다.
	*/
	template<typename T>
	class SmartPtr {
	public:
		template<typename U>
		SmartPtr(const SmartPtr<U>& other);
		...;
	};
	/*
	* 위 코드는 모든 T 타입 및 U 타입에 대해 SmartPtr<T> 객체가 SmartPtr<U> 로부터 생성될 수 있다는 의미이다. 
	* 이는 SmartPtr<U>의 참조자를 매개변수로 받아들이는 생성자가 SmartPtr<T> 내부에 들어있기 때문이다.
	* 이러한 꼴의 생성자를 일반화 복사 생성자(Generalize Copy Constructor)라고 한다.
	* 
	* 위 예제의 일반화 복사 생성자는 explicit으로 선언되지 않았다.
	* 이는 기본제공 포인터가 포인터 타입 간 타입 변환이 암시적으로 이루어지기 때문에 스마트 포인터도 이를 따르게 하는 것이다.
	* 
	* SmartPtr에 선언된 일반화 복사 생성자는 원하는 것보다 더 많은 것을 할 수 있다.
	* 클래스 계통 내에서는 계통상 현재 클래스 이하 클래스로의 타입 변환이 가능해지고, 기본타입을 타입 파라미터로 받는 경우 다른 기본타입 템플릿 클래스로도 타입 변환이 가능해진다.
	* 이는 의도하지 않은 동작이므로 상당히 해롭다.
	* 
	* 때문에 이 일반화 복사 생성자의 기능을 제한해야 한다.
	* auto_ptr 또는 shared_ptr에서 사용하는 방법을 그대로 따라서 SmartPtr도 get()을 통해 해당 스마트 포인터 객체에 존재하는 기본제공 포인터의 사본을 반환한다고 가정하면 된다.
	*/
	template<typename T>
	class SmartPtr {
		T* heldPtr;
	public:
		template<typename U>
		SmartPtr(const SmartPtr<U>& other) : heldPtr(other.get()) { ... }
		T* get() const { return heldPtr; }
		...;
	};
	/*
	* 이 예제에서는 멤버 초기화 리스트를 사용하여 SmartPtr<T>의 데이터 멤버인 T* 타입 포인터를 SmartPtr<U>에 들어 있는 U* 타입 포인터로 초기화했다.
	* 이러면 U*에서 T*로 진행되는 암시적 변환이 가능할 때에만 컴파일 에러가 발생하지 않는다.
	* 즉, SmartPtr<T>의 일반화 복사 생성자는 호환되는 타입의 매개변수를 받을 때에만 컴파일되게 된다.
	*/

	/*
	* 멤버 함수 템플릿의 활용은 비단 생성자에만 국한되는 것이 아니다. 생성자 외 멤버 함수에 이를 활용하는 가장 대표적인 예시로 대입 연산이 있다.
	* 예로 Shared_ptr의 경우 호환되는 모든 기본제공 포인터나 shared_ptr, auto_ptr, weak_ptr 객체에서 생성자 호출이 가능한데다가, 이들 중 weak_ptr을 제외한 나머지를 대입 연산에 사용할 수 있도록 되어 있다.
	*/

	/*
	* 만약 어떠한 shared_ptr 객체가 자신과 동일한 타입의 다른 shared_ptr 객체로부터 생성될 경우 컴파일러는 shared_ptr의 복사 생성자를 생성할까, 아니면 일반화 복사 생성자 템플릿을 인스턴스화할까?
	* 
	* C++은 복사 생성자가 필요한 때 프로그래머가 직접 선언하지 않은 경우 컴파일러가 직접 생성한다.
	* 그러나 일반화 복사 생성자를 어떠한 클래스 내부에 선언하면 컴파일러 자체의 복사 생성자 생성에 대해 영향을 주는 요소가 아니다.
	* 즉 일반화 복사 생성자는 보통의 복사 생성자와 다르며, 보통의 복사 생성자와는 다른 것이다.
	* 
	* 따라서 어떠한 클래스의 복사 생성을 전부 사용자가 정의하고 싶을 경우 일반화 복사 생성자는 물론 보통의 복사 생성자도 직접 선언해야 한다.
	* 물론 대입 연산자도 동일하다.
	*/
	template<class T> class shared_ptr {
	public:
		shared_ptr(shared_ptr const& r);

		template<class Y>
		shared_ptr(shared_ptr<Y> const& r);

		shared_ptr& operator=(shared_ptr const& r);

		template<class Y>
		shared_ptr& operator=(shared_ptr<Y> const& r);
		...;
	};
}
/*
* 결론.
* 1. 호환되는 모든 타입을 받아들이는 멤버 함수를 만들기 위해서는 멤버 함수 템플릿을 사용하자.
* 2. 일반화된 복사 생성 연산과 일반화된 대입 연산을 위해 멤버 템플릿을 선언했다고 해도 보통의 복사 생성자와 복사 대입 연산자는 직접 선언해야 한다.
*/

#endif