#include "ttab_mergearea.h"
#include "ttab_servers.h"
#include "sysconfig.h"
#include "xml.h"

namespace ngl
{
	ELOGLEVEL	sysconfig::m_logwritelevel		= ELOG_ERROR;
	ELOGLEVEL	sysconfig::m_logconsolelevel	= ELOG_ERROR;
	int32_t		sysconfig::m_logline			= 10000;
	int32_t		sysconfig::m_logflushtime		= 10;
	int32_t		sysconfig::m_consumings			= 100;
	std::string	sysconfig::m_xorkey;
	int32_t		sysconfig::m_xorkeynum			= 0;
	bool		sysconfig::m_isxor				= false;
	bool		sysconfig::m_varint				= false;
	bool		sysconfig::m_robot_test			= false;
	int32_t		sysconfig::m_kcpping			= 10;
	int32_t		sysconfig::m_kcppinginterval	= 20;
	int32_t		sysconfig::m_sessionewait		= 1;
	std::string	sysconfig::m_kcpsession;
	int32_t		sysconfig::m_open_servertime	= 0;
	int32_t		sysconfig::m_head_version		= 1;
	int32_t		sysconfig::m_rate_interval		= 1;
	int32_t		sysconfig::m_rate_count			= 20;
	int32_t		sysconfig::m_heart_beat_interval = 10;
	int32_t		sysconfig::m_net_timeout		= 600000;
	std::string	sysconfig::m_gmurl;
	std::set<i32_serverid> sysconfig::m_gatewayids;

	void sysconfig::init()
	{
		ngl::xmlinfo* lpublicxml = ngl::xmlnode::get_publicconfig();
		lpublicxml->find("logflushtime", m_logflushtime);
		lpublicxml->find("logline", m_logline);
		{
			int llevel = 0;
			lpublicxml->find("logwritelevel", llevel);
			m_logwritelevel = (ELOGLEVEL)llevel;
			lpublicxml->find("logconsolelevel", llevel);
			m_logconsolelevel = (ELOGLEVEL)llevel;
		}
		
		lpublicxml->find("consumings", m_consumings);
		
		do
		{
			if (!lpublicxml->find("isxor", m_isxor))
			{
				break;
			}
			if (!lpublicxml->find("xor_str", m_xorkey))
			{
				break;
			}
			int32_t lxorkeylen = (int32_t)m_xorkey.size();
			for (int i = 1;; ++i)
			{
				if (lxorkeylen < ((1 << i) - 1))
				{
					m_xorkeynum = i - 1;
					break;
				}
			}
			if (m_xorkeynum <= 0)
			{
				m_isxor = false;
			}
		} while (false);

		lpublicxml->find("varint", m_varint);

		lpublicxml->find("robot_test", m_robot_test);
		lpublicxml->find("kcpping", m_kcpping);
		lpublicxml->find("kcppinginterval", m_kcppinginterval);
		lpublicxml->find("sessionewait", m_sessionewait);
		lpublicxml->find("kcpsession", m_kcpsession);

		std::string lopen_servertime;
		lpublicxml->find("open_servertime", lopen_servertime);
		m_open_servertime = (int32_t)localtime::str2time(lopen_servertime.c_str(), "%Y/%m/%d %H:%M:%S");
		
		lpublicxml->find("head_version", m_head_version);

		lpublicxml->find("gmurl", m_gmurl);

		init_gatewayids();
	}

	void sysconfig::init_gatewayids()
	{
		m_gatewayids.clear();

		std::set<i16_area>* lareas = ttab_mergearea::instance().mergelist(tab_self_area);
		if (lareas == nullptr)
		{
			for (i16_area aarea : *lareas)
			{
				ttab_servers::instance().get_server(GATEWAY, aarea, sysconfig::m_gatewayids);
			}
		}
		ttab_servers::instance().get_server(GATEWAY, tab_self_area, m_gatewayids);
	}

	std::set<i32_serverid>& sysconfig::gatewayids()
	{
		return m_gatewayids;
	}
}//namespace ngl