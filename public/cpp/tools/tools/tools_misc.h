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
// File overview: Declares misc helpers and protobuf traits for tools.

#pragma once

#include "tools_core.h"

namespace ngl
{
	namespace tools
	{
		std::size_t strlen(const char* astr);

		std::string& type_name_handle(std::string& aname);

		template <typename T>
		std::string& type_name()
		{
			static std::string lname = std::string(detail::comp_type<T>());
			static std::atomic_bool lfirst = true;
			if (lfirst.exchange(false))
			{
				type_name_handle(lname);
			}
			return lname;
		}

		template <typename T>
		std::string& type_name(const T*)
		{
			return type_name<T>();
		}

		std::vector<const char*> split_str(char* apbuff, int32_t abuff_cnt);

		template <std::size_t N>
		void split_str(char* apbuff, int32_t abuff_cnt, std::array<const char*, N>& aarr)
		{
			aarr.fill(nullptr);
			if (apbuff == nullptr || abuff_cnt <= 0)
			{
				return;
			}

			std::size_t lidx = 0;
			int32_t lbeg = 0;
			for (int32_t li = 0; li < abuff_cnt; ++li)
			{
				if (apbuff[li] != ',')
				{
					continue;
				}

				apbuff[li] = '\0';
				if (lidx < N)
				{
					aarr[lidx++] = &apbuff[lbeg];
				}
				lbeg = li + 1;
				if (lbeg < abuff_cnt && apbuff[lbeg] == ' ')
				{
					++lbeg;
				}
			}

			if (lidx < N)
			{
				aarr[lidx] = &apbuff[lbeg];
			}
		}

		bool bit(int32_t atype, int32_t acjson);

		void erase_repeat(std::string& astr, char ach);

		template <typename ETYPE>
		bool equal(ETYPE av1, ETYPE av2)
		{
			return av1 == av2;
		}

		int32_t less_member();

		template <typename T>
		auto less_member(const T& alhs, const T& arhs)
		{
			return alhs <=> arhs;
		}

		template <typename T, typename... TARGS>
		auto less_member(const T& alhs, const T& arhs, const TARGS&... amems)
		{
			if (alhs != arhs)
			{
				return less_member(alhs, arhs);
			}
			return less_member(amems...);
		}

		std::string_view trim_ascii_spaces(std::string_view avalue);
	}

	template <typename T, typename = void>
	struct has_pb_desc : std::false_type
	{};

	template <typename T>
	struct has_pb_desc<T, std::void_t<decltype(T::descriptor())>> : std::is_same<
		decltype(T::descriptor()),
		const google::protobuf::Descriptor*
	>
	{};

	template <typename T>
	struct is_protobuf_message : std::conjunction<
		std::is_base_of<google::protobuf::MessageLite, std::remove_cv_t<std::remove_reference_t<T>>>,
		has_pb_desc<std::remove_cv_t<std::remove_reference_t<T>>>
	>
	{};
}
