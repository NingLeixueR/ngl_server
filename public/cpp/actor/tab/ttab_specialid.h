/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for tab.

#pragma once

#include "tools/tab/csv/ncsv.h"
#include "tools/tab/xml/xml.h"
#include "tools/type.h"

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
		i32_rolelv	m_rolemaxlv = 0;			// Player level
		i32_rolevip	m_rolemaxvip = 0;			// Player viplevel
		int32_t		m_createfamilconsume = 0;	// Createguild
		int32_t		m_familsignexp = 0;			// Guildsign intoguild
		int32_t		m_familsigndrop = 0;		// Guildsign in reward
		int32_t		m_familapplylistcount = 0;	// Playerrequestjoinguild
		int32_t		m_familjoininterval = 0;	// Playerrequestjoin/createguild time
		int32_t		m_friendsapplylistcount = 0;// Playerfriendsrequestlist
		int32_t		m_friendscount = 0;			// Playerfriends
		int32_t		m_ranklistmaxcount = 0;		// Rank list
		// Instance game
		int32_t						m_example_room_maxtime = 0;	// Instance game time
		std::map<int32_t, int32_t>	m_example_totalnumber;	// Instance game
		int32_t						m_example_room_readytime = 0; // Instance game playerconfirm time

		static ttab_specialid& instance()
		{
			static std::once_flag lfirst;
			std::call_once(lfirst, []()
				{
					ncsv::loadcsv<ttab_specialid>();
				}
			);
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
