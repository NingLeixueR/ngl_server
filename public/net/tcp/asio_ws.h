#pragma once

#include "asio_base.h"
#include "pack.h"
#include "nlog.h"
#include "threadtools.h"

#include "impl.h"


namespace ngl
{
	using ws_callback = std::function<bool(service_ws*, const char*, uint32_t)>;
	using ws_closecallback = std::function<void(i32_sessionid)>;
	using ws_connectcallback = std::function<void(i32_sessionid)>;
	using ws_sendfinishcallback = std::function<void(i32_sessionid, bool, pack*)>;

	class asio_ws
	{
	public:
		friend class service_io;

		// ---服务器
		// i16_port						aport		监听端口
		// i32_threadsize				athread		线程数
		// const tcp_callback&			acallfun	回调
		// const tcp_closecallback&		aclosefun	关闭回调
		// const tcp_sendfinishcallback& asenderrfun 发送失败的回调
		asio_ws(
			i16_port aport, 
			i32_threadsize athread, 
			const ws_callback& acallfun, 
			const ws_closecallback& aclosefun, 
			const ws_sendfinishcallback& asendfinishfun
		);

		// ---客户端
		// i32_threadsize				athread		线程数
		// const tcp_callback&			acallfun	回调
		// const tcp_closecallback&		aclosefun	关闭回调
		// const tcp_senderrorcallback& asenderrfun 发送失败的回调
		asio_ws(
			i32_threadsize athread, 
			const ws_callback& acallfun, 
			const ws_closecallback& aclosefun, 
			const ws_sendfinishcallback& asendfinishfun
		);

		service_ws* connect(const str_host& ahost, i16_port aport, const ws_connectcallback& afun, int acount = 5);

		bool sendpack(i32_sessionid asessionid, std::shared_ptr<pack>& apack);
		bool sendpack(i32_sessionid asessionid, std::shared_ptr<void>& apack);

		void close(i32_sessionid sessionid);
		void close(service_ws* ap);

		void close_net(i32_sessionid sessionid);

		bool sessionid2ipport(i32_sessionid assionid, std::pair<str_ip, i16_port>& apair);
		bool exist_session(i32_sessionid asession);

		void set_close(i32_sessionid asession, const std::function<void()>& afun);
	private:
		struct impl_asio_ws;
		ngl::impl<impl_asio_ws> m_impl_asio_ws;
	};

}

