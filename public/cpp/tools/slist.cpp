#include "slist.h"


namespace ngl
{
	void test_slist()
	{
		// # ���� ��������+ 800��Ԫ��  ���ͷ�һ��  �ظ�1w�� ����stl ��  ngl::slist
		int64_t lbeg = ngl::time_wheel::getms();
		std::list<int> lstl1;
		std::list<int> lstl2;
		for (int i = 0; i < 10000; ++i)
		{
			for (int j = 0; j < 400; ++j)
			{
				lstl1.push_back(j);
			}
			lstl1.swap(lstl2);
			for (int j = 0; j < 400; ++j)
			{
				lstl1.push_back(j);
			}
			for (int j = 0; j < 200; ++j)
			{
				lstl2.pop_front();
			}
			lstl1.insert(lstl1.begin(), lstl2.begin(), lstl2.end());
			lstl1.clear();
		}
		int64_t lconsuming = ngl::time_wheel::getms() - lbeg;
		std::cout << "stl:" << lconsuming << std::endl;
		lbeg = ngl::time_wheel::getms();
		ngl::slist_production<int>  ls1;
		ngl::slist_consumption<int> ls2;
		for (int i = 0; i < 10000; ++i)
		{
			for (int j = 0; j < 400; ++j)
			{
				ls1.push_back(j);
			}
			ls1.swap(ls2);
			for (int j = 0; j < 400; ++j)
			{
				ls1.push_back(j);
			}
			for (int j = 0; j < 200; ++j)
			{
				ls2.pop_front();
			}
			ls1.push_front(ls2);
			ls1.clear();
		}
		lconsuming = ngl::time_wheel::getms() - lbeg;
		std::cout << "slist:" << lconsuming << std::endl;
	}
}//namespace ngl