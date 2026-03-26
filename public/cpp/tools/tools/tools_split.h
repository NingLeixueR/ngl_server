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
// File overview: Declares split and splice helpers for tools.

#pragma once

#include "tools_text.h"
#include "tools_misc.h"

namespace ngl
{
	namespace tools
	{
		template <typename T>
		bool splite(const char* abuff, const char* afg, std::vector<T>& avec)
		{
			std::vector<std::string> lvec;
			if (!splite(abuff, afg, lvec))
			{
				return false;
			}

			std::vector<T> lparsed;
			lparsed.reserve(lvec.size());
			try
			{
				for (const std::string& litem : lvec)
				{
					lparsed.emplace_back(tools::lexical_cast<T>(litem.c_str()));
				}
			}
			catch (const boost::bad_lexical_cast&)
			{
				throw std::string("lexical_cast error");
			}
			avec = std::move(lparsed);
			return true;
		}

		template <typename T>
		bool splite(const char* abuff, const char* afg, std::set<T>& aset)
		{
			std::vector<std::string> lvec;
			if (!splite(abuff, afg, lvec))
			{
				return false;
			}

			std::set<T> lparsed;
			try
			{
				for (const std::string& litem : lvec)
				{
					lparsed.emplace(tools::lexical_cast<T>(litem.c_str()));
				}
			}
			catch (const boost::bad_lexical_cast&)
			{
				throw std::string("lexical_cast error");
			}
			aset = std::move(lparsed);
			return true;
		}

		bool splite(const char* abuff, const char* afg, std::vector<std::string>& avec);

		template <typename T>
		bool splite(int32_t aindex, const std::vector<std::string>& avec, T& adata)
		{
			if (aindex < 0)
			{
				return false;
			}
			const std::size_t lidx = static_cast<std::size_t>(aindex);
			if (lidx >= avec.size())
			{
				return false;
			}
			try
			{
				adata = tools::lexical_cast<T>(avec[lidx].c_str());
			}
			catch (...)
			{
				return false;
			}
			return true;
		}

		template <typename T>
		bool splite(int32_t aindex, const std::vector<std::string>& avec, std::vector<T>& adata)
		{
			if (aindex < 0)
			{
				return false;
			}
			for (int32_t li = aindex; li < static_cast<int32_t>(avec.size()); ++li)
			{
				adata.push_back(tools::lexical_cast<T>(avec[li].c_str()));
			}
			return true;
		}

		bool splite(int32_t aindex, const std::vector<std::string>& avec, std::vector<std::string>& adata)
		{
			if (aindex < 0)
			{
				return false;
			}
			adata.insert(adata.end(), avec.begin() + aindex, avec.end());
			return true;
		}

		template <std::size_t... INDEX, typename TTUPLE>
		bool splite_tuple(std::index_sequence<INDEX...>, const std::vector<std::string>& avec, TTUPLE& atup)
		{
			return (splite(static_cast<int32_t>(INDEX), avec, std::get<INDEX>(atup)) && ...);
		}

		template <std::size_t... INDEX, typename... ARGS>
		bool splite(std::index_sequence<INDEX...>, const char* abuff, const char* afg, ARGS&... aargs)
		{
			std::vector<std::string> lvec;
			if (!splite(abuff, afg, lvec))
			{
				return false;
			}

			std::tuple<std::decay_t<ARGS>...> lparsed;
			if (!splite_tuple(std::index_sequence<INDEX...>{}, lvec, lparsed))
			{
				return false;
			}

			((aargs = std::move(std::get<INDEX>(lparsed))), ...);
			return true;
		}

		template <typename... ARGS>
		bool splite(const char* abuff, const char* afg, ARGS&... aargs)
		{
			return splite(std::make_index_sequence<sizeof...(ARGS)>{}, abuff, afg, aargs...);
		}

		template <typename TFIRST = std::string, typename TSECOND = std::string>
		bool splite_special(
			const char* astr,
			const char* akey1,
			const char* akey2,
			std::vector<std::pair<TFIRST, TSECOND>>& avec)
		{
			if (astr == nullptr || akey1 == nullptr || akey2 == nullptr)
			{
				return false;
			}
			std::string ltmp = astr;
			ngl::tools::replace_ret(akey1, "", ltmp, ltmp);
			std::vector<std::string> lvec;
			if (!ngl::tools::splite(ltmp.c_str(), akey2, lvec))
			{
				return false;
			}

			std::vector<std::pair<TFIRST, TSECOND>> lparsed;
			lparsed.reserve(lvec.size());
			for (const std::string& litem : lvec)
			{
				std::pair<TFIRST, TSECOND> lpair;
				if (!ngl::tools::splite(litem.c_str(), ":", lpair.first, lpair.second))
				{
					return false;
				}
				lparsed.emplace_back(std::move(lpair));
			}
			avec = std::move(lparsed);
			return true;
		}

		template <typename TFIRST = std::string, typename TSECOND = std::string>
		bool splite_special(
			const char* astr,
			const char* akey1,
			const char* akey2,
			std::map<TFIRST, TSECOND>& amap)
		{
			if (astr == nullptr || akey1 == nullptr || akey2 == nullptr)
			{
				return false;
			}
			std::string ltmp = astr;
			ngl::tools::replace_ret(akey1, "", ltmp, ltmp);
			std::vector<std::string> lvec;
			if (!ngl::tools::splite(ltmp.c_str(), akey2, lvec))
			{
				return false;
			}

			std::map<TFIRST, TSECOND> lparsed;
			for (const std::string& litem : lvec)
			{
				std::pair<TFIRST, TSECOND> lpair;
				if (!ngl::tools::splite(litem.c_str(), ":", lpair.first, lpair.second))
				{
					return false;
				}
				lparsed.emplace(std::move(lpair));
			}
			amap = std::move(lparsed);
			return true;
		}

		template <typename T>
		inline std::function<std::string(const T&)> m_splicing = [](const T& adata)
		{
			return tools::lexical_cast<std::string>(adata);
		};

		template <typename TKEY, typename TVAL>
		inline std::function<std::string(const TKEY&, const TVAL&)> m_splicingmap = [](const TKEY& akey, const TVAL& aval)
		{
			std::string lret = tools::lexical_cast<std::string>(akey);
			lret += ':';
			lret += tools::lexical_cast<std::string>(aval);
			return lret;
		};

		template <typename T>
		bool splicing(
			const std::vector<T>& avec,
			const char* afg,
			std::string& astr,
			const std::function<std::string(const T&)>& afun = m_splicing<T>)
		{
			for (std::size_t li = 0; li < avec.size(); ++li)
			{
				if (li != 0)
				{
					astr += afg;
				}
				astr += afun(avec[li]);
			}
			return true;
		}

		bool splicing(const std::vector<std::string>& avec, const char* afg, std::string& astr, int32_t apos = 0);

		template <typename T>
		bool splicing(
			const std::set<T>& avec,
			const char* afg,
			std::string& astr,
			const std::function<std::string(const T&)>& afun = m_splicing<T>)
		{
			for (auto liter = avec.begin(); liter != avec.end(); ++liter)
			{
				if (liter != avec.begin())
				{
					astr += afg;
				}
				astr += afun(*liter);
			}
			return true;
		}

		template <typename TKEY, typename TVAL>
		bool splicing(
			const std::map<TKEY, TVAL>& amap,
			const char* afg,
			std::string& astr,
			const std::function<std::string(const TKEY&, const TVAL&)>& afun = m_splicingmap<TKEY, TVAL>)
		{
			for (auto liter = amap.begin(); liter != amap.end(); ++liter)
			{
				if (liter != amap.begin())
				{
					astr += afg;
				}
				astr += afun(liter->first, liter->second);
			}
			return true;
		}

		template <typename TKEY, typename TVAL>
		bool splicing(
			const google::protobuf::Map<TKEY, TVAL>& amap,
			const char* afg,
			std::string& astr,
			const std::function<std::string(const TKEY&, const TVAL&)>& afun = m_splicingmap<TKEY, TVAL>)
		{
			for (auto liter = amap.begin(); liter != amap.end(); ++liter)
			{
				if (liter != amap.begin())
				{
					astr += afg;
				}
				astr += afun(liter->first, liter->second);
			}
			return true;
		}

		bool splicing(const std::set<std::string>& aset, const char* afg, std::string& astr);

		template <typename T>
		bool splicing(int32_t aindex, const char* afg, std::string& astr, T& adata)
		{
			try
			{
				if (aindex != 0)
				{
					astr += afg;
				}
				astr += tools::lexical_cast<std::string>(adata);
			}
			catch (...)
			{
				return false;
			}
			return true;
		}

		template <std::size_t... INDEX, typename... ARGS>
		bool splicing(std::index_sequence<INDEX...>, const char* afg, std::string& astr, ARGS&... aargs)
		{
			return (splicing<ARGS>(static_cast<int32_t>(INDEX), afg, astr, aargs) && ...);
		}

		template <typename... ARGS>
		bool splicing(const char* afg, std::string& astr, ARGS&... aargs)
		{
			return splicing(std::make_index_sequence<sizeof...(ARGS)>{}, afg, astr, aargs...);
		}
	}
}
