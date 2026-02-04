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
#include "enum2name.h"

namespace ngl
{
    void test_nfilterword()
    { 
        {// 判断utf8编码
            std::string ltemp1 = "敏感词";
            std::string ltemp11;
            tools::to_utf8(ltemp1, ltemp11);
            std::cout << "ltemp1:" << (tools::isutf8(ltemp1) ? "true" : "false") << std::endl;
            std::cout << "ltemp11:" << (tools::isutf8(ltemp11) ? "true" : "false") << std::endl;

            std::string ltemp2 = "123";
            std::string ltemp22;
            tools::to_utf8(ltemp2, ltemp22);
            std::cout << "ltemp2:" << (tools::isutf8(ltemp2) ? "true" : "false") << std::endl;
            std::cout << "ltemp22:" << (tools::isutf8(ltemp22) ? "true" : "false") << std::endl;

            std::string ltemp3 = "12脏话3";
            std::string ltemp33;
            tools::to_utf8(ltemp3, ltemp33);
            std::cout << "ltemp3:" << (tools::isutf8(ltemp3) ? "true" : "false") << std::endl;
            std::cout << "ltemp33:" << (tools::isutf8(ltemp33) ? "true" : "false") << std::endl;
        }
        


        {
            nfilterword::instance().init();
            std::vector<std::string> filterWords = {
                "敏感词", "脏话", "123456", "Test"
            };
            for (auto& item : filterWords)
            {
                tools::to_utf8(item, item);
            }
            
            for (auto& wiord : filterWords)
            {
                nfilterword::instance().load(wiord);
            }

            nfilterword::instance().build();

            std::vector<std::string> testTexts = {
                "敏感词测试",
                "不要说脏话！",
                "密码是123456，注意保密",
                "TEST大小写也会被过滤",
                "无屏蔽字的正常文本",
                "嵌套屏蔽字：敏感词123456",
                "敏-感-词测试",
            };
            for (auto& item : testTexts)
            {
                tools::to_utf8(item, item);
            }

            for (std::string& text : testTexts)
            {
                auto filtered = nfilterword::instance().filter(text);
                tools::to_asscii(filtered, filtered);
                tools::to_asscii(text, text);
                std::cout << "sorc:" << text << std::endl;
                std::cout << "obj:" << filtered << std::endl;
            }

            {// 检查名字是否合法
                std::vector<std::string> lnames = {
                    "你好??！123@#",
                    "你好！123@#",
                    "你好",
                    "abc",
                    "你好\n",
                    "你好\r",
                    "你好\v",
                    "好敏感词"
                };
                std::vector<std::string> lnames2;
                for (auto& item : lnames)
                {
                    std::string ltemp;
                    tools::to_utf8(item, ltemp);
                    lnames2.push_back(ltemp);
                }
                em<nfilterword::enfilter>::set(nfilterword::enfilter::enfilter_success, "成功");
                em<nfilterword::enfilter>::set(nfilterword::enfilter::enfilter_emojispecial, "失败:存在特殊符号");
                em<nfilterword::enfilter>::set(nfilterword::enfilter::enfilter_charcount, "失败:字符个数不符合要求");
                em<nfilterword::enfilter>::set(nfilterword::enfilter::enfilter_filter, "失败:存在屏蔽字");

                for(int i = 0;i< lnames2.size();++i)
                {
                    nfilterword::enfilter lerror = nfilterword::instance().check_name(lnames2[i], 1, 20);
                    std::cout << lnames[i] << ":" << em<nfilterword::enfilter>::name(lerror) << std::endl;
                }               
            }
        }
    }
}//namespace ngl