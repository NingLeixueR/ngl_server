#include "nprotocol.h"
#include "xmlinfo.h"
#include "tools.h"
#include "nlog.h"
#include "nlog.h"

#include <string>

namespace ngl
{
	void xmlinfo::plog()
	{
		auto lstream = log_error();
		(*lstream) << "##########xmlinfo##########" << std::endl;
		for (const auto& [_key, _val] : m_data)
		{
			(*lstream) << "[" << _key << ":" << _val << "]" << std::endl;
		}
		(*lstream) << "##########xmlinfo#########" << std::endl;
		lstream->print("");
	}
	
	bool xmlinfo::find(const char* akey, bool& adata)
	{
		std::string* lp = tools::findmap<std::string, std::string>(m_data, akey);
		if (lp == nullptr)
		{
			return false;
		}
		adata = *lp == "true" || *lp == "TRUE";
		return true;
	}

	bool xmlinfo::find(const char* akey, std::string& adata)
	{
		std::string* lp = tools::findmap<std::string, std::string>(m_data, akey);
		if (lp == nullptr)
		{
			return false;
		}
		adata = *lp;
		return true;
	}
}// namespace ngl