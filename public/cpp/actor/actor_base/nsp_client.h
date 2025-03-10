#pragma once

#include "ndb_modular.h"

namespace ngl
{
	//# 订阅/发布[数据副本]
	//# nsp_client接收nsp_server分发的数据与修改数据后向nsp_server同步
	template <typename TDerived, typename T>
	class nsp_client
	{
	public:
		using tnsp_client = nsp_client<TDerived, T>;
	private:
		nsp_client() = delete;
		nsp_client(const nsp_client&) = delete;
		nsp_client& operator=(const nsp_client&) = delete;

		static actor*									m_actor;
		static std::map<i16_area, i64_actorid>			m_nspserver;
		static std::map<i16_area, bool>					m_register;
		static std::set<i64_actorid>					m_dataid;
		static std::function<void(int64_t, const T&, bool)>	m_changedatafun;
	public:
		static std::map<i64_actorid, T> m_data;
	private:
		template <typename TX>
		static std::string& type_name()
		{
			return tools::type_name<TX>();
		}

		static void log(const char* amessage)
		{
			m_actor->log_error()->print("{} {}:{}", amessage, type_name<TDerived>(), type_name<T>());
		}

		static void channel_data(TDerived*, const message<np_channel_data<T>>& adata)
		{
			const np_channel_data<T>& recv = *adata.get_data();
			std::map<int64_t, T>& lmap = *recv.m_data.m_data;
			bool lfirstsynchronize = recv.m_firstsynchronize;
			std::ranges::for_each(lmap, [lfirstsynchronize](const auto& apair)
				{
					if (!m_dataid.empty() && m_dataid.find(apair.first) == m_dataid.end())
					{
						return;
					}
					m_data[apair.first] = apair.second;
					if (m_changedatafun != nullptr)
					{
						m_changedatafun(apair.first, apair.second, lfirstsynchronize);
					}
				});
		}

		static void channel_register_reply(TDerived*, const message<np_channel_register_reply<T>>& adata)
		{
			log("nsp_client np_channel_register_reply");
			auto& recv = *adata.get_data();
			m_register[nguid::area(recv.m_actorid)] = true;
		}

		static void channel_check(TDerived*, const message<np_channel_check>& adata)
		{
			auto lprecv = adata.get_data();
			if (m_register[lprecv->m_area])
			{
				twheel::wheel().removetimer(lprecv->m_timer);
				return;
			}
			register_echannel(lprecv->m_area);
		}

	public:
		static void init(ENUM_ACTOR atype, TDerived* aactor, const std::set<i64_actorid>& adataid)
		{
			const std::set<i16_area>* lsetarea = ttab_servers::get_arealist(nconfig::m_nodeid);
			Assert(lsetarea->empty() == false)
			for (i16_area area : *lsetarea)
			{
				m_nspserver[area] = nguid::make(atype, area, nguid::none_actordataid());
				m_register[area] = false;
			}
			m_actor		= aactor;
			m_dataid	= adataid;
			// 更新数据
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_data<T>>(
				std::bind_front(&tnsp_client::channel_data)
			);

			// 注册回复
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_register_reply<T>>(
				std::bind_front(&tnsp_client::channel_register_reply)
			);

			// 检查
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_check>(
				std::bind_front(&tnsp_client::channel_check)
			);

			i64_actorid lactorid = m_actor->id_guid();
			std::ranges::for_each(m_register, [lactorid](const auto& apair)
				{
					i16_area larea = apair.first;
					wheel_parm lparm
					{
						.m_ms = 1000,
						.m_intervalms = [](int64_t) {return 1000; } ,
						.m_count = 0x7fffffff,
						.m_fun = [larea,lactorid](const wheel_node* anode)
						{
							auto pro = std::make_shared<np_channel_check>(
								np_channel_check{
									.m_timer = anode->m_timerid,
									.m_area = larea,
								});
							actor::static_send_actor(lactorid, nguid::make(), pro);
						}
					};
					twheel::wheel().addtimer(lparm);
				});			
		}

		static const T* getconst(i64_actorid aactorid)
		{
			T* lp = tools::findmap(m_data, aactorid);
			return lp;
		}

		static T* get(i64_actorid aactorid)
		{
			T* lp = tools::findmap(m_data, aactorid);
			return lp;
		}

		static T* add(i64_actorid aactorid)
		{
			return &m_data[aactorid];
		}

		static void change(i64_actorid aactorid)
		{
			T* lpdata = tools::findmap(m_data, aactorid);
			if (lpdata == nullptr)
			{
				return;
			}

			auto pro = std::make_shared<np_channel_data<T>>();
			pro->m_data.make();
			(*pro->m_data.m_data)[aactorid] = *lpdata;
			actor::static_send_actor(m_nspserver[nguid::area(aactorid)], nguid::make(), pro);
		}

		// # 如果数据发生变化
		static void set_changedata_fun(const std::function<void(int64_t, const T&, bool)>& afun)
		{
			m_changedatafun = afun;
		}
	private:
		static void register_echannel(i16_area aarea)
		{
			log("nsp_client register");
			auto pro = std::make_shared<np_channel_register<T>>();
			pro->m_actorid = m_actor->id_guid();

			log_error()->print("nsp_client register: {} -> {}", nguid(pro->m_actorid), nguid(m_nspserver[aarea]));
			actor::static_send_actor(m_nspserver[aarea], nguid::make(), pro);
		}
	};

	template <typename TDerived, typename T>
	std::map<i64_actorid, T> nsp_client<TDerived, T>::m_data;

	template <typename TDerived, typename T>
	std::map<i16_area, i64_actorid> nsp_client<TDerived, T>::m_nspserver = {};

	template <typename TDerived, typename T>
	actor* nsp_client<TDerived, T>::m_actor = nullptr;

	template <typename TDerived, typename T>
	std::map<i16_area, bool> nsp_client<TDerived, T>::m_register = {};

	template <typename TDerived, typename T>
	std::set<i64_actorid> nsp_client<TDerived, T>::m_dataid;

	template <typename TDerived, typename T>
	std::function<void(int64_t, const T&, bool)>	nsp_client<TDerived, T>::m_changedatafun;
}//namespace ngl