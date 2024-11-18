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

		void reload()final
		{
#define IF_NAME_VAL(NAME)							\
	if(ltab.m_name == #NAME)						\
	{												\
		tovalue(m_##NAME, ltab.m_value.c_str());	\
		return;										\
	}
			std::ranges::for_each(m_tablecsv, [this](const auto& pair)
				{
					const tab_specialid& ltab = pair.second;
					IF_NAME_VAL(rolemaxlv)
					IF_NAME_VAL(rolemaxvip)
					IF_NAME_VAL(createfamilconsume)
					IF_NAME_VAL(familsignexp)
					IF_NAME_VAL(familsigndrop)
					IF_NAME_VAL(familapplylistcount)
					IF_NAME_VAL(friendsapplylistcount)
					IF_NAME_VAL(friendscount)
				});
		}
	};
}//namespace ngl
