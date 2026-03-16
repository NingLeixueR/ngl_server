/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project: ngl_server
* License: MIT
*/
// File overview: Declares interfaces for xml.

#pragma once

#include "actor/tab/csvtable.h"
#include "tools/type.h"

#include <map>
#include <set>
#include <string>
#include <vector>

namespace ngl
{
	enum ELOGLEVEL
	{
		ELOG_NONE	= -1,
		ELOG_DEBUG	= 0,
		ELOG_INFO	= 1,
		ELOG_WARN	= 2,
		ELOG_ERROR	= 3,
		ELOG_MAX	= 4,
	};

	struct sysconfig
	{
	private:
		static ELOGLEVEL	m_logwritelevel;
		static ELOGLEVEL	m_logconsolelevel;
		static int32_t		m_logline;
		static int32_t		m_logflushtime;

		static int32_t		m_consumings;

		static std::string	m_xorkey;
		static int32_t		m_xorkeynum;
		static bool			m_isxor;
		static bool			m_varint;
		static bool			m_robot_test;

		static int32_t		m_kcpping;
		static int32_t		m_kcppinginterval;
		static std::string	m_kcpsession;

		static int32_t		m_sessionewait;
		static int32_t		m_open_servertime;
		static int32_t		m_head_version;

		static int32_t		m_rate_interval;
		static int32_t		m_rate_count;
		static int32_t		m_heart_beat_interval;
		static int32_t		m_net_timeout;
		static std::map<std::string, int32_t, std::less<>> m_nodecountbyname;
		static std::map<NODE_TYPE, int32_t> m_nodecountbytype;

		static int32_t		m_gamecount;
		static std::string	m_gmurl;
		static std::set<i32_serverid> m_gatewayids;
		static std::string m_lua;
	public:
		static void init();

		static int32_t logline() { return m_logline; }
		static int32_t logflushtime() { return m_logflushtime; }
		static int32_t logwritelevel() { return m_logwritelevel; }
		static int32_t logconsolelevel() { return m_logconsolelevel; }
		static int32_t consumings() { return m_consumings; }
		static const std::string& xorkey() { return m_xorkey; }
		static int32_t xorkeynum() { return m_xorkeynum; }
		static bool isxor() { return m_isxor; }
		static bool varint() { return m_varint; }
		static bool robot_test() { return m_robot_test; }
		static int32_t kcpping() { return m_kcpping; }
		static int32_t kcppinginterval() { return m_kcppinginterval; }
		static const std::string& kcpsession() { return m_kcpsession; }
		static int32_t sessionwait() { return m_sessionewait; }
		static int32_t open_servertime() { return m_open_servertime; }
		static int32_t head_version() { return m_head_version; }
		static int32_t rate_interval() { return m_rate_interval; }
		static int32_t rate_count() { return m_rate_count; }
		static int32_t heart_beat_interval() { return m_heart_beat_interval; }
		static int32_t net_timeout() { return m_net_timeout; }
		static const std::string& gmurl() { return m_gmurl; }
		static const std::string& lua() { return m_lua; }

		static int32_t node_count(const char* anodename);
		static int32_t node_count(NODE_TYPE atype);

		static void init_gatewayids(int atcout);
		static const std::set<i32_serverid>& gatewayids();
	};
}//namespace ngl