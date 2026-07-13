#if 0

// #51. new 및 delete를 작성할 때 따라야 할 기존 관례를 잘 알아 두자.


// 
{
	/*
	* 기존 관례에 맞는 operator new를 구현하기 위해서는 네 가지의 조건을 만족해야 한다.
	* 1. 반환값이 제대로 되어 있어야 한다.
	* 2. 가용 메모리가 부족한 경우 new 핸들러 함수를 호출해야 한다.
	* 3. 크기가 없는 메모리 요청에 대한 대비책을 갖추어야 한다.
	* 4. 실수로 기본 형태의 new가 가려지지 않게 해야 한다.
	* 
	* operator new의 반환값 부분은, 그냥 요청된 메모리를 마련 가능할 경우 해당 메모리에 대한 포인터를 반환해주면 된다.
	* 다만 구현은 좀 어렵다. operator new는 메모리 할당이 실패할 때마다 new 핸들러를 호출하는 식으로, 메모리 할당을 2회 이상 시도하기 때문이다.
	* 즉 어떠한 메모리를 해제하는 데에 실마리가 될 수 있는 동작을 new 핸들러에서 가능한 것으로 본다는 것이다.
	* operator new가 예외를 던지는 경우는 오직 new 핸들러에 대한 포인터가 널인 경우 뿐이다.
	* 그리고 new는 0바이트 할당을 요구받아도 적법한 포인터를 반환해야 한다.
	* 
	* 요구사항을 정리하여 비멤버 버전 operator new를 슈도코드로 만들어보면 다음과 같다.
	*/
	void* operator new(std::size_t size) throw(std::bad_alloc) {
		using namespace std;
		// 0바이트 요청 시 1바이트로 간주
		if (size == 0) size = 1;

		while (1) {
			size 바이트 할당 시도;
			if (할당 성공) return 할당된 메모리에 대한 포인터;

			new_handler globalHandler = set_new_handler(0);
			set_new_handler(globalHandler);

			if (globalHandler) (*globalHandler)();
			else throw std::bad_alloc;
		}
	}
	/*
	* set_new_handler를 호출하고 그 반환을 가져오는 방법밖에 없으므로 일단 위와 같이 할 수밖에 없었다만, 이건 실무에서도 효과적인 코드이다.
	* 최소한, 단일 스레드 동작일 경우 이건 유효하다.
	* 물론 다중일 경우스레드 안전성이 보장되어야 하므로 스레드 잠금을 걸어야 한다.
	* 
	* operator new 내부 무한 루프는 메모리 할당이 성공하든, new 핸들러가 new 핸들러로서의 동작을 실행하든 둘 중 하나는 되어야 탈출 가능하다.
	* 참고로 new 핸들러로서의 동작은 다음과 같다.
	* 
	* 1. 사용할 수 있는 메모리를 더 많이 확보한다.
	* 2. 다른 new 핸들러를 설치한다.
	* 3. new 핸들러의 설치를 제거한다.
	* 4. 예외를 던진다.
	* 5. 복귀하지 않는다.
	* 
	* operator new가 멤버일 경우, 그것은 파생으로 상속되는 함수이다.
	* 위의 경우 할당 시도 메모리 크기가 size바이트로 되어 있는데, 특정 클래스 전용 할당자를 생성하여 할당 효율을 최적화하기 위해 사용자 정의 메모리 관리자를 작성할 수 있다.
	* 이 때 특정 클래스란 해당 클래스만을 가리킬 뿐이며, 그것 혹은 그것의 파생을 통칭하는 것은 아니다.
	* 그러나 상속 때문에 파생의 객체를 담을 메모리를 할당하는 데에 기본의 operator new가 호출되는 일이 발생한다.
	*/
	class Base {
	public:
		static void* operator new(std::size_t size) throw(std::bad_alloc);
		...;
	};
	class Derived : public Base { ... };
	Derived* p = new Derived;
	/*
	* 만약 Base 클래스 전용 new가 이런 상황에 대해 특정 조치를 하도록 설계되지 않았을 경우, 전체 설계를 변경하지 않고 사용 가능한 가장 좋은 해결 방법은 틀린 메모리 크기가 들어왔음을 함수 시작부에서 확인한 후 표준 operator new를 호출하는 쪽으로 가게 하는 것이다.
	*/
	void* operator new(std::size_t size) throw(std::bad_alloc) {
		if (size != sizeof(Base)) return ::operator new(size);
	}
	/*
	* 여기에는 0바이트 점검이 없는데, 사실 sizeof(Base)와 size를 비교하는 부분에 그것이 합쳐져 있다.
	* C++에는 모든 독립구조(freestanding) 객체는 반드시 크기가 0을 넘어야 한다는 이상한 금기가 있으므로, 이로 인해 sizeof(Base)가 0이 될 일은 없다.
	* 즉 size가 0일 경우 if가 거짓이 되므로 ::operator new가 리턴되어 호출된다.
	* 
	* 만약 배열에 대한 메모리 할당을 클래스 전용 방식으로 하고 싶을 경우, operator new 대신 operator new[]를 구현하면 된다.
	* 이걸 직접 구현하는 건 선택이간 하다만, operator new[] 내부에서 할 일은 단순히 원시 메모리 덩어리를 할당하는 것 이외에는 존재하지 않는다.
	* 이 시점에서는 배열 메모리에 아직 생기지도 않은 클래스 객체에 대해 아무것도 할 수 없다. 사실 배열 내부에 몇 개의 객체가 들어갈 수 있는지도 계산하지 못한다.
	* 그 이유는 다음과 같다.
	* 
	* 1. 객체 하나가 어느 정도의 크기를 가질지 유추할 수 없다.
	*	 상속으로 인해 파생의 객체를 할당하는 데에 operator new[]가 호출될 수 있다. 
	*	 그리고 파생의 객체는 대체적으로 기본의 객체보다 더 크다. 때문에 Base::operator new[] 내부에서도 배열에 들어가는 객체 하나의 크기가 sizeof(Base)라는 가정이 불가능하다.
	*	 즉, Base::operator new[]에서 할당한 배열 메모리에 들어가는 객체의 갯수를 (요구 바이트 / sizeof(Base))로 계산이 불가하다.
	* 
	* 2. operator new[]로 넘어가는 size_t형 인자는 객체들을 담기에 딱 맞는 메모리 양보다 더 많이 설정되어 있을 수 있다.
	* 
	* operator new를 작성할 경우 지켜야 하는 관례는 이것으로 끝이다.
	* operator delete는 new보다 더 간단한데, C++은 널 포인터에 대한 delete 적용이 항상 안전하도록 보장한다는 사실만 적용하면 된다.
	*/
	void operator delete(void* rawMemory) throw() {
		// 널 포인터가 delete되는 경우 아무것도 하지 않는다.
		if (rawMemory == 0) return;
		rawMemory가 가리키는 메모리 해제;
	}
	/*
	* operator delete의 클래스 전용 버전도 단순하기는 매한가지이다. 삭제될 메모리의 크기를 점검하는 코드를 넣어줘야 한다는 점만 빼면.
	* 클래스 전용 operator new가 틀린 크기의 메모리 요청을 ::operator new로 넘기도록 구현되었다면, delete도 틀린 크기로 할당된 메모리 삭제 요청을 ::operator delete로 넘기게 구현하면 된다.
	*/
	class Base {
	public:
		static void* operator new(std::size_t size) throw(std::bad_alloc);
		...;
	};

	void Base::operator delete(void* rawMemory, std::size_t size) throw() {
		if (rawMemory == 0) return;

		if (size != sizeof(Base)) {
			::operator delete(rawMemory);
			return;
		}

		rawMemory가 가리키는 메모리 해제;
		return;
	}
	/*
	* 참고로 가상 소멸자가 없는 기본 클래스로부터 파생된 클래스의 객체를 삭제하려 할 경우 operator delete로 넘기는 size_t 값이 이상할 수 있다.
	* 가상 소멸자는 반드시 빼먹지 말자.
	*/
}
/*
* 결론.
* 1. 관례적으로 operator new는 메모리 할당을 반복하여 시도하는 무한 루프를 가져야 하고, 메모리 할당 요구를 만족시킬 수 없을 때 new 핸들러를 호출해야 하며 0바이트 요청에 대한 대책도 존재해야 한다.
*	 클래스 전용의 경우 자신이 할당하기로 예정된 크기보다 더 큰, 즉 틀린 메모리 블록에 대한 요구 또한 처리해야 한다.
* 2. operator delete는 널 포인터가 들어왔을 때 아무것도 하면 안 된다. 클래스 전용 버전의 경우 예정 크기보다 더 큰 블록을 처리할 수 있어야 한다.
*/

#endif