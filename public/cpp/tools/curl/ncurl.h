/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "threadtools.h"
#include "nprotocol.h"
#include "nwork.h"
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

	struct http_parm
	{
		using callback = std::function<void(int, http_parm&)>;

		ENUM_MODE			m_mode = ENUM_MODE_NULL;				// http模式
		ENUM_TYPE			m_type = ENUM_TYPE_NULL;				// http类型
		CURL*				m_curl = nullptr;						// curl指针
		std::string			m_url;									// 请求的url
		std::string			m_param;								// 请求参数
		int					m_timeout = 0;							// 超时时间
		std::string			m_cookies;								// cookie
		curl_slist*			m_headers = nullptr;				// http头
		callback			m_callback = nullptr;					// 回调
		std::string			m_recvdata;								// 接收的数据

		~http_parm()
		{
			if (m_headers != nullptr)
			{
				curl_slist_free_all(m_headers);
				m_headers = nullptr;
			}
			if (m_curl != nullptr)
			{
				curl_easy_cleanup(m_curl);
				m_curl = nullptr;
			}
		}

		void headers(std::vector<std::string>& aheaders)
		{
			int lsize = (int)aheaders.size();
			for (int i = 0; i < lsize; i++)
			{
				m_headers = curl_slist_append(m_headers, aheaders[i].c_str());
				curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headers);
			}
		}

		void log(int aerror)
		{
			log_info()->print(
				"**********************[http_parm]***********************\n"
				"error:[{}]\n"
				"url:[{}]\n"
				"param[{}]\n"
				"mode[{}]\n"
				"type[{}]\n"
				"data[{}]\n"
				"********************************************************"
				, aerror, m_url, m_param
				, (m_mode == ENUM_MODE_HTTP ? "http" : "https")
				, (m_type == ENUM_TYPE_POST ? "post" : "get")
				, m_recvdata
			);
		}
	};

	struct mail_param
	{
	private:
		std::shared_ptr<ngl::sem> m_sem = nullptr;								
	public:
		std::string m_smtp;
		std::string m_email;
		std::string m_password;
		std::string m_name;
		std::string m_title;
		std::string m_content;
		std::vector<std::pair<std::string, std::string>> m_recvs;// key:mail value:name

		void wait()
		{
			if (m_sem != nullptr)
			{
				m_sem->wait();
			}
		}

		void set_wait()
		{
			m_sem = std::make_shared<ngl::sem>();
		}
		
		void post()
		{
			if (m_sem != nullptr)
			{
				m_sem->post();
			}
		}
	};

	class ncurl
	{
		ncurl(const ncurl&) = delete;
		ncurl& operator=(const ncurl&) = delete;

		nwork<http_parm> m_works;

		ncurl();

		~ncurl() = default;

		static ncurl& instance()
		{
			static ncurl temp;
			return temp;
		}

		void work(http_parm& ahttp);

		CURLcode visit(http_parm& ahttp);

		static size_t callback(void* buffer, size_t size, size_t nmemb, std::string* lpVoid);
	public:
		// # 设置http类型
		static void set_mode(std::shared_ptr<http_parm>& ahttp, ENUM_MODE aval);

		// # 设置post/get类型
		static void set_type(std::shared_ptr<http_parm>& ahttp, ENUM_TYPE aval);

		// # 设置url
		static void set_url(std::shared_ptr<http_parm>& ahttp, const std::string& aurl);
		static void set_url(std::shared_ptr<http_parm>& ahttp, const char* aurl);

		// # 设置访问参数(parm xx=xx&xx=xx&xx=xx)
		static void set_param(std::shared_ptr<http_parm>& ahttp, const std::string& aparam);

		// # 设置http头 
		static void set_headers(std::shared_ptr<http_parm>& ahttp, std::vector<std::string>& aheaders);

		// # 设置回调
		static void set_callback(std::shared_ptr<http_parm>& ahttp, const std::function<void(int, http_parm&)>& aback);

		// # 辅助设置http访问参数
		template <typename T>
		static void param(std::string& aparam, const char* akey, const T& aval)
		{
			if (aparam.empty() == false)
			{
				aparam += '&';
			}
			aparam += std::format("{}={}", akey, aval);
		}

		template <typename ...TARGS, int32_t... INDEX>
		static void param(std::string& aparam, std::index_sequence<INDEX...>, const std::array<const char*, sizeof...(TARGS)>& akeys, TARGS&... aargs)
		{
			(param(aparam, akeys[INDEX], aargs), ...);
		}

		template <typename ...TARGS>
		static void param(std::string& aparam, const std::array<const char*, sizeof...(TARGS)>& akeys, TARGS&... aargs)
		{
			param(aparam, std::make_index_sequence<sizeof...(TARGS)>{}, akeys, aargs...);
		}
		
		// # 发送
		static void send(std::shared_ptr<http_parm>& adata);

		static std::shared_ptr<http_parm> http();

		static std::shared_ptr<mail_param> mail();

		static void sendemail(std::shared_ptr<mail_param>& aparm);
	};

	void test_manage_curl();
	void test_mail(const char* atitle, const char* acontent, const std::vector<std::pair<std::string, std::string>>& amailvec = {});
}//namespace ngl