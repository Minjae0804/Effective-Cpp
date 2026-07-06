#if 0

// #47. 타입에 대한 정보가 필요하다면 특성정보 클래스를 활용하라


// 
{
	/*
	* STL은 기본적으로 컨테이너, 반복자, 알고리즘 템플릿으로 구성되어 있지만 utility라고 불리는 템플릿도 몇 개 들어 있다.
	* 유틸리티 템플릿 중 하나는 advance라 불리는데, 이것은 지정된 반복자를 지정된 거리만큼 이동시키는 역할을 한다.
	*/
	template<typename iterT, typename DistT>
	void advance(iterT& iter, DistT d);

	/*
	* 간단히 보면 advance는 그냥 iter += d만 하면 될 것 같지만 사실 그리 할 수는 없다.
	* += 연산을 지원하는 반복자는 임의 접근 반복자밖에 없기 때문이다.
	* 임의 접근 반복자 이외의 반복자는 ++ 또는 --를 d번 반복 적용하는 것으로 구현해야 한다.
	* 
	* STL 반복자는 각 반복자가 지원하는 연산에 따라 다섯 개의 범주로 나뉜다.
	* 1. 입력 반복자(input iterator)
	*	 전진만 가능하고 한 번에 한 칸씩만 이동하며 자신이 가리키는 위치에서 읽기만 가능하고 읽을 수 있는 횟수가 단 한 번 뿐인 이터레이터.
	*    istream_iterator가 대표적인 입력 반복자이다.
	* 2. 출력 반복자(output iterator)
	*	 입력 반복자와 비슷하나, 출력용인 점이 다르다. 자신이 가리키는 위치에서 쓰기만 가능하며, 쓸 수 있는 횟수가 단 한 번뿐이다.
	*    ostream_iterator가 대표적인 출력 반복자이다.
	* 3. 순방향 반복자(forward iterator)
	*	 입력 및 출력 반복자의 기능에 더해 자신이 가리키고 있는 위치에서 읽기와 쓰기를 동시에, 무제한으로 할 수 있는 이터레이터.
	*	 즉, 해당 범주의 이터레이터는 멀티 패스 알고리즘에 문제 없이 사용 가능하다.
	* 4. 양방향 반복자(bidirectional iterator)
	*	 순방향 반복자에 뒤로 갈 수 있는 기능을 추가한 것.
	* 5. 임의 접근 반복자(random access iterator)
	*	 양방향 반복자에 반복자 산술연산 수행 기능을 추가한 것. 즉, 주어진 반복자를 임의 거리만큼 이동시키는 것이 상수 시간 내로 가능한 이터레이터.
	* 
	* C++ STL에는 위 다섯 가지의 이터레이터 범주 각각을 식별하는 데에 사용하는 태그 구조체가 정의되어 있다.
	* 참고로 구조체간 상속 관계를 보면 is-a 관계임을 알 수 있다.
	*/

	/*
	* 이렇게 반복자들이 각각 가능한 것이 있고 못 하는 것이 있다면, 구현에 신경을 좀 써야 한다.
	* 그 중 한 가지 방법이라면 최소 공통 분모(lowest-common-denominator) 전략이 있다.
	* 이터레이터를 주어진 횟수만큼 반복적으로 증가시키거나 감소시키는 루프를 돌리는 것이다.
	* 
	* 물론, 이 방법은 선형 시간이 걸리게 된다.
	* 상수 시간의 반복자 산술 연산을 사용할 수 있는 임의 접근 이터레이터에서는 확실한 손해가 된다.
	* 때문에, 임의 접근 이터레이터가 주어졌을 경우 상수 시간 연산을 사용할 수 있게 하고 싶다.
	* 
	* 문제는 이것이 가능하려면 주어진 이터레이터가 임의 접근 이터레이터인지를 판단할 수 있어야 한다.
	* 즉, 타입을 알아야 한다.
	* 그럼 어떻게 해야 하느냐, 특성 정보(traits)를 쓰면 된다.
	* 
	* 특성정보는 C++에 미리 정의된 문법구조가 아니며 키워드 또한 아니다.
	* 그냥 C++ 프로그래머들이 따르는 구현 기법이며 관례다.
	* 
	* 특성정보가 되기 위해서는 몇 가지 요구사항이 지켜져야 하는데, 그 중 하나는 원시 타입과 사용자 타입 모두에서 돌아가야 한다는 것이 있다.
	* 예로 advance는 포인터 및 int를 받아서 호출될 경우에도 제대로 동작할 수 있어야 한다.
	* 다만, 이것의 정확한 의미는 "특성정보 기법은 포인터 등 기본제공 타입에 적용이 가능해야 한다"라는 것이다.
	* 
	* 이 사실을 뒤집어 생각해본다면, 어떠한 타입 내 중첩된 정보 등으로는 구현이 안 된다는 말로도 풀이할 수 있다.
	* 결국 어떤 타입의 특성정보는 해당 타입의 외부에 존재하는 것이어야 하겠다.
	*/

	/*
	* 특성정보를 다루는 표준적인 방법은 해당 특성정보를 템플릿 및 해당 템플릿의 1개 이상의 특수화 버전에 집어넣는 것이다.
	* 이터레이터의 경우 표준 라이브러리의 특성정보용 템플릿이 iterator_traits라는 이름으로 준비되어 있다.
	*/
	template<typename iterT>
	struct iterator_traits;
	/*
	* 보다시피 iterator_traits는 구조체 템플릿이다.
	* 이전부터 이어져 온 관례에 따라 특성정보는 항상 구조체로 구현하는 것으로 굳어져 있다.
	* 또한 위처럼 특성정보를 구현하는 데 사용한 구조체를 가리켜 특성정보 클래스라고 부른다.
	* 
	* iterator_traits<iterT> 내부에는 iterT 타입 각각에 대해 iterator_category라는 이름의 typedef 타입이 선언되어 있다.
	* 이렇게 선언된 typedef 타입이 바로 iterT의 이터레이터 범주를 가리킨다.
	* 
	* iterator_traits는 이 이터레이터 범주를 두 부분으로 나누러 구현한다.
	* 첫 번째 부분은 사용자 정의 이터레이터 타입에 대한 구현인데, 사용자 정의 이터레이터 타입으로 하여금 iterator_category라는 이름의 typedef 타입을 내부에 가질 것을 요구사항으로 둔다.
	* 이때 이 typedef 타입은 해당 태그 구조체에 대응되어야 한다. 예로 deque의 이터레이터는 임의 접근 이터레이터이므로 그것에 사용될 수 있는 반복자는 다음과 같은 형태가 될 것이다.
	*/
	template<...>
	class deque {
	public:
		class iterator {
		public:
			typedef random_access_iterator_tag iterator_category;
			...;
		};
		...;
	};
	/*
	* 다른 예로 list의 이터레이터는 양방향 이터레이터이므로 다음과 같이 되어 있다.
	*/
	template<...>
	class list {
	public:
		class iterator {
		public:
			typedef bidirectional_iterator_tag iterator_category;
			...;
		};
		...;
	};
	/*
	* 이 iterator 클래스가 내부에 지닌 중첩 typedef 타입을 똑같이 재생한 것이 iterator_traits이다.
	*/
	template<typename iterT>
	struct iterator_traits {
		typedef typename iterT::iterator_category iterator_category;
		...;
	};
	/*
	* 위의 코드는 확실히 사용자 정의 타입에 대해서는 잘 돌아가지만, 이터레이터의 실제 타입이 포인터인 경우에는 전혀 돌아가지 않는다.
	* 포인터 내부에 typedef 타입이 중첩된다는 것부터가 말도 안 되기 때문이다.
	* iterator_traits 구현의 두 번째 부분은 바로 이터레티터가 포인터인 경우의 처리이다.
	* 
	* 포인터 타입 이터레이터를 지원하기 위해 iterator_traits는 포인터 타입에 대한 부분 템플릿 특수화(partial template specialization) 버전을 제공한다.
	* 사실 포인터의 동작 원리는 임의 접근 이터레이터와 같으므로, iterator_traits가 이런 식으로 지원하는 이터레이터 범주가 바로 임의 접근 반복자이다.
	*/
	template<typename iterT>
	struct iterator_traits<iterT*> {
		typedef random_access_iterator_tag iterator_category;
		...;
	};
	/*
	* 고로 특성정보 클래스의 설계 및 구현 방법은 다음과 같다.
	* 1. 다른 사람이 사용하도록 열어 주고 싶은 타입 관련 정보를 정의한다.
	* 2. 해당 정보를 식별하기 위한 이름을 선택한다.
	* 3. 지원하고자 하는 타입 관련 정보를 담은 템플릿 및 해당 템플릿의 특수화 버전을 제공한다.
	* 
	* 이렇게 iterator_traits가 주어졌으므로, advance의 의사코드는 다음과 같이 잡을 수 있다.
	*/
	template<typename iterT, typename DistT>
	void advance(iterT& iter, DistT& dist) {
		if (typeid(typename std::iterator_traits<iterT>::iterator_category) == typeid(std::random_access_iterator_tag))
			...;
	}
	/*
	* 다만 이 형태는 원하는 형태가 아닌데, iterT의 타입은 컴파일타임에 파악되므로 std::iterator_traits<iterT>::iterator_category의 파악 시점도 컴파일타임이다.
	* if문은 프로그램 실행 중 평가되므로 효율적이지 못하다.
	* 
	* 이를 해결하려면 주어진 타입에 대한 평가를 컴파일타임에 수행하는 조건문이다. 물론, C++에는 매우 효과적인 방법이 존재한다. 바로 오버로딩이다.
	* 어떠한 함수 f를 오버로딩한다는 것은 매개변수 리스트가 다르지만 식별자는 동일한 오버로드 버전 함수를 여러 개 만든다는 것이다.
	* 이 상태에서 f를 호출하면 컴파일러는 넘긴 인자를 보고 호출시 전후관계에 가장 잘 맞는 오버로드 버전을 고른다.
	* 따라서, 컴파일타임에 타입기반 선택되는 조건처리 구문요소가 바로 이거다.
	* 이를 통해 advance가 원하는 대로 동작하게 만들면 된다.
	*/

	template<typename IterT, typename DistT> void doAdvance(IterT& iter, DistT& dist, std::random_access_iterator_tag) { iter += d; }

	template<typename IterT, typename DistT> void doAdvance(IterT& iter, DistT& dist, std::bidirectional_iterator_tag) { 
		if (d >= 0) { while (d--) ++iter; }
		else { while (d++) --iter; }
	}

	template<typename IterT, typename DistT> void doAdvance(IterT& iter, DistT& dist, std::input_iterator_tag) { 
		if (d < 0) throw std::out_of_range("Negative Distance");
		while (d--) ++iter;
	}
	/*
	* forward_iterator_tag는 input_iterator_tag로부터 상속을 받은 자식이므로 input_iterator_tag를 매개변수로 받는 doAdvance는 순방향 반복자도 받을 수 있다.
	* 
	* 보면 advance는 임의 접근 이터레이터 및 양방향 이터레이터에 대해 양수 및 음수 거리를 받을 수 있지만, 이외 이터레이터에 대해 음수 거리 이동을 시도하면 미정의 동작이 발생한다.
	* 때문에 위에서는 예외를 던지도록 설계했다.
	* 
	* 이제 advance가 할 것은 오버로딩된 doAdvance()를 호출하는 것뿐이다.
	*/
	template<typename IterT, typename DistT>
	void advance(IterT& iter, DistT& dist) {
		doAdvance(iter, dist, typename std::iterator_traits<IterT>::iterator_category());
	}
	/*
	* 특성정보 클래스의 사용법을 마지막으로 정리해 보면 아래와 같다.
	* 1. 작업자 역할을 맡을 함수, 또는 함수 템플릿을 특성정보 매개변수를 각각 다르게 하여 오버로딩한다.
	*	 그리고 전달되는 해당 특성정보에 맞추어 각 오버로드 버전을 구현한다.
	* 2. 작업자를 호출하는 주작업자 역할을 맡을 함수 혹은 함수 템플릿을 만든다.
	*	 이 때 특성정보 클래스에서 제공되는 정보를 넘겨서 작업자를 호출하도록 구현한다.
	*/

}
/*
* 결론.
* 1. 특성정보 클래스는 컴파일 도중 사용 가능한 타입 관련 정보를 생성한다. 특성정보 클래스는 템플릿 및 템플릿 특수 버전을 사용하여 구현한다.
* 2. 함수 오버로딩 기법과 결합하여 특성정보 클래스를 사용하면 컴파일 타임에 결정되는 타입별 조건문을 구사할 수 있다.
*/

#endif