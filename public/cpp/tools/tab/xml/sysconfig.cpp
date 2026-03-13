/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project: ngl_server
* License: MIT
*/

#include "actor/tab/ttab_mergearea.h"
#include "actor/tab/ttab_servers.h"
#include "tools/tab/xml/sysconfig.h"
#include "tools/tab/xml/xml.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <string_view>

namespace ngl
{
	namespace
	{
		template <typename T>
		bool find_first(const xarg_info& ainfo, std::initializer_list<const char*> akeys, T& aout)
		{
			for (const char* key : akeys)
			{
				if (ainfo.find(key, aout))
				{
					return true;
				}
			}
			return false;
		}

		ELOGLEVEL sanitize_log_level(int32_t alevel, ELOGLEVEL afallback)
		{
			if (alevel < ELOG_NONE || alevel >= ELOG_MAX)
			{
				return afallback;
			}
			return static_cast<ELOGLEVEL>(alevel);
		}

		int32_t sanitize_positive(int32_t avalue, int32_t afallback)
		{
			return avalue > 0 ? avalue : afallback;
		}

		int32_t sanitize_non_negative(int32_t avalue, int32_t afallback)
		{
			return avalue >= 0 ? avalue : afallback;
		}

		int32_t parse_open_servertime(const xarg_info& ainfo, int32_t afallback)
		{
			std::string text;
			if (!ainfo.find("open_servertime", text) || text.empty())
			{
				return afallback;
			}
			return static_cast<int32_t>(localtime::str2time(text.c_str(), "%Y/%m/%d %H:%M:%S"));
		}

		int32_t xor_cycle_length(const std::string& akey)
		{
			if (akey.empty())
			{
				return 0;
			}
			if (akey.size() > static_cast<std::size_t>(std::numeric_limits<int32_t>::max()))
			{
				return 0;
			}
			return static_cast<int32_t>(akey.size());
		}
	}

	ELOGLEVEL	sysconfig::m_logwritelevel = ELOG_ERROR;
	ELOGLEVEL	sysconfig::m_logconsolelevel = ELOG_ERROR;
	int32_t		sysconfig::m_logline = 10000;
	int32_t		sysconfig::m_logflushtime = 10;
	int32_t		sysconfig::m_consumings = 100;
	std::string	sysconfig::m_xorkey;
	int32_t		sysconfig::m_xorkeynum = 0;
	bool		sysconfig::m_isxor = false;
	bool		sysconfig::m_varint = false;
	bool		sysconfig::m_robot_test = false;
	int32_t		sysconfig::m_kcpping = 10;
	int32_t		sysconfig::m_kcppinginterval = 20;
	int32_t		sysconfig::m_sessionewait = 1;
	std::string	sysconfig::m_kcpsession;
	int32_t		sysconfig::m_open_servertime = 0;
	int32_t		sysconfig::m_head_version = 1;
	int32_t		sysconfig::m_rate_interval = 1;
	int32_t		sysconfig::m_rate_count = 20;
	int32_t		sysconfig::m_heart_beat_interval = 10;
	int32_t		sysconfig::m_net_timeout = 600000;
	std::map<std::string, int32_t, std::less<>> sysconfig::m_nodecountbyname;
	std::map<NODE_TYPE, int32_t> sysconfig::m_nodecountbytype;
	int32_t		sysconfig::m_gamecount = 0;
	std::string	sysconfig::m_gmurl;
	std::set<i32_serverid> sysconfig::m_gatewayids;
	std::string	sysconfig::m_lua;

	void sysconfig::init()
	{
		xarg_info* lpublicxml = nconfig.info();
		if (lpublicxml == nullptr)
		{
			tools::no_core_dump();
			return;
		}

		ELOGLEVEL logwritelevel = m_logwritelevel;
		ELOGLEVEL logconsolelevel = m_logconsolelevel;
		int32_t logline = m_logline;
		int32_t logflushtime = m_logflushtime;
		int32_t consumings = m_consumings;
		std::string xorkey;
		int32_t xorkeynum = 0;
		bool isxor = false;
		bool varint = m_varint;
		bool robot_test = m_robot_test;
		int32_t kcpping = m_kcpping;
		int32_t kcppinginterval = m_kcppinginterval;
		int32_t sessionewait = m_sessionewait;
		std::string kcpsession = m_kcpsession;
		int32_t open_servertime = m_open_servertime;
		int32_t head_version = m_head_version;
		int32_t rate_interval = m_rate_interval;
		int32_t rate_count = m_rate_count;
		int32_t heart_beat_interval = m_heart_beat_interval;
		int32_t net_timeout = m_net_timeout;
		std::string gmurl = m_gmurl;
		std::string lua = m_lua;
		std::map<std::string, int32_t, std::less<>> nodecountbyname;
		std::map<NODE_TYPE, int32_t> nodecountbytype;

		find_first(*lpublicxml, { "logline" }, logline);
		find_first(*lpublicxml, { "logflushtime", "log_flushtime" }, logflushtime);

		int32_t level = static_cast<int32_t>(logwritelevel);
		if (find_first(*lpublicxml, { "logwritelevel" }, level))
		{
			logwritelevel = sanitize_log_level(level, logwritelevel);
		}
		level = static_cast<int32_t>(logconsolelevel);
		if (find_first(*lpublicxml, { "logconsolelevel" }, level))
		{
			logconsolelevel = sanitize_log_level(level, logconsolelevel);
		}

		find_first(*lpublicxml, { "consumings" }, consumings);
		find_first(*lpublicxml, { "varint" }, varint);
		find_first(*lpublicxml, { "robot_test" }, robot_test);
		find_first(*lpublicxml, { "kcpping" }, kcpping);
		find_first(*lpublicxml, { "kcppinginterval" }, kcppinginterval);
		find_first(*lpublicxml, { "sessionewait" }, sessionewait);
		find_first(*lpublicxml, { "kcpsession" }, kcpsession);
		find_first(*lpublicxml, { "head_version" }, head_version);
		find_first(*lpublicxml, { "gmurl" }, gmurl);
		find_first(*lpublicxml, { "lua" }, lua);
		open_servertime = parse_open_servertime(*lpublicxml, open_servertime);

		if (find_first(*lpublicxml, { "isxor" }, isxor) && isxor && find_first(*lpublicxml, { "xor_str" }, xorkey))
		{
			xorkeynum = xor_cycle_length(xorkey);
			isxor = xorkeynum > 0;
		}
		else
		{
			xorkey.clear();
			xorkeynum = 0;
			isxor = false;
		}

		lpublicxml->foreach([&nodecountbyname, &nodecountbytype](const std::pair<const std::string, std::string>& apair)
			{
				std::string_view key(apair.first);
				const std::size_t suffix_pos = key.rfind("_count");
				if (suffix_pos == std::string_view::npos || suffix_pos + 6 != key.size())
				{
					return;
				}

				std::string nodename(key.substr(0, suffix_pos));
				NODE_TYPE type = em<NODE_TYPE>::get_enum(nodename.c_str());
				if (type == em<NODE_TYPE>::enum_null())
				{
					return;
				}

				try
				{
					int32_t count = tools::lexical_cast<int32_t>(apair.second);
					count = std::max<int32_t>(1, count);
					nodecountbyname[nodename] = count;
					nodecountbytype[type] = count;
				}
				catch (...)
				{
				}
			}
		);

		logline = sanitize_positive(logline, m_logline);
		logflushtime = sanitize_positive(logflushtime, m_logflushtime);
		consumings = sanitize_non_negative(consumings, m_consumings);
		kcpping = sanitize_positive(kcpping, m_kcpping);
		kcppinginterval = sanitize_positive(kcppinginterval, m_kcppinginterval);
		sessionewait = sanitize_positive(sessionewait, m_sessionewait);
		head_version = sanitize_positive(head_version, m_head_version);
		rate_interval = sanitize_positive(rate_interval, m_rate_interval);
		rate_count = sanitize_positive(rate_count, m_rate_count);
		heart_beat_interval = sanitize_positive(heart_beat_interval, m_heart_beat_interval);
		net_timeout = sanitize_positive(net_timeout, m_net_timeout);

		m_logwritelevel = logwritelevel;
		m_logconsolelevel = logconsolelevel;
		m_logline = logline;
		m_logflushtime = logflushtime;
		m_consumings = consumings;
		m_xorkey = std::move(xorkey);
		m_xorkeynum = xorkeynum;
		m_isxor = isxor;
		m_varint = varint;
		m_robot_test = robot_test;
		m_kcpping = kcpping;
		m_kcppinginterval = kcppinginterval;
		m_sessionewait = sessionewait;
		m_kcpsession = std::move(kcpsession);
		m_open_servertime = open_servertime;
		m_head_version = head_version;
		m_rate_interval = rate_interval;
		m_rate_count = rate_count;
		m_heart_beat_interval = heart_beat_interval;
		m_net_timeout = net_timeout;
		m_nodecountbyname = std::move(nodecountbyname);
		m_nodecountbytype = std::move(nodecountbytype);
		m_gamecount = node_count("game");
		m_gmurl = std::move(gmurl);
		m_lua = std::move(lua);

		init_gatewayids(node_count("gateway"));
	}

	int32_t sysconfig::node_count(const char* anodename)
	{
		if (anodename == nullptr || *anodename == '\0')
		{
			return 1;
		}
		auto itor = m_nodecountbyname.find(anodename);
		if (itor == m_nodecountbyname.end())
		{
			return 1;
		}
		return itor->second;
	}

	int32_t sysconfig::node_count(NODE_TYPE atype)
	{
		auto itor = m_nodecountbytype.find(atype);
		if (itor == m_nodecountbytype.end())
		{
			return 1;
		}
		return itor->second;
	}

	void sysconfig::init_gatewayids(int atcout)
	{
		m_gatewayids.clear();
		if (atcout <= 0)
		{
			return;
		}

		const tab_servers* self_tab = ttab_servers::instance().const_tab();
		if (self_tab == nullptr)
		{
			return;
		}

		std::set<i32_serverid> lgatewayids;
		if (std::set<i16_area>* lareas = ttab_mergearea::instance().mergelist(self_tab->m_area); lareas != nullptr)
		{
			for (i16_area aarea : *lareas)
			{
				ttab_servers::instance().serverid(GATEWAY, aarea, lgatewayids);
			}
		}
		ttab_servers::instance().serverid(GATEWAY, self_tab->m_area, lgatewayids);

		const int max_tcount = std::min<int>(atcout, static_cast<int>(std::numeric_limits<int16_t>::max()));
		for (i32_serverid ltabid : lgatewayids)
		{
			if (ltabid < static_cast<i32_serverid>(std::numeric_limits<int16_t>::min()) ||
				ltabid > static_cast<i32_serverid>(std::numeric_limits<int16_t>::max()))
			{
				tools::no_core_dump();
				continue;
			}

			for (int i = 1; i <= max_tcount; ++i)
			{
				m_gatewayids.insert(nnodeid::nodeid(static_cast<int16_t>(ltabid), static_cast<int16_t>(i)));
			}
		}
	}

	const std::set<i32_serverid>& sysconfig::gatewayids()
	{
		return m_gatewayids;
	}
}//namespace ngl
