#include "manage_curl.h"
#include "json_write.h"
#include "json_read.h"
#include "nprotocol.h"
#include "nlog.h"

#include <thread>
#include <format>
#include <utf8.h> 

namespace ngl
{
	struct http_parm::impl_http
	{
		ENUM_MODE		m_mode;
		ENUM_TYPE		m_type;
		CURL*			m_curl;
		std::string		m_url;
		std::string		m_param;
		int				m_timeout;
		std::string		m_cookies;
		curl_slist*		m_http_headers;
		std::function<void(int, http_parm&)> m_callback;

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

	http_parm::http_parm()
	{
		m_impl_http.make_unique();
	}

	http_parm::~http_parm()
	{}

	void http_parm::headers(std::vector<std::string>& m_headers)
	{
		m_impl_http()->headers(m_headers);
	}

	void http_parm::log(int aerror)
	{
		m_impl_http()->plog(aerror, m_recvdata);
	}

	struct manage_curl::impl_manage_curl
	{
		std::unique_ptr<std::thread>			m_thread;
		work_list<std::shared_ptr<http_parm>>		m_list;

		impl_manage_curl() :
			m_thread(nullptr),
			m_list([this](std::shared_ptr<http_parm>& item)
				{
					work(*item);
				})
		{
		}

		void send(std::shared_ptr<http_parm>& adata)
		{
			m_list.push_back(adata);
		}

		void work(http_parm& ahttp)
		{
			CURLcode lretvalue = visit(ahttp);
			if (CURLE_OK != lretvalue)
			{
				ahttp.log(lretvalue);
				return;
			}
			if (ahttp.m_impl_http()->m_callback != nullptr)
			{
				ahttp.m_impl_http()->m_callback(lretvalue, ahttp);
			}
		}

		CURLcode visit(http_parm& ahttp)
		{
			std::unique_ptr<http_parm::impl_http>& lhttp = ahttp.m_impl_http();
			if (lhttp->m_type == ENUM_TYPE_POST)
			{
				//curl_easy_setopt(lhttp->m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
				curl_easy_setopt(lhttp->m_curl, CURLOPT_POST, 1);
				if (lhttp->m_param != "")
				{
					curl_easy_setopt(lhttp->m_curl, CURLOPT_POSTFIELDS, lhttp->m_param.c_str());
				}
				curl_easy_setopt(lhttp->m_curl, CURLOPT_URL, lhttp->m_url.c_str());
			}
			else//get
			{
				if (!lhttp->m_param.empty())
				{
					lhttp->m_url += std::format("?{}", lhttp->m_param);
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

	void manage_curl::set_mode(std::shared_ptr<http_parm>& ahttp, ENUM_MODE aval)
	{ 
		ahttp->m_impl_http()->m_mode = aval; 
	}

	void manage_curl::set_type(std::shared_ptr<http_parm>& ahttp, ENUM_TYPE aval)
	{ 
		ahttp->m_impl_http()->m_type = aval; 
	}

	void manage_curl::set_url(std::shared_ptr<http_parm>& ahttp, const std::string& aurl)
	{ 
		ahttp->m_impl_http()->m_url = aurl; 
	}

	void manage_curl::set_url(std::shared_ptr<http_parm>& ahttp, const char* aurl)
	{ 
		ahttp->m_impl_http()->m_url = aurl; 
	}

	void manage_curl::set_param(std::shared_ptr<http_parm>& ahttp, const std::string& astrparam)
	{ 
		ahttp->m_impl_http()->m_param = astrparam;
	}

	void manage_curl::set_headers(std::shared_ptr<http_parm>& ahttp, std::vector<std::string>& aheaders)
	{ 
		ahttp->headers(aheaders); 
	}

	void manage_curl::set_callback(std::shared_ptr<http_parm>& ahttp, std::function<void(int, http_parm&)> aback)
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

	void manage_curl::send(std::shared_ptr<http_parm>& adata)
	{
		getInstance().m_impl_manage_curl()->send(adata);
	}

	std::shared_ptr<http_parm> manage_curl::make_http()
	{
		return std::make_shared<http_parm>();
	}

	class email_sender
	{
		email_sender();
		~email_sender() = default;
	public:
		static email_sender& getInstance()
		{
			static email_sender ltemp;
			return ltemp;
		}
		void send(const manage_curl::parameter& aparm);

	private:
		static size_t callback(void* ptr, size_t size, size_t nmemb, void* userp);
		void sendmail(const manage_curl::parameter& aparm);
		void run();

		std::list<manage_curl::parameter>	m_list; 
		int32_t								m_index = 0;
	};

	struct email_sender_helper
	{
		static std::jthread				m_thread;
		static std::shared_mutex		m_mutex;
		static ngl::sem					m_sem;
	};

	std::jthread		email_sender_helper::m_thread;
	std::shared_mutex	email_sender_helper::m_mutex;
	ngl::sem			email_sender_helper::m_sem;

	email_sender::email_sender()
	{
		email_sender_helper::m_thread = std::jthread(std::bind_front(&email_sender::run, this));
	}

	size_t email_sender::callback(void* ptr, size_t size, size_t nmemb, void* userp)
	{
		std::string data((const char*)userp);
		size_t to_copy = std::min(data.size() - email_sender::getInstance().m_index, size * nmemb);
		memcpy(ptr, data.c_str() + email_sender::getInstance().m_index, to_copy);
		email_sender::getInstance().m_index += to_copy;
		return to_copy;
	}

	void email_sender::sendmail(const manage_curl::parameter& aparm)
	{
		m_index = 0;
		CURL* curl;
		CURLcode res;

		// 初始化libcurl
		curl = curl_easy_init();

		if (curl)
		{
			curl_slist* recipients = NULL;

			// 设置SMTP服务器、发件人和收件人
			curl_easy_setopt(curl, CURLOPT_URL, aparm.m_smtp.c_str());
			curl_easy_setopt(curl, CURLOPT_MAIL_FROM, aparm.m_email.c_str());
			// 设置SMTP认证信息
			curl_easy_setopt(curl, CURLOPT_USERNAME, aparm.m_email.c_str());
			curl_easy_setopt(curl, CURLOPT_PASSWORD, aparm.m_password.c_str());

			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
			//是否打开调试信息
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
			curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);

			// 设置收件人
			for (const auto& [ mailarr, name ] : aparm.m_recvs)
			{
				recipients = curl_slist_append(recipients, std::format("<{}>", mailarr).c_str());
			}
			curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

			curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

			std::string payload = std::format("From: {}<{}>\r\n", aparm.m_name, aparm.m_email);
			payload += "To: ";
			for (int i = 0; i < aparm.m_recvs.size(); ++i)
			{
				if (i != 0)
				{
					payload += ",";
				}
				payload += std::format("{}<{}>", aparm.m_recvs[i].second, aparm.m_recvs[i].first);
			}
			payload += "\r\n";
			payload += std::format("Subject: {}\r\n", aparm.m_title);
			payload += "\r\n"; // 空行表示header部分结束
			payload += std::format("{}\r\n", aparm.m_content); // 邮件内容
						
			curl_easy_setopt(curl, CURLOPT_READDATA, payload.c_str());

			curl_easy_setopt(curl, CURLOPT_READFUNCTION, &email_sender::callback); // 设置读取数据的回调函数

			curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, nullptr);
			// 发送邮件
			res = curl_easy_perform(curl);

			// 检查结果
			if (res != CURLE_OK)
			{
				std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
			}
			else
			{
				std::cout << "email send successfully!" << std::endl;
			}

			// 清理
			curl_slist_free_all(recipients);
			curl_easy_cleanup(curl);
		}
	}

	void email_sender::run()
	{
		std::list<manage_curl::parameter> templist;
		while (true)
		{
			email_sender_helper::m_sem.wait();
			{
				monopoly_shared_lock(email_sender_helper::m_mutex);
				m_list.swap(templist);
			}
			for (manage_curl::parameter& item : templist)
			{
				sendmail(item);
			}
			templist.clear();
		}
	}

	void email_sender::send(const manage_curl::parameter& aparm)
	{
		monopoly_shared_lock(email_sender_helper::m_mutex);
		m_list.push_back(aparm);
		email_sender_helper::m_sem.post();
		return;
	}

	void manage_curl::sendemail(const parameter& aparm)
	{
		ngl::email_sender::getInstance().send(aparm);
	}

	void test_mail(const char* atitle, const char* acontent, const std::vector<std::pair<std::string, std::string>>& amailvec/* = {}*/)
	{
		ngl::manage_curl::parameter lparm
		{
			.m_smtp = xmlnode::m_mail.m_mailarg.m_smtp,
			.m_email = xmlnode::m_mail.m_mailarg.m_email,
			.m_password = xmlnode::m_mail.m_mailarg.m_password,
			.m_name = xmlnode::m_mail.m_mailarg.m_name,
			.m_title = atitle,
			.m_content = acontent,
			.m_recvs = amailvec
		};
		if (amailvec.empty())
		{
			lparm.m_recvs.emplace_back(std::make_pair("libo1@youxigu.com", "李博"));
			lparm.m_recvs.emplace_back(std::make_pair("348634371@qq.com", "李博QQ"));
		}
		manage_curl::sendemail(lparm);
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

		ngl::manage_curl::set_callback(lhttp, [&lbool](int anum, ngl::http_parm& aparm)
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

	void test_kkkk()
	{
		auto lhttp = ngl::manage_curl::make_http();
		ngl::manage_curl::set_mode(lhttp, ngl::ENUM_MODE_HTTPS);
		ngl::manage_curl::set_type(lhttp, ngl::ENUM_TYPE_GET);
		ngl::manage_curl::set_url(lhttp, "https://xxxxxxxxxxx");

		std::string lparm;
		std::string toCheck = "日本人sb3";
		std::string toCheck2;
		ngl::tools::to_utf8(toCheck, toCheck2);

		std::vector<uint32_t> utf32_str;
		utf8::utf8to32(toCheck2.begin(), toCheck2.end(), std::back_inserter(utf32_str));

		ngl::manage_curl::param(lparm, "toCheck", toCheck2.c_str());
		ngl::manage_curl::param(lparm, "app", "xxx");
		ngl::manage_curl::param(lparm, "byPinyin", "false");

		std::string lstrmd5 = "yyyy";
		lstrmd5 += toCheck2;

		std::string lmd5str = ngl::tools::md5(lstrmd5);
		ngl::manage_curl::param(lparm, "sig", lmd5str.c_str());

		ngl::manage_curl::set_param(lhttp, lparm);

		ngl::manage_curl::set_callback(lhttp, [&utf32_str](int anum, ngl::http_parm& aparm)
			{
				//{"3:4":{"app":"[]","level":2,"startPos":3,"endPos":4,"maskWord":"sb"},"0:1":{"app":"[]","level":2,"startPos":0,"endPos":1,"maskWord":"日本"}}
				std::cout << aparm.m_recvdata;
				cJSON* json = cJSON_Parse(aparm.m_recvdata.c_str());
				if (json == NULL) {
					printf("解析JSON失败!\n");
					return 1;
				}

				cJSON* item = NULL;
				for (item = (json != 0) ? (json)->child : 0; item != 0; item = item->next)
				{
					// 打印键
					printf("Key: %s\n", item->string);  // item->string 是键名
					int beg = 0;
					int end = 0;
					ngl::tools::splite(item->string, ":", beg, end);
					for (int i = beg; i <= end; ++i)
					{
						utf32_str[i] = '*';
					}
				}
				std::string result_utf8;
				utf8::utf32to8(utf32_str.begin(), utf32_str.end(), std::back_inserter(result_utf8));
				std::string result_utf82;
				ngl::tools::to_asscii(result_utf8, result_utf82);

				std::cout << std::endl << result_utf82;
			});
		ngl::manage_curl::send(lhttp);
	}
}//namespace ngl