#include "notify.h"
#include <iostream>

namespace ngl
{
	class ntest
	{
	public:
		static ntest& getInstance()
		{
			static ntest ltemp;
			return ltemp;
		}

		bool kkk1(notify_parm_test* anotify)
		{
			std::cout << "kkk1:" << anotify->m_test << std::endl;
			return true;
		}
		bool kkk2(notify_parm_test* anotify)
		{
			std::cout << "kkk2:" << anotify->m_test << std::endl;
			return true;
		}

		bool kkk3(notify_parm_test1* anotify)
		{
			std::cout << "kkk3:" << anotify->m_test << std::endl;
			return true;
		}

		bool kkk4(notify_parm_test1* anotify)
		{
			std::cout << "kkk4:" << anotify->m_test << std::endl;
			return true;
		}

		bool kkk5(notify_parm_test1* anotify)
		{
			std::cout << "kkk5:" << anotify->m_test << std::endl;
			return true;
		}
	};

	class ntest2
	{
	public:
		static ntest2& getInstance()
		{
			static ntest2 ltemp;
			return ltemp;
		}

		bool kkk1(notify_parm_test* anotify)
		{
			std::cout << "kkk11:" << anotify->m_test << std::endl;
			return true;
		}
		bool kkk2(notify_parm_test* anotify)
		{
			std::cout << "kkk12:" << anotify->m_test << std::endl;
			return true;
		}

		bool kkk3(notify_parm_test1* anotify)
		{
			std::cout << "kkk13:" << anotify->m_test << std::endl;
			return true;
		}

		bool kkk4(notify_parm_test1* anotify)
		{
			std::cout << "kkk14:" << anotify->m_test << std::endl;
			return true;
		}

		bool kkk5(notify_parm_test1* anotify)
		{
			std::cout << "kkk15:" << anotify->m_test << std::endl;
			return true;
		}
	};

	void test_notify()
	{
		int lparmid2 = notify_parm<notify_parm_test>::rfun(&ntest::kkk2, &ntest::getInstance());
		int lparmid1 = notify_parm<notify_parm_test>::rfun(&ntest::kkk1, &ntest::getInstance());
		notify_parm<notify_parm_test>::rfun(&ntest2::kkk2, &ntest2::getInstance());
		notify_parm<notify_parm_test>::rfun(&ntest2::kkk1, &ntest2::getInstance());


		notify_parm<notify_parm_test1>::rfun(&ntest::kkk3, &ntest::getInstance());
		notify_parm<notify_parm_test1>::rfun(&ntest::kkk4, &ntest::getInstance());
		notify_parm<notify_parm_test1>::rfun(&ntest::kkk5, &ntest::getInstance());
		notify_parm<notify_parm_test1>::rfun(&ntest2::kkk3, &ntest2::getInstance());
		notify_parm<notify_parm_test1>::rfun(&ntest2::kkk4, &ntest2::getInstance());
		notify_parm<notify_parm_test1>::rfun(&ntest2::kkk5, &ntest2::getInstance());

		notify_parm_test lparm;
		lparm.m_test = 1989;
		notify<notify_parm_test>::execute(lparm);
		std::cout << "#############################" << std::endl;
		notify_parm<notify_parm_test>::moverfun(lparmid2);
		lparm.m_test = 2022;
		notify<notify_parm_test>::execute(lparm);
		std::cout << "#############################" << std::endl;
		notify_parm_test1 lparm1;
		lparm1.m_test = 1988;
		notify<notify_parm_test1>::execute(lparm1);
	}
}