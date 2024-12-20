#pragma once

#include "serialize.h"
#include "type.h"
#include "pack.h"
#include "nlog.h"

namespace ngl
{
	class encryption_bytexor
	{
	public:
		template <typename Y>
		static int bytes(Y&)
		{
			return 0;
		}

		template <typename Y>
		static int bytes(np_actor_forward<Y, EPROTOCOL_TYPE_CUSTOM, true, ngl::forward>& adata)
		{
			return adata.m_recvpack->m_len - adata.m_recvpack->m_pos;
		}

		template <typename Y>
		static int bytes(np_actor_forward<Y, EPROTOCOL_TYPE_PROTOCOLBUFF, true, ngl::forward>& adata)
		{
			return adata.m_recvpack->m_len - adata.m_recvpack->m_pos;
		}

		template <typename Y>
		static int bytes(np_actor_forward<Y, EPROTOCOL_TYPE_CUSTOM, false, ngl::forward>& adata)
		{
			return adata.m_recvpack->m_len - adata.m_recvpack->m_pos;
		}

		template <typename Y>
		static int bytes(np_actor_forward<Y, EPROTOCOL_TYPE_PROTOCOLBUFF, false, ngl::forward>& adata)
		{
			return adata.m_recvpack->m_len - adata.m_recvpack->m_pos;
		}

		template <typename Y>
		static int recv_bytes(Y&)
		{
			return 0;
		}

		template <typename Y>
		static int recv_bytes(np_actor_forward<Y, EPROTOCOL_TYPE_CUSTOM, true, ngl::forward>& adata)
		{
			return adata.m_recvpack != nullptr ? adata.m_recvpack->m_len : 0;
		}

		template <typename Y>
		static int recv_bytes(np_actor_forward<Y, EPROTOCOL_TYPE_PROTOCOLBUFF, true, ngl::forward>& adata)
		{
			return adata.m_recvpack != nullptr ? adata.m_recvpack->m_len : 0;
		}

		template <typename Y>
		static int recv_bytes(np_actor_forward<Y, EPROTOCOL_TYPE_CUSTOM, false, ngl::forward>& adata)
		{
			return adata.m_recvpack != nullptr ? adata.m_recvpack->m_len : 0;
		}

		template <typename Y>
		static int recv_bytes(np_actor_forward<Y, EPROTOCOL_TYPE_PROTOCOLBUFF, false, ngl::forward>& adata)
		{
			return adata.m_recvpack != nullptr ? adata.m_recvpack->m_len : 0;
		}

		static bool check_xor(int aprotocolnum)
		{
			if (tprotocol::protocol<np_gm>() == aprotocolnum
				|| tprotocol::protocol<np_gm_response>() == aprotocolnum)
			{
				return false;
			}
			return sysconfig::isxor();
		}

		static bool check_xor(std::shared_ptr<pack>& apack)
		{
			return check_xor(apack->m_head.getvalue(EPH_PROTOCOLNUM));
		}

		template <typename Y>
		static bool check_xor(Y& adata)
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
						"[##structbytes::tostruct()] [T:{}] [{} != {}]"
						, tools::type_name<T>(), apack->m_head.getvalue(EPH_BYTES), apack->m_pos
					);
					return false;
				}

				// ### encryption bytexor
				if (encryption_bytexor::check_xor(apack))
				{
					ngl::tools::bytexor(apack->m_buff, apack->m_len, 0);
				}
				
				if (adata.ParseFromArray(apack->m_buff, apack->m_head.getvalue(EPH_BYTES)) == false)
				{
					return false;
				}
				if (aissetpos == true)
				{
					apack->m_pos = adata.ByteSize();
				}
			}
			return true;
		}

		static bool tobytes(std::shared_ptr<pack>& apack, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::pair<char*, int> lpair;
			apack->m_head.reservebuff(apack->m_buff, apack->m_len, lpair);
			if (adata.SerializeToArray(lpair.first, lpair.second) == false)
			{
				log_error()->print("[##structbytes::tobytes()] [T:{}] [actorid:{}] [requestactorid:{}] ", tools::type_name<T>(), aactorid, arequestactorid);
				return false;
			}

			// # encryption bytexor
			if(encryption_bytexor::check_xor(adata))
			{
				ngl::tools::bytexor(lpair.first, lpair.second, 0);
			}
			
			// ### sethead start ###
			apack->m_head.m_data[EPH_BYTES] = (apack->m_len - pack_head::size()) + encryption_bytexor::bytes(adata);
			apack->m_head.set_mask();
			apack->m_head.set_protocol(tprotocol::protocol<T>());
			apack->m_head.set_actor(aactorid, arequestactorid);
			apack->m_head.set_time();
			apack->m_head.set_protocoltype(tprotocol::protocol_type<T>());
			//log_error()->print(
			// "##tobytes## tprotocol::protocol<{}>() = {}", 
			// tools::type_name<T>(), tprotocol::protocol<T>()
			//);
			// ### sethead finish ###

			ngl::serialize lser2(apack->m_buff, apack->m_len);
			if (!apack->m_head.push(lser2))
			{
				return false;
			}
			apack->m_pos = apack->m_len;
			return true;
		}
	};
}// namespace ngl