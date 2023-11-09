#include "rfun.h"

#include <iostream>

namespace ngl
{
	class testrfunbase
	{
	public:
		virtual int64_t hash_code() = 0;
	}; 
	
	std::map<rguid, rfuns::mame_function> rfuns::m_fun;


	class testrfun1
	{
	public:
		void handle(const int athread, const int& avalue)
		{
			std::cout << avalue << std::endl;
		}
	};

	class testrfun2
	{
	public:
		void handle(const int& athread, const float& avalue)
		{
			std::cout << avalue << std::endl;
		}
	};

	class testrfun3
	{
	public:
		void handle(const float& avalue)
		{
			std::cout << avalue << std::endl;
		}
	};


	struct tparm
	{
		int m_id;
	};

	class testrfun4
	{
	public:
		void handle(tparm& avalue)
		{
			std::cout << avalue.m_id << std::endl;
		}
	};

	void test_rfun()
	{
		rfuns::registers(&testrfun1::handle, "¹þ¹þ¹þ");
		rfuns::registers(&testrfun2::handle, "²âÊÔÒ»ÏÂ");

		testrfun1 la;
		testrfun2 lb;
		int lvint = 1989;
		float lvfloat = 2001;
		rfuns::handle_switch(&la, 1, lvint);
		rfuns::handle_switch(&la, 1, lvfloat);

		rfuns::handle_switch(&lb, 1, lvint);
		rfuns::handle_switch(&lb, 1, lvfloat);

		rfuns::registers(&testrfun3::handle, "6666666");
		rfuns::handle_switch(&lb, lvfloat);

		tparm ltparm;
		ltparm.m_id = 798;
		testrfun4 ld;
		rfuns::registers(&testrfun4::handle, "777777777");
		rfuns::handle_switch(&ld, ltparm);

	}


}