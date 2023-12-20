#pragma once

#include "manage_csv.h"
#include "type.h"


namespace ngl
{
	struct ttab_servers : public manage_csv<tab_servers>
	{
		static std::map<int16_t, std::vector<tab_servers*>> m_areaofserver;

		ttab_servers()
		{}

		virtual void reload()
		{
			m_areaofserver.clear();
			for (std::pair<const int, tab_servers>& pair : tablecsv)
			{
				m_areaofserver[pair.second.m_area].push_back(&pair.second);
			}
		}

		static tab_servers* tab(i32_serverid aserverid)
		{
			ttab_servers* ttab = allcsv::get<ttab_servers>();
			assert(ttab != nullptr);
			auto itor = ttab->tablecsv.find(aserverid);
			return &itor->second;
		}

		static tab_servers* tab()
		{
			return tab(nconfig::m_nodeid);
		}

		static tab_servers* tab(const std::string& aname,int area, int32_t atcount)
		{
			ttab_servers* ttab = allcsv::get<ttab_servers>();
			assert(ttab != nullptr);
			for (std::pair<const int, tab_servers>& item : ttab->tablecsv)
			{
				if (item.second.m_area == area && item.second.m_name == aname && item.second.m_tcount == atcount)
					return &item.second;
			}
			return nullptr;
		}

		static ENET_PROTOCOL netprotocol(i32_serverid aserverid)
		{
			tab_servers* ltab = tab(aserverid);
			if (ltab == nullptr)
				return ENET_TCP;
			return ltab->m_net;
		}

		static ENET_PROTOCOL netprotocol()
		{
			return netprotocol(nconfig::m_nodeid);
		}

		static NODE_TYPE node_type(i32_serverid aserverid)
		{
			tab_servers* ltab = tab(aserverid);
			assert(ltab != nullptr);
			return ltab->m_type;
		}

		static NODE_TYPE node_type()
		{
			return node_type(nconfig::m_nodeid);
		}

		static tab_servers* node_tnumber(NODE_TYPE atype, int32_t anumber)
		{
			ttab_servers* ttab = allcsv::get<ttab_servers>();
			assert(ttab != nullptr);
			for (std::pair<const int, tab_servers>& pair : ttab->tablecsv)
			{
				if (pair.second.m_type == atype && pair.second.m_tcount == anumber)
					return &pair.second;
			}
			return nullptr;
		}

		static void foreach_server(const std::function<void(const tab_servers*)>& afun)
		{
			for (const auto& [_area, _vec] : m_areaofserver)
			{
				for (const tab_servers* iserver : _vec)
				{
					afun(iserver);
				}
			}
		}

		static void foreach_server(NODE_TYPE atype, const std::function<void(const tab_servers*)>& afun)
		{
			for (const auto& [_area, _vec] : m_areaofserver)
			{
				//if (aislocal && _area != tab()->m_area)
				//	continue;
				for (const tab_servers* iserver : _vec)
				{
					if (iserver->m_type == atype)
					{
						afun(iserver);
					}
				}
			}
		}

		static void foreach_server(NODE_TYPE atype, i16_area area, const std::function<void(const tab_servers*)>& afun)
		{
			for (const auto& [_area, _vec] : m_areaofserver)
			{
				if (_area != area)
					continue;
				for (const tab_servers* iserver : _vec)
				{
					if (iserver->m_type == atype)
					{
						afun(iserver);
					}
				}
			}
		}


		static tab_servers* find_first(NODE_TYPE atype, const std::function<bool(tab_servers*)>& afun)
		{
			std::vector<tab_servers*>* litem = tools::findmap(m_areaofserver, tab()->m_area);
			if (litem == nullptr)
				return nullptr;
			for (tab_servers* iserver : *litem)
			{
				if (iserver->m_type == atype && afun(iserver))
				{
					return iserver;
				}
			}
			return nullptr;
		}
	};


}

#define tab_self_area  ttab_servers::tab()->m_area