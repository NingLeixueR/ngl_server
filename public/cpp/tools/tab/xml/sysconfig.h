/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project Name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
*
* This file is part of the ngl_server project, released under the MIT License.
* You are free to use, modify, and distribute this project in accordance with the license terms,
* including for commercial purposes, provided that you retain the original copyright and license notice.
*
* For full license details, see the LICENSE file in the project root directory:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "actor/tab/csvtable.h"
#include "tools/type.h"

#include <vector>
#include <set>

namespace ngl
{
	enum ELOGLEVEL
	{
		ELOG_NONE = -1,		// None
		ELOG_DEBUG = 0,		// Debug information
		ELOG_INFO = 1,		// Information print
		ELOG_WARN = 2,		// Warning
		ELOG_ERROR = 3,		// Error
		ELOG_MAX = 4,		// Boundary value
	};

	struct sysconfig
	{
	private:
		// [[ Log related
		static ELOGLEVEL	m_logwritelevel;		// Log level (write to file)
		static ELOGLEVEL	m_logconsolelevel;		// Log level (console display)
		static int32_t		m_logline;				// Line count of a single log file
		static int32_t		m_logflushtime;			// Interval for writing logs to file
		// ]] Log related

		static int32_t		m_consumings;			// Check actor message time consumption

		// Set simple encryption
		static std::string	m_xorkey;
		static int32_t		m_xorkeynum;			// 2^n-1, n is the character count of xorkey
		static bool			m_isxor;
		static bool			m_varint;				// Whether to enable varint
		static bool			m_robot_test;			// Whether to enable robot test

		static int32_t		m_kcpping;				// Interval for KCP to send ping (ms)
		static int32_t		m_kcppinginterval;		// Interval for KCP to check ping (ms)
		static std::string	m_kcpsession;			// KCP session

		static int32_t		m_sessionewait;			// Waiting time for gateway to remove session (ms)
		static int32_t		m_open_servertime;		// Server startup time
		static int32_t		m_head_version;			// Version number of the protocol header

		static int32_t		m_rate_interval;		// Rate detection interval (seconds)
		static int32_t		m_rate_count;		// Rate, messages per second
		static int32_t		m_heart_beat_interval;	// Heartbeat interval (seconds)
		static int32_t		m_net_timeout;			// Used to check the time difference between packet header and server time
		static std::map<std::string, int32_t>		m_nodecountbyname;			// Node count by name
		static std::map<NODE_TYPE, int32_t>			m_nodecountbytype;			// Node count by type

		static int32_t		m_gamecount;			// Game node count
		static std::string	m_gmurl;				// GM URL

		// # Gateway list
		static std::set<i32_serverid> m_gatewayids;

		static std::string m_lua;					// Lua directory path
	public:
		static void init();

		static int32_t	logline() { return m_logline; }
		static int32_t	logflushtime() { return m_logflushtime; }
		static int32_t	logwritelevel() { return m_logwritelevel; }
		static int32_t	logconsolelevel() { return m_logconsolelevel; }
		static int32_t	consumings() { return m_consumings; }
		static std::string& xorkey() { return m_xorkey; }
		static int32_t	xorkeynum() { return m_xorkeynum; }
		static bool	isxor() { return m_isxor; }
		static bool varint() { return m_varint; }
		static bool robot_test() { return m_robot_test; }
		static int32_t kcpping() { return m_kcpping; }
		static int32_t kcppinginterval() { return m_kcppinginterval; }
		static std::string& kcpsession() { return m_kcpsession; }
		static int32_t sessionwait() { return m_sessionewait; }
		static int32_t open_servertime() { return m_open_servertime; }
		static int32_t head_version() { return m_head_version; }
		static int32_t rate_interval() { return m_rate_interval; }
		static int32_t rate_count() { return m_rate_count; }
		static int32_t heart_beat_interval() { return m_heart_beat_interval; }
		static int32_t net_timeout() { return m_net_timeout; }
		static std::string& gmurl() { return m_gmurl; }
		static std::string& lua() { return m_lua; }

		static int32_t node_count(const char* anodename);
		static int32_t node_count(NODE_TYPE atype);

		static void init_gatewayids(int atcout);
		static std::set<i32_serverid>& gatewayids();
	};
}//namespace ngl
