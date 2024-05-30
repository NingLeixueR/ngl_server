#include "ttab_errormessage.h"
#include "ttab_attribute.h"
#include "ttab_calendar.h"
#include "ttab_servers.h"
#include "ttab_random.h"
#include "ttab_dbload.h"
#include "manage_csv.h"
#include "csvtable.h"
#include "drop.h"

namespace ngl
{
	std::map<std::string, reload_csv::trefun> reload_csv::m_fun;

	bool reload_csv::save(const std::string& atabhash, const std::string& acsvcontent)
	{
		trefun* lfun = tools::findmap(m_fun, atabhash);
		if (lfun == nullptr)
			return false;
		if (lfun->m_save == nullptr)
			return false;
		lfun->m_save(acsvcontent);
		return true;
	}

	bool reload_csv::reload(const std::string& atabhash)
	{
		trefun* lfun = tools::findmap(m_fun, atabhash);
		if (lfun == nullptr)
			return false;
		if (lfun->m_reload == nullptr)
			return false;
		lfun->m_reload();
		return true;
	}

	bool reload_csv::readcsv(const std::string& atabhash, std::string& acsvcontent)
	{
		trefun* lfun = tools::findmap(m_fun, atabhash);
		if (lfun == nullptr)
			return false;
		if (lfun->m_readfile == nullptr)
			return false;
		lfun->m_readfile(acsvcontent);
		return true;
	}

	std::map<std::string, csvbase*> allcsv::m_data; // key: TAB::name()

	template <typename TTAB,typename TTABCLASS>
	void loadcsv()
	{
		//static bool isinitload = false;
		//if (isinitload == false)
		//{
			csvbase* lp = new TTABCLASS();
			allcsv::add(TTAB::name(), lp);
			lp->load();
			//lp->reload();
		//	isinitload = true;
		//}
	}

	void allcsv::load()
	{
		loadcsv<tab_random, ttab_random>();
		loadcsv<tab_calendar, ttab_calendar>();
		loadcsv<tab_attribute, ttab_attribute>();

		loadcsv<tab_consume, manage_csv<tab_consume>>();
		loadcsv<tab_mail, manage_csv<tab_mail>>();
		loadcsv<tab_chat, manage_csv<tab_chat>>();
		loadcsv<tab_activity, manage_csv<tab_activity>>();	
		loadcsv<tab_activity_drawcompliance, manage_csv<tab_activity_drawcompliance>>();
		loadcsv<tab_equip, manage_csv<tab_equip>>();
		loadcsv<tab_equiplv, manage_csv<tab_equiplv>>();
		loadcsv<tab_errormessage, manage_csv<tab_errormessage>>();
		loadcsv<tab_item, manage_csv<tab_item>>();
		loadcsv<tab_specialid, manage_csv<tab_specialid>>();
		loadcsv<tab_synthesis, manage_csv<tab_synthesis>>();
		loadcsv<tab_task, manage_csv<tab_task>>();
		loadcsv<tab_card, manage_csv<tab_card>>();
		loadcsv<tab_servers, ttab_servers>();
		loadcsv<tab_dbload, ttab_dbload>();
		loadcsv<tab_recharge, manage_csv<tab_recharge>>();
	}

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
		return lp == nullptr ? nullptr : *lp;
	}

	void allcsv::foreach_version(std::map<std::string, int32_t>& aversion)
	{
		for (const auto& [key, value] : m_data)
		{
			aversion.insert(std::make_pair(key, value->version()));
		}
	}

	std::map<std::string, csvbase*>& allcsv::all()
	{
		return m_data;
	}
}// namespace ngl