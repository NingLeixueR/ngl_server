#include "regular.h"


namespace ngl
{
	void regular::sregex(const std::string& apattern, const std::string& adata, const std::function<void(std::string&)>& afun)
	{
		std::regex r(apattern);
		std::sregex_iterator it(adata.begin(), adata.end(), r);
		std::sregex_iterator end;
		for (; it != end; ++it)
		{
			std::string lstr = it->str();
			afun(lstr);
		}
	}
	///---- values = std::smatch[1];
	void regular::smatch(const std::string& aexpression, const std::string& adata, const std::function<void(std::smatch&)>& afun)
	{
		std::smatch what;
		std::regex expression(aexpression);
		std::string::const_iterator start = adata.begin();
		std::string::const_iterator end = adata.end();
		for (; std::regex_search(start, end, what, expression);)
		{
			afun(what);
			start = what[0].second;
		}
	}

	void regular::replace(const std::string& aexpression, const std::string& areleace, const std::string& adata, std::string& aret)
	{
		std::regex reg(aexpression);
		aret = std::regex_replace(adata, reg, areleace);
	}

	bool regular::replace_ret(const std::string& aexpression, const std::string& areleace, std::string& adata, std::string& aret)
	{
		replace(aexpression, areleace, adata, aret);
		return aret != adata;
	}

}