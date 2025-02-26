#pragma once
 
#include <string>
#include <vector>
#include <utility>

#include "curl/curl.h"

namespace ngl
{
    class email_sender
    {
    public:
        struct parameter
        {
            std::string m_smtp;
            std::string m_email;
            std::string m_password;
            std::string m_name;
        };

        email_sender(const parameter& aparm);
        ~email_sender() {}
        void content(const std::string& atitle, const std::string& acontent);
        void add_recv_email(const std::string& aemail, const std::string& aname);
        void send();

    private:
        parameter m_pram;
        std::vector<std::pair<std::string, std::string>> m_recvs;// key:mail value:name
        std::string m_title;
        std::string m_content;
    };

    void test_mail(const char* atitle, const char* acontent);
}//namespace ngl