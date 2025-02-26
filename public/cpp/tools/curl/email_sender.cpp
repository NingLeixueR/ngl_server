#include "email_sender.h"

namespace ngl
{
	email_sender::email_sender(const parameter& aparm):
		m_pram(aparm)
	{
	}

	void email_sender::content(const std::string& atitle, const std::string& acontent)
	{
		m_title = atitle;
		m_content = acontent;
	}

	void email_sender::add_recv_email(const std::string& aemail, const std::string& aname)
	{
		std::pair<std::string, std::string> lpair(aemail, aname);
		m_recvs.push_back(lpair);
	}

	size_t g_payload_source(void* ptr, size_t size, size_t nmemb, void* userp) 
	{
		const char* data = (const char*)userp;
		static size_t index = 0;
		size_t len = strlen(data);
		size_t to_copy = std::min(len - index, size * nmemb);
		memcpy(ptr, data + index, to_copy);
		index += to_copy;
		return to_copy;
	}

	void email_sender::send()
	{
		CURL* curl;
		CURLcode res;

		// 初始化libcurl
		curl = curl_easy_init();

		if (curl) 
		{
			curl_slist* recipients = NULL;

			// 设置SMTP服务器、发件人和收件人
			curl_easy_setopt(curl, CURLOPT_URL, m_pram.m_smtp.c_str());
			curl_easy_setopt(curl, CURLOPT_MAIL_FROM, m_pram.m_email.c_str());
			// 设置SMTP认证信息
			curl_easy_setopt(curl, CURLOPT_USERNAME, m_pram.m_email.c_str());
			curl_easy_setopt(curl, CURLOPT_PASSWORD, m_pram.m_password.c_str());

			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
			//是否打开调试信息
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
			curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);

			// 设置收件人
			for (std::pair<std::string, std::string>& item :m_recvs)
			{
				recipients = curl_slist_append(recipients, std::format("<{}>", item.first).c_str());
			}
			curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

			curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

			std::string payload = std::format("From: {}<{}>\r\n", m_pram.m_name, m_pram.m_email);
			payload += "To: ";
			for (int i = 0; i < m_recvs.size(); ++i)
			{
				if (i != 0)
				{
					payload += ",";
				}
				payload += std::format("{}<{}>", m_recvs[i].second, m_recvs[i].first);
				//payload += m_recvs[i].first;
			}
			payload += "\r\n";
			payload += std::format("Subject: {}\r\n", m_title);
			payload += "\r\n"; // 空行表示header部分结束
			payload += std::format("{}\r\n", m_content); // 邮件内容
			
			curl_easy_setopt(curl, CURLOPT_READDATA, payload.c_str());

			curl_easy_setopt(curl, CURLOPT_READFUNCTION, g_payload_source); // 设置读取数据的回调函数
			//curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL); // 传递邮件内容

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
				std::cout << "Email sent successfully!" << std::endl;
			}

			// 清理
			curl_slist_free_all(recipients);
			curl_easy_cleanup(curl);
		}
	}

	void test_mail(const char* atitle, const char* acontent)
	{
		ngl::email_sender::parameter lparm
		{
			.m_smtp = xmlnode::m_mail.m_mailarg.m_smtp,
			.m_email = xmlnode::m_mail.m_mailarg.m_email,
			.m_password = xmlnode::m_mail.m_mailarg.m_password,
			.m_name = xmlnode::m_mail.m_mailarg.m_name,
		};

		ngl::email_sender lmail(lparm);
		lmail.add_recv_email("libo1@youxigu.com", "李博");
		lmail.add_recv_email("348634371@qq.com", "李博QQ");
		lmail.content(atitle, acontent);
		lmail.send();
	}
}//namespace ngl
