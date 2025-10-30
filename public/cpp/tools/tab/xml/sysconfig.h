#pragma once

#include "csvtable.h"
#include "type.h"

#include <vector>
#include <set>

namespace ngl
{
	enum ELOGLEVEL
	{
		ELOG_NONE = -1,		// ��
		ELOG_DEBUG = 0,		// ������Ϣ
		ELOG_INFO = 1,		// ��ӡ
		ELOG_WARN = 2,		// ����
		ELOG_ERROR = 3,		// ����
		ELOG_MAX = 4,		// �߽�ֵ
	};

	struct sysconfig
	{
	private:
		// [[ ��־��� 
		static ELOGLEVEL	m_logwritelevel;		// ��־�ȼ�(д���ļ�)
		static ELOGLEVEL	m_logconsolelevel;		// ��־�ȼ�(����̨��ʾ)
		static int32_t		m_logline;				// ������־�ļ�������
		static int32_t		m_logflushtime;			// ��־д���ļ��ļ��ʱ��
		// ]] ��־��� 

		static int32_t		m_consumings;			// ���actor��Ϣ��ʱ

		// ���ü򵥼���
		static std::string	m_xorkey;
		static int32_t		m_xorkeynum;			//2^n-1 nΪxorkey���ַ���
		static bool			m_isxor;
		static bool			m_varint;				// �Ƿ���varint
		static bool			m_robot_test;			// �Ƿ�����˲���

		static int32_t		m_kcpping;				// kcp����ping�ļ��(����)
		static int32_t		m_kcppinginterval;		// kcp���ping�ļ��(����)
		static std::string	m_kcpsession;			// kcp session

		static int32_t		m_sessionewait;			// �ȴ�gateway�Ƴ�session��ʱ��(����)
		static int32_t		m_open_servertime;		// ����ʱ��
		static int32_t		m_head_version;			// Э��ͷ�İ汾��

		static int32_t		m_rate_interval;		// ���ʼ��ļ��(��λ��)
		static int32_t		m_rate_count;			// ���� ÿ�������
		static int32_t		m_heart_beat_interval;	// �������(��λ��)
		static int32_t		m_net_timeout;			// ��������ͷ�е�ʱ���������ʱ��
		static std::map<std::string, int32_t>		m_nodecountbyname;			// �������
		static std::map<NODE_TYPE, int32_t>			m_nodecountbytype;			// �������

		static int32_t		m_gamecount;			// game����
		static std::string	m_gmurl;				// gm url

		// # �����б�
		static std::set<i32_serverid> m_gatewayids;

		static std::string m_lua;					// lua����Ŀ¼
	public:
		static void init();

		static int32_t	logline()			{ return m_logline; }
		static int32_t	logflushtime()		{ return m_logflushtime; }
		static int32_t	logwritelevel()		{ return m_logwritelevel; }
		static int32_t	logconsolelevel()	{ return m_logconsolelevel; }
		static int32_t	consumings()		{ return m_consumings; }
		static std::string&	xorkey()		{ return m_xorkey; }
		static int32_t	xorkeynum()			{ return m_xorkeynum; }
		static bool	isxor()					{ return m_isxor; }
		static bool varint()				{ return m_varint; }
		static bool robot_test()			{ return m_robot_test; }
		static int32_t kcpping()			{ return m_kcpping; }
		static int32_t kcppinginterval()	{ return m_kcppinginterval; }
		static std::string& kcpsession()	{ return m_kcpsession; }
		static int32_t sessionwait()		{ return m_sessionewait; }
		static int32_t open_servertime()	{ return m_open_servertime; }
		static int32_t head_version()		{ return m_head_version; }
		static int32_t rate_interval()		{ return m_rate_interval; }		
		static int32_t rate_count()			{ return m_rate_count; }
		static int32_t heart_beat_interval() { return m_heart_beat_interval; }
		static int32_t net_timeout()		{ return m_net_timeout; }
		static std::string& gmurl()			{ return m_gmurl; }
		static std::string& lua()			{ return m_lua; }
		
		static int32_t node_count(const char* anodename);
		static int32_t node_count(NODE_TYPE atype);

		static void init_gatewayids(int atcout);
		static std::set<i32_serverid>& gatewayids();
	};
}//namespace ngl