#include "nsp.h"

namespace ngl
{
	void nsp_regload::init(i64_actorid aactorid)
	{
		std::set<i16_area> lareaset;
		ttab_servers::instance().get_arealist_nonrepet(nconfig::m_tid, lareaset);
		auto ltype = (ENUM_ACTOR)nguid::type(aactorid);
		for (i16_area area : lareaset)
		{
			m_nspserver[area] = nguid::make(ltype, area, nguid::none_actordataid());
			m_register[area] = false;
			m_loadfinish[area] = false;
		}
	}

	void nsp_regload::set_register(i16_area aarea)
	{
		m_register[aarea] = false;
	}

	bool nsp_regload::is_register(i16_area aarea)const
	{
		if (!m_register.contains(aarea))
		{
			return false;
		}
		return m_register[aarea];
	}

	bool nsp_regload::is_register()const
	{
		for (auto [_area, _register] : m_register)
		{
			if (!_register)
			{
				return false;
			}
		}
		return true;
	}

	void nsp_regload::set_loadfinish(i16_area aarea)
	{
		m_loadfinish[aarea] = false;
	}

	bool nsp_regload::is_loadfinish(i16_area aarea)const
	{
		if (!m_loadfinish.contains(aarea))
		{
			return false;
		}
		return m_loadfinish[aarea];
	}

	i64_actorid nsp_regload::nspserid(i16_area aarea)const
	{
		auto itor = m_nspserver.find(aarea);
		if (itor == m_nspserver.end())
		{
			tools::no_core_dump();
			return -1;
		}
		return itor->second;
	}

	bool nsp_regload::is_loadfinish()const
	{
		for (auto [_area, _loadfinish] : m_loadfinish)
		{
			if (!_loadfinish)
			{
				return false;
			}
		}
		return true;
	}

	void nsp_regload::foreach_nspser(const std::function<void(i16_area, i64_actorid)>& afun)const
	{
		for (auto [_area, _serverid] : m_nspserver)
		{
			afun(_area, _serverid);
		}
	}

	void operator_field::set_field(const std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>>& anode_fieldnumbers)
	{
		// 合并前进行检测, 出现重复且不一致的字段,直接异常处理
		std::ranges::for_each(anode_fieldnumbers, [this](const auto& apair)
			{
				if (m_node_fieldnumbers.contains(apair.first))
				{
					std::map<i32_fieldnumber, epb_field>& lmap = m_node_fieldnumbers[apair.first];
					for (const auto& [_fieldnumber, _fieldtype] : apair.second)
					{

						auto itor = lmap.find(_fieldnumber);
						if (itor != lmap.end())
						{
							if (itor->second != _fieldtype)
							{
								tools::no_core_dump();
							}
						}
						else
						{
							lmap.insert(std::make_pair(_fieldnumber, _fieldtype));
						}
					}
				}
				else
				{
					m_node_fieldnumbers.insert(apair);
				}
			});
	}

	void operator_field::add_field(i16_actortype atype, i32_fieldnumber afieldnumber, epb_field afieldtype)
	{
		m_node_fieldnumbers[atype][afieldnumber] = afieldtype;
	}

	std::map<i32_fieldnumber, epb_field>* operator_field::get_field(i16_actortype atype)
	{
		if (m_node_fieldnumbers.contains(atype))
		{
			return &m_node_fieldnumbers[atype];
		}
		return nullptr;
	}

	void care_data::init(bool aread)
	{
		m_read = aread;
		m_all = true;
	}

	void care_data::init(const std::set<i64_actorid>& aids)
	{
		m_read = true;
		m_all = false;
		m_readids = aids;
	}

	void care_data::init(const std::set<i64_actorid>& areadids, const std::set<i64_actorid>& awriteids)
	{
		m_read = false;
		m_all = false;
		m_readids = areadids;
		m_writeids = awriteids;
	}

	void care_data::init(bool aread, const std::set<i64_actorid>& awriteids)
	{
		m_read = false;
		m_all = true;
		m_writeids = awriteids;
	}

	bool care_data::is_care(i64_actorid adataid)const
	{
		if (m_read)
		{
			return m_all || m_readids.contains(adataid);
		}
		else
		{
			return m_all || m_writeids.contains(adataid) || m_readids.contains(adataid);
		}
	}

	bool care_data::is_read(i64_actorid adataid)const
	{
		return m_all || m_writeids.contains(adataid) || m_readids.contains(adataid);
	}

	bool care_data::is_write(i64_actorid adataid)const
	{
		if (m_read)
		{
			return false;
		}
		else
		{
			return m_all || m_writeids.contains(adataid);
		}
	}

	bool care_data::is_readall()const
	{
		return m_all;
	}

	bool care_data::is_writeall()const
	{
		if (m_read)
		{
			return false;
		}
		return m_all;
	}

	std::set<i64_actorid>& care_data::readids()
	{
		return m_readids;
	}

	std::set<i64_actorid>& care_data::writeids()
	{
		return m_writeids;
	}
}//namespace ngl