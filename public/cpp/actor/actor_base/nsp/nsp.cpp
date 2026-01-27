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
#include "pb_field.h"
#include "nsp.h"

namespace ngl
{
	void nsp_regload::init(i64_actorid aactorid)
	{
		std::set<i16_area> lareaset;
		ttab_servers::instance().get_arealist_nonrepet(nconfig.tid(), lareaset);
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
		if (!m_register.contains(aarea))
		{
			tools::no_core_dump();
			return;
		}
		m_register[aarea] = true;
	}

	bool nsp_regload::is_register(i16_area aarea)const
	{
		auto itor = m_register.find(aarea);
		if (itor == m_register.end())
		{
			return false;
		}
		return itor->second;
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
		if (!m_loadfinish.contains(aarea))
		{
			tools::no_core_dump();
			return;
		}
		m_loadfinish[aarea] = true;
	}

	bool nsp_regload::is_loadfinish(i16_area aarea)const
	{
		auto itor = m_loadfinish.find(aarea);
		if (itor == m_loadfinish.end())
		{
			return false;
		}
		return itor->second;
	}

	i64_actorid nsp_regload::nspserid(i16_area aarea)const
	{
		auto itor = m_nspserver.find(aarea);
		if (itor == m_nspserver.end())
		{
			tools::no_core_dump();
			return nguid::make();
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

	void operator_field::init(bool anspserver)
	{
		m_nspserver = anspserver;
	}

	void operator_field::set_field(i16_actortype atype, const std::map<i32_fieldnumber, epb_field>& anode_fieldnumbers)
	{
		if (m_node_fieldnumbers.contains(atype))
		{
			std::map<i32_fieldnumber, epb_field>& lmap = m_node_fieldnumbers[atype];
			for (auto& [_fieldnumber, _fieldtype] : anode_fieldnumbers)
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
			m_node_fieldnumbers.insert(std::make_pair(atype, anode_fieldnumbers));
		}
	}

	void operator_field::set_field(const std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>>& anode_fieldnumbers)
	{
		// 合并前进行检测, 出现重复且不一致的字段,直接异常处理
		std::ranges::for_each(anode_fieldnumbers, [this](auto& apair)
			{
				set_field(apair.first, apair.second);
			});
	}

	void operator_field::nspser_add_field(std::map<i32_fieldnumber, epb_field>& afieldmap, i32_fieldnumber afieldnumber, epb_field afieldtype)
	{
		if (!afieldmap.empty() && afieldmap.contains(afieldnumber))
		{
			auto itor = afieldmap.find(afieldnumber);
			if (itor != afieldmap.end() && itor->second == afieldtype)
			{
				tools::no_core_dump();
			}
		}
		afieldmap[afieldnumber] = afieldtype;
	}

	void operator_field::nspcli_add_field(std::map<i32_fieldnumber, epb_field>& afieldmap, i32_fieldnumber afieldnumber, epb_field afieldtype)
	{
		if (!afieldmap.empty() && afieldmap.contains(afieldnumber))
		{
			auto itor = afieldmap.find(afieldnumber);
			if (itor != afieldmap.end() && itor->second == epb_field_write)
			{
				return;
			}
		}
		afieldmap[afieldnumber] = afieldtype;
	}

	void operator_field::add_field(i16_actortype atype, i32_fieldnumber afieldnumber, epb_field afieldtype)
	{
		if (m_nspserver)
		{
			nspser_add_field(m_node_fieldnumbers[atype], afieldnumber, afieldtype);
		}
		else
		{
			nspcli_add_field(m_node_fieldnumbers[atype], afieldnumber, afieldtype);
		}
	}

	std::map<i32_fieldnumber, epb_field>* operator_field::get_field(i16_actortype atype)
	{
		if (m_node_fieldnumbers.contains(atype))
		{
			return &m_node_fieldnumbers[atype];
		}
		return nullptr;
	}

	std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>>& operator_field::field_numbers()
	{
		return m_node_fieldnumbers;
	}

	void care_data::init(bool aread)
	{
		m_core.m_read = aread;
		m_core.m_all = true;
	}

	void care_data::init(const std::set<i64_actorid>& aids)
	{
		m_core.m_read = true;
		m_core.m_all = false;
		m_core.m_readids = aids;
	}

	void care_data::init(const std::set<i64_actorid>& areadids, const std::set<i64_actorid>& awriteids)
	{
		m_core.m_read = false;
		m_core.m_all = false;
		m_core.m_readids = areadids;
		m_core.m_writeids = awriteids;
	}

	void care_data::init(bool aread, const std::set<i64_actorid>& awriteids)
	{
		m_core.m_read = false;
		m_core.m_all = true;
		m_core.m_writeids = awriteids;
	}

	void care_data::init(const nsp_care& acore)
	{
		m_core = acore;
	}

	bool care_data::is_care(i64_actorid adataid)const
	{
		if (m_core.m_read)
		{
			return m_core.m_all || m_core.m_readids.contains(adataid);
		}
		else
		{
			return m_core.m_all || m_core.m_writeids.contains(adataid) || m_core.m_readids.contains(adataid);
		}
	}

	bool care_data::is_read(i64_actorid adataid)const
	{
		return m_core.m_all || m_core.m_writeids.contains(adataid) || m_core.m_readids.contains(adataid);
	}

	bool care_data::is_write(i64_actorid adataid)const
	{
		if (m_core.m_read)
		{
			return false;
		}
		else
		{
			return m_core.m_all || m_core.m_writeids.contains(adataid);
		}
	}

	bool care_data::is_readall()const
	{
		return m_core.m_all;
	}

	bool care_data::is_writeall()const
	{
		if (m_core.m_read)
		{
			return false;
		}
		return m_core.m_all;
	}

	std::set<i64_actorid>& care_data::readids()
	{
		return m_core.m_readids;
	}

	std::set<i64_actorid>& care_data::writeids()
	{
		return m_core.m_writeids;
	}

	const nsp_care& care_data::get_core()const
	{
		return m_core;
	}
}//namespace ngl