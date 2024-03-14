#include "splite.h"
#include "tools.h"

#include <boost/lexical_cast.hpp>
#include <vector>
#include <string>

namespace ngl
{ 
	//A类地址：10.0.0.0--10.255.255.255
	bool is_a_address(const std::vector<std::string>& lvec)
	{
		if (lvec.empty())
			return false;
		return lvec[0] == "10";
	}

	//B类地址：172.16.0.0--172.31.255.255
	bool is_b_address(const std::vector<std::string>& lvec)
	{
		if (lvec.size() < 2)
			return false;
		if (lvec[0] != "172")
			return false;
		int lnum = boost::lexical_cast<int>(lvec[1]);
		return lnum >= 16 && lnum <= 31;
	}

	//C类地址：192.168.0.0--192.168.255.255
	bool is_c_address(const std::vector<std::string>& lvec)
	{
		if (lvec.size() < 2)
			return false;
		return lvec[0] == "192" && lvec[1] == "168";
	}

	bool tools::is_lanip(const std::string& aip)
	{
		if (aip == "127.0.0.1")
			return true;
		std::vector<std::string> lvec;
		splite::division(aip.c_str(), ".", lvec);
		return is_a_address(lvec) || is_b_address(lvec) || is_c_address(lvec);
	}

	bool tools::is_mem(const char* astr1, const char* astr2)
	{
		int llen1 = strlen(astr1);
		int llen2 = strlen(astr2);
		if (llen1 != llen2)
			return false;
		return memcmp(astr1, astr2, llen1) == 0;
	}
}//namespace ngl