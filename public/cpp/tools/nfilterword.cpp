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
        //////utf8
        {
            // 1. 创建过滤器（替换符为*，大小写不敏感）
            nfilterword<std::u8string>::instance().init();

            // 2. 加载屏蔽字库
            std::vector<std::u8string> filterWords = {
                u8"敏感词", u8"脏话", u8"123456", u8"Test" // 测试大小写不敏感
            };
            for (auto& wiord : filterWords)
            {
                nfilterword<std::u8string>::instance().load(wiord);
            }

            nfilterword<std::u8string>::instance().build();

            // 3. 测试过滤不同文本
            std::vector<std::u8string> testTexts = {
                u8"敏感词测试",
                u8"不要说脏话！",
                u8"密码是123456，注意保密",
                u8"TEST大小写也会被过滤", // 测试大小写不敏感
                u8"无屏蔽字的正常文本",
                u8"嵌套屏蔽字：敏感词123456" // 测试嵌套屏蔽字
            };

            // 4. 输出过滤结果
            for (const std::u8string& text : testTexts)
            {
                std::u8string filtered = nfilterword<std::u8string>::instance().filter(text);
                std::cout << "原文本：" << text << "\n过滤后：" << filtered << std::endl;
            }
        }
        {
            // 1. 创建过滤器（替换符为*，大小写不敏感）
            nfilterword<std::wstring>::instance().init();

            // 2. 加载屏蔽字库
            std::vector<std::wstring> filterWords = {
                L"敏感词", L"脏话", L"123456", L"Test" // 测试大小写不敏感
            };
            for (auto& wiord : filterWords)
            {
                nfilterword<std::wstring>::instance().load(wiord);
            }

            nfilterword<std::wstring>::instance().build();

            // 3. 测试过滤不同文本
            std::vector<std::wstring> testTexts = {
                L"敏感词测试",
                L"不要说脏话！",
                L"密码是123456，注意保密",
                L"TEST大小写也会被过滤", // 测试大小写不敏感
                L"无屏蔽字的正常文本",
                L"嵌套屏蔽字：敏感词123456" // 测试嵌套屏蔽字
            };

            // 4. 输出过滤结果
            for (const std::wstring& text : testTexts)
            {
                auto filtered = nfilterword<std::wstring>::instance().filter(text);
                std::cout << "原文本：" << text << "\n过滤后：" << filtered << std::endl;
            }
        }
    }

}//namespace ngl