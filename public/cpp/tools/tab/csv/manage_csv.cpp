/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
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