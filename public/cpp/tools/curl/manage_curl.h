#pragma once

#include "threadtools.h"
#include "nlog.h"
#include "impl.h"

#include <functional>
#include <string>
#include <curl.h>
#include <list>

#ifdef WIN32
#pragma comment ( lib, "ws2_32.lib" )
#pragma comment ( lib, "winmm.lib" )
#pragma comment ( lib, "wldap32.lib" )
#pragma comment ( lib, "Crypt32.lib" )
#endif //WIN32

namespace ngl
{
	enum ENUM_MODE
	{
		ENUM_MODE_NULL,		// 未设置
		ENUM_MODE_HTTP,		// http方式
		ENUM_MODE_HTTPS,	// https方式
	};

	enum ENUM_TYPE
	{
		ENUM_TYPE_NULL,		// 未设置
		ENUM_TYPE_POST,		// POST方式
		ENUM_TYPE_GET,		// GET方式
	};

	struct _http
	{
		struct impl_http;
		impl<impl_http> m_impl_http;

		std::string m_recvdata;

		_http();
		~_http();
		void headers(std::vector<std::string>& m_headers);
		void log(int aerror);		
	};

	class manage_curl
	{
		manage_curl(const manage_curl&) = delete;
		manage_curl& operator=(const manage_curl&) = delete;

		struct impl_manage_curl;
		ngl::impl<impl_manage_curl> m_impl_manage_curl;

		manage_curl();
		~manage_curl();

		static manage_curl& getInstance()
		{
			static manage_curl temp;
			return temp;
		}
	public:
		// # 设置http类型
		static void set_mode(std::shared_ptr<_http>& ahttp, ENUM_MODE aval);

		// # 设置post/get类型
		static void set_type(std::shared_ptr<_http>& ahttp, ENUM_TYPE aval);

		// # 设置url
		static void set_url(std::shared_ptr<_http>& ahttp, const std::string& aurl);
		static void set_url(std::shared_ptr<_http>& ahttp, const char* aurl);

		// # 设置访问参数(parm xx=xx&xx=xx&xx=xx)
		static void set_param(std::shared_ptr<_http>& ahttp, const std::string& astrparam);

		// # 设置http头 
		static void set_headers(std::shared_ptr<_http>& ahttp, std::vector<std::string>& aheaders);

		// # 设置回调
		static void set_callback(std::shared_ptr<_http>& ahttp, std::function<void(int, _http&)> aback);

		// # 辅助设置http访问参数
		static void param(std::string& astrparam, const char* akey, const char* aval);
		static void param(std::string& astrparam, const char* akey, int aval);
		
		// # 发送
		static void send(std::shared_ptr<_http>& adata);

		static std::shared_ptr<_http> make_http();
	};

	void test_manage_curl();
}//namespace ngl
