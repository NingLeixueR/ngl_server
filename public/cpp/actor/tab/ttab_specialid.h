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
#pragma once

#include "ncsv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	struct ttab_specialid :
		public csv<tab_specialid>
	{
		ttab_specialid(const ttab_specialid&) = delete;
		ttab_specialid& operator=(const ttab_specialid&) = delete;

		void reload()final
		{
			std::cout << "[ttab_specialid] reload" << std::endl;
			std::cout << "[ttab_specialid] reload" << std::endl;
			foreach([&](tab_specialid& atab)
				{
					bool lread = read_value(
						atab
						, {
							"rolemaxlv", "rolemaxvip", "createfamilconsume", "familsignexp", "familsigndrop", "familapplylistcount", "friendsapplylistcount",
							"familjoininterval", "friendscount", "ranklistmaxcount", "example_room_maxtime", "example_room_readytime"
						}
						, m_rolemaxlv, m_rolemaxvip, m_createfamilconsume, m_familsignexp, m_familsigndrop, m_familapplylistcount, m_friendsapplylistcount
						, m_familjoininterval, m_friendscount, m_ranklistmaxcount, m_example_room_maxtime, m_example_room_readytime
					);
					std::string lexample_totalnumber;
					if (lread == false && read_value(atab, { "example_totalnumber" }, lexample_totalnumber))
					{
						if (tools::splite_special(lexample_totalnumber.c_str(), "\\[", "]", m_example_totalnumber) == false)
						{
							return;
						}
					}
				}
			);
		}
	public:
		using type_tab = tab_specialid;

		ttab_specialid() = default;
	public:
		i32_rolelv	m_rolemaxlv = 0;			// 玩家最大等级
		i32_rolevip	m_rolemaxvip = 0;			// 玩家最大vip等级
		int32_t		m_createfamilconsume = 0;	// 创建军团的消耗
		int32_t		m_familsignexp = 0;			// 军团签到给军团增加的经验值
		int32_t		m_familsigndrop = 0;		// 军团签到的奖励
		int32_t		m_familapplylistcount = 0;	// 玩家请求加入军团的最大数量
		int32_t		m_familjoininterval = 0;	// 玩家请求加入/创建军团的冷却时间
		int32_t		m_friendsapplylistcount = 0;// 玩家好友请求列表数量
		int32_t		m_friendscount = 0;			// 玩家好友数量
		int32_t		m_ranklistmaxcount = 0;		// 排行榜最大条目数
		// 例子游戏
		int32_t						m_example_room_maxtime = 0;	// 例子游戏的最大匹配时间
		std::map<int32_t, int32_t>	m_example_totalnumber;	// 例子游戏的匹配人数
		int32_t						m_example_room_readytime = 0; // 例子游戏的等待玩家确认的最大时间

		static ttab_specialid& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_specialid>();
			}
			return *ncsv::get<ttab_specialid>();
		}

		// # std::map<int, tab_specialid>& tabs()
		// # tab_specialid* tab(int aid)

		void tovalue(int32_t& apvalue, const char* astr)
		{
			apvalue = tools::lexical_cast<int32_t>(astr);
		}

		void tovalue(std::string& apvalue, const char* astr)
		{
			apvalue = astr;
		}

		template <typename T>
		inline bool rvalue(const tab_specialid& atab, const char* akey, T& adata)
		{
			if (atab.m_name == akey)
			{
				tovalue(adata, atab.m_value.c_str());
				return true;
			}
			return false;
		}

		template <std::size_t... INDEX, typename ...ARG>
		inline bool read_value(const tab_specialid& atab, std::index_sequence<INDEX...>, const std::array<const char*, sizeof...(ARG)>& akeys, ARG&... adatas)
		{
			return (rvalue<ARG>(atab, akeys[INDEX], adatas), ...);
		}

		template <typename ...ARG>
		inline bool read_value(const tab_specialid& atab, const std::array<const char*, sizeof...(ARG)>& akeys, ARG&... adatas)
		{
			return read_value(atab, std::make_index_sequence<sizeof...(ARG)>{}, akeys, adatas...);
		}
	};
}//namespace ngl
