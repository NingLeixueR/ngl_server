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
#include "csvtable.h"
#include "ncsv.h"
#include "drop.h"

namespace ngl
{
	void csv_base::set_path(const std::string& apath, const std::string& aname)
	{
		m_path = std::format("{}/{}", apath, aname);
		if (tools::directories_exists(m_path) == false)
		{
			m_path = apath;
		}
	}

	std::string& csv_base::path()
	{
		return m_path;
	}

	std::string csv_base::m_path;

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

	std::map<std::string, std::shared_ptr<csv_base>> ncsv::m_csv; // key: TAB::name()

	void ncsv::add(const char* akey, std::shared_ptr<csv_base>& ap)
	{
		if (m_csv[akey] != nullptr)
		{
			m_csv.erase(akey);
		}
		m_csv[akey] = ap;
	}

	csv_base* ncsv::get_csvbase(const std::string& akey)
	{
		std::shared_ptr<csv_base>* lp = tools::findmap(m_csv, akey);
		if (lp == nullptr)
		{
			return nullptr;
		}
		return lp->get();
	}

	void ncsv::foreach_verify(std::map<std::string, std::string>& averify)
	{
		for (auto& [key, value] : m_csv)
		{
			averify.insert(std::make_pair(key, value->verify()));
		}
	}

	std::map<std::string, std::shared_ptr<csv_base>>& ncsv::all()
	{
		return m_csv;
	}
}// namespace ngl