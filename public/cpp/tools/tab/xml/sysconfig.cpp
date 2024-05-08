#include "sysconfig.h"
#include "xmlnode.h"


namespace ngl
{

	ELOG	sysconfig::m_loglevel		= ELOG_ERROR;
	int32_t	sysconfig::m_logline		= 10000;
	int32_t	sysconfig::m_logflushtime	= 10;
	bool	sysconfig::m_logiswrite		= true;
	bool	sysconfig::m_logconsole		= false;
	int32_t	sysconfig::m_consumings		= 100;
	std::string	sysconfig::m_xorkey;
	int32_t	sysconfig::m_xorkeynum		= 0;	//2^n-1 nÎªxorkeyµÄ×Ö·ûÊý
	bool	sysconfig::m_isxor			= false;
	bool	sysconfig::m_varint			= false;
	bool	sysconfig::m_robot_test		= false;

	void sysconfig::init()
	{
		ngl::xmlinfo* lpublicxml = ngl::xmlnode::get_publicconfig();
		lpublicxml->find("logiswrite", m_logiswrite);
		lpublicxml->find("logflushtime", m_logflushtime);
		lpublicxml->find("logline", m_logline);
		lpublicxml->find("logconsole", m_logconsole);
		int llevel = 0;
		lpublicxml->find("loglevel", llevel);
		m_loglevel = (ELOG)llevel;

		lpublicxml->find("consumings", m_consumings);
		
		do
		{
			if (!lpublicxml->isxor(m_isxor))
				break;
			if (!lpublicxml->xor_str(m_xorkey))
				break;
			int32_t lxorkeylen = m_xorkey.size();
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

		lpublicxml->varint(m_varint);

		lpublicxml->find("robot_test", m_robot_test);
	}
}