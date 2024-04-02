#pragma once

#include "manage_session.h"
#include "ttab_servers.h"
#include "xmlnode.h"
#include "net_tcp.h"
#include "net_ws.h"

namespace ngl
{
	class nets
	{
		static std::array<net_protocol*, ENET_COUNT> m_net;
	public:
		static net_protocol* net_first()
		{
			for (int i = 0; i < ENET_COUNT; ++i)
			{
				if (m_net[i] != nullptr)
					return m_net[i];
			}
			return nullptr;
		}

		static ENET_PROTOCOL session2type(i32_session asession)
		{
			return (ENET_PROTOCOL)(asession >> 24);
		}

		static net_protocol* net(i32_session asession)
		{
			ENET_PROTOCOL ltype = session2type(asession);
			return nettype(ltype);
		}

		static net_protocol* nettype(ENET_PROTOCOL atype)
		{
			if (ttab_servers::isefficient(atype) && m_net[atype] != nullptr)
			{
				return m_net[atype];
			}
			return nullptr;
		}

		static bool init(i32_threadsize asocketthreadnum, bool aouternet)
		{
			tab_servers* tab = ttab_servers::tab();
			if (tab == nullptr)
				return false;
			for (net_works& item : tab->m_net)
			{
				if (m_net[item.m_type] != nullptr)
					continue;
				auto& lserver = m_net[item.m_type];
				switch (item.m_type)
				{
				case ENET_TCP:
					lserver = new net_tcp((int8_t)ENET_TCP);
					break;
				case ENET_WS:
					lserver = new net_ws((int8_t)ENET_WS);
					break;
				default:
					continue;
				}
				lserver->init(item.m_port, asocketthreadnum, aouternet);
			}
			return true;
		}

		template <typename T>
		static bool sendbyserver(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			i32_session lsession = manage_session::get_sessionid(aserverid);
			if (lsession == -1)
				return false;
			return sendbysession(lsession, adata, aactorid, arequestactorid);
		}

		template <typename T>
		static bool sendbysession(i32_session asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			net_protocol* lpprotocol = net(asession);
			if (lpprotocol == nullptr)
			{
				return false;
			}
			return lpprotocol->send(asession, adata, aactorid, arequestactorid);
		}

		template <typename T>
		static bool sendmore(const std::map<i32_sessionid, i64_actorid>& asession, T& adata, i64_actorid aactorid)
		{
			std::set<ENET_PROTOCOL> lset;
			for (auto itor = asession.begin(); itor != asession.end(); ++itor)
			{
				ENET_PROTOCOL ltype = session2type(itor->first);
				if (ttab_servers::isefficient(ltype))
				{
					lset.insert(ltype);
				}
			}
			std::pair<std::shared_ptr<pack>, std::shared_ptr<pack>> lpair = net_protocol::more_pack(adata, aactorid);
			if (lpair.first == nullptr)
			{
				return false;
			}
			for (ENET_PROTOCOL item : lset)
			{
				net_protocol* lpprotocol = ngl::nets::nettype(item);
				if (lpprotocol != nullptr)
				{
					lpprotocol->sendmore(asession, aactorid, lpair);
				}
			}
			return true;
		}

		template <typename T, typename TSTL>
		static bool sendmore(const TSTL& asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			std::set<ENET_PROTOCOL> lset;
			for (auto itor = asession.begin(); itor != asession.end(); ++itor)
			{
				ENET_PROTOCOL ltype = session2type(*itor);
				if (ttab_servers::isefficient(ltype))
				{
					lset.insert(ltype);
				}
			}
			std::pair<std::shared_ptr<pack>, std::shared_ptr<pack>> lpair = net_protocol::more_pack(adata, aactorid);
			if (lpair.first == nullptr)
			{
				return false;
			}
			for (ENET_PROTOCOL item : lset)
			{
				net_protocol* lpprotocol = ngl::nets::nettype(item);
				if (lpprotocol != nullptr)
				{
					lpprotocol->sendmore(asession, aactorid, arequestactorid, lpair);
				}
			}
			return true;
		}

		static bool sendpack(i32_sessionid asession, std::shared_ptr<pack>& apack)
		{
			net_protocol* lpprotocol = net(asession);
			if (lpprotocol == nullptr)
			{
				return false;
			}
			return lpprotocol->net_send(asession, apack);
		}

		static bool sendpack(i32_sessionid asession, std::shared_ptr<void>& apack)
		{
			net_protocol* lpprotocol = net(asession);
			if (lpprotocol == nullptr)
			{
				return false;
			}
			return lpprotocol->net_send(asession, apack);
		}


		static const std::string& ip(net_works const* apstruct)
		{
			return nconfig::node_type() != ROBOT ? apstruct->m_nip : apstruct->m_ip;
		}

		static net_works const* ipport(i32_serverid aserverid, std::pair<str_ip, i16_port>& apair)
		{
			net_works const* lpstruct = ttab_servers::connect(aserverid);
			if (lpstruct == nullptr)
			{
				return nullptr;
			}
			apair = std::make_pair(ip(lpstruct), lpstruct->m_port);
			return lpstruct;
		}

		static bool connect(i32_serverid aserverid, const std::function<void(i32_session)>& afun, bool await, bool areconnection)
		{
			i32_session lsession = manage_session::get_sessionid(aserverid);
			if (lsession != -1)
			{
				afun(lsession);
				return true;
			}
			std::pair<str_ip, i16_port> lpair;
			net_works const*  lpstruct = ipport(aserverid, lpair);
			if (lpstruct == nullptr)
			{
				return false;
			}
			net_protocol* lserver = m_net[lpstruct->m_type];
			if (lserver == nullptr)
			{
				return false;
			}
			LogLocalInfo("Connect Server %@%:%", aserverid, lpair.first, lpair.second);

			return lserver->connect(lpair.first
				, lpair.second
				, [aserverid, afun](i32_session asession)
				{
					manage_session::add(aserverid, asession);
					if (afun != nullptr)
						afun(asession);
				}, await, areconnection);
		}
	};
}

namespace ngl
{
	template <typename TSTL>
	bool net_protocol::sendmore_stl(const TSTL& asession, i64_actorid aactorid, i64_actorid arequestactorid, std::pair<std::shared_ptr<pack>, std::shared_ptr<pack>>& apair)
	{
		for (i32_sessionid item : asession)
		{
			if (nets::session2type(item) == (ENET_PROTOCOL)m_index)
				continue;
			apair.first->set_actor(aactorid, arequestactorid);
			net_send(item, apair.first);
			if (apair.second != nullptr)
				net_send(item, apair.second);
		}
		return  true;
	}

	template <typename T>
	static std::pair<std::shared_ptr<pack>, std::shared_ptr<pack>> net_protocol::more_pack(T& adata, i64_actorid aactorid)
	{
		std::pair<std::shared_ptr<pack>, std::shared_ptr<pack>> lpair;
		lpair.first = net_pack<T>::npack(&nets::net_first()->get_pool(), adata, aactorid, 0);
		if (lpair.first == nullptr)
			return std::make_pair<std::shared_ptr<pack>, std::shared_ptr<pack>>(nullptr, nullptr);
		//LogLocalWarn("SEND Session[%] Protocol Num[%] Name[%] Data[%]!", asession, (int)T::PROTOCOL, T::name(), adata);

		std::shared_ptr<pack>& lpack_ = sendpack_t::get_pack(adata);
		if (lpack_ != nullptr)
		{
			if (encryption_bytexor::check_xor(lpack_))
			{
				char* lbuff = &lpack_->m_buff[lpack_->m_pos];
				int32_t llen = lpack_->m_len - lpack_->m_pos;
				int32_t lpos = lpair.first->m_len - pack_head::size();
				ngl::encryption::bytexor(lbuff, llen, lpos);
			}
		}
		lpair.second = lpack_;
		return lpair;
	}


	template <typename T, bool IS_SEND /*= true*/>
	bool handle_pram_send<T, IS_SEND>::sendbyserver(i32_serverid aserverid, const nguid& aactorid, const nguid& arequestactorid, handle_pram& adata)
	{
		if (IS_SEND == false)
			return true;
		return nets::sendbyserver(aserverid, *(T*)adata.m_data.get(), aactorid, arequestactorid);
	}

	template <typename T>
	bool handle_pram::netsend(i32_sessionid asession, T& adata, const nguid& aactorid, const nguid& arequestactorid)
	{
		return nets::net()->send(asession, adata, aactorid.id(), arequestactorid.id());
	}

	template <typename T>
	bool actor_base::send_server(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return nets::sendbyserver(aserverid, adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::sendpacktoserver(i32_serverid aserverid, std::shared_ptr<pack>& apack)
	{
		return nets::net()->sendpackbyserver(aserverid, apack);
	}

	template <typename T>
	bool actor_base::sendpackbysession(i32_sessionid asession, std::shared_ptr<pack>& apack)
	{
		return nets::net()->sendpack(asession, apack);
	}

	template <typename T>
	std::shared_ptr<pack> actor_base::net_pack(T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return ::ngl::net_pack<T>::npack(&nets::net_first()->get_pool(), adata, aactorid, arequestactorid);
	}

	template <typename T>
	bool actor_base::send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
	{
		return nets::sendbysession(asession, adata, aactorid, arequestactorid);
	}
}//namespace ngl




	
