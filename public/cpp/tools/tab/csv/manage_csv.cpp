#include "ttab_errormessage.h"
#include "ttab_specialid.h"
#include "ttab_attribute.h"
#include "ttab_mergearea.h"
#include "ttab_calendar.h"
#include "ttab_ranklist.h"
#include "ttab_servers.h"
#include "ttab_random.h"
#include "ttab_dbload.h"
#include "manage_csv.h"
#include "ttab_task.h"
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

	template <typename TTAB>
	void loadcsv()
	{
		using TAB = typename TTAB::type_tab;
		static bool isinitload = false;
		if (isinitload == false)
		{
			csvbase* lp = new TTAB();
			allcsv::add(TAB::name(), lp);
			lp->load();
			lp->reload();
			isinitload = true;
		}
	}

	void allcsv::load()
	{
		loadcsv<ttab_random>();
		loadcsv<ttab_calendar>();
		loadcsv<ttab_attribute>();
		loadcsv<ttab_servers>();
		loadcsv<ttab_specialid>();
		loadcsv<ttab_task>();
		loadcsv<ttab_servers>();
		loadcsv<ttab_dbload>();
		loadcsv<ttab_mergearea>();
		loadcsv<ttab_ranklist>();

		loadcsv<manage_csv<tab_consume>>();
		loadcsv<manage_csv<tab_mail>>();
		loadcsv<manage_csv<tab_chat>>();
		loadcsv<manage_csv<tab_activity>>();	
		loadcsv<manage_csv<tab_activity_drawcompliance>>();
		loadcsv<manage_csv<tab_equip>>();
		loadcsv<manage_csv<tab_equiplv>>();
		loadcsv<manage_csv<tab_errormessage>>();
		loadcsv<manage_csv<tab_item>>();
		loadcsv<manage_csv<tab_synthesis>>();
		loadcsv<manage_csv<tab_card>>();
		loadcsv<manage_csv<tab_recharge>>();		
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