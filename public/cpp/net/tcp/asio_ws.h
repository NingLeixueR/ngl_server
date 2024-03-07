#pragma once

#include "threadtools.h"
#include "asio_base.h"
#include "pack.h"
#include "nlog.h"
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
		asio_ws(
			i16_port aport									// 监听端口
			, i32_threadsize athread						// 线程数
			, const ws_callback& acallfun					// 回调
			, const ws_closecallback& aclosefun				// 关闭回调
			, const ws_sendfinishcallback& asendfinishfun	// 发送失败的回调
		);

		// ---客户端
		asio_ws(
			i32_threadsize athread							// 线程数
			, const ws_callback& acallfun					// 回调	
			, const ws_closecallback& aclosefun				// 关闭回调	
			, const ws_sendfinishcallback& asendfinishfun	// 发送失败的回调			
		);

		service_ws* connect(
			const str_host& ahost						// ip
			, i16_port aport							// 端口
			, const ws_connectcallback& afun			// 连接回调
			, int acount = 5							// 连接失败自动重连次数
		);

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

