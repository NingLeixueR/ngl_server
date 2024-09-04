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

		static i32_rolelv	m_rolemaxlv;			// 玩家最大等级
		static i32_rolevip	m_rolemaxvip;			// 玩家最大vip等级
		static int32_t		m_createfamilconsume;	// 创建军团的消耗
		static int32_t		m_familsignexp;			// 军团签到给军团增加的经验值
		static int32_t		m_familsigndrop;		// 军团签到的奖励
		static int32_t		m_familapplylistcount;	// 玩家请求加入军团的最大数量
		static int32_t		m_familjoininterval;	// 玩家请求加入/创建军团的冷却时间

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
			std::ranges::for_each(tablecsv, [this](const auto& pair)
				{
					const tab_specialid& ltab = pair.second;
					IF_NAME_VAL(rolemaxlv)
					IF_NAME_VAL(rolemaxvip)
					IF_NAME_VAL(createfamilconsume)
					IF_NAME_VAL(familsignexp)
					IF_NAME_VAL(familsigndrop)
					IF_NAME_VAL(familapplylistcount)
				});
		}
	};
}//namespace ngl
