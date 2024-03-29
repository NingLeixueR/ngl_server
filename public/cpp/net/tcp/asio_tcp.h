#pragma once

#include "threadtools.h"
#include "asio_base.h"
#include "pack.h"
#include "nlog.h"
#include "impl.h"

namespace ngl
{
	using tcp_callback				= std::function<bool(service_tcp*, const char*, uint32_t)>;
	using tcp_closecallback			= std::function<void(i32_sessionid)>;
	using tcp_connectcallback		= std::function<void(i32_sessionid)>;
	using tcp_sendfinishcallback	= std::function<void(i32_sessionid, bool, pack*)>;

	class asio_tcp
	{
	public:
		friend class service_tcp;

		// ---服务器
		asio_tcp(
			i8_sesindex aindex
			, i16_port aport									// 监听端口
			, i32_threadsize athread							// 线程数
			, const tcp_callback& acallfun						// 回调
			, const tcp_closecallback& aclosefun				// 关闭回调
			, const tcp_sendfinishcallback& asendfinishfun		// 发送失败的回调
		);

		// ---客户端
		asio_tcp(
			i8_sesindex aindex
			, i32_threadsize athread								// 线程数
			, const tcp_callback& acallfun						// 回调
			, const tcp_closecallback& aclosefun				// 关闭回调
			, const tcp_sendfinishcallback& asendfinishfun		//发送失败的回调
		);

		service_tcp* connect(
			const str_ip& aip							// ip
			, i16_port aport							// 端口
			, const tcp_connectcallback& afun			// 连接回调
			, int acount = 5							// 连接失败自动重连次数
		);

		bool sendpack(i32_sessionid asessionid, std::shared_ptr<pack>& apack);
		bool sendpack(i32_sessionid asessionid, std::shared_ptr<void>& apack);

		void close(i32_sessionid sessionid);
		void close(service_tcp* ap);

		void close_net(i32_sessionid sessionid);

		bool sessionid2ipport(i32_sessionid assionid, std::pair<str_ip, i16_port>& apair);

		bool exist_session(i32_sessionid asession);

		void set_close(i32_sessionid asession, const std::function<void()>& afun);
	private:
		struct impl_asio_tcp;
		ngl::impl<impl_asio_tcp> m_impl_asio_tcp;
	};	
}// namespace ngl