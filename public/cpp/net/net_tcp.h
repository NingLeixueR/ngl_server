#pragma once

#include "net_protocol.h"

namespace ngl
{
	class net_tcp : public net_protocol
	{
		net_tcp(const net_tcp&) = delete;
		net_tcp& operator=(const net_tcp&) = delete;

		asio_tcp*				m_server;
		std::vector<segpack*>	m_segpackvec;
	private:
		bool socket_recv(service_io* ap, const char* abuff, uint32_t abufflen);
	public:
		net_tcp(int8_t aindex):
			net_protocol(aindex),
			m_server(nullptr)
		{}

		virtual bool init(i16_port aport, i32_threadsize asocketthreadnum, bool	aouternet);

		virtual void close_net(i32_sessionid asession);
		
		virtual bool connect(
			const std::string& aip, 
			i16_port aport, 
			const std::function<void(i32_sessionid)>& afun
		);
		
		virtual void set_close(
			int asession, const std::string& aip, i16_port aport, 
			const std::function<void(i32_sessionid)>& afun
		);

		//# 发送消息
		virtual bool net_send(i32_sessionid asession, std::shared_ptr<pack>& lpack);

		//# 发送消息
		virtual bool net_send(i32_sessionid asession, std::shared_ptr<void>& lpack);	

		//# 查询session是否存在
		virtual bool exist_session(i32_sessionid asession);
	};
}// namespace ngl