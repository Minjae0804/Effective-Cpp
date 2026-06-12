#if 0

// #26. 변수 정의는 늦출 수 있을 때까지 늦춰라


// 
{
	/*
	* 생성자 또는 소멸자를 끌고 다니는 타입으로 변수를 정의하면 물게 되는 비용이 2가지 있다.
	* 1. 프로그램 제어 흐름이 변수 정의에 닿을 때 생성자가 호출되는 비용
	* 2. 해당 변수가 유효범위를 벗어날 때 소멸자가 호출되는 비용
	* 변수가 정의되었으나 사용되지 않은 경우에도 비용이 부과된다.
	*/
	std::string encryptPassword(const std::string& password) {
		using namespace std;
		string encrypted;
		if (password.length() < MinimumPasswordLength) {
			throw logic_error("Password is too short");
		}
		...;
	}
	/*
	* 해당 함수는 주어진 비밀번호가 충분히 길 경우 해당 비밀번호를 암호화하여 반환한다.
	* 비밀번호가 너무 짧으면 logic_error 타입 예외를 던지도록 구현되어 있다.
	* 
	* encrypted 객체가 이 함수에서 완전히 안 쓰인다고는 할 수 없으나, 예외가 발생하면 해당 변수는 확실하게 쓰이지 않게 된다. 
	* 즉, encryptPassword()가 예외를 던지더라도 encrypted 생성 및 소멸에 대해서는 비용이 발생한다.
	* 따라서 해당 변수의 정의는 꼭 필요해지기 전까지 미루는 게 성능상 낫다.
	*/

	std::string encryptPassword(const std::string& password) {
		using namespace std;
		if (password.length() < MinimumPasswordLength) {
			throw logic_error("Password is too short");
		}
		// 예외 처리 이후로 이동
		string encrypted;
		...;
	}
	/*
	* 그러나 위 코드는 아직 완전히 최적화되지 않았다.
	* 이는 encrypted가 정의될 때 초기화 인자가 전혀 없는 것이 이유다. 즉 기본 생성자가 호출된다.
	* 
	* 객체를 생성한 후 값을 대입하는 것은 효율적이지 못하다.
	*/
	void encrypt(std::string& s);

	std::string encryptedPassword(const std::string& password) {
		...;
		// 기본 생성되는 encrypted에 password를 대입
		std::string encrypted;
		encrypted = password;

		encrypt(encrypted);
		return encrypted;
	}
	/*
	* 이 경우 가장 좋은 방법은 encrypted를 password로 초기화하는 것이다.
	*/
	std::string encryptedPassword(const std::string& password) {
		...;
		// 기본 생성되는 encrypted에 password를 대입
		std::string encrypted(password);

		encrypt(encrypted);
		return encrypted;
	}
	/*
	* 어떤 변수를 사용해야 할 때가 오기 전까지 그 변수의 정의는 물론, 초기화 인자를 얻기 전까지 정의를 늦출 수 있는지도 고려해야 한다.
	* 이러면 성능 최적화에 더불어 해당 변수의 의미가 명확한 상태에서 초기화가 발생하므로 문서화에도 도움이 된다.
	* 
	* 그러면 루프는?
	*/
	
	// 1. 루프 외부에 정의
	Widget w;
	for (int i = 0; i < n; ++i) { w = i에 따라 달라지는 값; }

	// 2. 루프 내부에 정의
	for (int i = 0; i < n; ++i) { Widget w(i에 따라 달라지는 값); ...; }
	/*
	* 위의 경우 각 연산에 들어가는 비용은 다음과 같다.
	* 1.의 경우 생성자 1회, 소멸자 1회, 대입 n회 발생한다.
	* 2.의 경우 생성자 n회, 소멸자 n회 발생한다.
	* 
	* 클래스 중에는 대입에 들어가는 비용이 생성자-소멸자 쌍보다 더 적은 경우가 있는데, 이 경우 1.이 효율이 훨씬 더 좋다.
	* 이 차이는 n이 커지면 커질수록 더욱 커진다.
	* 반면 그렇지 않을 경우에는 2.가 더 효율적이다.
	* 
	* 참고로 1.을 쓰면 w라는 이름을 볼 수 있는 스코프가 2.보다 넓어지므로 프로그램의 이해도와 유지보수성이 더 안 좋아질 수도 있다.
	* 그러므로 다음과 같은 조건을 둘 다 만족할 경우 바로 2.를 사용하는 것이 좋다.
	* 1. 대입이 생성자-소멸자 쌍보다 비용이 덜 드는 경우
	* 2. 전체 코드에서 수행 성능에 민감한 부분을 건드리는 것이 아닌 경우
	*/
}
/*
* 결론.
* 1. 변수 정의는 늦출 수 있을 때까지 늦춰라. 프로그램이 더 깔끔해지고 효율도 좋아진다.
*/

#endif