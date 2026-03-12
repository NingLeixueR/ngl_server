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

#include "tools/serialize/nserialize.h"
#include "actor/protocol/tprotocol.h"
#include "tools/serialize/pack.h"
#include "tools/type.h"

#include <format>
#include <iostream>
#include <set>

namespace ngl
{
	class encryption_bytexor
	{
	public:
		static i32_protocolnum get_registered_protocolnum(const char* aname)
		{
			if (aname == nullptr || *aname == '\0')
			{
				return -1;
			}
			tprotocol::info* lpinfo = tprotocol::get(aname);
			return lpinfo == nullptr ? -1 : lpinfo->m_protocol;
		}

		static bool check_xor(int aprotocolnum)
		{
			static const i32_protocolnum lprotocol1 = get_registered_protocolnum(tools::type_name<np_gm>().c_str());
			static const i32_protocolnum lprotocol2 = get_registered_protocolnum(tools::type_name<np_gm_response>().c_str());
			if ((lprotocol1 != -1 && aprotocolnum == lprotocol1) || (lprotocol2 != -1 && aprotocolnum == lprotocol2))
			{
				return false;
			}
			return sysconfig::isxor();
		}

		template <typename Y>
		static bool check_xor()
		{
			return check_xor(tprotocol::protocol<Y>());
		}
	};

	template <typename T>
	class structbytes
	{
	public:
		static bool tostruct(std::shared_ptr<pack>& apack, T& adata, [[maybe_unused]] bool aissetpos = false)
		{
			if (apack == nullptr)
			{
				return false;
			}
			const int32_t lbytes = apack->m_head.getvalue(EPH_BYTES);
			if (lbytes < 0)
			{
				return false;
			}
			if (apack->m_buff != nullptr)
			{
				if (lbytes != apack->m_pos)
				{
					std::cout << std::format("[##structbytes::tostruct() {} byte<{}!={}>]", tools::type_name<T>(), lbytes, apack->m_pos) << std::endl;
					return false;
				}

				if (encryption_bytexor::check_xor(apack->m_head.getvalue(EPH_PROTOCOLNUM)))
				{
					ngl::tools::bytexor(apack->m_buff, apack->m_len, 0);
				}

				ngl::ser::serialize_pop lser(apack->m_buff, lbytes);
				if (!ngl::ser::nserialize::pop(&lser, adata))
				{
					return false;
				}
			}
			else if (lbytes != 0)
			{
				return false;
			}
			return true;
		}

		template <typename Y>
		static bool tobytes(std::shared_ptr<pack>& apack, const Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			if (apack == nullptr || apack->m_buff == nullptr)
			{
				return false;
			}
			std::pair<char*, int> lpair;
			apack->m_head.reservebuff(apack->m_buff, apack->m_len, lpair);
			if (lpair.first == nullptr)
			{
				return false;
			}
			ngl::ser::serialize_push lser(lpair.first, lpair.second);
			if (!ngl::ser::nserialize::push(&lser, adata))
			{
				std::cout << std::format("[##structbytes::tobytes() {} actorid:{} requestactorid:{}]", tools::type_name<T>(), aactorid, arequestactorid) << std::endl;
				return false;
			}
			const int32_t lpayloadbytes = lser.pos();
			if (lpayloadbytes < 0 || lpayloadbytes > lpair.second)
			{
				return false;
			}

			// # encryption bytexor
			if (lpayloadbytes > 0 && encryption_bytexor::check_xor<T>())
			{
				ngl::tools::bytexor(lpair.first, lpayloadbytes, 0);
			}
			
			// ### sethead start ###
			apack->m_head.m_data[EPH_BYTES] = lpayloadbytes;
			apack->m_head.set_mask();
			apack->m_head.set_time();
			apack->m_head.set_actor(aactorid, arequestactorid);
			apack->m_head.set_protocol(tprotocol::protocol<T>());

			ngl::ser::serialize_push lserhead(apack->m_buff, apack->m_len);
			if (!ngl::ser::nserialize::push(&lserhead, apack->m_head))
			{
				return false;
			}
			apack->m_len = pack_head::size() + lpayloadbytes;
			apack->m_pos = apack->m_len;
			return true;
		}
	};
}// namespace ngl
