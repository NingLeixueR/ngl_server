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
// File overview: Implements logic for tools.


#include "tools/nfilterword.h"
#include "tools/enum2name.h"

#include <limits>

namespace ngl
{
    std::vector<nacnode> nfilterword::m_nodes;
    int nfilterword::m_root = -1;

    void nfilterword::ensure_initialized()
    {
        if (m_root >= 0 && static_cast<std::size_t>(m_root) < m_nodes.size())
        {
            return;
        }
        init();
    }

    void nfilterword::init()
    {
        m_nodes.clear();
        m_root = newnode();
    }

    void nfilterword::clear()
    {
        m_nodes.clear();
        m_root = -1;
    }

    void nfilterword::advance_state(char c, int& cur)
    {
        if (m_root < 0 || static_cast<std::size_t>(m_root) >= m_nodes.size())
        {
            cur = -1;
            return;
        }
        if (cur < 0 || static_cast<std::size_t>(cur) >= m_nodes.size())
        {
            cur = m_root;
        }

        while (cur != m_root)
        {
            if (cur < 0 || static_cast<std::size_t>(cur) >= m_nodes.size())
            {
                cur = m_root;
                break;
            }

            auto& lchildren = m_nodes[cur].m_children;
            if (auto litor = lchildren.find(c); litor != lchildren.end())
            {
                cur = litor->second;
                return;
            }

            const int lfail = m_nodes[cur].m_fail;
            if (lfail < 0 || static_cast<std::size_t>(lfail) >= m_nodes.size())
            {
                cur = m_root;
                break;
            }
            cur = lfail;
        }

        auto& lroot_children = m_nodes[m_root].m_children;
        if (auto litor = lroot_children.find(c); litor != lroot_children.end())
        {
            cur = litor->second;
        }
        else
        {
            cur = m_root;
        }
    }

    bool nfilterword::append_matches(int cur, int i, std::vector<std::pair<int, int>>& res)
    {
        bool lmatched = false;
        for (int temp = cur; temp >= 0 && static_cast<std::size_t>(temp) < m_nodes.size() && temp != m_root;)
        {
            if (m_nodes[temp].len > 0)
            {
                res.emplace_back(i - m_nodes[temp].len + 1, m_nodes[temp].len);
                lmatched = true;
            }

            const int lfail = m_nodes[temp].m_fail;
            if (lfail < 0 || static_cast<std::size_t>(lfail) >= m_nodes.size())
            {
                break;
            }
            temp = lfail;
        }
        return lmatched;
    }

    bool nfilterword::match_exists(char c, int& cur)
    {
        if (m_root < 0 || static_cast<std::size_t>(m_root) >= m_nodes.size())
        {
            return false;
        }

        advance_state(c, cur);
        return cur >= 0 && static_cast<std::size_t>(cur) < m_nodes.size() && m_nodes[cur].m_has_match;
    }

    void nfilterword::load(const std::string& apattern)
    {
        if (apattern.empty())
        {
            return;
        }
        ensure_initialized();
        int cur = m_root;

        for (auto c : apattern)
        {
            auto litor = m_nodes[cur].m_children.find(c);
            if (litor == m_nodes[cur].m_children.end())
            {
                const int lnext = newnode();
                litor = m_nodes[cur].m_children.emplace(c, lnext).first;
            }
            cur = litor->second;
        }
        if (apattern.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        {
            return;
        }
        m_nodes[cur].len = static_cast<int>(apattern.size());
        m_nodes[cur].m_has_match = true;
    }

    void nfilterword::build()
    {
        ensure_initialized();
        m_nodes[m_root].m_has_match = m_nodes[m_root].len > 0;
        std::queue<int> q;
        for (auto& pair : m_nodes[m_root].m_children)
        {
            int child = pair.second;
            m_nodes[child].m_fail = m_root;
            m_nodes[child].m_has_match = m_nodes[child].len > 0 || m_nodes[m_root].m_has_match;
            q.push(child);
        }

        while (!q.empty())
        {
            int p = q.front();
            q.pop();

            for (auto& pair : m_nodes[p].m_children)
            {
                auto c = pair.first;
                int u = pair.second;
                int fail_p = m_nodes[p].m_fail;

                int lnext = m_root;
                while (fail_p != -1)
                {
                    auto& lfail_children = m_nodes[fail_p].m_children;
                    if (auto litor = lfail_children.find(c); litor != lfail_children.end())
                    {
                        lnext = litor->second;
                        break;
                    }
                    fail_p = m_nodes[fail_p].m_fail;
                }

                m_nodes[u].m_fail = lnext;
                m_nodes[u].m_has_match = m_nodes[u].len > 0 || m_nodes[lnext].m_has_match;
                q.push(u);
            }
        }
    }

    bool nfilterword::match(char c, int& cur, int i, std::vector<std::pair<int, int>>& res)
    {
        if (m_root < 0 || static_cast<std::size_t>(m_root) >= m_nodes.size())
        {
            return false;
        }

        advance_state(c, cur);
        return append_matches(cur, i, res);
    }

    std::vector<std::pair<int, int>> nfilterword::match(const std::string& text)
    {
        std::vector<std::pair<int, int>> res;
        if (text.empty())
        {
            return res;
        }
        ensure_initialized();
        int cur = m_root;

        // Wide characters (in )
        for (std::string::size_type i = 0; i < text.size(); ++i)
        {
            match(text[i], cur, static_cast<int>(i), res);
        }
        return res;
    }

    std::string nfilterword::filter(const std::string& text)
    {
        std::string result = text;
        std::vector<std::pair<int, int>> matches = match(text);
        // Replaceall *
        for (auto& [start, len] : matches)
        {
            const std::size_t begin = start < 0 ? 0U : static_cast<std::size_t>(start);
            const std::size_t end = std::min(result.size(), begin + static_cast<std::size_t>(std::max(len, 0)));
            for (std::size_t i = begin; i < end; ++i)
            {
                result[i] = '*';
            }
        }
        return result;
    }

    bool nfilterword::utf8to32(const std::string& text1, std::u32string& text2)
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

    bool nfilterword::utf32to8(const std::u32string& text1, std::string& text2)
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

    bool nfilterword::is_filter(const std::string& text)
    {
        if (text.empty())
        {
            return false;
        }
        ensure_initialized();

        int cur = m_root;
        try
        {
            auto liter = text.begin();
            const auto lend = text.end();
            while (liter != lend)
            {
                const auto lcode_begin = liter;
                const char32_t lcodepoint = utf8::next(liter, lend);
                if (is_emojispecial(lcodepoint))
                {
                    continue;
                }
                for (auto lbyte = lcode_begin; lbyte != liter; ++lbyte)
                {
                    if (match_exists(*lbyte, cur))
                    {
                        return true;
                    }
                }
            }
        }
        catch (...)
        {
            return false;
        }
        return false;
    }

    int32_t nfilterword::charcount(const std::u32string& astr)
    {
        int32_t lcount = 0;
        for (char32_t codepoint : astr)
        {
            if (codepoint < 0x80)
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

    const char* nfilterword::enfilter_message(nfilterword::enfilter astat)
    {
        if (em<nfilterword::enfilter>::empty())
        {
            em<nfilterword::enfilter>::set(nfilterword::enfilter::enfilter_success, "成功");
            em<nfilterword::enfilter>::set(nfilterword::enfilter::enfilter_invalid_utf8, "失败:UTF-8 非法");
            em<nfilterword::enfilter>::set(nfilterword::enfilter::enfilter_emojispecial, "失败:存在特殊符号");
            em<nfilterword::enfilter>::set(nfilterword::enfilter::enfilter_charcount, "失败:字符个数不符合要求");
            em<nfilterword::enfilter>::set(nfilterword::enfilter::enfilter_filter, "失败:存在屏蔽字");
        }
        return em<nfilterword::enfilter>::name(astat);
    }

    nfilterword::enfilter nfilterword::check_name(const std::string& astr, int32_t amincount, int32_t amaxcount)
    {
        std::u32string ltemp;
        if (!utf8to32(astr, ltemp))
        {
            return enfilter_invalid_utf8;
        }
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

    bool nfilterword::is_language_char(uint32_t codepoint)
    {
        return (codepoint >= 0x0041 && codepoint <= 0x005A) ||  // Translated comment.
            (codepoint >= 0x0061 && codepoint <= 0x007A) ||  // Translated comment.
            (codepoint >= 0x4E00 && codepoint <= 0x9FFF) ||  // Translated comment.
            (codepoint >= 0x3400 && codepoint <= 0x4DBF) ||  // A
            (codepoint >= 0x20000 && codepoint <= 0x2A6DF) || // B
            (codepoint >= 0x3040 && codepoint <= 0x309F) ||  // Translated comment.
            (codepoint >= 0x30A0 && codepoint <= 0x30FF) ||  // Translated comment.
            (codepoint >= 0xAC00 && codepoint <= 0xD7AF) ||  // Translated comment.
            (codepoint >= 0x0400 && codepoint <= 0x04FF) ||  // Translated comment.
            (codepoint >= 0x0370 && codepoint <= 0x03FF) ||  // Translated comment.
            (codepoint >= 0x0600 && codepoint <= 0x06FF);    // Translated comment.
    }

    bool nfilterword::is_emojispecial(char32_t codepoint)
    {
        // : If, returnfalse
        if (is_language_char(codepoint))
        {
            return false;
        }

        // : Checkwhether 「 」(newly added )
        return (
            // ===== Newly added: ASCII (pack \n, \t, \r )=====
            (codepoint >= 0x0000 && codepoint <= 0x001F) ||  // 0X0000-0x001F
            (codepoint == 0x007F) ||                          // 0X7F delete
            // 1. BaseASCII /
            (codepoint >= 0x0020 && codepoint <= 0x002F) ||
            (codepoint >= 0x003A && codepoint <= 0x0040) ||
            (codepoint >= 0x005B && codepoint <= 0x0060) ||
            (codepoint >= 0x007B && codepoint <= 0x007E) ||
            // Translated comment.
            (codepoint >= 0xFF00 && codepoint <= 0xFFEF) ||
            // 3. Emoji
            (codepoint >= 0x2600 && codepoint <= 0x27BF) ||
            (codepoint >= 0x1F300 && codepoint <= 0x1F5FF) ||
            (codepoint >= 0x1F600 && codepoint <= 0x1F64F) ||
            (codepoint >= 0x1F680 && codepoint <= 0x1F6FF) ||
            (codepoint >= 0x1F900 && codepoint <= 0x1F9FF) ||
            (codepoint >= 0x1FA00 && codepoint <= 0x1FAFF) ||
            // Translated comment.
            (codepoint >= 0x2200 && codepoint <= 0x22FF) ||
            (codepoint >= 0x2100 && codepoint <= 0x214F) ||
            (codepoint >= 0x2500 && codepoint <= 0x257F) ||
            (codepoint >= 0x1D400 && codepoint <= 0x1D7FF) ||
            // Translated comment.
            (codepoint >= 0x2190 && codepoint <= 0x21FF) ||
            (codepoint >= 0x2900 && codepoint <= 0x297F) ||
            (codepoint >= 0x2B00 && codepoint <= 0x2BFF) ||
            // 6. /Special
            (codepoint >= 0x2000 && codepoint <= 0x206F) ||
            (codepoint >= 0x2300 && codepoint <= 0x23FF) ||
            (codepoint >= 0x3200 && codepoint <= 0x32FF) ||
            // Translated comment.
            (codepoint >= 0x00A0 && codepoint <= 0x00FF) ||
            (codepoint >= 0x20A0 && codepoint <= 0x20CF) ||
            // Translated comment.
            (codepoint == 0x00A9 || codepoint == 0x00AE || codepoint == 0x2122 || codepoint == 0x2605) ||
            // 9. Emoji /
            (codepoint >= 0x1F3FB && codepoint <= 0x1F3FF) ||
            (codepoint == 0xFE0F || codepoint == 0x200D)
            );
    }

    bool nfilterword::is_emojispecial(const std::u32string& astr)
    {
        for (std::size_t i = 0; i < astr.size(); ++i)
        {
            if (is_emojispecial(astr[i]))
            {
                return true;
            }
        }
        return false;
    }

    void test_nfilterword()
    { 
        {// Checkutf8
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
            nfilterword::init();
            std::vector<std::string> filterWords = {
                "敏感词", "脏话", "123456", "Test"
            };
            for (auto& item : filterWords)
            {
                tools::to_utf8(item, item);
            }
            
            for (auto& wiord : filterWords)
            {
                nfilterword::load(wiord);
            }

            nfilterword::build();

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
                auto filtered = nfilterword::filter(text);
                tools::to_asscii(filtered, filtered);
                tools::to_asscii(text, text);
                std::cout << "sorc:" << text << std::endl;
                std::cout << "obj:" << filtered << std::endl;
            }

            {// Whether
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
                for (std::size_t i = 0; i < lnames2.size(); ++i)
                {
                    nfilterword::enfilter lerror = nfilterword::check_name(lnames2[i], 1, 20);
                    std::cout << lnames[i] << ":" << nfilterword::enfilter_message(lerror) << std::endl;
                }               
            }
        }
    }
}//namespace ngl