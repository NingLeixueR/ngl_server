#pragma once

#include "tprotocol.h"
#include "nprotocol.h"
#include "nguid.h"
#include "type.h"

#include <cstdint>
#include <memory>

namespace ngl
{
	class actor_base;
	using ptractor = std::shared_ptr<actor_base>;

	struct actor_node_session
	{
		i32_sessionid	m_session;
		nactornode		m_node;

		actor_node_session();
		actor_node_session(i32_sessionid asession, const nactornode& anode);
		explicit actor_node_session(const nactornode& anode);
	};

	struct handle_pram;

	template <typename T, bool IS_SEND = true>
	class handle_pram_send
	{
	public:
		static bool sendbyserver(
			i32_serverid aserverid, const nguid& aactorid, const nguid& arequestactorid, const handle_pram& adata
		);

		static bool send(
			const nguid& aactorid, const nguid& arequestactorid, handle_pram& adata
		);

		static bool sendclient(
			const nguid& aactorid, const nguid& arequestactorid, handle_pram& adata
		);
	};

	struct handle_pram
	{
		i32_protocolnum			m_enum			= -1;
		std::shared_ptr<void>	m_data			= nullptr;
		std::shared_ptr<pack>	m_pack			= nullptr;
		EPROTOCOL_TYPE			m_protocoltype	= EPROTOCOL_TYPE_CUSTOM;
		nguid					m_actor;
		nguid					m_requestactor;

		using forwardtype = std::function<
			void(const std::map<i32_serverid, actor_node_session>&, const std::map<nguid, i32_serverid>&, handle_pram&)
		>;

		forwardtype				m_forwardfun;			// 转发函数
		bool					m_forwardtype = false;	// 转发给所有类型
		std::function<void()>	m_failfun;				// 如何actor_client都找不到目标actor则调用

		//# 根据[连接]获取[id]
		static i32_serverid		get_server(i64_actorid aactorid);

		//# 根据[actorid]获取[gatewayid]
		static i32_serverid		get_gatewayid(i64_actorid aactorid);

		//# 根据[服务器类型]获取[服务器列表]
		static void				get_serverlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec);

		//# 是否是无效的actor guid
		static bool				is_actoridnone(const nguid& aguid);

		template <typename T>
		static bool	netsend(
			i32_sessionid asession, T& adata, const nguid& aactorid, const nguid& arequestactorid
		);
		
		static bool	netsendpack(i32_serverid aserverid, std::shared_ptr<pack>& apack);
		static bool	netsendpack(i32_serverid aserverid, std::shared_ptr<void>& apack);
		
		template <typename T, bool IS_SEND = true>
		static void	make_forwardfun(
			const nguid& aactorid, const nguid& arequestactorid, handle_pram& apram
		)
		{
			apram.m_forwardfun = 
				[aactorid, arequestactorid](
					const std::map<i32_serverid, actor_node_session>&, 
					const std::map<nguid, i32_serverid>&,
					handle_pram& adata
				)
			{
				handle_pram_send<T, IS_SEND>::send(aactorid, arequestactorid, adata);
			};
		}

		template <typename T, bool IS_SEND = true>
		static void	make_client(
			const nguid& aactorid, const nguid& arequestactorid, handle_pram& apram
		)
		{
			apram.m_forwardfun = 
				[aactorid, arequestactorid](
					const std::map<i32_serverid, actor_node_session>&, 
					const std::map<nguid, i32_serverid>&, 
					handle_pram& adata
				)
				{
					handle_pram_send<T, IS_SEND>::sendclient(aactorid, arequestactorid, adata);
				};
		}

		template <typename T, bool IS_SEND = true, bool IS_FORWARDFUN = true>
		static handle_pram create(
			const nguid& aid, const nguid& arid, std::shared_ptr<T>& adata, const std::function<void()>& afailfun = nullptr
		)
		{
			handle_pram lpram;
			lpram.m_enum			= tprotocol::protocol<T>();
			lpram.m_data			= adata;
			lpram.m_actor			= aid;
			lpram.m_requestactor	= arid;
			lpram.m_protocoltype	= (EPROTOCOL_TYPE)tprotocol::protocol_type<T>();
			lpram.m_forwardfun		= nullptr;
			if (IS_FORWARDFUN)
				make_forwardfun<T, IS_SEND>(aid, arid, lpram);
			lpram.m_failfun			= afailfun;
			return std::move(lpram);
		}

		template <typename T, bool IS_SEND = true>
		static handle_pram create(
			const nguid& aid, const nguid& arid
			, std::shared_ptr<np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>>& adata
			, const std::function<void()>& afailfun = nullptr
		)
		{
			handle_pram lpram;
			lpram.m_enum			= tprotocol::protocol<T>();
			lpram.m_data			= adata;
			lpram.m_actor			= aid;
			lpram.m_requestactor	= arid;
			lpram.m_protocoltype	= (EPROTOCOL_TYPE)tprotocol::protocol_type<T>();
			lpram.m_forwardfun		= nullptr;
			make_client<T, IS_SEND>(aid, arid, lpram);
			lpram.m_failfun			= afailfun;
			return std::move(lpram);
		}

		static handle_pram create_pack(
			const nguid& aid, const nguid& arid, std::shared_ptr<pack>& apack
		)
		{
			handle_pram lpram;
			lpram.m_data			= apack;
			lpram.m_actor			= aid;
			lpram.m_requestactor	= arid;
			make_forwardfun<pack, true>(aid, arid, lpram);
			return std::move(lpram);
		}
	};

	template <typename T, bool IS_SEND /*= true*/>
	bool handle_pram_send<T, IS_SEND>::send(
		const nguid& aactorid, const nguid& arequestactorid, handle_pram& adata
	)
	{
		i32_serverid lserverid = handle_pram::get_server(aactorid);
		if (lserverid == -1)
		{
			if (adata.m_forwardtype && handle_pram::is_actoridnone(aactorid))
			{
				std::set<i32_serverid> lset;
				handle_pram::get_serverlist(aactorid.type(), lset);
				for (i32_serverid ltempserverid : lset)
				{
					handle_pram_send<T, IS_SEND>::sendbyserver(
						ltempserverid, aactorid, arequestactorid, adata
					);
				}
				return true;
			}
			return false;
		}
		return handle_pram_send<T, IS_SEND>::sendbyserver(
			lserverid, aactorid, arequestactorid, adata
		);
	}

	template <>
	class handle_pram_send<pack, true>
	{
	public:
		static bool sendbyserver(
			i32_serverid aserverid, const nguid&, const nguid&, handle_pram& adata
		)
		{
			return handle_pram::netsendpack(aserverid, adata.m_data);
		}

		static bool send(
			const nguid& aactorid, const nguid& arequestactorid, handle_pram& adata
		)
		{
			i32_serverid lserverid = handle_pram::get_server(aactorid);
			if (lserverid == -1)
			{
				if (handle_pram::is_actoridnone(aactorid))
				{
					std::set<i32_serverid> lset;
					handle_pram::get_serverlist(aactorid.type(), lset);
					for (i32_serverid ltempserverid : lset)
					{
						handle_pram_send<pack, true>::sendbyserver(
							ltempserverid, aactorid, arequestactorid, adata
						);
					}
					return true;
				}
				if (adata.m_failfun != nullptr)
					adata.m_failfun();
				return false;
			}
			return handle_pram_send<pack, true>::sendbyserver(
				lserverid, aactorid, arequestactorid, adata
			);
		}
	};

	template <typename T, bool IS_SEND /*= true*/>
	bool handle_pram_send<T, IS_SEND>::sendclient(
		const nguid&, const nguid& arequestactorid, handle_pram& adata
	)
	{
		auto ldata = (np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>*)
			adata.m_data.get();
		std::vector<i32_actordataid>& luid	= ldata->m_uid;
		std::vector<i16_area>& larea		= ldata->m_area;
		std::set<i32_serverid> lgateway;
		for (int i = 0; i < luid.size() && i < larea.size(); ++i)
		{
			i64_actorid lroleactor = nguid::make(ACTOR_ROLE, larea[i], luid[i]);
			i32_serverid lserverid = handle_pram::get_gatewayid(lroleactor);
			if (lserverid > 0)
				lgateway.insert(lserverid);
		}
		for (i32_serverid lserverid : lgateway)
		{
			handle_pram_send<
				np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>, true
			>::sendbyserver(lserverid, nguid::make(), arequestactorid, adata);
		}
		return true;
	}
}//namespace ngl
