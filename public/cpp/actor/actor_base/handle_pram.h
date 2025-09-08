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
		i32_sessionid	m_session = -1;	// 服务器session
		nactornode		m_node;		    // 服务器基本信息

		actor_node_session() = default;
		actor_node_session(i32_sessionid asession, const nactornode& anode);
		explicit actor_node_session(const nactornode& anode);
	};

	struct handle_pram;

	template <typename T>
	class handle_pram_send
	{
	public:
		static bool sendbyserver(i32_serverid aserverid, const nguid& aactorid, const nguid& arequestactorid, const T& adata);
		static bool sendbyserver(i32_serverid aserverid, const handle_pram& adata);

		static bool send(handle_pram& adata);
		static bool sendmass(handle_pram& adata);

		static bool sendclient(handle_pram& adata);
	};

	struct handle_pram
	{
		i32_protocolnum			m_enum			= -1;						// 协议号
		std::shared_ptr<void>	m_data			= nullptr;					// 协议结构
		std::shared_ptr<pack>	m_pack			= nullptr;					// 如果是网络消息会携带pack信息
		nguid					m_actor			= nguid::make();			// 发送给哪个actor
		nguid					m_requestactor	= nguid::make();			// 哪个actor发送的
		std::set<i64_actorid>   m_massactors;								// 群发列表

		using forwardtype = std::function<
			void(const std::map<i32_serverid, actor_node_session>&, const std::map<nguid, i32_serverid>&, handle_pram&)
		>;

		forwardtype				m_forwardfun	= nullptr;			// 转发函数
		bool					m_forwardtype	= false;			// 转发给所有类型
		std::function<void()>	m_failfun		= nullptr;			// 如何actor_client都找不到目标actor则调用

		bool					m_issend		= true;				// 是否会发送给其他进程

		//# 根据[连接]获取[id]
		static i32_serverid		get_server(i64_actorid aactorid);

		static void server_actor_send(const nguid& aguid, const std::function<void()>& afun);

		//# 根据[actorid]获取[gatewayid]
		static i32_serverid		gatewayid(i64_actorid aactorid);

		//# 根据[服务器类型]获取[服务器列表]
		static void				get_serverlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec);

		//# 是否是无效的actor guid
		static bool				is_actoridnone(const nguid& aguid);

		//# 通过session发送消息T
		template <typename T>
		static bool	netsend(i32_sessionid asession, T& adata, const nguid& aactorid, const nguid& arequestactorid);
		
		//# 向服务器发送pack
		static bool	netsendpack(i32_serverid aserverid, std::shared_ptr<pack>& apack);
		static bool	netsendpack(i32_serverid aserverid, std::shared_ptr<void>& apack);
		
		template <typename T, bool IS_SEND = true>
		static void	make_forwardfun(handle_pram& apram)
		{
			static auto lfun = [](
				const std::map<i32_serverid, actor_node_session>&, 
				const std::map<nguid, i32_serverid>&, handle_pram& adata
				)
			{
				handle_pram_send<T>::send(adata);
			};
			if constexpr (IS_SEND)
			{
				apram.m_forwardfun = lfun;
			}
			else
			{
				apram.m_forwardfun = nullptr;
			}
		}

		template <typename T, bool IS_SEND = true>
		static void	make_massfun(handle_pram& apram)
		{
			static auto lfun = [](const std::map<i32_serverid, actor_node_session>&
				, const std::map<nguid, i32_serverid>&
				, handle_pram& adata
				)
				{
					handle_pram_send<T>::sendmass(adata);
				};
			if constexpr (IS_SEND)
			{
				apram.m_forwardfun = lfun;
			}
			else
			{
				apram.m_forwardfun = nullptr;
			}
		}

		template <typename T>
		static void	make_client(handle_pram& apram)
		{
			static auto lfun = [](const std::map<i32_serverid, actor_node_session>&
				, const std::map<nguid, i32_serverid>&
				, handle_pram& adata
				)
			{
				handle_pram_send<T>::sendclient(adata);
			};
			apram.m_forwardfun = lfun;
		}

		template <typename T, bool IS_SEND = true, bool IS_FORWARDFUN = true>
		static handle_pram create(
			const nguid& aid
			, const nguid& arid
			, const std::shared_ptr<T>& adata
			, const std::function<void()>& afailfun = nullptr
		)
		{
			handle_pram lpram;
			lpram.m_enum			= tprotocol::protocol<T>();
			lpram.m_data			= adata;
			lpram.m_actor			= aid;
			lpram.m_requestactor	= arid;
			lpram.m_forwardfun		= nullptr;
			lpram.m_issend			= IS_SEND;
			make_forwardfun<T, IS_FORWARDFUN&& IS_SEND>(lpram);
			lpram.m_failfun			= afailfun;
			return lpram;
		}

		//Y:forward或者T
		template <typename T, typename Y>
		static handle_pram create(
			const nguid& aid, const nguid& arid
			, const std::shared_ptr<np_actor_forward<T, forward_g2c<Y>>>& adata
			, const std::function<void()>& afailfun = nullptr
		)
		{
			handle_pram lpram;
			lpram.m_enum			= tprotocol::protocol<T>();
			lpram.m_data			= adata;
			lpram.m_actor			= aid;
			lpram.m_requestactor	= arid;
			lpram.m_forwardfun		= nullptr;
			lpram.m_issend			= true;
			make_client<T>(lpram);
			lpram.m_failfun			= afailfun;
			return lpram;
		}

		template <typename T, typename Y>
		static handle_pram create(
			const nguid& aid, const nguid& arid
			, const std::shared_ptr<np_actor_forward<T, forward_c2g<Y>>>& adata
			, const std::function<void()>& afailfun = nullptr
		)
		{
			handle_pram lpram;
			lpram.m_enum = tprotocol::protocol<T>();
			lpram.m_data = adata;
			lpram.m_actor = aid;
			lpram.m_requestactor = arid;
			lpram.m_forwardfun = nullptr;
			lpram.m_issend = true;
			lpram.m_forwardfun = nullptr;
			lpram.m_failfun = afailfun;
			return lpram;
		}

		template <typename T, bool IS_SEND = true>
		static handle_pram create(
			const std::set<i64_actorid>& aids, const nguid& arid, const std::shared_ptr<T>& adata
		)
		{
			handle_pram lpram;
			lpram.m_enum			= tprotocol::protocol<T>();
			lpram.m_data			= adata;
			lpram.m_actor			= nguid::make();
			lpram.m_massactors		= aids;
			lpram.m_requestactor	= arid;
			lpram.m_forwardfun		= nullptr;
			lpram.m_issend			= IS_SEND;
			make_massfun<T, IS_SEND>(lpram);
			return lpram;
		}

		static handle_pram create_pack(const nguid& aid, const nguid& arid, const std::shared_ptr<pack>& apack)
		{
			handle_pram lpram;
			lpram.m_data			= apack;
			lpram.m_actor			= aid;
			lpram.m_requestactor	= arid;
			make_forwardfun<pack, true>(lpram);
			return lpram;
		}
	};

	template <typename T>
	bool handle_pram_send<T>::send(handle_pram& adata)
	{
		nguid lactorid = adata.m_actor;
		nguid lrequestactor = adata.m_requestactor;
		std::set<i64_actorid>& lmassactors = adata.m_massactors;
		if (lactorid != nguid::make() && lmassactors.empty())
		{
			i32_serverid lserverid = handle_pram::get_server(lactorid);
			if (lserverid == -1)
			{
				if (adata.m_forwardtype && handle_pram::is_actoridnone(lactorid))
				{//# 转发给所有类型为nguid::type(aactorid)的actor
					std::set<i32_serverid> lset;
					handle_pram::get_serverlist(lactorid.type(), lset);
					for (i32_serverid ltempserverid : lset)
					{
						handle_pram_send<T>::sendbyserver(ltempserverid, adata);
					}
					return true;
				}

				// # 加入等待队列
				log_error()->print(
					"handle_pram_send<{}>::send fail add wait list actorid[{}] serverid[{}]"
					, tools::type_name<T>(), nguid(lactorid), lactorid
				);
				handle_pram::server_actor_send(lactorid, [adata]()
					{
						handle_pram ldata = adata;
						handle_pram_send<T>::send(ldata);
					});
				return false;
			}
			return handle_pram_send<T>::sendbyserver(lserverid, adata);
		}
		return false;
	}

	template <typename T>
	bool handle_pram_send<T>::sendmass(handle_pram& adata)
	{
		nguid lactorid = adata.m_actor;
		nguid lrequestactor = adata.m_requestactor;
		std::set<i64_actorid>& lmassactors = adata.m_massactors;
		if (lactorid != nguid::make() && lmassactors.empty())
		{
			std::map<i32_serverid, std::set<i64_actorid>> lserveractors;
			for (i64_actorid actorid : lmassactors)
			{
				i32_serverid lserverid = handle_pram::get_server(actorid);
				if (lserverid != -1 && lserverid != nconfig::m_nodeid)
				{
					lserveractors[lserverid].insert(actorid);
				}
			}

			std::shared_ptr<T> ldata = std::static_pointer_cast<T>(adata.m_data);
			np_mass_actor<T> pro(ldata);

			for (const auto& item1 : lserveractors)
			{
				pro.m_actorids.clear();
				i32_serverid lserverid = item1.first;
				pro.m_actorids = item1.second;
				handle_pram_send<np_mass_actor<T>>::sendbyserver(lserverid, lactorid, lrequestactor, pro);
			}
		}
		return true;
	}

	template <>
	class handle_pram_send<pack>
	{
	public:
		static bool sendbyserver(i32_serverid aserverid, handle_pram& adata)
		{
			return handle_pram::netsendpack(aserverid, adata.m_data);
		}

		static bool send(handle_pram& adata)
		{
			nguid lactorid = adata.m_actor;
			nguid lrequestactor = adata.m_requestactor;
			i32_serverid lserverid = handle_pram::get_server(lactorid);
			if (lserverid == -1)
			{
				if (adata.m_forwardtype && handle_pram::is_actoridnone(lactorid))
				{//# 转发给所有类型为nguid::type(aactorid)的actor
					std::set<i32_serverid> lset;
					handle_pram::get_serverlist(lactorid.type(), lset);
					for (i32_serverid ltempserverid : lset)
					{
						handle_pram_send<pack>::sendbyserver(ltempserverid, adata);
					}
					return true;
				}
				if (adata.m_failfun != nullptr)
				{
					adata.m_failfun();
				}
				// # 加入等待队列
				log_error()->print(
					"handle_pram_send<pack>::send fail add wait list actorid[{}] serverid[{}]"
					, nguid(lactorid), lactorid
				);
				handle_pram::server_actor_send(lactorid, [adata]()
					{
						handle_pram ldata = adata;
						handle_pram_send<pack>::send(ldata);
					});
				return false;
			}
			return handle_pram_send<pack>::sendbyserver(lserverid, adata);
		}
	};

	template <typename T>
	bool handle_pram_send<T>::sendclient(handle_pram& adata)
	{

		nguid lactorid			= adata.m_actor;
		nguid lrequestactor		= adata.m_requestactor;
		auto ldata				= (np_actor_forward<T, forward_g2c<T>>*)adata.m_data.get();
		std::vector<i32_actordataid>& luid	= ldata->m_data.m_uid;
		std::vector<i16_area>& larea		= ldata->m_data.m_area;
		std::set<i32_serverid> lgateway;
		for (int i = 0; i < luid.size() && i < larea.size(); ++i)
		{
			i64_actorid lroleactor = nguid::make(ACTOR_ROLE, larea[i], luid[i]);
			i32_serverid lserverid = handle_pram::gatewayid(lroleactor);
			if (lserverid > 0)
			{
				lgateway.insert(lserverid);
			}
		}
		for (i32_serverid lserverid : lgateway)
		{
			handle_pram_send<np_actor_forward<T, forward_g2c<T>>>::sendbyserver(
				lserverid, adata
			);
		}
		return true;
	}
}//namespace ngl
