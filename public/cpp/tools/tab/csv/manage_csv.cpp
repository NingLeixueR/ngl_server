/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "manage_csv.h"
#include "csvtable.h"
#include "drop.h"

namespace ngl
{
	std::string csvbase::m_path;

	std::map<std::string, reload_csv::trefun> reload_csv::m_fun;

	bool reload_csv::save(const std::string& atabhash, const std::string& acsvcontent)
	{
		trefun* lfun = tools::findmap(m_fun, atabhash);
		if (lfun == nullptr)
		{
			return false;
		}
		if (lfun->m_save == nullptr)
		{
			return false;
		}
		lfun->m_save(acsvcontent);
		return true;
	}

	bool reload_csv::reload(const std::string& atabhash)
	{
		trefun* lfun = tools::findmap(m_fun, atabhash);
		if (lfun == nullptr)
		{
			return false;
		}
		if (lfun->m_reload == nullptr)
		{
			return false;
		}
		lfun->m_reload();
		return true;
	}

	bool reload_csv::readcsv(const std::string& atabhash, std::string& acsvcontent)
	{
		trefun* lfun = tools::findmap(m_fun, atabhash);
		if (lfun == nullptr)
		{
			return false;
		}
		if (lfun->m_readfile == nullptr)
		{
			return false;
		}
		lfun->m_readfile(acsvcontent);
		return true;
	}

	std::map<std::string, csvbase*> allcsv::m_data; // key: TAB::name()

	void allcsv::add(const char* akey, csvbase* ap)
	{
		if (m_data[akey] != nullptr)
		{
			delete m_data[akey];
			m_data[akey] = nullptr;
		}
		m_data[akey] = ap;
	}

	csvbase* allcsv::get_csvbase(const std::string& akey)
	{
		csvbase** lp = tools::findmap(m_data, akey);
		if (lp == nullptr)
		{
			return nullptr;
		}
		return *lp;
	}

	void allcsv::foreach_verify(std::map<std::string, std::string>& averify)
	{
		for (const auto& [key, value] : m_data)
		{
			averify.insert(std::make_pair(key, value->verify()));
		}
	}

	std::map<std::string, csvbase*>& allcsv::all()
	{
		return m_data;
	}
}// namespace ngl