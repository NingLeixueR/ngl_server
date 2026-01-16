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
#include "manage_curl.h"
#include "nprotocol.h"

#include <thread>
#include <format>
#include <utf8.h> 

namespace ngl
{
	manage_curl::manage_curl() :
		m_thread(nullptr),
		m_list([this](std::shared_ptr<http_parm>& item)
			{
				work(*item);
			})
	{}

	manage_curl::~manage_curl()
	{
	}

	void manage_curl::send(std::shared_ptr<http_parm>& adata)
	{
		instance().m_list.push_back(adata);
	}

	void manage_curl::work(http_parm& ahttp)
	{
		CURLcode lretvalue = visit(ahttp);
		if (CURLE_OK != lretvalue)
		{
			ahttp.log(lretvalue);
			return;
		}
		if (ahttp.m_callback != nullptr)
		{
			ahttp.m_callback(lretvalue, ahttp);
		}
	}

	CURLcode manage_curl::visit(http_parm& ahttp)
	{
		if (ahttp.m_type == ENUM_TYPE_POST)
		{
			//curl_easy_setopt(lhttp->m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(ahttp.m_curl, CURLOPT_POST, 1);
			if (ahttp.m_param != "")
			{
				curl_easy_setopt(ahttp.m_curl, CURLOPT_POSTFIELDS, ahttp.m_param.c_str());
			}
			curl_easy_setopt(ahttp.m_curl, CURLOPT_URL, ahttp.m_url.c_str());
		}
		else//get
		{
			if (!ahttp.m_param.empty())
			{
				ahttp.m_url += std::format("?{}", ahttp.m_param);
			}
			curl_easy_setopt(ahttp.m_curl, CURLOPT_URL, ahttp.m_url.c_str());
		}
		curl_easy_setopt(ahttp.m_curl, CURLOPT_WRITEFUNCTION, &manage_curl::callback_write);
		curl_easy_setopt(ahttp.m_curl, CURLOPT_WRITEDATA, &ahttp.m_recvdata);

		if (ahttp.m_mode == ENUM_MODE_HTTPS)
		{
			curl_easy_setopt(ahttp.m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(ahttp.m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
		}

		if (!ahttp.m_cookies.empty())
		{
			curl_easy_setopt(ahttp.m_curl, CURLOPT_COOKIE, ahttp.m_cookies.c_str());
		}

		if (ahttp.m_timeout > 0)
		{
			//curl_easy_setopt( curl, CURLOPT_CONNECTTIMEOUT, 10);
			curl_easy_setopt(ahttp.m_curl, CURLOPT_TIMEOUT, ahttp.m_timeout);
		}
		else
		{
			curl_easy_setopt(ahttp.m_curl, CURLOPT_TIMEOUT, 10);
		}

		curl_easy_setopt(ahttp.m_curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(ahttp.m_curl, CURLOPT_FORBID_REUSE, 1L); //禁掉alarm后会有大量CLOSE_WAIT 

		return curl_easy_perform(ahttp.m_curl);
	}

	size_t manage_curl::callback_write(void* buffer, size_t size, size_t nmemb, std::string* lpVoid)
	{
		if (buffer == nullptr)
		{
			return 0;
		}
		char* pData = (char*)buffer;
		lpVoid->append(pData, size * nmemb);
		return nmemb;
	}

	void manage_curl::set_mode(std::shared_ptr<http_parm>& ahttp, ENUM_MODE aval)
	{ 
		ahttp->m_mode = aval; 
	}

	void manage_curl::set_type(std::shared_ptr<http_parm>& ahttp, ENUM_TYPE aval)
	{ 
		ahttp->m_type = aval; 
	}

	void manage_curl::set_url(std::shared_ptr<http_parm>& ahttp, const std::string& aurl)
	{ 
		ahttp->m_url = aurl; 
	}

	void manage_curl::set_url(std::shared_ptr<http_parm>& ahttp, const char* aurl)
	{ 
		ahttp->m_url = aurl; 
	}

	void manage_curl::set_param(std::shared_ptr<http_parm>& ahttp, const std::string& aparam)
	{ 
		ahttp->m_param = aparam;
	}

	void manage_curl::set_headers(std::shared_ptr<http_parm>& ahttp, std::vector<std::string>& aheaders)
	{ 
		ahttp->headers(aheaders); 
	}

	void manage_curl::set_callback(std::shared_ptr<http_parm>& ahttp, const std::function<void(int, http_parm&)>& aback)
	{ 
		ahttp->m_callback = aback; 
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
		static email_sender& instance()
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
		size_t to_copy = std::min(data.size() - email_sender::instance().m_index, size * nmemb);
		memcpy(ptr, data.c_str() + email_sender::instance().m_index, to_copy);
		email_sender::instance().m_index += (int32_t)to_copy;
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
			curl_slist* recipients = nullptr;

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
		ngl::email_sender::instance().send(aparm);
	}

	void test_mail(const char* atitle, const char* acontent, const std::vector<std::pair<std::string, std::string>>& amailvec/* = {}*/)
	{
		ngl::manage_curl::parameter lparm
		{
			.m_smtp = nconfig.mail().m_smtp,
			.m_email = nconfig.mail().m_email,
			.m_password = nconfig.mail().m_password,
			.m_name = nconfig.mail().m_name,
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
		
	}
}//namespace ngl