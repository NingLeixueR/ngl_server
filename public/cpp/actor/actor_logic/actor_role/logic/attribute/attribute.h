#pragma once 

#include <functional>
#include <vector>
#include <array>
#include <set>
#include <map>

#include "attribute_value.h"
#include "ttab_attribute.h"
#include "manage_csv.h"
#include "events_map.h"
#include "net.pb.h"
#include "nlog.h"
#include "net.h"

namespace ngl
{
	class attribute
	{
		// ģ������(ֻ��ģ��䷢���仯ͨ��updata[EnumModule,attribute_value])
		std::map<EnumModule, attribute_value> m_moduledata;
		bool m_sync;

		attribute_value& root()
		{
			return m_moduledata[EnumModule::E_ModuleRoot];
		}

		void add_module(EnumModule aenum)
		{
			auto itor = m_moduledata.find(aenum);
			if (itor == m_moduledata.end())
			{
				return;
			}
			for (const auto& [key, value] : itor->second.m_crattr)
			{
				ttab_attribute::add(m_moduledata[key].m_orattr, value);
			}
		}

		void dec_module(EnumModule aenum)
		{
			auto itor = m_moduledata.find(aenum);
			if (itor == m_moduledata.end())
			{
				return;
			}
			ttab_attribute::dec(root().m_attr, itor->second.m_fight);
			for (const auto& [key, value] : itor->second.m_crattr)
			{
				ttab_attribute::dec(m_moduledata[key].m_orattr, value);
				if (key != EnumModule::E_ModuleRoot)
				{
					ttab_attribute::dec(root().m_attr, m_moduledata[key].m_fight);
				}
			}
		}

		bool is_module_root(EnumModule aenum)
		{
			return aenum == EnumModule::E_ModuleRoot;
		}

		bool is_module_null(EnumModule aenum)
		{
			return aenum == EnumModule::E_ModuleNull;
		}

		void update_module(EnumModule aenum)
		{
			auto itor = m_moduledata.find(aenum);
			if (itor == m_moduledata.end())
			{
				return;
			}
			itor->second.update();
			ttab_attribute::add(root().m_attr, itor->second.m_fight);
			for (const auto& [key, value] : itor->second.m_crattr)
			{
				ttab_attribute::add(m_moduledata[key].m_orattr, value);
				if (is_module_root(key) == false)
				{
					m_moduledata[key].update();
					ttab_attribute::add(root().m_attr, m_moduledata[key].m_fight);
				}
			}
			root().update();
		}
	public:
		attribute() :
			m_sync(false)
		{}
		
		bool sync()
		{
			return m_sync;
		}

		void set_sync(bool async)
		{
			m_sync = async;
		}

		// # ��ʼ������ ��ģ����������
		void init_data(EnumModule aenum, attribute_value& avalue)
		{
			m_moduledata[aenum] = avalue;
			m_moduledata[aenum].m_module = aenum;
		}

		// # ���������ģ������ ��������������ս��
		void init()
		{
			for (EnumModule i = E_ModuleRoot;i < E_ModuleCount;i = (EnumModule)(i + 1))
			{
				add_module(i);
			}
			for (std::pair<const EnumModule, attribute_value>& lpair : m_moduledata)
			{
				if (lpair.first == E_ModuleRoot)
				{
					continue;
				}
				lpair.second.update();
				ttab_attribute::add(root().m_attr, lpair.second.m_fight);
			}
			root().update();
		}

		// #### ����ģ������
		void updata(EnumModule aenum, attribute_value& avalue)
		{
			dec_module(aenum);
			avalue.m_orattr.swap(m_moduledata[aenum].m_orattr);
			m_moduledata[aenum] = avalue;
			m_moduledata[aenum].m_module = aenum;
			update_module(aenum);
			m_sync = true;
		}

		// #### ս��
		int64_t fight()
		{
			return root().m_fightscore;
		}

		const map_attrvalue& get_attribute()
		{
			return root().m_fight;
		}

		//EnumModule aenum
		const map_attrvalue& get_attribute(EnumModule aenum)
		{
			return m_moduledata[aenum].m_fight;
		}

		void printf()
		{
			root().printf();
		}

		template <typename T>
		void topb(T& apro)
		{
			for (std::pair<const EnumModule, attribute_value>& item : m_moduledata)
			{
				pbnet::UnitModule* lpmodule = apro.add_m_modules();
				item.second.topb(*lpmodule);
			}
		}
	};
	class aoimap;
	// ��̬����,������ս���иı������
	class dynamic_attribute
	{
	private:
		std::map<EnumAttribute, int64_t> m_dynamic;
		aoimap* m_map;
		int64_t m_unitid;
		int32_t m_stat = pbnet::eunitstat_normal;	// ״̬	 
	public:
		dynamic_attribute(aoimap* amap, int64_t aunitid) :
			m_map(amap),
			m_unitid(aunitid)
		{}

		// ����[ģ������]���ɶ�̬����
		void init_dynamic(std::map<EnumModule, attribute_value>& amoduledata)
		{
			m_dynamic = amoduledata[EnumModule::E_ModuleRoot].get_fight();
		}

		// �Ƿ�����
		bool is_death()
		{
			return m_dynamic[E_Hp] <= 0;
		}

		// �޸Ķ�̬����
		void change_attribute(EnumAttribute aattribute, int32_t avalue)
		{
			m_dynamic[aattribute] += avalue;
			if (is_death())
			{
				np_eevents_map_death lparm;
				lparm.m_deathunitid = m_unitid;
				actor_events_map::trigger_event(lparm);
			}
		}

		// �޷��ƶ�
		void set_stat(pbnet::eunitstat astat)
		{
			m_stat |= (int32_t)astat;
		}

		void clear_stat()
		{
			m_stat = pbnet::eunitstat_normal;
		}

		// �Ƿ�����ƶ�
		bool ismove()const
		{
			return (m_stat & (int32_t)pbnet::eunitstat_nomove) == 0;
		}

		// �Ƿ������ͨ����
		bool isnormalattack()const
		{
			return (m_stat & (int32_t)pbnet::eunitstat_nonormalattack) == 0;
		}

		// �Ƿ�����ͷż���
		bool isreleaseskill()const
		{
			return (m_stat & (int32_t)pbnet::eunitstat_noreleaseskill) == 0;
		}
	};

	void test_attribute();
}// namespace ngl