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

#pragma once

#include "utf8cpp/utf8/cpp11.h"
#include "utf8cpp/utf8.h"
#include "tools.h"

#include <unordered_map>
#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <locale>
#include <queue>

namespace ngl
{

    // 修正AC节点结构：用哈希表存储子节点（适配中文等任意字符）
    struct nacnode
    {
        std::unordered_map<char, int> m_children;  // 宽字符 -> 子节点索引
        int m_fail = -1;
        int len = 0;  // 模式串长度，0表示非结尾
    };

    class nfilterword
    {
    private:
        static std::vector<nacnode> m_nodes;
        static int m_root;

        // 创建新节点
        static int newnode()
        {
            m_nodes.emplace_back();
            return m_nodes.size() - 1;
        }

        nfilterword() = delete;
        nfilterword(const nfilterword&) = delete;
        nfilterword(const nfilterword&&) = delete;
    public:
        // 初始化
        static void init();

        // 清理
        static void clear();

        // 插入模式串（支持任意宽字符：中文/英文/符号）
        static void load(const std::string& apattern);

        // 构建失败指针（BFS，逻辑适配哈希表）
        static void build();

        static bool match(char c, int& cur, int i, std::vector<std::pair<int, int>>& res);

        // 匹配文本，返回所有匹配的<起始位置, 模式串长度>
        static std::vector<std::pair<int, int>> match(const std::string& text);

        // 辅助函数：过滤文本（将匹配的字符替换为*）
        static std::string filter(const std::string& text);

        // 辅助函数: 8转换为32
        static bool utf8to32(const std::string& text1, std::u32string& text2);

        // 辅助函数: 32转换为8
        static bool utf32to8(const std::u32string& text1, std::string& text2);

        // 是否存在屏蔽字
        static bool is_filter(const std::string& text);

        // 字数按照ASCII一个字，汉字两个字算
        static int32_t charcount(const std::u32string& astr);

        enum enfilter
        {
            enfilter_success,               // 成功
            enfilter_emojispecial,          // 失败:存在特殊符号
            enfilter_charcount,             // 失败:字符个数不符合要求
            enfilter_filter,                // 失败:存在屏蔽字
        };

        static const char* enfilter_message(enfilter astat);

        // 是否可以用来做名字
        static enfilter check_name(const std::string& astr, int32_t amincount, int32_t amaxcount);

        // 辅助函数：判断码点是否为「各国语言文字」
        static bool is_language_char(uint32_t codepoint);

        // 核心函数：判断码点是否为「非文字符号」
        static bool is_emojispecial(char32_t codepoint);
       
        // 是否包含特殊符号
        static bool is_emojispecial(const std::u32string& astr);
    };

    void test_nfilterword();
}//namespace ngl