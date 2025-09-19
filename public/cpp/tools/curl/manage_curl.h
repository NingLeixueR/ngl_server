#pragma once

#include "threadtools.h"
#include "nprotocol.h"
#include "worklist.h"
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
		ENUM_MODE_NULL,		// δ����
		ENUM_MODE_HTTP,		// http��ʽ
		ENUM_MODE_HTTPS,	// https��ʽ
	};

	enum ENUM_TYPE
	{
		ENUM_TYPE_NULL,		// δ����
		ENUM_TYPE_POST,		// POST��ʽ
		ENUM_TYPE_GET,		// GET��ʽ
	};

	struct http_parm
	{
		ENUM_MODE			m_mode = ENUM_MODE_NULL;	// httpģʽ
		ENUM_TYPE			m_type = ENUM_TYPE_NULL;	// http����
		CURL*				m_curl = nullptr;			// curlָ��
		std::string			m_url;						// �����url
		std::string			m_param;					// �������
		int					m_timeout = 0;				// ��ʱʱ��
		std::string			m_cookies;					// cookie
		curl_slist*			m_http_headers = nullptr;	// httpͷ
		using callback = std::function<void(int, http_parm&)>;
		callback			m_callback;					// �ص�
		std::string			m_recvdata;					// ���յ�����

		http_parm()
		{}

		~http_parm()
		{
			if (m_http_headers != nullptr)
			{
				curl_slist_free_all(m_http_headers);
				m_http_headers = nullptr;
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
				m_http_headers = curl_slist_append(m_http_headers, aheaders[i].c_str());
				curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_http_headers);
			}
		}

		void log(int aerror)
		{
			log_info()->print("error[{}]url[{}]param[{}]mode[{}]type[{}]data[{}]"
				, aerror, m_url, m_param
				, (m_mode == ENUM_MODE_HTTP ? "http" : "https")
				, (m_type == ENUM_TYPE_POST ? "post" : "get")
				, m_recvdata
			);
		}
	};

	class manage_curl
	{
		manage_curl(const manage_curl&) = delete;
		manage_curl& operator=(const manage_curl&) = delete;

		std::unique_ptr<std::thread>			m_thread;
		worklist<std::shared_ptr<http_parm>>	m_list;

		manage_curl();

		~manage_curl();

		static manage_curl& instance()
		{
			static manage_curl temp;
			return temp;
		}

		void work(http_parm& ahttp);

		CURLcode visit(http_parm& ahttp);

		static size_t callback_write(void* buffer, size_t size, size_t nmemb, std::string* lpVoid);
	public:
		// # ����http����
		static void set_mode(std::shared_ptr<http_parm>& ahttp, ENUM_MODE aval);

		// # ����post/get����
		static void set_type(std::shared_ptr<http_parm>& ahttp, ENUM_TYPE aval);

		// # ����url
		static void set_url(std::shared_ptr<http_parm>& ahttp, const std::string& aurl);
		static void set_url(std::shared_ptr<http_parm>& ahttp, const char* aurl);

		// # ���÷��ʲ���(parm xx=xx&xx=xx&xx=xx)
		static void set_param(std::shared_ptr<http_parm>& ahttp, const std::string& astrparam);

		// # ����httpͷ 
		static void set_headers(std::shared_ptr<http_parm>& ahttp, std::vector<std::string>& aheaders);

		// # ���ûص�
		static void set_callback(std::shared_ptr<http_parm>& ahttp, const std::function<void(int, http_parm&)>& aback);

		// # ��������http���ʲ���
		static void param(std::string& astrparam, const char* akey, const std::string& aval);
		static void param(std::string& astrparam, const char* akey, const char* aval);
		static void param(std::string& astrparam, const char* akey, int aval);
		
		// # ����
		static void send(std::shared_ptr<http_parm>& adata);

		static std::shared_ptr<http_parm> make_http();

		struct parameter
		{
			std::string m_smtp;
			std::string m_email;
			std::string m_password;
			std::string m_name;
			std::string m_title;
			std::string m_content;
			std::vector<std::pair<std::string, std::string>> m_recvs;// key:mail value:name
		};
		static void sendemail(const parameter& aparm);
	};

	void test_manage_curl();
	void test_mail(const char* atitle, const char* acontent, const std::vector<std::pair<std::string, std::string>>& amailvec = {});
}//namespace ngl