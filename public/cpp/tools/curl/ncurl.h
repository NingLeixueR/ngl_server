/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for curl.

#pragma once

#include "actor/protocol/nprotocol.h"
#include "tools/threadtools.h"
#include "tools/log/nlog.h"
#include "tools/nwork.h"
#include "tools/impl.h"

#include <curl/curl.h>
#include <array>
#include <functional>
#include <memory>
#include <string>
#include <list>
#include <vector>

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
		ENUM_MODE_NULL,		// Set
		ENUM_MODE_HTTP,		// http
		ENUM_MODE_HTTPS,	// https
	};

	enum ENUM_TYPE
	{
		ENUM_TYPE_NULL,		// Set
		ENUM_TYPE_POST,		// POST
		ENUM_TYPE_GET,		// GET
	};

	struct http_parm
	{
		using callback = std::function<void(int, http_parm&)>;

		http_parm() :
			m_curl(curl_easy_init())
		{
		}

		http_parm(const http_parm&) = delete;
		http_parm& operator=(const http_parm&) = delete;
		http_parm(http_parm&&) = delete;
		http_parm& operator=(http_parm&&) = delete;

		ENUM_MODE			m_mode = ENUM_MODE_NULL;				// HTTP or HTTPS.
		ENUM_TYPE			m_type = ENUM_TYPE_NULL;				// GET or POST.
		CURL*				m_curl = nullptr;						// Reused easy handle.
		std::string			m_url;									// Base request URL.
		std::string			m_param;								// Query string or POST body.
		int					m_timeout = 0;							// Request timeout in seconds.
		std::string			m_cookies;								// Optional Cookie header.
		curl_slist*			m_headers = nullptr;					// Optional request headers.
		callback			m_callback = nullptr;					// Completion callback.
		std::string			m_recvdata;								// Response body buffer.

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

		void headers(const std::vector<std::string>& aheaders)
		{
			if (m_headers != nullptr)
			{
				curl_slist_free_all(m_headers);
				m_headers = nullptr;
			}
			for (const std::string& lheader : aheaders)
			{
				m_headers = curl_slist_append(m_headers, lheader.c_str());
			}
			if (m_curl != nullptr)
			{
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
		// Optional synchronization primitive so callers can wait for a worker
		// thread to finish sending the message.
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

		// Background worker queue that runs blocking libcurl requests off-thread.
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
		// # Sethttptype
		static void set_mode(std::shared_ptr<http_parm>& ahttp, ENUM_MODE aval);

		// # Setpost/gettype
		static void set_type(std::shared_ptr<http_parm>& ahttp, ENUM_TYPE aval);

		// # Seturl
		static void set_url(std::shared_ptr<http_parm>& ahttp, const std::string& aurl);
		static void set_url(std::shared_ptr<http_parm>& ahttp, const char* aurl);

		// Appends `key=value` pairs into the request parameter string.
		static void set_param(std::shared_ptr<http_parm>& ahttp, const std::string& aparam);

		// # Sethttp
		static void set_headers(std::shared_ptr<http_parm>& ahttp, const std::vector<std::string>& aheaders);

		// # Setcallback
		static void set_callback(std::shared_ptr<http_parm>& ahttp, const std::function<void(int, http_parm&)>& aback);

		// Helper for building query strings from parallel key/value lists.
		template <typename T>
		static void param(std::string& aparam, const char* akey, const T& aval)
		{
			if (akey == nullptr || *akey == '\0')
			{
				return;
			}
			if (aparam.empty() == false)
			{
				aparam += '&';
			}
			aparam += std::format("{}={}", akey, aval);
		}

		template <typename ...TARGS, std::size_t... INDEX>
		static void param(std::string& aparam, std::index_sequence<INDEX...>, const std::array<const char*, sizeof...(TARGS)>& akeys, const TARGS&... aargs)
		{
			(param(aparam, akeys[INDEX], aargs), ...);
		}

		template <typename ...TARGS>
		static void param(std::string& aparam, const std::array<const char*, sizeof...(TARGS)>& akeys, const TARGS&... aargs)
		{
			param(aparam, std::make_index_sequence<sizeof...(TARGS)>{}, akeys, aargs...);
		}
		
		// Queues an HTTP request on the background worker.
		static void send(std::shared_ptr<http_parm>& adata);

		static std::shared_ptr<http_parm> http();

		static std::shared_ptr<mail_param> mail();

		// Queues an SMTP send and optionally blocks until completion.
		static void sendemail(std::shared_ptr<mail_param>& aparm);
	};

	void test_manage_curl();
	void test_mail(const char* atitle, const char* acontent, const std::vector<std::pair<std::string, std::string>>& amailvec = {});
}//namespace ngl
