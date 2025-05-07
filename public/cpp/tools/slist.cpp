#include "slist.h"


namespace ngl
{
	void test_slist()
	{
		// # 测试 两个队列+ 800个元素  再释放一半  重复1w次 测试stl 与  ngl::slist
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

		lbeg = ngl::time_wheel::getms();
		std::deque<int> ldeque1;
		std::deque<int> ldeque2;
		for (int i = 0; i < 10000; ++i)
		{
			for (int j = 0; j < 400; ++j)
			{
				ldeque1.push_back(j);
			}
			ldeque1.swap(ldeque2);
			for (int j = 0; j < 400; ++j)
			{
				ldeque1.push_back(j);
			}
			for (int j = 0; j < 200; ++j)
			{
				ldeque2.pop_front();
			}
			ldeque1.insert(ldeque1.begin(), ldeque2.begin(), ldeque2.end());
			ldeque1.clear();
		}
		lconsuming = ngl::time_wheel::getms() - lbeg;
		std::cout << "deque:" << lconsuming << std::endl;
	}
}//namespace ngl