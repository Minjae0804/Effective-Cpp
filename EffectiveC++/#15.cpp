#if 0

// #15. 자원 관리 클래스에서 관리되는 자원은 외부에서 접근이 가능하도록 하라


//
{
	/*
	* 말 그대로, 자원 관리 클래스에서 관리되는 자원은 외부에서 접근이 가능하여야 한다.
	* 현장에서 사용되는 다양한 API들은 실제 자원을 직접 참조하도록 만들어져 있기 때문이다.
	* 
	* #13에서는 팩토리 함수의 호출 결과인 포인터를 담기 위해 auto_ptr이나 shared_ptr을 사용하는 방법이 소개되었다.
	* 다음과 같은 코드가 있다고 해 보자.
	*/
	std::tr1::shared_ptr<Investment> pInv(createInvestment());

	// 투자금이 유입된 이후로 경과한 날수
	int daysHeld(const Investment* pi);

	// 에러 발생
	int days = daysHeld(pInv);

	/*
	* 위 코드는 컴파일이 안 된다. daysHeld 함수는 Investment* 타입의 실 포인터를 원하나, pInv는 std::tr1::shared_ptr<Investment> 타입이기 때문이다.
	* 때문에 RAII 클래스의 객체를 그 객체가 감싸고 있는 실 자원으로 변환할 방법이 필요하다. 이러한 목적에 일반적인 방법을 쓴다면 두 가지의 방법이 있다.
	* 바로 명시적 변환(explicit conversion)과 암시적 변환(implicit conversion)이다.
	* 
	* shared_ptr과 auto_ptr은 명시적 변환을 수행하는 get이라는 멤버 함수를 제공한다. 
	* 다시 말해, 이 함수를 사용하면 각 타입으로 생성한 스마트 포인터 객체에 들어있는 실 포인터의 사본을 얻을 수 있다.
	*/
	int days = daysHeld(pInv.get());

	/*
	* 제대로 만들어진 스마트 포인터 클래스 대부분이 그렇듯 shared_ptr과 auto_ptr은 포인터 역참조 연산자도 오버로딩한다. 
	* 따라서 자신이 관리하는 실제 포인터에 대한 암시적 변환도 쉽게 할 수 있다.
	*/
	class Investment {
		...
	public:
		bool isTaxFree() const;
		...
	};

	Investment* createInvestment();
	// shared_ptr 사용
	std::tr1::shared_ptr<Investment> pi1(createInvestment());
	// operator->를 통해 자원 접근
	bool taxable1 = !(pi1->isTaxFree());
	...
	// auto_ptr 사용
	std::auto_ptr<Investment> pi2(createInvestment());
	// operator*를 통해 자원 접근
	bool taxable2 = !((*pi2).isTaxFree());

	/*
	* RAII 객체 내부에 존재하는 실제 자원을 얻어낼 필요가 종종 생기므로, RAII 클래스를 설계하는 경우 암시적 변환 함수를 통해 자원 접근을 매끄럽게 하는 경우도 있다.
	*/
	FontHandle getFont();

	void releaseFont(FontHandle fh);

	class Font {
		// 폰트 자원
		FontHandle f;
	public:
		// 자원 획득. 여기에서는 값에 의한 전달이 수행된다. 자원 해제를 C API로 하기 때문.
		explicit Font(FontHandle fh) : f(fh) {}
		~Font() { releaseFont(f); }
	};

	/*
	* 이 경우 Font 객체를 FontHandle로 변환해야 할 경우도 적지 않을 것임을 예상할 수 있다.
	* Font 클래스에서는 이를 위한 명시적 변환 함수로 get()을 제공할 수 있다.
	*/
	class Font {
		...
	public:
		// 명시적 변환 함수
		FontHandle get() const { return f; }
		...
	};
	/*
	* 이렇게 해 두면 사용은 가능하긴 하겠다만, 사용자는 하부 수준 API를 사용하고 싶을 때마다 get()을 호출하여야 할 것이다.
	*/
	void changeFontSize(FontHandle f, int newSize);
	Font f(getFont());
	int newFontSize;
	...
	// Font에서 FontHandle로 명시적으로 변경 후 넘긴다.
	changeFontSize(f.get(), newFontSize);

	/*
	* 이는 FontHandle로의 암시적 변환 함수를 Font에서 제공하도록 하여 해결할 수 있다.
	*/
	class Font {
		...
	public:
		// 암시적 변환 함수
		operator FontHandle() const { return f; }
		...
	};

	Font f(getFont());
	int newFontSize;
	...
	// f에서 FontHandle로 암시적 변환 수행
	changeFontSize(f, newFontSize);
	/*
	* 다만 암시적 변환은 사용자가 실수할 여지가 많아진다.
	* Font를 사용하려고 했는데 FontHandle로 변환되어버리는 경우가 발생할 수도 있다는 것.
	*/
	Font f1(getFont());
	...
	// f1을 복사하려는 의도로 보이나, 실제로는 f1이 FontHandle로 변환되어 복사된다.
	FontHandle f2 = f1;
	/*
	* 이 경우 Font 객체인 f1이 관리하는 폰트(FontHandle)가 f2를 통해서도 직접 사용할 수 있는 상태가 된다.
	* 이는 댕글링 포인터를 발생시킬 가능성이 높으므로 좋지 않다.
	* 
	* RAII 클래스를 실제 자원으로 변환할 방법으로서 명시적 변환을 제공할 것인지, 아니면 암시적 변환을 허용할 것인지에 대한 결정은 RAII 클래스의 용도와 사용 환경에 따라 달라진다.
	* 잘 설계된 클래스라면, 맞게 쓰기에는 쉽게, 틀리게 쓰기에는 어렵게 만들어져야 할 것이다.
	* 보통은 명시적 변환을 사용하는 쪽이 나은 경우가 많으나, 암시적 변환의 자연스러움이 유용한 경우도 많다.
	*/

	/*
	* RAII 클래스에서 자원 접근 함수를 열어주는 것은 사실 캡슐화 위배이나, RAII 클래스는 애초에 데이터 은닉이 목표가 아니다.
	* 자원 관리가 목적인 클래스이므로, 캡슐화는 선택사항이 된다.
	* 
	* shared_ptr의 경우 참조 카운팅 매커니즘에 필요한 장치들은 모두 캡슐화하나 자신이 관리하는 포인터에 쉽게 접근할 방도도 제공한다.
	* 사용자가 볼 필요 없는 세부사항은 보이지 않되, 사용자가 접근해야 할 데이터는 열어주는 것이다.
	*/
}
/*
* 결론.
* 1. 실제 자원을 직접 접근해야 하는 API도 많으므로, RAII 클래스는 해당 클래스가 관리하는 자원에 접근할 방법을 마련하여야 한다.
* 2. 자원 접근은 명시적 변환 또는 암시적 변환을 통해 가능하다. 안전성만 따지면 명시적 변환이 좋으나, 사용자 편의성 부분에서는 암시적 변환이 괜찮다.
*/

#endif