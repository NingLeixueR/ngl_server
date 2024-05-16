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
		std::shared_ptr<np_actor_logitem> lstream = log();
		(*lstream) << "##########xmlinfo start##########" << std::endl;
		for (const auto& [_key, _val] : m_data)
		{
			(*lstream) << "[" << _key << ":" << _val << "]" << std::endl;
		}
		(*lstream) << "##########xmlinfo finish#########" << std::endl;
		lstream->error("");
	}
	
	bool xmlinfo::find(const char* akey, bool& adata)
	{
		std::string* lp = tools::findmap<std::string, std::string>(m_data, akey);
		if (lp == nullptr)
			return false;
		adata = *lp == "true" || *lp == "TRUE";
		return true;
	}

	bool xmlinfo::find(const char* akey, std::string& adata)
	{
		std::string* lp = tools::findmap<std::string, std::string>(m_data, akey);
		if (lp == nullptr)
			return false;
		adata = *lp;
		return true;
	}

	bool xmlinfo::ishavekey(const char* akey)
	{
		return m_data.find(akey) != m_data.end();
	}

	bool xmlinfo::threadnum(i32_threadsize& avalue)
	{
		return find("threadnum", avalue);
	}

	bool xmlinfo::actorthreadnum(i32_threadsize& avalue)
	{
		return find("actorthreadnum", avalue);
	}

	bool xmlinfo::id(i32_id& avalue)
	{
		return find("id", avalue);
	}

	bool xmlinfo::port(i16_port& avalue)
	{
		return find("port", avalue);
	}

	bool xmlinfo::db(i32_serverid& avalue)
	{
		return find("db", avalue);
	}

	bool xmlinfo::actorserver(i32_serverid& avalue)
	{
		return find("actorserver", avalue);
	}

	bool xmlinfo::name(std::string& avalue)
	{
		return find("name", avalue);
	}

	bool xmlinfo::ip(std::string& avalue)
	{
		return find("ip", avalue);
	}

	bool xmlinfo::line(i32_line& avalue)
	{
		return find("line", avalue);
	}

	bool xmlinfo::area(i16_area& avalue)
	{
		return find("area", avalue);
	}

	bool xmlinfo::varint(bool& avalue)
	{
		if (find("varint", avalue) == false)
			avalue = false;
		return true;
	}

	bool xmlinfo::xor_str(std::string& avalue)
	{
		if (find("xor_str", avalue) == false)
			avalue = "";
		return true;
	}

	bool xmlinfo::isxor(bool& avalue)
	{
		if (find("isxor", avalue) == false)
			avalue = false;
		return true;
	}

	bool xmlinfo::reloadcsv(i32_serverid& avalue)
	{
		if (find("reloadcsv", avalue) == false)
			avalue = false;
		return true;
	}
}// namespace ngl