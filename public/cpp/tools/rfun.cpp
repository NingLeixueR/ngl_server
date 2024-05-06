#include "rfun.h"

#include <iostream>

namespace ngl
{
	class sb1
	{
	public:
		template <typename T>
		static void func()
		{
			std::cout << dtype_name(T) << std::endl;
		}
	};
	class testrfun
	{
	public:
		void handle1(const int athread, const int& avalue)
		{
			std::cout << avalue << std::endl;
		}

		void handle2(const sb1& athread)
		{
			std::cout << std::endl;
		}
	};

	void test_rfun()
	{
		ngl::rfuns lrfuns;
		lrfuns.registers(&testrfun::handle1, "void handle(const int athread, const int& avalue)");
		lrfuns.registers(&testrfun::handle2, "void handle(const sb1& athread)");
		testrfun la;
		int lvint = 1989;
		lrfuns.handle_switch<testrfun, const int, const int&>(&la, 1, lvint);
		sb1 lsb1;
		lrfuns.handle_switch<testrfun, const sb1&>(&la, lsb1);
	}
}// namespace ngl