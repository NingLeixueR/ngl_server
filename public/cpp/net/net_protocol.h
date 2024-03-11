#pragma once

#include <vector>
#include <memory>
#include <boost/array.hpp>

#include "handle_pram.h"
#include "structbytes.h"
#include "actor_base.h"
#include "asio_tcp.h"
#include "asio_kcp.h"
#include "asio_ws.h"
#include "segpack.h"
#include "impl.h"
#include "pack.h"

namespace ngl
{
	template <typename T>
	class net_pack
	{
	public:
		static std::shared_ptr<pack> npack(bpool* apool, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			ngl::serialize_bytes lbytes;
			int lbuffbyte = adata.ByteSize() + pack_head::size();
			std::shared_ptr<pack> lpack = pack::make_pack(apool, lbuffbyte);			
			if (structbytes<T>::tobytes(lpack, adata, aactorid, arequestactorid) == false)
				return nullptr;
			return lpack;
		}
	};

	class sendpack_t
	{
	public:
		template <typename Y>
		static std::shared_ptr<pack>& get_pack(Y&)
		{
			static std::shared_ptr<pack> lnull = nullptr;
			return lnull;
		}

		template <typename Y>
		static std::shared_ptr<pack>& get_pack(
			np_actor_forward<Y, EPROTOCOL_TYPE_CUSTOM, true, ngl::forward>& adata
		)
		{
			return adata.m_recvpack;
		}

		template <typename Y>
		static std::shared_ptr<pack>& get_pack(
			np_actor_forward<Y, EPROTOCOL_TYPE_PROTOCOLBUFF, true, ngl::forward>& adata
		)
		{
			return adata.m_recvpack;
		}

		template <typename Y>
		static std::shared_ptr<pack>& get_pack(
			np_actor_forward<Y, EPROTOCOL_TYPE_CUSTOM, false, ngl::forward>& adata
		)
		{
			return adata.m_recvpack;
		}

		template <typename Y>
		static std::shared_ptr<pack>& get_pack(
			np_actor_forward<Y, EPROTOCOL_TYPE_PROTOCOLBUFF, false, ngl::forward>& adata
		)
		{
			return adata.m_recvpack;
		}
	};

	class net_protocol
	{
	protected:
		struct impl_net_protocol;
		ngl::impl<impl_net_protocol> m_impl_net_protocol;

		net_protocol();

		bool socket_recv(int asessionid, int aislanip, const char* abuff, uint32_t abufflen);
	public:
		~net_protocol();

		bpool& get_pool();
		
		//## i16_port		aport				端口号
		//## i32_threadsize asocketthreadnum	线程数
		//## bool			aouternet			是否允许非内网主动连接
		virtual bool init(i16_port aport, i32_threadsize asocketthreadnum, bool	aouternet);

		//## 关闭socket连接以及加载的数据
		//## 通知上层应用
		virtual void close(i32_sessionid asession);
		//## 逻辑层主动关闭连接(这样就不需要通知上层应用)
		virtual void close_net(i32_sessionid asession) = 0;

		//## 发送消息
		virtual bool net_send(i32_sessionid asession, std::shared_ptr<pack>& lpack) = 0;
		virtual bool net_send(i32_sessionid asession, std::shared_ptr<void>& lpack) = 0;
		//## 服务器是否存在此session id
		virtual bool exist_session(i32_sessionid asession) = 0;

		int socketthreadnum();
		int port();
		bool sendpack(i32_sessionid asession, std::shared_ptr<pack>& apack);
		bool sendpack(i32_sessionid asession, std::shared_ptr<void>& apack);
		bool sendpackbyserver(i32_serverid aserverid, std::shared_ptr<pack>& apack);
		void set_server(i32_serverid aserverid, i32_sessionid asession);
		i32_sessionid get_sessionid(i32_serverid aserverid);
		i32_serverid get_serverid(i32_sessionid asession);
		i64_actorid moreactor();

		virtual void set_close(
			int asession
			, const std::string& aip
			, i16_port aport
			, const std::function<void(i32_sessionid)>& afun
		) = 0;

		virtual bool connect(
			const std::string& aip
			, i16_port aport
			, const std::function<void(i32_sessionid)>& afun
		) = 0;

		virtual bool connect(
			i32_serverid aserverid
			, const std::string& aip
			, i16_port aport
			, const std::function<void(i32_sessionid)>& afun
			, bool await
			, bool areconnection
		);

		//// --- 发送消息
		template <typename T>
		bool send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&get_pool(), adata, aactorid, arequestactorid);
			if (lpack == nullptr)
				return false;
			if (sendpack(asession, lpack) == false)
				return false;
			std::shared_ptr<pack>& lpack2 = sendpack_t::get_pack(adata);
			if (lpack2 == nullptr)
				return true;
			if (encryption_bytexor::check_xor(lpack2))
				ngl::encryption::bytexor(lpack2->m_buff, lpack2->m_len, lpack->m_len - pack_head::size());
			if (sendpack(asession, lpack2) == false)
				return false;
			return true;
		}

		// key: session values:aactorid
		// std::map<uint32_t, uint32_t>& asession
		template <typename T>
		bool sendmore(const std::map<i32_sessionid, i64_actorid>& asession, T& adata, i64_actorid aactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&get_pool(), adata, aactorid, 0);
			if (lpack == nullptr)
				return false;
			//LogLocalWarn("SEND Session[%] Protocol Num[%] Name[%] Data[%]!", asession, (int)T::PROTOCOL, T::name(), adata);

			std::shared_ptr<pack>& lpack_ = sendpack_t::get_pack(adata);
			if (lpack_ != nullptr)
			{
				if (encryption_bytexor::check_xor(lpack_))
				{
					char* lbuff = &lpack_->m_buff[lpack_->m_pos];
					int32_t llen = lpack_->m_len - lpack_->m_pos;
					int32_t lpos = lpack->m_len - pack_head::size();
					ngl::encryption::bytexor(lbuff, llen, lpos);
				}
			}
			
			for (auto& item : asession)
			{
				lpack->set_actor(item.second, aactorid);
				net_send(item.first, lpack);
				if (lpack_ != nullptr)
					net_send(item.first, lpack_);
			}
			return  true;
		}

		template <typename T, typename TSTL>
		bool sendmore_stl(const TSTL& asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::shared_ptr<pack> lpack = net_pack<T>::npack(&get_pool(), adata, aactorid, arequestactorid);
			if (lpack == nullptr)
				return false;
			//LogLocalWarn("SEND Session[%] Protocol Num[%] Name[%] Data[%]!", asession, (int)T::PROTOCOL, T::name(), adata);
			std::shared_ptr<pack>& lpack_ = sendpack_t::get_pack(adata);
			if (lpack_ != nullptr)
			{
				if (encryption_bytexor::check_xor(lpack_))
				{
					char* lbuff = &lpack_->m_buff[lpack_->m_pos];
					int32_t llen = lpack_->m_len - lpack_->m_pos;
					int32_t lpos = lpack->m_len - pack_head::size();
					ngl::encryption::bytexor(lbuff, llen, lpos);
				}
			}
			
			for (auto& item : asession)
			{
				net_send(item, lpack);
				if (lpack_ != nullptr)
					net_send(item, lpack_);				
			}
			return true;
		}

		template <typename T>
		bool sendmore(const std::vector<i32_sessionid>& asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			return sendmore_stl(asession, adata, aactorid, arequestactorid);
		}

		template <typename T>
		bool sendmore(const std::list<i32_sessionid>& asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			return sendmore_stl(asession, adata, aactorid, arequestactorid);
		}

		template <typename T>
		bool sendmore(const std::set<i32_sessionid>& asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			return sendmore_stl(asession, adata, aactorid, arequestactorid);
		}

		template <typename T>
		bool send_server(i32_serverid aserverid, T& adata, uint64_t aactorid, uint64_t arequestactorid)
		{
			i32_sessionid lsession = get_sessionid(aserverid);
			if (lsession == -1)
				return false;
			return send(lsession, adata, aactorid, arequestactorid);
		}

		template <typename T>
		void send_client(i32_actordataid auid, i16_area aarea, i32_gatewayid agateway, T& adata)
		{
			np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T> pro;
			pro.m_uid.push_back(auid);
			pro.m_area.push_back(aarea);
			pro.set_data(&adata);
			if (agateway != 0)
				sendtoserver(agateway, pro, moreactor(), moreactor());
		}

		template <typename T>
		void send_client(const std::vector<std::pair<i32_actordataid, i16_area>>& avec, i32_gatewayid agateway, T& adata)
		{
			np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T> pro;
			for (int i = 0; i < avec.size(); ++i)
			{
				pro.m_uid.push_back(avec[i].first);
				pro.m_area.push_back(avec[i].second);
			}
			pro.set_data(&adata);
			if (agateway != 0)
				sendtoserver(agateway, pro, moreactor(), moreactor());
		}
	};
}// namespace ngl