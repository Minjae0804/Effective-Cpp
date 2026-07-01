#if 0

// #44. 매개변수에 독립적인 코드는 템플릿으로부터 분리시켜라


// 
{
	/*
	* 아무 생각 없이 템플릿을 사용하면 코드 비대화(code bloat)가 초래될 수 있다. 즉 동일 내용의 코드가 여러 개로 중복되어 이진 파일로 구워지게 된다.
	* 때문에 이진 코드가 템플릿으로 인해 비대해지는 경우를 미연에 방지하는 것이 중요하다.
	* 
	* 우선적으로 사용할 수 있는 방법은 공통성 및 가변성 분석(cmmonality and variability analysis)가 있다.
	* 이 방법은 간단한데, 각 함수나 클래스를 분석하여 공통 부분을 다른 신규 공간에 붙이고 그것을 호출하게 하는 것이다.
	* 
	* 템플릿에서도 당연히 이 방법을 사용할 수 있지만, 템플릿이 아닌 코드에서는 코드 중복이 암시적이라는 특징이 있어 중복을 알아채기가 힘들다.
	* 때문에 많이 보고 알아내야 한다.
	*/
	template<typename T, std::size_t n>
	class SqareMatrix {
	public:
		...;
		void invert();
	};
	/*
	* 이 템플릿은 T라는 타입 매개변수도 받지만 size_t 타입의 비타입 매개변수인 n도 받도록 되어 있다.
	* 비타입 매개변수는 타입 매개변수보다는 덜 흔하지만 적법히 인정되는 매개변수이다.
	*/
	SqareMatrix<double, 5> sm1;
	...;
	sm1.invert();

	SqareMatrix<double, 10> sm2;
	...;
	sm2.invert();
	/*
	* 이 경우 invert()의 사본이 인스턴스화되는데, 만들어지는 사본의 갯수가 두 개이다.
	* 이 둘은 동일 함수일 수가 없다. 한 쪽은 5*5 행렬에 대해 동작할 함수이며, 다른 쪽은 10*10 행렬에 대해 동작할 함수이기 때문이다.
	* 그러나 상수 외 두 함수는 완전히 같다. 이 때문에 코드 비대화가 발생한다.
	*/
	template<typename T>
	class SquareMatrixBase {
	protected:
		void invert(std::size_t matrixSize);
		...;
	};

	template<typename T, std::size_t n>
	class SqareMatrix : private SquareMatrixBase<T> {
		using SquareMatrixBase<T>::invert;
	public:
		...;
		void invert() { this->invert(n); }
	};
	/*
	* 위 코드의 경우 동일 타입 객체를 원소로 지니는 모든 정방행렬은 오직 한 가지의 SquareMatrixBase 클래스를 공유하게 된다.
	* 즉, 동일 원소 타입의 정방행렬이 사용하는 기본 클래스 버전의 invert()도 오직 단일 사본이 된다.
	* 
	* SquareMatrixBase::invert()는 파생에서 코드 복제를 피할 목적으로만 마련한 장치이므로 protected로 선언되었다는 것도 중요하다. 
	* 참고로 해당 함수의 호출에 드는 추가 비용은 없어야 한다.
	* 기본 클래스의 invert()를 호출하도록 구현된 파생의 invert()가 바로 인라인 함수이기 때문이다.
	* 
	* 또한 해당 함수는 this->가 붙어 있는데, 이는 템플릿화된 기본 클래스의 멤버 함수명이 파생에서 가려지는 문제를 피해가기 위한 것이다.
	* 다만 이미 그러한 역할을 하는 using 선언이 존재하므로 불필요한 부분이기는 하다.
	*/
	/*
	* 아직 해결하지 못한 문제가 남아있는데, SquareMatrixBase::invert()는 자신이 상대할 데이터가 무엇인지를 어떻게 알 수 있을까?
	* 정방행렬의 크기야 뭐 매개변수로 받으므로 쉽게 알 수 있으나 진짜 행렬을 저장한 데이터가 어디에 있는지는 어떻게 알 수 있을까.
	* 이 정보를 아는 쪽은 파생밖에 없다. 그러므로 기본에서 역행렬을 생성할 수 있도록 정방행렬의 메모리 주소를 파생이 기본으로 넘기면 되지 않을까?
	* 
	* 이는 행렬 값을 담는 메모리에 대한 포인터를 SquareMatrixBase가 저장하게 하는 것이다. 그리고 이 클래스 템플릿에 행렬 크기도 저장하게 한다.
	* 결과적으로 다음과 같은 설계가 나오게 된다.
	*/
	template<typename T>
	class SquareMatrixBase {
		std::size_t size;
		T* pData;
	protected:
		SquareMatrixBase(std::size_t n, T* pMem) : size(n), pData(pMem) {}
		void setDataPtr(T* ptr) { pData = ptr; }
		void invert(std::size_t matrixSize);
		...;
	};
	/*
	* 이렇게 설계해 두면 메모리 할당 방법의 결정권한이 파생으로 넘어간다.
	* 파생 구현에 따라 SquareMatrix 객체 내부에 데이터 멤버로 직접 넣는 것으로 결정할 수도 있다.
	*/
	template<typename T, std::size_t n>
	class SqareMatrix : private SquareMatrixBase<T> {
		T data[n * n];
	public:
		...;
		SquareMatrix() : SquareMatrixBase<T>(n, data) {}
		void invert() { this->invert(n); }
	};
	/*
	* 다만 이렇게 파생을 만들면 동적 메모리 할당이 필요 없는 객체가 되지만, 객체 자체의 크기가 꽤 커질 수도 있다.
	* 이 방법이 마음에 들지 않는 경우 각 행렬의 데이터를 힙에 둘 수도 있다.
	*/
	template<typename T, std::size_t n>
	class SqareMatrix : private SquareMatrixBase<T> {
		boost::scoped_array<T> pData;
	public:
		...;
		SquareMatrix() : SquareMatrixBase<T>(n, 0), pData(new T[n * n]) { this->setDataPtr(pData.get()); }
		void invert() { this->invert(n); }
	};

	/*
	* 어느 메모리에 데이터를 저장하느냐에 따라 설계가 좀 달라지기는 하다만 코드 비대화 측면에서 매우 중요한 성과를 손에 쥘 수 있다는 점에 대해서는 같다.
	* 중요 성과란 바로 SquareMatrix에 속한 멤버 함수 중 상당수가 기본 클래스 버전을 호출하는 단순 인라인 함수가 될 수 있으며, 동일 타입의 데이터를 원소로 지니는 모든 정방행렬들이 행렬 크기에 상관없이 이 기본 클래스 버전의 사본을 공유한다는 것이다.
	* 이와 동시에 행렬 크기가 다른 squareMatrix 객체는 저마다 고유의 타입을 지닌다는 점도 아주 중요하다.
	*/

	/*
	* 다만 이러한 최적화가 과연 무엇과 트레이드오프되는 결정인지는 고려할 필요가 있다.
	* 일반적으로 코드 분리를 하지 않을 경우 실행 성능 측면에서 더 좋은 결과가 나올 확률이 높다. 
	* 다만, 여러 행렬 크기에 대해 단일한 invert()를 두면 실행 코드의 크기가 작아지는 이점에 이어 프로그램의 작업 세트 크기가 작아지면서 명령어 캐시 내 참조 지역성도 향상된다.
	* 무엇이 더 옳은 결정인지는 일단 두 가지를 전부 적용해 보고 더 나은 것을 선택하는 게 맞다.
	* 
	* 효율에 대해 생각해 볼 부분이 더 있는데, 바로 객체의 크기이다.
	* invert()와 비슷한 크기 독립형 함수를 기본 클래스로 옮기다 보면 각 객체의 전체 크기가 늘어나게 된다.
	* 물론 조금만 고민하면 크기가 안 늘어나게끔 만들 수도 있겠지만, 그렇게 얻는 것이 있으면 잃는 것도 있음을 기억해야 한다.
	*/
}
/*
* 결론.
* 1. 템플릿을 사용하면 비슷비슷한 클래스와 함수가 여러 개 만들어진다. 따라서, 템플릿 매개변수에 종속되지 않은 템플릿 코드는 비대화의 원인이 된다.
* 2. 비타입 템플릿 매개변수로 생기는 코드 비대화의 경우 템플릿 매개변수를 함수 매개변수 혹은 클래스 데이터 멤버로 대체함으로서 비대화를 종종 없앨 수 있다.
* 3. 타입 매개변수로 생기는 코드 비대화의 경우 동일 이진 표현구조를 통해 인스턴스화되는 타입들이 단일 함수 구현을 공유하게 함으로서 비대화를 감소시킬 수 있다.
*/

#endif