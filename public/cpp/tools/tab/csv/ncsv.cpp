/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for csv.


#include "tools/tab/csv/ncsv.h"

namespace ngl
{
	void csv_base::set_path(const std::string& apath, const std::string& aname)
	{
		m_path = std::format("{}/{}", apath, aname);
		// Some deployments keep CSV files directly under the root path, so fall
		// back when the named subdirectory does not exist.
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
		if (akey == nullptr || ap == nullptr)
		{
			return;
		}
		m_csv.insert_or_assign(akey, ap);
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
		// Expose every table hash so remote reload tools can detect stale data
		// before pushing replacements.
		for (auto& [key, value] : m_csv)
		{
			averify.insert_or_assign(key, value->verify());
		}
	}

	std::map<std::string, std::shared_ptr<csv_base>>& ncsv::all()
	{
		return m_csv;
	}
}// namespace ngl
