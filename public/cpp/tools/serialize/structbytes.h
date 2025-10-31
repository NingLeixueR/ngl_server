/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "nserialize.h"
#include "type.h"
#include "pack.h"
#include "nlog.h"

#include <set>

namespace ngl
{
	class encryption_bytexor
	{
		static std::set<i32_protocolnum> m_protocolnum;

		static void check_init()
		{
			m_protocolnum.insert(tprotocol::protocol<np_gm>());
			m_protocolnum.insert(tprotocol::protocol<np_gm_response>());
		}
	public:
		static bool check_xor(int aprotocolnum)
		{
			if (m_protocolnum.contains(aprotocolnum))
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
		static bool tostruct(std::shared_ptr<pack>& apack, T& adata, bool aissetpos = false)
		{
			if (apack->m_buff != nullptr)
			{
				if (apack->m_head.getvalue(EPH_BYTES) != apack->m_pos)
				{
					log_error()->print(
						"[##structbytes::tostruct() {} byte<{}!={}>]"
						, tools::type_name<T>(), apack->m_head.getvalue(EPH_BYTES), apack->m_pos
					);
					return false;
				}

				if (encryption_bytexor::check_xor(apack->m_head.getvalue(EPH_PROTOCOLNUM)))
				{
					ngl::tools::bytexor(apack->m_buff, apack->m_len, 0);
				}

				ngl::ser::serialize_pop lser(apack->m_buff, apack->m_head.getvalue(EPH_BYTES));
				if (!ngl::ser::nserialize::pop(&lser, adata))
				{
					return false;
				}
			}
			return true;
		}

		template <typename Y>
		static bool tobytes(std::shared_ptr<pack>& apack, Y& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::pair<char*, int> lpair;
			apack->m_head.reservebuff(apack->m_buff, apack->m_len, lpair);
			ngl::ser::serialize_push lser(lpair.first, lpair.second);
			if (!ngl::ser::nserialize::push(&lser, adata))
			{
				log_error()->print(
					"[##structbytes::tobytes() {} actorid:{} requestactorid:{}] "
					, tools::type_name<T>(), aactorid, arequestactorid
				);
				return false;
			}

			// # encryption bytexor
			if(encryption_bytexor::check_xor<T>())
			{
				ngl::tools::bytexor(lpair.first, lpair.second, 0);
			}
			
			// ### sethead start ###
			apack->m_head.m_data[EPH_BYTES] = (apack->m_len - pack_head::size());
			apack->m_head.set_mask();
			apack->m_head.set_time();
			apack->m_head.set_actor(aactorid, arequestactorid);
			apack->m_head.set_protocol(tprotocol::protocol<T>());

			ngl::ser::serialize_push lserhead(apack->m_buff, apack->m_len);
			if (!ngl::ser::nserialize::push(&lserhead, apack->m_head))
			{
				return false;
			}
			apack->m_pos = apack->m_len;
			return true;
		}
	};
}// namespace ngl