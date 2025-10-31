/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "net_protocol.h"

namespace ngl
{
	class net_tcp : public net_protocol
	{
		net_tcp() = delete;
		net_tcp(const net_tcp&) = delete;
		net_tcp& operator=(const net_tcp&) = delete;

		asio_tcp*				m_server;
		std::vector<segpack*>	m_segpackvec;
	private:
		bool socket_recv(service_io* ap, const char* abuff, int32_t abufflen);
	public:
		net_tcp(int8_t aindex):
			net_protocol(aindex),
			m_server(nullptr)
		{}

		// # �����˿� �߳�����
		virtual bool init(i16_port aport, i32_threadsize asocketthreadnum, bool	aouternet);

		// # �ر�session
		virtual void close_net(i32_sessionid asession);
		
		// # ����ip:aport
		virtual bool connect(const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun);
		
		// # ����socket�رպ�Ķ�������,afun:connect��parm3
		virtual void set_close(int asession, const std::string& aip, i16_port aport, const std::function<void(i32_sessionid)>& afun);

		// # ������Ϣ
		virtual bool net_send(i32_sessionid asession, std::shared_ptr<pack>& lpack);

		// # ������Ϣ
		virtual bool net_send(i32_sessionid asession, std::shared_ptr<void>& lpack);	

		// # ��ѯsession�Ƿ����
		virtual bool exist_session(i32_sessionid asession);
	};
}// namespace ngl