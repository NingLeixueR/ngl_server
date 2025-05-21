#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_servers :
		public manage_csv<tab_servers>
	{
		ttab_servers(const ttab_servers&) = delete;
		ttab_servers& operator=(const ttab_servers&) = delete;
		using type_tab = tab_servers;

		static std::map<int16_t, std::vector<tab_servers*>> m_areaofserver;

		ttab_servers()
		{}

		static const std::map<int, tab_servers>& tablecsv()
		{
			const ttab_servers* ttab = allcsv::get<ttab_servers>();
			assert(ttab != nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_servers* tab(int32_t aid)
		{
			const auto& lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		static const tab_servers* tab()
		{
			return tab(nconfig::m_nodeid);
		}

		void reload()final
		{
			m_areaofserver.clear();
			for (std::pair<const int, tab_servers>& pair : m_tablecsv)
			{
				m_areaofserver[pair.second.m_area].push_back(&pair.second);
			}
		}

		static const tab_servers* tab(const std::string& aname, int area, int32_t atcount)
		{
			const ttab_servers* ttab = allcsv::get<ttab_servers>();
			assert(ttab != nullptr);
			for (const std::pair<const int, tab_servers>& item : ttab->m_tablecsv)
			{
				if (item.second.m_area == area && item.second.m_name == aname && item.second.m_tcount == atcount)
				{
					return &item.second;
				}
			}
			return nullptr;
		}

		static const net_works* nworks(ENET_PROTOCOL atype, const tab_servers* atab)
		{
			for (auto& item : atab->m_net)
			{
				if (item.m_type == atype)
				{
					return &item;
				}
			}
			return nullptr;
		}

		static const net_works* nworks(ENET_PROTOCOL atype)
		{
			return nworks(atype, tab());
		}

		static const net_works* get_nworks(ENET_PROTOCOL atype)
		{
			//nconfig::m_nodeid
			const tab_servers* ltab = tab();
			if (ltab == nullptr)
			{
				return nullptr;
			}
			return nworks(atype, ltab);
		}

		static const net_works* get_nworks(const std::string& aname, int area, int32_t atcount, ENET_PROTOCOL atype)
		{
			const tab_servers* ltab = tab(aname, area, atcount);
			if (ltab == nullptr)
			{
				return nullptr;
			}
			return nworks(atype, ltab);
		}

		static bool isefficient(ENET_PROTOCOL atype)
		{
			return atype == ENET_TCP || atype == ENET_WS;
		}
	private:
		static const net_works* connect(i32_serverid alocalserver, i32_serverid aotherserver)
		{
			assert(alocalserver != aotherserver);
			const tab_servers* ltab1 = tab(alocalserver);
			const tab_servers* ltab2 = tab(aotherserver);
			if (alocalserver > aotherserver)
			{
				std::swap(ltab1, ltab2);
			}
			for (const net_works& item1 : ltab1->m_net)
			{
				if (isefficient(item1.m_type) == false)
				{
					continue;
				}
				for (const net_works& item2 : ltab2->m_net)
				{
					if (item1.m_type == item2.m_type)
					{
						// ����other�Ľṹ
						return alocalserver > aotherserver ? &item1 : &item2;
					}
				}
			}
			return nullptr;
		}
	public:
		static net_works const* connect(i32_serverid aserverid)
		{
			return connect(nconfig::m_nodeid, aserverid);
		}

		static NODE_TYPE node_type(i32_serverid aserverid)
		{
			const tab_servers* ltab = tab(aserverid);
			assert(ltab != nullptr);
			return ltab->m_type;
		}

		static NODE_TYPE node_type()
		{
			return node_type(nconfig::m_nodeid);
		}

		static const tab_servers* node_tnumber(NODE_TYPE atype, int32_t anumber)
		{
			const ttab_servers* ttab = allcsv::get<ttab_servers>();
			assert(ttab != nullptr);
			for (const std::pair<const int, tab_servers>& pair : ttab->m_tablecsv)
			{
				if (pair.second.m_type == atype && pair.second.m_tcount == anumber)
				{
					return &pair.second;
				}
			}
			return nullptr;
		}

		// �������з�����
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

		// ��ȡ��������(���������ǿ��������Ҫת��Ϊ�������������)
		static const std::set<i16_area>* get_area(i16_area aarea)
		{
			static std::map<i16_area, std::set<i16_area>> lmap;
			if (aarea < 0)
			{
				const std::set<i16_area>* lset = tools::findmap(lmap, aarea);
				if (lset != nullptr)
				{
					return lset;
				}
				std::set<i16_area>& ltempset = lmap[aarea];
				auto itor = m_areaofserver.find(aarea);
				if (itor == m_areaofserver.end())
				{
					return nullptr;
				}
				if (itor->second.empty())
				{
					return nullptr;
				}
				const tab_servers* ltab = *itor->second.begin();
				if (ltab == nullptr)
				{
					return nullptr;
				}
				for (i32_serverid itemid : ltab->m_actorserver)
				{
					const tab_servers* tabactor = tab(itemid);
					if (ltab == nullptr)
					{
						continue;
					}
					ltempset.insert(tabactor->m_area);
				}
				return &ltempset;
			}
			else
			{
				lmap[aarea].insert(aarea);
				return &lmap[aarea];
			}
		}

		// ����������	atype
		// ����			aarea������������,��Ҫ�ṩ���������atype��������
		static bool foreach_server(NODE_TYPE atype, i16_area aarea, const std::function<void(const tab_servers*)>& afun)
		{
			const std::set<i16_area>* larea = get_area(aarea);
			if (larea == nullptr)
			{
				return false;
			}

			for (const auto& [_area, _vec] : m_areaofserver)
			{
				if (larea->find(_area) == larea->end())
				{
					continue;
				}
				for (const tab_servers* iserver : _vec)
				{
					if (atype == FAIL)
					{
						afun(iserver);
					}
					else if (iserver->m_type == atype)
					{
						afun(iserver);
					}
				}
			}
			return true;
		}

		static bool get_server(NODE_TYPE atype, i16_area aarea, std::vector<i32_serverid>& avec)
		{
			return foreach_server(atype, aarea, [&avec](const tab_servers* iserver)
				{
					avec.push_back(iserver->m_id);
				});
		}

		static tab_servers* find_first(NODE_TYPE atype, const std::function<bool(tab_servers*)>& afun)
		{
			std::vector<tab_servers*>* litem = tools::findmap(m_areaofserver, tab()->m_area);
			if (litem == nullptr)
			{
				return nullptr;
			}
			for (tab_servers* iserver : *litem)
			{
				if (iserver->m_type == atype && afun(iserver))
				{
					return iserver;
				}
			}
			return nullptr;
		}

		// ��ȡ��������������
		static const std::set<i16_area>* get_arealist(i32_serverid aserverid)
		{
			const tab_servers* ltab = tab(aserverid);
			if (ltab == nullptr)
			{
				return nullptr;
			}
			return get_area(ltab->m_area);
		}
	};
}//namespace ngl

#define tab_self_area  ngl::ttab_servers::tab()->m_area
#define tab_self_cros_area  ngl::ttab_servers::tab()->m_crossarea
