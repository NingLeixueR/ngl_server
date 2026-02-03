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

#include "nfilterword.h"

// ========== 重载 operator<<，让 cout 支持 std::u8string ==========
std::ostream& operator<<(std::ostream& os, const std::u8string& u8_str) 
{
    std::string ltemp;
    for (auto item : u8_str)
    {
        ltemp += item;
    }
    std::wstring lws;
    ngl::tools::utf82wasscii(ltemp, lws);
    std::string lasscii;
    ngl::tools::wasscii2asscii(lws, lasscii);
    // 转换为 const char* 并输出
    os << lasscii;
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::wstring& u8_str)
{
    std::string lasscii;
    ngl::tools::wasscii2asscii(u8_str, lasscii);
    // 转换为 const char* 并输出
    os << lasscii;
    return os;
}

namespace ngl
{
    void test_nfilterword()
    {        
        {
            nfilterword<std::string>::instance().init();
            std::vector<std::string> filterWords = {
                "敏感词", "脏话", "123456", "Test"
            };
            for (auto& wiord : filterWords)
            {
                nfilterword<std::string>::instance().load(wiord);
            }

            nfilterword<std::string>::instance().build();

            std::vector<std::string> testTexts = {
                "敏感词测试",
                "不要说脏话！",
                "密码是123456，注意保密",
                "TEST大小写也会被过滤",
                "无屏蔽字的正常文本",
                "嵌套屏蔽字：敏感词123456"
            };

            for (const std::string& text : testTexts)
            {
                auto filtered = nfilterword<std::string>::instance().filter(text);
                std::cout << "原文本：" << text << "\n过滤后：" << filtered << std::endl;
            }
        }
    }

}//namespace ngl