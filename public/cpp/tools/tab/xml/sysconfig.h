#pragma once

#include "type.h"
#include <vector>

namespace ngl
{
	enum ELOGLEVEL
	{
		ELOG_NONE = -1,		// ��
		ELOG_DEBUG = 0,		// ������Ϣ
		ELOG_INFO,			// ��ӡ
		ELOG_WARN,			// ����
		ELOG_ERROR,			// ����
	};

	struct sysconfig
	{
	private:
		//##### ��־���
		static ELOGLEVEL	m_loglevel;				// ��־�ȼ�
		static int32_t		m_logline;				// ������־�ļ�������
		static int32_t		m_logflushtime;			// ��־flushʱ��
		static bool			m_logiswrite;			// ��־�Ƿ�д���ļ�
		static bool			m_logconsole;			// �Ƿ��ӡ������̨

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

		static std::string	m_gmurl;				// gm url

		// # �����б�
		static std::vector<i32_serverid> m_gatewayids;
	public:
		static void init();

		static ELOGLEVEL loglevel()		{ return m_loglevel; }
		static int32_t logline()		{ return m_logline; }
		static int32_t logflushtime()	{ return m_logflushtime; }
		static bool logiswrite()		{ return m_logiswrite; }
		static bool logconsole()		{ return m_logconsole; }
		static int32_t consumings()		{ return m_consumings; }
		static std::string& xorkey()	{ return m_xorkey; }
		static int32_t xorkeynum()		{ return m_xorkeynum; }
		static bool isxor()				{ return m_isxor; }
		static bool varint()			{ return m_varint; }
		static bool robot_test()		{ return m_robot_test; }
		static int32_t kcpping()		{ return m_kcpping; }
		static int32_t kcppinginterval(){ return m_kcppinginterval; }
		static std::string& kcpsession(){ return m_kcpsession; }
		static int32_t sessionwait()	{ return m_sessionewait; }
		static int32_t open_servertime(){ return m_open_servertime; }
		static int32_t head_version()	{ return m_head_version; }
		static int32_t rate_interval()	{ return m_rate_interval; }		
		static int32_t rate_count()		{ return m_rate_count; }
		static int32_t heart_beat_interval() { return m_heart_beat_interval; }
		static int32_t net_timeout()	{ return m_net_timeout; }
		static std::string& gmurl()		{ return m_gmurl; }
		
		static void init_gatewayids();
		static std::vector<i32_serverid>& gatewayids();
	};
}//namespace ngl