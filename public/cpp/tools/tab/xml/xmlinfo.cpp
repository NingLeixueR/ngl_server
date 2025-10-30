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
		log_error()->print("xmlinfo:{}", m_data);
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

	void xmlinfo::foreach(const std::function<void(const std::pair<const std::string, std::string>&)>& afun)
	{
		for (const auto& item : m_data)
		{
			afun(item);
		}
	}
}// namespace ngl