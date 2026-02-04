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
        std::vector<nacnode> m_nodes;
        int m_root = 0;

        // 创建新节点
        int newnode()
        {
            m_nodes.emplace_back();
            return m_nodes.size() - 1;
        }

        nfilterword() = default;
    public:       
        static nfilterword& instance()
        {
            static nfilterword ltemp;
            return ltemp;
        }

        void init()
        {
            m_root = newnode();
        }

        void clear()
        {
            m_nodes.clear();
            m_root = 0;
        }

        // 插入模式串（支持任意宽字符：中文/英文/符号）
        void load(const std::string& apattern)
        {
            if (apattern.empty())
            {
                return;
            }
            int cur = m_root;

            for (auto c : apattern)
            {  // 遍历宽字符（中文每个字是一个wchar_t）
                // 若当前字符的子节点不存在，则创建
                auto& ltemp = m_nodes[cur].m_children;
                if (!ltemp.contains(c))
                {
                    m_nodes[cur].m_children[c] = newnode();
                }
                cur = m_nodes[cur].m_children[c];
            }
            m_nodes[cur].len = apattern.size();  // 标记模式串结束，记录长度
        }

        // 构建失败指针（BFS，逻辑适配哈希表）
        void build()
        {
            std::queue<int> q;
            // 初始化根节点的子节点：失败指针指向根
            for (auto& pair : m_nodes[m_root].m_children)
            {
                int child = pair.second;
                m_nodes[child].m_fail = m_root;
                q.push(child);
            }

            // BFS遍历所有节点
            while (!q.empty())
            {
                int p = q.front();
                q.pop();

                // 遍历当前节点的所有子节点
                for (auto& pair : m_nodes[p].m_children)
                {
                    auto c = pair.first;   // 当前字符
                    int u = pair.second;      // 当前子节点索引
                    int fail_p = m_nodes[p].m_fail;  // 父节点的失败指针

                    // 回溯失败指针：直到找到包含当前字符的节点，或根节点
                    while (fail_p != -1 && !m_nodes[fail_p].m_children.contains(c))
                    {
                        fail_p = m_nodes[fail_p].m_fail;
                    }

                    // 设置当前节点的失败指针
                    m_nodes[u].m_fail = (fail_p == -1) ? m_root : m_nodes[fail_p].m_children[c];
                    q.push(u);
                }
            }
        }

        bool match(char c, int& cur, int i, std::vector<std::pair<int, int>>& res)
        {
            // 失配回退：沿失败指针找匹配的节点
            while (cur != m_root && !m_nodes[cur].m_children.contains(c))
            {
                cur = m_nodes[cur].m_fail;
            }

            // 匹配成功则移动到子节点，否则留在根节点
            if (m_nodes[cur].m_children.contains(c))
            {
                cur = m_nodes[cur].m_children[c];
            }

            // 收集所有匹配的模式串（遍历失败链，处理嵌套匹配）
            int temp = cur;
            while (temp != m_root)
            {
                if (m_nodes[temp].len > 0)
                {
                    int start = i - m_nodes[temp].len + 1;  // 计算起始位置
                    res.emplace_back(start, m_nodes[temp].len);
                }
                temp = m_nodes[temp].m_fail;
            }
            return !res.empty();
        }

        // 匹配文本，返回所有匹配的<起始位置, 模式串长度>
        std::vector<std::pair<int, int>> match(const std::string& text)
        {
            std::vector<std::pair<int, int>> res;
            int cur = m_root;

            // 遍历宽字符文本（中文每个字是一个元素）
            for (int i = 0; i < text.size(); ++i) 
            {
                match(text[i], cur, i, res);
            }
            return res;
        }

        // 辅助函数：过滤文本（将匹配的字符替换为*）
        std::string filter(const std::string& text)
        {
            std::string result = text;
            std::vector<std::pair<int, int>> matches = match(text);
            // 替换所有匹配的字符为*
            for (auto& [start, len] : matches)
            {
                for (int i = start; i < start + len; ++i)
                {
                    result[i] = '*';
                }
            }
            return result;
        }

        bool utf8to32(const std::string& text1, std::u32string& text2)
        {
            try
            {
                text2 = utf8::utf8to32(text1);
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        bool utf32to8(const std::u32string& text1, std::string& text2)
        {
            try
            {
                text2 = utf8::utf32to8(text1);
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        // 是否存在屏蔽字
        bool is_filter(const std::string& text)
        {
            // 去除特殊符号
            std::u32string ltemp1;
            if (!utf8to32(text, ltemp1))
            {
                return false;
            }
            std::u32string ltemp2;
            for (auto item : ltemp1)
            {
                if (!is_emojispecial(item))
                {
                    ltemp2 += item;
                }
            }
            std::string ltemp3;
            if (!utf32to8(ltemp2, ltemp3))
            {
                return false;
            }

            int cur = m_root;
            std::vector<std::pair<int, int>> res;
            for (int i = 0; i < ltemp3.size(); ++i)
            {
                if (match(ltemp3[i], cur, i, res))
                {
                    return true;
                }
            }
            return false;
        }

        // 字数按照ASCII一个字，汉字两个字算
        static int32_t charcount(const std::u32string& astr)
        {
            int32_t lcount = 0;
            for (int32_t i = 0; i < astr.size(); ++i)
            {
                if (astr[i] < 0x80) 
                {
                    ++lcount;
                }
                else 
                {
                    lcount += 2;
                }
            }
            return lcount;
        }

        enum enfilter
        {
            enfilter_success,
            enfilter_emojispecial,
            enfilter_charcount,
            enfilter_filter
        };

        // 是否可以用来做名字
        enfilter check_name(const std::string& astr, int32_t amincount, int32_t amaxcount)
        {
            std::u32string ltemp = utf8::utf8to32(astr);
            if (is_emojispecial(ltemp))
            {
                return enfilter_emojispecial;
            }
            int32_t lcount = charcount(ltemp);
            if (lcount < amincount || lcount > amaxcount)
            {
                return enfilter_charcount;
            }
            if (is_filter(astr))
            {
                return enfilter_filter;
            }
            return enfilter_success;
        }

        // 辅助函数：判断码点是否为「各国语言文字」（需排除）
        static bool is_language_char(uint32_t codepoint)
        {
            return (codepoint >= 0x0041 && codepoint <= 0x005A) ||  // 大写英文字母
                (codepoint >= 0x0061 && codepoint <= 0x007A) ||  // 小写英文字母
                (codepoint >= 0x4E00 && codepoint <= 0x9FFF) ||  // 常用汉字
                (codepoint >= 0x3400 && codepoint <= 0x4DBF) ||  // 汉字扩展A
                (codepoint >= 0x20000 && codepoint <= 0x2A6DF) || // 汉字扩展B
                (codepoint >= 0x3040 && codepoint <= 0x309F) ||  // 日文平假名
                (codepoint >= 0x30A0 && codepoint <= 0x30FF) ||  // 日文片假名
                (codepoint >= 0xAC00 && codepoint <= 0xD7AF) ||  // 韩文谚文
                (codepoint >= 0x0400 && codepoint <= 0x04FF) ||  // 俄文西里尔字母
                (codepoint >= 0x0370 && codepoint <= 0x03FF) ||  // 希腊文字母
                (codepoint >= 0x0600 && codepoint <= 0x06FF);    // 阿拉伯字母
        }

        // 核心函数：判断码点是否为「非文字符号」（修复后）
        static bool is_emojispecial(char32_t codepoint)
        {
            // 第一步：如果是语言文字，直接返回false
            if (is_language_char(codepoint))
            {
                return false;
            }

            // 第二步：判断是否属于「符号区间」（新增控制字符区间）
            return (
                // ===== 新增：ASCII控制字符（包含\n、\t、\r等）=====
                (codepoint >= 0x0000 && codepoint <= 0x001F) ||  // 0x0000-0x001F 控制字符
                (codepoint == 0x007F) ||                          // 0x7F 删除符
                // 1. 基础ASCII标点/符号（原有）
                (codepoint >= 0x0020 && codepoint <= 0x002F) ||
                (codepoint >= 0x003A && codepoint <= 0x0040) ||
                (codepoint >= 0x005B && codepoint <= 0x0060) ||
                (codepoint >= 0x007B && codepoint <= 0x007E) ||
                // 2. 全角符号/标点（原有）
                (codepoint >= 0xFF00 && codepoint <= 0xFFEF) ||
                // 3. Emoji全量符号（原有）
                (codepoint >= 0x2600 && codepoint <= 0x27BF) ||
                (codepoint >= 0x1F300 && codepoint <= 0x1F5FF) ||
                (codepoint >= 0x1F600 && codepoint <= 0x1F64F) ||
                (codepoint >= 0x1F680 && codepoint <= 0x1F6FF) ||
                (codepoint >= 0x1F900 && codepoint <= 0x1F9FF) ||
                (codepoint >= 0x1FA00 && codepoint <= 0x1FAFF) ||
                // 4. 数学符号（原有）
                (codepoint >= 0x2200 && codepoint <= 0x22FF) ||
                (codepoint >= 0x2100 && codepoint <= 0x214F) ||
                (codepoint >= 0x2500 && codepoint <= 0x257F) ||
                (codepoint >= 0x1D400 && codepoint <= 0x1D7FF) ||
                // 5. 箭头符号（原有）
                (codepoint >= 0x2190 && codepoint <= 0x21FF) ||
                (codepoint >= 0x2900 && codepoint <= 0x297F) ||
                (codepoint >= 0x2B00 && codepoint <= 0x2BFF) ||
                // 6. 装饰/特殊符号（原有）
                (codepoint >= 0x2000 && codepoint <= 0x206F) ||
                (codepoint >= 0x2300 && codepoint <= 0x23FF) ||
                (codepoint >= 0x3200 && codepoint <= 0x32FF) ||
                // 7. 货币/单位符号（原有）
                (codepoint >= 0x00A0 && codepoint <= 0x00FF) ||
                (codepoint >= 0x20A0 && codepoint <= 0x20CF) ||
                // 8. 版权/标记符号（原有）
                (codepoint == 0x00A9 || codepoint == 0x00AE || codepoint == 0x2122 || codepoint == 0x2605) ||
                // 9. Emoji修饰/组合符号（原有）
                (codepoint >= 0x1F3FB && codepoint <= 0x1F3FF) ||
                (codepoint == 0xFE0F || codepoint == 0x200D)
                );
        }
       
        static bool is_emojispecial(const std::u32string& astr)
        {
            int32_t lcount = astr.size();
            for (int i = 0; i < lcount; ++i) 
            {
                if (is_emojispecial(astr[i]))
                {
                    return true;
                }               
            }
            return false;
        }
    };

    // 测试示例
    void test_nfilterword();
}//namespace ngl