#pragma once

#include "ndb_modular.h"

namespace ngl
{
	template <typename TDerived, typename T>
	class nsp_client
	{
		nsp_client() = delete;
		nsp_client(const nsp_client&) = delete;
		nsp_client& operator=(const nsp_client&) = delete;

		static std::vector<i64_actorid>					m_nspserver;
		static actor*									m_actor;
		static bool										m_register;
		static std::set<i64_actorid>					m_dataid;
		static bool										m_recvdatafinish;
		static std::function<void(const T&)>			m_recvdatafinishfun;
		static std::function<void(int64_t, const T&)>	m_changedatafun;
	public:
		static std::map<i64_actorid, T> m_data;

		template <typename TX>
		static std::string& type_name()
		{
			return tools::type_name<TX>();
		}

		static void init(ENUM_ACTOR atype, TDerived* aactor, const std::set<i64_actorid>& adataid)
		{
			std::vector<i16_area> lvecarea = ttab_servers::get_arealist(nconfig::m_nodeid);
			Assert(lvecarea.empty() == false)
			for (i16_area area : lvecarea)
			{
				m_nspserver.push_back(nguid::make(atype, area, nguid::none_actordataid()));
			}
			m_actor		= aactor;
			m_dataid	= adataid;
			// 更新数据
			actor::register_actor_s<
				EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_data<T>
			>([](TDerived*, message<np_channel_data<T>>& adata)
				{
					m_actor->log_error()->print("nsp_client {}:{}", type_name<TDerived>(), type_name<T>());
					auto& recv = *adata.get_data();
					std::map<int64_t, T>& lmap = *recv.m_data.m_data;
					std::ranges::for_each(lmap, [](const auto& apair)
						{
							if (!m_dataid.empty() && m_dataid.find(apair.first) == m_dataid.end())
							{
								return;
							}
							m_data[apair.first] = apair.second;
							if (m_changedatafun != nullptr)
							{
								m_changedatafun(apair.first, apair.second);
							}
						});
					if (m_recvdatafinish == false && m_recvdatafinishfun != nullptr)
					{
						m_recvdatafinish = true;
						std::ranges::for_each(lmap, [](const auto& apair)
							{
								m_recvdatafinishfun(apair.second);
							});
					}
				});

			// 注册回复
			actor::register_actor_s<
				EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_register_reply<T>
			>([](TDerived*, message<np_channel_register_reply<T>>&)
				{
					m_actor->log_error()->print("nsp_client register reply {}:{}", type_name<TDerived>(), type_name<T>());
					m_register = true;
				});

			wheel_parm lparm
			{
				.m_ms = 1000,
				.m_intervalms = [](int64_t) {return 1000; } ,
				.m_count = 0x7fffffff,
				.m_fun = [aactor](const wheel_node* anode)
				{
					if (m_register)
					{
						twheel::wheel().removetimer(anode->m_timerid);
						return;
					}
					register_echannel();
				}
			};
			twheel::wheel().addtimer(lparm);
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
			auto itor = m_data.find(aactorid);
			if (itor == m_data.end())
				return;

			auto pro = std::make_shared<np_channel_data<T>>();
			pro->m_data.make();
			(*pro->m_data.m_data)[aactorid] = itor->second;
			actor::static_send_actor(m_nspserver, nguid::make(), pro);
		}

		// # 如果数据部分复制到位就执行以下操作
		static void set_recv_data_finish(const std::function<void(const T&)>& afun)
		{
			m_recvdatafinishfun = afun;
			if (m_recvdatafinish == false)
			{
				m_recvdatafinish = true;
				std::ranges::for_each(m_data, [](const auto& apair)
					{
						m_recvdatafinishfun(apair.second);
					});			
			}
		}

		// # 如果数据发生变化
		static void set_changedata_fun(const std::function<void(int64_t, const T&)>& afun)
		{
			m_changedatafun = afun;
		}
	private:
		static void register_echannel()
		{
			m_actor->log_error()->print("nsp_client register {}:{}", type_name<TDerived>(), type_name<T>());
			auto pro = std::make_shared<np_channel_register<T>>();
			pro->m_actorid = m_actor->id_guid();
			actor::static_send_actor(m_nspserver, nguid::make(), pro);
		}
	};

	template <typename TDerived, typename T>
	std::map<i64_actorid, T> nsp_client<TDerived, T>::m_data;

	template <typename TDerived, typename T>
	std::vector<i64_actorid> nsp_client<TDerived, T>::m_nspserver = {};

	template <typename TDerived, typename T>
	actor* nsp_client<TDerived, T>::m_actor = nullptr;

	template <typename TDerived, typename T>
	bool nsp_client<TDerived, T>::m_register = false;

	template <typename TDerived, typename T>
	std::set<i64_actorid> nsp_client<TDerived, T>::m_dataid;

	template <typename TDerived, typename T>
	bool nsp_client<TDerived, T>::m_recvdatafinish = false;
	
	template <typename TDerived, typename T>
	std::function<void(const T&)> nsp_client<TDerived, T>::m_recvdatafinishfun = nullptr;

	template <typename TDerived, typename T>
	std::function<void(int64_t, const T&)>	nsp_client<TDerived, T>::m_changedatafun;
}//namespace ngl