#include "splite.h"
#include "tools.h"


namespace ngl
{
	template <typename T>
	void _splite(const char* abuff, const char* afg, std::vector<T>& avec)
	{
		std::vector<std::string> lvec;
		boost::split(lvec, abuff, boost::is_any_of(afg));
		for (std::vector<std::string>::iterator itor = lvec.begin();
			itor != lvec.end(); ++itor)
		{
			avec.push_back(boost::lexical_cast<T>(*itor));
		}
	}

	void splite::division(const char* abuff, const char* afg, std::vector<int>& avec)
	{
		_splite<int>(abuff, afg, avec);
	}

	void splite::division(const char* abuff, const char* afg, std::vector<float>& avec)
	{
		_splite<float>(abuff, afg, avec);
	}

	void splite::division(const char* astr, const char* afg, std::vector<std::string>& avec)
	{
		boost::split(avec, astr, boost::is_any_of(afg));
	}

	void splite::division(const char* abuff, const char* afg, std::vector<double>& avec)
	{
		_splite<double>(abuff, afg, avec);
	}

	template <typename T>
	static void _tostr(const std::vector<T>& avec, const char afg, std::string& astr)
	{
		for (const T& item : avec)
		{
			if (!astr.empty())
				astr += afg;
			astr += boost::lexical_cast<T>(item);
		}
	}

	void splite::recast(const std::vector<std::string>& avec, const char afg, std::string& astr)
	{
		for (const std::string& item : avec)
		{
			if (!astr.empty())
				astr += afg;
			astr += item;
		}
	}

	void splite::recast(const std::vector<int>& avec, const char afg, std::string& astr)
	{
		_tostr(avec, afg, astr);
	}

	void splite::recast(const std::vector<float>& avec, const char afg, std::string& astr)
	{
		_tostr(avec, afg, astr);
	}

	void splite::recast(const std::vector<double>& avec, const char afg, std::string& astr)
	{
		_tostr(avec, afg, astr);
	}

}