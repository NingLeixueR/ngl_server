#include "ttab_activity.h"
#include "ttab_activity_drawcompliance.h"
#include "ttab_card.h"
#include "ttab_chat.h"
#include "ttab_consume.h"
#include "ttab_equip.h"
#include "ttab_equiplv.h"
#include "ttab_item.h"
#include "ttab_mail.h"
#include "ttab_map.h"
#include "ttab_matching.h"
#include "ttab_plays.h"
#include "ttab_recharge.h"
#include "ttab_synthesis.h"
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
		loadcsv<ttab_servers>();
		loadcsv<ttab_specialid>();

		//if (NODE_TYPE::DB == nconfig::node_type())
		{
			loadcsv<ttab_mergearea>();
			loadcsv<ttab_dbload>();
		}
		//if (NODE_TYPE::WORLD == nconfig::node_type())
		{
			loadcsv<ttab_calendar>();
			loadcsv<ttab_ranklist>();
			loadcsv<ttab_activity_drawcompliance>();
			loadcsv<ttab_activity>();
			loadcsv<ttab_matching>(); 
			loadcsv<ttab_plays>();
		}
		//if (NODE_TYPE::GAME == nconfig::node_type())
		{
			loadcsv<ttab_task>();
			loadcsv<ttab_errormessage>();
			loadcsv<ttab_recharge>();
		}
		//if (NODE_TYPE::GAME == nconfig::node_type() || NODE_TYPE::WORLD == nconfig::node_type())
		{
			loadcsv<ttab_attribute>();
			loadcsv<ttab_random>();
			loadcsv<ttab_synthesis>();
			loadcsv<ttab_consume>();
			loadcsv<ttab_equiplv>();
			loadcsv<ttab_equip>();
			loadcsv<ttab_item>();
			loadcsv<ttab_card>();
			loadcsv<ttab_mail>();
			loadcsv<ttab_chat>();
			loadcsv<ttab_map>();
		}
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