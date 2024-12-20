#include "manage_curl.h"
#include "json_write.h"
#include "json_read.h"
#include "nprotocol.h"
#include "nlog.h"

#include <thread>

namespace ngl
{
	struct _http::impl_http
	{
		ENUM_MODE		m_mode;
		ENUM_TYPE		m_type;
		CURL*			m_curl;
		std::string		m_url;
		std::string		m_param;
		int				m_timeout;
		std::string		m_cookies;
		curl_slist*		m_http_headers;
		std::function<void(int, _http&)> m_callback;

		impl_http() :
			m_curl(curl_easy_init()),
			m_mode(ENUM_MODE_NULL),
			m_type(ENUM_TYPE_NULL),
			m_timeout(0),
			m_http_headers(nullptr)
		{}

		~impl_http()
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

		void headers(std::vector<std::string>& m_headers)
		{
			int lsize = (int)m_headers.size();
			for (int i = 0; i < lsize; i++)
			{
				m_http_headers = curl_slist_append(m_http_headers, m_headers[i].c_str());
				curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_http_headers);
			}
		}

		void plog(int aerror, std::string& arecv)
		{
			ngl::log_info()->print("error[{}]url[{}]param[{}]mode[{}]type[{}]data[{}]"
				, aerror, m_url, m_param
				, (m_mode == ENUM_MODE_HTTP ? "http" : "https")
				, (m_type == ENUM_TYPE_POST ? "post" : "get")
				, arecv.c_str()
			);
		}
	};

	_http::_http()
	{
		m_impl_http.make_unique();
	}

	_http::~_http()
	{}

	void _http::headers(std::vector<std::string>& m_headers)
	{
		m_impl_http()->headers(m_headers);
	}

	void _http::log(int aerror)
	{
		m_impl_http()->plog(aerror, m_recvdata);
	}

	struct manage_curl::impl_manage_curl
	{
		std::unique_ptr<std::thread>			m_thread;
		work_list<std::shared_ptr<_http>>		m_list;

		impl_manage_curl() :
			m_thread(nullptr),
			m_list([this](std::shared_ptr<_http>& item)
				{
					work(*item);
				})
		{
		}

		void send(std::shared_ptr<_http>& adata)
		{
			m_list.push_back(adata);
		}

		void work(_http& ahttp)
		{
			CURLcode lretvalue = visit(ahttp);
			if (CURLE_OK != lretvalue)
			{
				ahttp.log(lretvalue);
				return;
			}
			if(ahttp.m_impl_http()->m_callback != nullptr)
				ahttp.m_impl_http()->m_callback(lretvalue, ahttp);
		}

		CURLcode visit(_http& ahttp)
		{
			std::unique_ptr<_http::impl_http>& lhttp = ahttp.m_impl_http();
			if (lhttp->m_type == ENUM_TYPE_POST)
			{
				//curl_easy_setopt(lhttp->m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
				curl_easy_setopt(lhttp->m_curl, CURLOPT_POST, 1);
				if (lhttp->m_param != "")
					curl_easy_setopt(lhttp->m_curl, CURLOPT_POSTFIELDS, lhttp->m_param.c_str());
				curl_easy_setopt(lhttp->m_curl, CURLOPT_URL, lhttp->m_url.c_str());
			}
			else//get
			{
				if (!lhttp->m_param.empty())
				{
					lhttp->m_url += '?';
					lhttp->m_url += lhttp->m_param;
				}
				curl_easy_setopt(lhttp->m_curl, CURLOPT_URL, lhttp->m_url.c_str());
			}
			curl_easy_setopt(lhttp->m_curl, CURLOPT_WRITEFUNCTION, &impl_manage_curl::callback_write);
			curl_easy_setopt(lhttp->m_curl, CURLOPT_WRITEDATA, &ahttp.m_recvdata);

			if (lhttp->m_mode == ENUM_MODE_HTTPS)
			{
				curl_easy_setopt(lhttp->m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
				curl_easy_setopt(lhttp->m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
			}

			if (!lhttp->m_cookies.empty())
			{
				curl_easy_setopt(lhttp->m_curl, CURLOPT_COOKIE, lhttp->m_cookies.c_str());
			}

			if (lhttp->m_timeout > 0)
			{
				//curl_easy_setopt( curl, CURLOPT_CONNECTTIMEOUT, 10);
				curl_easy_setopt(lhttp->m_curl, CURLOPT_TIMEOUT, lhttp->m_timeout);
			}
			else
			{
				curl_easy_setopt(lhttp->m_curl, CURLOPT_TIMEOUT, 10);
			}

			curl_easy_setopt(lhttp->m_curl, CURLOPT_NOSIGNAL, 1L);
			curl_easy_setopt(lhttp->m_curl, CURLOPT_FORBID_REUSE, 1L); //禁掉alarm后会有大量CLOSE_WAIT 

			return curl_easy_perform(lhttp->m_curl);
		}

		static size_t callback_write(void* buffer, size_t size, size_t nmemb, std::string* lpVoid)
		{
			if (buffer == nullptr)
			{
				return 0;
			}
			char* pData = (char*)buffer;
			lpVoid->append(pData, size * nmemb);
			return nmemb;
		}
	};

	manage_curl::manage_curl()
	{
		m_impl_manage_curl.make_unique();
	}

	manage_curl::~manage_curl()
	{
	}

	void manage_curl::set_mode(std::shared_ptr<_http>& ahttp, ENUM_MODE aval)
	{ 
		ahttp->m_impl_http()->m_mode = aval; 
	}

	void manage_curl::set_type(std::shared_ptr<_http>& ahttp, ENUM_TYPE aval)
	{ 
		ahttp->m_impl_http()->m_type = aval; 
	}

	void manage_curl::set_url(std::shared_ptr<_http>& ahttp, const std::string& aurl)
	{ 
		ahttp->m_impl_http()->m_url = aurl; 
	}

	void manage_curl::set_url(std::shared_ptr<_http>& ahttp, const char* aurl)
	{ 
		ahttp->m_impl_http()->m_url = aurl; 
	}

	void manage_curl::set_param(std::shared_ptr<_http>& ahttp, const std::string& astrparam)
	{ 
		ahttp->m_impl_http()->m_param = astrparam;
	}

	void manage_curl::set_headers(std::shared_ptr<_http>& ahttp, std::vector<std::string>& aheaders)
	{ 
		ahttp->headers(aheaders); 
	}

	void manage_curl::set_callback(std::shared_ptr<_http>& ahttp, std::function<void(int, _http&)> aback)
	{ 
		ahttp->m_impl_http()->m_callback = aback; 
	}

	void manage_curl::param(std::string& astrparam, const char* akey, const char* aval)
	{
		if (astrparam.empty() == false)
		{
			astrparam += '&';
		}
		astrparam += std::format("{}={}", akey, aval);
	}

	void manage_curl::param(std::string& astrparam, const char* akey, int aval)
	{
		if (astrparam.empty() == false)
		{
			astrparam += '&';
		}
		astrparam += std::format("{}={}", akey, aval);
	}

	void manage_curl::send(std::shared_ptr<_http>& adata)
	{
		getInstance().m_impl_manage_curl()->send(adata);
	}

	std::shared_ptr<_http> manage_curl::make_http()
	{
		return std::make_shared<_http>();
	}
	
	void test_manage_curl()
	{
		auto lhttp = ngl::manage_curl::make_http();
		ngl::manage_curl::set_mode(lhttp, ngl::ENUM_MODE_HTTPS);
		ngl::manage_curl::set_type(lhttp, ngl::ENUM_TYPE_POST);
		ngl::manage_curl::set_url(lhttp, "xxx");

		std::string lparm;
		std::stringstream lstream;
		lstream
			<< "userID=" << 7709523
			<< "token=" << "pU3T0Cq0mac3yUfLEf0jTFygIN8kGq8B"
			<< "yWpx3hWQHFhSnTCj#311#6KuRKuaAjLJ5sYRy";

		ngl::manage_curl::param(lparm, "appid", 311);
		ngl::manage_curl::param(lparm, "userID", 7709523);
		ngl::manage_curl::param(lparm, "token", "pU3T0Cq0mac3yUfLEf0jTFygIN8kGq8B");
		ngl::manage_curl::param(lparm, "sign", tools::md5(lstream.str()).c_str());
		ngl::manage_curl::set_param(lhttp, lparm);
		bool lbool = true;

		ngl::manage_curl::set_callback(lhttp, [&lbool](int anum, ngl::_http& aparm)
			{
				log_error()->print("curl callback [{}]", aparm.m_recvdata);
				lbool = false;
			});
		ngl::manage_curl::send(lhttp);
		while (lbool)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		}
	}
}//namespace ngl