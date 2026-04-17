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
// File overview: Implements HTTP GET/POST requests and email sending via libcurl.


#include "actor/protocol/nprotocol.h"
#include "tools/tools/tools_curl.h"

#include <algorithm>
#include <cstring>
#include <format>
#include <iostream>
#include <sstream>
#include <thread>

namespace ngl::tools
{
	namespace
	{
		struct mail_payload_state
		{
			const std::string* m_payload = nullptr;
			size_t m_offset = 0;
		};

		std::string make_request_url(const http_parm& ahttp)
		{
			if (ahttp.m_param.empty())
			{
				return ahttp.m_url;
			}

			// GET parameters are appended onto the URL, while POST keeps them in
			// the request body.
			std::string lurl = ahttp.m_url;
			if (lurl.empty())
			{
				return std::string();
			}
			if (lurl.find('?') == std::string::npos)
			{
				lurl += '?';
			}
			else if (!lurl.ends_with('?') && !lurl.ends_with('&'))
			{
				lurl += '&';
			}
			lurl += ahttp.m_param;
			return lurl;
		}
	}

	curl::curl() :
		m_works(std::bind_front(&curl::work, this))
	{}

	void curl::send(std::shared_ptr<http_parm>& adata)
	{
		if (adata == nullptr)
		{
			return;
		}
		instance().m_works.push_back(adata);
	}

	void curl::work(http_parm& ahttp)
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

	CURLcode curl::visit(http_parm& ahttp)
	{
		if (ahttp.m_curl == nullptr)
		{
			return CURLE_FAILED_INIT;
		}
		if (ahttp.m_url.empty())
		{
			return CURLE_URL_MALFORMAT;
		}

		ahttp.m_recvdata.clear();
		curl_easy_reset(ahttp.m_curl);
		const std::string lrequest_url = make_request_url(ahttp);

		if (ahttp.m_type == ENUM_TYPE_POST)
		{
			curl_easy_setopt(ahttp.m_curl, CURLOPT_POST, 1L);
			if (!ahttp.m_param.empty())
			{
				curl_easy_setopt(ahttp.m_curl, CURLOPT_POSTFIELDS, ahttp.m_param.c_str());
				curl_easy_setopt(ahttp.m_curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(ahttp.m_param.size()));
			}
			curl_easy_setopt(ahttp.m_curl, CURLOPT_URL, ahttp.m_url.c_str());
		}
		else
		{
			curl_easy_setopt(ahttp.m_curl, CURLOPT_HTTPGET, 1L);
			curl_easy_setopt(ahttp.m_curl, CURLOPT_URL, lrequest_url.c_str());
		}
		curl_easy_setopt(ahttp.m_curl, CURLOPT_WRITEFUNCTION, &curl::callback);
		curl_easy_setopt(ahttp.m_curl, CURLOPT_WRITEDATA, &ahttp.m_recvdata);
		curl_easy_setopt(ahttp.m_curl, CURLOPT_HTTPHEADER, ahttp.m_headers);

		if (ahttp.m_mode == ENUM_MODE_HTTPS)
		{
			curl_easy_setopt(ahttp.m_curl, CURLOPT_SSL_VERIFYPEER, ahttp.m_verify_peer ? 1L : 0L);
			curl_easy_setopt(ahttp.m_curl, CURLOPT_SSL_VERIFYHOST, ahttp.m_verify_host);
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
		curl_easy_setopt(ahttp.m_curl, CURLOPT_FORBID_REUSE, 1L); // Alarmafter CLOSE_WAIT

		return curl_easy_perform(ahttp.m_curl);
	}

	size_t curl::callback(void* buffer, size_t size, size_t nmemb, std::string* lpVoid)
	{
		const size_t lbytes = size * nmemb;
		if (buffer == nullptr)
		{
			return 0;
		}
		if (lpVoid == nullptr)
		{
			return lbytes;
		}
		const char* ldata = static_cast<const char*>(buffer);
		lpVoid->append(ldata, lbytes);
		return lbytes;
	}

	void curl::set_mode(std::shared_ptr<http_parm>& ahttp, ENUM_MODE aval)
	{ 
		if (ahttp != nullptr)
		{
			ahttp->m_mode = aval;
		}
	}

	void curl::set_type(std::shared_ptr<http_parm>& ahttp, ENUM_TYPE aval)
	{ 
		if (ahttp != nullptr)
		{
			ahttp->m_type = aval;
		}
	}

	void curl::set_tls_verification(std::shared_ptr<http_parm>& ahttp, bool averify_peer, long averify_host)
	{
		if (ahttp != nullptr)
		{
			ahttp->m_verify_peer = averify_peer;
			ahttp->m_verify_host = averify_host;
		}
	}

	void curl::set_url(std::shared_ptr<http_parm>& ahttp, const std::string& aurl)
	{ 
		if (ahttp != nullptr)
		{
			ahttp->m_url = aurl;
		}
	}

	void curl::set_url(std::shared_ptr<http_parm>& ahttp, const char* aurl)
	{ 
		if (ahttp != nullptr)
		{
			ahttp->m_url = aurl == nullptr ? "" : aurl;
		}
	}

	void curl::set_param(std::shared_ptr<http_parm>& ahttp, const std::string& aparam)
	{ 
		if (ahttp != nullptr)
		{
			ahttp->m_param = aparam;
		}
	}

	void curl::set_headers(std::shared_ptr<http_parm>& ahttp, const std::vector<std::string>& aheaders)
	{ 
		if (ahttp != nullptr)
		{
			ahttp->headers(aheaders);
		}
	}

	void curl::set_callback(std::shared_ptr<http_parm>& ahttp, const std::function<void(int, http_parm&)>& aback)
	{ 
		if (ahttp != nullptr)
		{
			ahttp->m_callback = aback;
		}
	}

	std::shared_ptr<http_parm> curl::http()
	{
		return tools::nwork<http_parm>::make_shared();
	}

	std::shared_ptr<mail_param> curl::mail()
	{
		return tools::nwork<mail_param>::make_shared();
	}

	class email_sender
	{
		~email_sender() = default;

		tools::nwork<mail_param> m_works;

		email_sender() :
			m_works(std::bind_front(&email_sender::work, this))
		{}

		static email_sender& instance()
		{
			static email_sender ltemp;
			return ltemp;
		}
	public:
		static void send(std::shared_ptr<mail_param>& aparm)
		{
			if (aparm == nullptr)
			{
				return;
			}
			instance().m_works.push_back(aparm);
		}
	private:
		static size_t callback(void* ptr, size_t size, size_t nmemb, void* userp)
		{
			if (ptr == nullptr)
			{
				return 0;
			}

			auto* lstate = static_cast<mail_payload_state*>(userp);
			if (lstate == nullptr || lstate->m_payload == nullptr)
			{
				return 0;
			}
			if (lstate->m_offset >= lstate->m_payload->size())
			{
				return 0;
			}

			const size_t lremaining = lstate->m_payload->size() - lstate->m_offset;
			const size_t lto_copy = std::min(lremaining, size * nmemb);
			// libcurl repeatedly pulls the payload until this callback returns 0.
			memcpy(ptr, lstate->m_payload->data() + lstate->m_offset, lto_copy);
			lstate->m_offset += lto_copy;
			return lto_copy;
		}
		
		void work(mail_param& aparm)
		{
			struct post_guard
			{
				mail_param& m_param;

				~post_guard()
				{
					m_param.post();
				}
			} lguard{ aparm };

			CURL* lcurl = curl_easy_init();

			if (lcurl == nullptr)
			{
				std::cerr << "curl_easy_init() failed" << std::endl;
				return;
			}

				curl_slist* recipients = nullptr;

				// Configure the SMTP connection and sender credentials.
				curl_easy_setopt(lcurl, CURLOPT_URL, aparm.m_smtp.c_str());
				curl_easy_setopt(lcurl, CURLOPT_MAIL_FROM, aparm.m_email.c_str());
				curl_easy_setopt(lcurl, CURLOPT_USERNAME, aparm.m_email.c_str());
				curl_easy_setopt(lcurl, CURLOPT_PASSWORD, aparm.m_password.c_str());

				curl_easy_setopt(lcurl, CURLOPT_SSL_VERIFYPEER, 1L);
				curl_easy_setopt(lcurl, CURLOPT_VERBOSE, 0L);
				curl_easy_setopt(lcurl, CURLOPT_TIMEOUT, 60L);
				curl_easy_setopt(lcurl, CURLOPT_CONNECTTIMEOUT, 30L);

				// Build the SMTP recipient list.
				for (const auto& [mailaddr, name] : aparm.m_recvs)
				{
					(void)name;
					recipients = curl_slist_append(recipients, std::format("<{}>", mailaddr).c_str());
				}
				curl_easy_setopt(lcurl, CURLOPT_MAIL_RCPT, recipients);

				curl_easy_setopt(lcurl, CURLOPT_UPLOAD, 1L);

				std::string payload = std::format("From: {}<{}>\r\n", aparm.m_name, aparm.m_email);
				payload += "To: ";
				for (std::size_t i = 0; i < aparm.m_recvs.size(); ++i)
				{
					if (i != 0)
					{
						payload += ",";
					}
					payload += std::format("{}<{}>", aparm.m_recvs[i].second, aparm.m_recvs[i].first);
				}
				payload += "\r\n";
				payload += std::format("Subject: {}\r\n", aparm.m_title);
				payload += "\r\n"; // End of mail headers.
				payload += std::format("{}\r\n", aparm.m_content); // Mail body.

				mail_payload_state lstate;
				lstate.m_payload = &payload;
				curl_easy_setopt(lcurl, CURLOPT_READDATA, &lstate);

				curl_easy_setopt(lcurl, CURLOPT_READFUNCTION, &email_sender::callback); // Stream the RFC822 payload.

				curl_easy_setopt(lcurl, CURLOPT_DEBUGFUNCTION, nullptr);
				const CURLcode res = curl_easy_perform(lcurl);

				if (res != CURLE_OK)
				{
					std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
				}
				else
				{
					std::cout << "email send successfully!" << std::endl;
				}

				curl_slist_free_all(recipients);
				curl_easy_cleanup(lcurl);
		}
	};

	void curl::sendemail(std::shared_ptr<mail_param>& aparm)
	{
		if (aparm == nullptr)
		{
			return;
		}
		email_sender::send(aparm);
		aparm->wait();
	}

	void test_mail(const char* atitle, const char* acontent, const std::vector<std::pair<std::string, std::string>>& amailvec/* = {}*/)
	{
		auto lparm = curl::mail();

		lparm->m_smtp = nconfig.mail().m_smtp;
		lparm->m_email = nconfig.mail().m_email;
		lparm->m_password = nconfig.mail().m_password;
		lparm->m_name = nconfig.mail().m_name;
		lparm->m_title = atitle;
		lparm->m_content = acontent;
		lparm->m_recvs = amailvec;

		if (amailvec.empty())
		{
			lparm->m_recvs.emplace_back(std::make_pair("libo1@youxigu.com", "李博"));
			lparm->m_recvs.emplace_back(std::make_pair("348634371@qq.com", "李博QQ"));
		}
		curl::sendemail(lparm);
	}

	void test_manage_curl()
	{
		constexpr const char* kExampleToken = "<replace-with-token>";

		auto lhttp = curl::http();
		curl::set_mode(lhttp, ENUM_MODE_HTTPS);
		curl::set_type(lhttp, ENUM_TYPE_POST);
		curl::set_url(lhttp, "xxx");

		std::string lparm;
		std::stringstream lstream;
		lstream
			<< "userID=" << 7709523
			<< "token=" << kExampleToken
			<< "yWpx3hWQHFhSnTCj#311#6KuRKuaAjLJ5sYRy";

		curl::param(lparm, "appid", 311);
		curl::param(lparm, "userID", 7709523);
		curl::param(lparm, "token", kExampleToken);
		curl::param(lparm, "sign", tools::md5(lstream.str()).c_str());
		curl::set_param(lhttp, lparm);
		bool lbool = true;

			curl::set_callback(lhttp, [&lbool]([[maybe_unused]] int anum, http_parm& aparm)
			{
				log_error()->print("curl callback [{}]", aparm.m_recvdata);
				lbool = false;
			}
		);
		curl::send(lhttp);
		while (lbool)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		}
	}
}//namespace ngl
