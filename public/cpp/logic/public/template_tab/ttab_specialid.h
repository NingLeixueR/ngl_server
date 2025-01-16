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
		static int32_t		m_friendsapplylistcount;// 玩家好友请求列表数量
		static int32_t		m_friendscount;			// 玩家好友数量
		static int32_t		m_ranklistmaxcount;		// 排行榜最大条目数
		
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
				return false;	
			}
			return true;
		}

		template <typename T, typename ...ARG>
		inline bool read_value(const tab_specialid& atab, const char* akey, T& adata, ARG&... adatas)
		{
			return read_value(atab, akey, adata) && read_value(atab, adatas...);
		}

		void reload()final
		{
#define de_pram(NAME) #NAME,m_##NAME

			std::ranges::for_each(m_tablecsv, [this](const auto& pair)
				{
					read_value(
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
					);					
				});
		}
	};
}//namespace ngl
