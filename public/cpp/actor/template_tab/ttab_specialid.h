#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_specialid :
		public manage_csv<tab_specialid>
	{
		ttab_specialid(const ttab_specialid&) = delete;
		ttab_specialid& operator=(const ttab_specialid&) = delete;

	public:
		i32_rolelv	m_rolemaxlv = 0;			// ������ȼ�
		i32_rolevip	m_rolemaxvip = 0;			// ������vip�ȼ�
		int32_t		m_createfamilconsume = 0;	// �������ŵ�����
		int32_t		m_familsignexp = 0;			// ����ǩ�����������ӵľ���ֵ
		int32_t		m_familsigndrop = 0;		// ����ǩ���Ľ���
		int32_t		m_familapplylistcount = 0;	// ������������ŵ��������
		int32_t		m_familjoininterval = 0;	// ����������/�������ŵ���ȴʱ��
		int32_t		m_friendsapplylistcount = 0;// ��Һ��������б�����
		int32_t		m_friendscount = 0;			// ��Һ�������
		int32_t		m_ranklistmaxcount = 0;		// ���а������Ŀ��
		// ������Ϸ
		int32_t						m_example_room_maxtime = 0;	// ������Ϸ�����ƥ��ʱ��
		std::map<int32_t, int32_t>	m_example_totalnumber;	// ������Ϸ��ƥ������
		int32_t						m_example_room_readytime = 0; // ������Ϸ�ĵȴ����ȷ�ϵ����ʱ��

	private:
		ttab_specialid()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_specialid] reload" << std::endl;
#define de_pram(NAME) #NAME,m_##NAME

			for (const auto& pair : m_tablecsv)
			{
				bool lread = read_value(
					pair.second
					, de_pram(rolemaxlv)
					, de_pram(rolemaxvip)
					, de_pram(createfamilconsume)
					, de_pram(familsignexp)
					, de_pram(familsigndrop)
					, de_pram(familapplylistcount)
					, de_pram(friendsapplylistcount)
					, de_pram(familjoininterval)
					, de_pram(friendscount)
					, de_pram(ranklistmaxcount)
					, de_pram(example_room_maxtime)
					, de_pram(example_room_readytime)
				);
				std::string lexample_totalnumber;
				if (lread == false && read_value(pair.second, "example_totalnumber", lexample_totalnumber))
				{
					if (tools::splite_special(lexample_totalnumber.c_str(), "\\[", "]", m_example_totalnumber) == false)
					{
						return;
					}
				}
			}
		}

	public:
		using type_tab = tab_specialid;

		static ttab_specialid& instance()
		{
			static ttab_specialid ltemp;
			return ltemp;
		}

		const std::map<int, tab_specialid>* tablecsv()
		{
			const ttab_specialid* ttab = allcsv::get<ttab_specialid>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
				return nullptr;
			}
			return &ttab->m_tablecsv;
		}

		const tab_specialid* tab(int32_t aid)
		{
			auto lmap = tablecsv();
			auto itor = lmap->find(aid);
			if (itor == lmap->end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		void tovalue(int32_t& apvalue, const char* astr)
		{
			apvalue = tools::lexical_cast<int32_t>(astr);
		}

		void tovalue(std::string& apvalue, const char* astr)
		{
			apvalue = astr;
		}

		template <typename T>
		inline bool read_value(const tab_specialid& atab, const char* akey, T& adata)
		{
			if (atab.m_name == akey)
			{
				tovalue(adata, atab.m_value.c_str());
				return true;
			}
			return false;
		}

		template <typename T, typename ...ARG>
		inline bool read_value(const tab_specialid& atab, const char* akey, T& adata, ARG&... adatas)
		{
			if (read_value(atab, akey, adata))
			{
				return true;
			}
			return read_value(atab, adatas...);
		}
	};
}//namespace ngl
