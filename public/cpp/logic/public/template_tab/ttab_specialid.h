#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{

	struct ttab_specialid : public manage_csv<tab_specialid>
	{
		ttab_specialid(const ttab_specialid&) = delete;
		ttab_specialid& operator=(const ttab_specialid&) = delete;

		static i32_rolelv	m_rolemaxlv;			// ������ȼ�
		static i32_rolevip	m_rolemaxvip;			// ������vip�ȼ�
		static int32_t		m_createfamilconsume;	// �������ŵ�����
		static int32_t		m_familsignexp;			// ����ǩ�����������ӵľ���ֵ
		static int32_t		m_familsigndrop;		// ����ǩ���Ľ���
		static int32_t		m_familapplylistcount;	// ������������ŵ��������
		static int32_t		m_familjoininterval;	// ����������/�������ŵ���ȴʱ��
		static int32_t		m_friendsapplylistcount;// ��Һ��������б�����
		static int32_t		m_friendscount;			// ��Һ�������
		static int32_t		m_ranklistmaxcount;		// ���а������Ŀ��
		// ������Ϸ
		static int32_t						m_example_room_maxtime;	// ������Ϸ�����ƥ��ʱ��
		static std::map<int32_t, int32_t>	m_example_totalnumber;	// ������Ϸ��ƥ������
		static int32_t						m_example_room_readytime; // ������Ϸ�ĵȴ����ȷ�ϵ����ʱ��
		

		ttab_specialid()
		{}

		static void tovalue(int32_t& apvalue, const char* astr)
		{
			apvalue = tools::lexical_cast<int32_t>(astr);
		}

		static void tovalue(std::string& apvalue, const char* astr)
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

		void reload()final
		{
#define de_pram(NAME) #NAME,m_##NAME

			std::ranges::for_each(m_tablecsv, [this](const auto& pair)
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
				});
		}
	};
}//namespace ngl