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

#include <array>
#include <limits>

namespace ngl
{
    namespace
    {
        std::array<int, 256> g_root_transitions{};
        std::vector<int> g_output_links;

        int find_child(const std::vector<nacnode>& anodes, int node, unsigned char c)
        {
            const auto& lchildren = anodes[node].m_children;
            if (auto litor = lchildren.find(static_cast<char>(c)); litor != lchildren.end())
            {
                return litor->second;
            }
            return -1;
        }

        bool is_ascii_emojispecial(unsigned char c)
        {
            return c <= 0x1F ||
                c == 0x7F ||
                (c >= 0x20 && c <= 0x2F) ||
                (c >= 0x3A && c <= 0x40) ||
                (c >= 0x5B && c <= 0x60) ||
                (c >= 0x7B && c <= 0x7E);
        }
    }

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
        g_output_links.assign(m_nodes.size(), -1);
        g_root_transitions.fill(m_root);
    }

    void nfilterword::clear()
    {
        m_nodes.clear();
        m_root = -1;
        g_output_links.clear();
        g_root_transitions.fill(-1);
    }

    void nfilterword::advance_state(char c, int& cur)
    {
        const unsigned char lbyte = static_cast<unsigned char>(c);
        if (m_root < 0 || static_cast<std::size_t>(m_root) >= m_nodes.size())
        {
            cur = -1;
            return;
        }
        if (cur < 0 || static_cast<std::size_t>(cur) >= m_nodes.size())
        {
            cur = m_root;
        }
        if (cur == m_root)
        {
            cur = g_root_transitions[lbyte];
            return;
        }

        while (cur != m_root)
        {
            const int lnext = find_child(m_nodes, cur, lbyte);
            if (lnext >= 0)
            {
                // Fast path: follow the trie edge without touching fail links.
                cur = lnext;
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

        cur = g_root_transitions[lbyte];
    }

    bool nfilterword::append_matches(int cur, int i, std::vector<std::pair<int, int>>& res)
    {
        if (cur < 0 || static_cast<std::size_t>(cur) >= m_nodes.size() || cur == m_root)
        {
            return false;
        }

        bool lmatched = false;
        if (m_nodes[cur].len > 0)
        {
            res.emplace_back(i - m_nodes[cur].len + 1, m_nodes[cur].len);
            lmatched = true;
        }

        for (int temp = g_output_links[cur]; temp >= 0 && static_cast<std::size_t>(temp) < m_nodes.size() && temp != m_root; temp = g_output_links[temp])
        {
            res.emplace_back(i - m_nodes[temp].len + 1, m_nodes[temp].len);
            lmatched = true;
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
        return cur >= 0 && m_nodes[cur].m_has_match;
    }

    void nfilterword::load(const std::string& apattern)
    {
        if (apattern.empty())
        {
            return;
        }
        ensure_initialized();
        int cur = m_root;

        for (char lraw : apattern)
        {
            const unsigned char c = static_cast<unsigned char>(lraw);
            auto litor = m_nodes[cur].m_children.find(static_cast<char>(c));
            if (litor == m_nodes[cur].m_children.end())
            {
                const int lnext = newnode();
                g_output_links.push_back(-1);
                litor = m_nodes[cur].m_children.emplace(static_cast<char>(c), lnext).first;
                if (cur == m_root)
                {
                    g_root_transitions[c] = lnext;
                }
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
        if (g_output_links.size() < m_nodes.size())
        {
            g_output_links.resize(m_nodes.size(), -1);
        }
        g_output_links[m_root] = -1;
        m_nodes[m_root].m_has_match = m_nodes[m_root].len > 0;
        std::queue<int> q;
        for (auto& pair : m_nodes[m_root].m_children)
        {
            int child = pair.second;
            m_nodes[child].m_fail = m_root;
            g_output_links[child] = -1;
            m_nodes[child].m_has_match = m_nodes[child].len > 0;
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
                // Output links chain together terminal nodes reachable through fail edges.
                g_output_links[u] = m_nodes[lnext].len > 0 ? lnext : g_output_links[lnext];
                m_nodes[u].m_has_match = m_nodes[u].len > 0 || g_output_links[u] != -1;
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
        // Matches are byte ranges because the trie is built over UTF-8 bytes.
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
                const unsigned char lfirst = static_cast<unsigned char>(*liter);
                if (lfirst < 0x80)
                {
                    ++liter;
                    if (is_ascii_emojispecial(lfirst))
                    {
                        continue;
                    }

                    advance_state(static_cast<char>(lfirst), cur);
                    if (m_nodes[cur].m_has_match)
                    {
                        return true;
                    }
                    continue;
                }

                const auto lcode_begin = liter;
                const char32_t lcodepoint = utf8::next(liter, lend);
                if (is_emojispecial(lcodepoint))
                {
                    // Special characters do not participate in forbidden-word matching.
                    continue;
                }
                for (auto lbyte = lcode_begin; lbyte != liter; ++lbyte)
                {
                    advance_state(*lbyte, cur);
                    if (m_nodes[cur].m_has_match)
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
        return (codepoint >= 0x0041 && codepoint <= 0x005A) ||   // Latin uppercase letters.
            (codepoint >= 0x0061 && codepoint <= 0x007A) ||      // Latin lowercase letters.
            (codepoint >= 0x4E00 && codepoint <= 0x9FFF) ||      // CJK Unified Ideographs.
            (codepoint >= 0x3400 && codepoint <= 0x4DBF) ||      // CJK Extension A.
            (codepoint >= 0x20000 && codepoint <= 0x2A6DF) ||    // CJK Extension B.
            (codepoint >= 0x3040 && codepoint <= 0x309F) ||      // Hiragana.
            (codepoint >= 0x30A0 && codepoint <= 0x30FF) ||      // Katakana.
            (codepoint >= 0xAC00 && codepoint <= 0xD7AF) ||      // Hangul syllables.
            (codepoint >= 0x0400 && codepoint <= 0x04FF) ||      // Cyrillic.
            (codepoint >= 0x0370 && codepoint <= 0x03FF) ||      // Greek.
            (codepoint >= 0x0600 && codepoint <= 0x06FF);        // Arabic.
    }

    bool nfilterword::is_emojispecial(char32_t codepoint)
    {
        // Normal language letters are always allowed.
        if (is_language_char(codepoint))
        {
            return false;
        }

        // Reject punctuation, emoji, and other symbol-heavy ranges that should not appear in names.
        return (
            // ASCII control bytes and punctuation, including whitespace controls.
            (codepoint >= 0x0000 && codepoint <= 0x001F) ||
            (codepoint == 0x007F) ||
            (codepoint >= 0x0020 && codepoint <= 0x002F) ||
            (codepoint >= 0x003A && codepoint <= 0x0040) ||
            (codepoint >= 0x005B && codepoint <= 0x0060) ||
            (codepoint >= 0x007B && codepoint <= 0x007E) ||
            // Full-width compatibility forms and punctuation.
            (codepoint >= 0xFF00 && codepoint <= 0xFFEF) ||
            // Emoji and pictograph blocks.
            (codepoint >= 0x2600 && codepoint <= 0x27BF) ||
            (codepoint >= 0x1F300 && codepoint <= 0x1F5FF) ||
            (codepoint >= 0x1F600 && codepoint <= 0x1F64F) ||
            (codepoint >= 0x1F680 && codepoint <= 0x1F6FF) ||
            (codepoint >= 0x1F900 && codepoint <= 0x1F9FF) ||
            (codepoint >= 0x1FA00 && codepoint <= 0x1FAFF) ||
            // Math and technical symbol blocks.
            (codepoint >= 0x2200 && codepoint <= 0x22FF) ||
            (codepoint >= 0x2100 && codepoint <= 0x214F) ||
            (codepoint >= 0x2500 && codepoint <= 0x257F) ||
            (codepoint >= 0x1D400 && codepoint <= 0x1D7FF) ||
            // Arrows and geometric symbol ranges.
            (codepoint >= 0x2190 && codepoint <= 0x21FF) ||
            (codepoint >= 0x2900 && codepoint <= 0x297F) ||
            (codepoint >= 0x2B00 && codepoint <= 0x2BFF) ||
            // General punctuation and enclosed marks.
            (codepoint >= 0x2000 && codepoint <= 0x206F) ||
            (codepoint >= 0x2300 && codepoint <= 0x23FF) ||
            (codepoint >= 0x3200 && codepoint <= 0x32FF) ||
            // Latin-1 supplement and currency symbol ranges.
            (codepoint >= 0x00A0 && codepoint <= 0x00FF) ||
            (codepoint >= 0x20A0 && codepoint <= 0x20CF) ||
            // Common standalone symbol marks.
            (codepoint == 0x00A9 || codepoint == 0x00AE || codepoint == 0x2122 || codepoint == 0x2605) ||
            // Emoji modifiers and joiners used to build composite emoji.
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
