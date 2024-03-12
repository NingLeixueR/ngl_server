#pragma once

#include <functional>
#include <string>
#include <curl.h>
#include <list>

#include "threadtools.h"
#include "nlog.h"
#include "impl.h"

#ifdef WIN32
#pragma comment ( lib, "ws2_32.lib" )
#pragma comment ( lib, "winmm.lib" )
#pragma comment ( lib, "wldap32.lib" )
#pragma comment ( lib, "Crypt32.lib" )
//#pragma comment(lib,"libcurl-d.lib")
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
		struct impl_manage_curl;
		ngl::impl<impl_manage_curl> m_impl_manage_curl;

		manage_curl();
		~manage_curl();
	public:
		static manage_curl& getInstance()
		{
			static manage_curl temp;
			return temp;
		}
		/** 设置http类型 */
		static void set_mode(_http& ahttp, ENUM_MODE aval);
		static void set_type(_http& ahttp, ENUM_TYPE aval);
		static void set_url(_http& ahttp, const std::string& aurl);
		static void set_url(_http& ahttp, const char* aurl);
		//xx=xx&xx=xx&xx=xx
		static void set_param(_http& ahttp, const std::string& astrparam);
		static void set_headers(_http& ahttp, std::vector<std::string>& aheaders);
		static void set_callback(_http& ahttp, std::function<void(int, _http&)> aback);
		static void param(std::string& astrparam, const char* akey, const char* aval);
		static void param(std::string& astrparam, const char* akey, int aval);
		void send(_http* adata);
		static _http* make_http();
	};

	void test_manage_curl();
}//namespace ngl
