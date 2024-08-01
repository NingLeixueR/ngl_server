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

		static i64_actorid						m_nspserver;
		static actor*							m_actor;
		static bool								m_register;
		static std::set<i64_actorid>			m_dataid;
		static bool								m_recvdatafinish;
		static std::function<void(const T&)>	m_recvdatafinishfun;
		static std::function<void(int64_t, const T&)>	m_changedatafun;
	public:
		static std::map<i64_actorid, T> m_data;

		static void init(
			i64_actorid anspserver, TDerived* aactor, const std::set<i64_actorid>& adataid
		)
		{
			m_nspserver = anspserver;
			m_actor		= aactor;
			m_dataid	= adataid;
			// 更新数据
			actor::register_actor_s<
				EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_data<T>
			>([](TDerived* apTDerived, message<np_channel_data<T>>& adata)
				{
					m_actor->log_error()->print(
						"nsp_client {}:{}",
						tools::type_name<TDerived>(), tools::type_name<T>()
					);
					auto& recv = *adata.get_data();
					std::map<int64_t, T>& lmap = *recv.m_data.m_data;
					for (std::pair<const int64_t, T>& lpair : lmap)
					{
						if (!m_dataid.empty())
						{
							if (m_dataid.find(lpair.first) == m_dataid.end())
							{
								continue;
							}
						}						
						m_data[lpair.first] = lpair.second;
						if (m_changedatafun != nullptr)
						{
							m_changedatafun(lpair.first, lpair.second);
						}
					}
					if (m_recvdatafinish == false && m_recvdatafinishfun != nullptr)
					{
						m_recvdatafinish = true;
						for (std::pair<const int64_t, T>& lpair : lmap)
						{
							m_recvdatafinishfun(lpair.second);
						}
					}
				});

			// 注册回复
			actor::register_actor_s<
				EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_register_reply<T>
			>([](TDerived* apTDerived, message<np_channel_register_reply<T>>& ainfo)
				{
					m_actor->log_error()->print(
						"nsp_client register reply {}:{}", tools::type_name<TDerived>(), tools::type_name<T>()
					);
					m_register = true;
				});

			wheel_parm lparm
			{
				.m_ms = 1000,
				.m_intervalms = [](int64_t) {return 1000; } ,
				.m_count = 0x7fffffff,
				.m_fun = [aactor](wheel_node* anode)
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
			if (m_recvdatafinish)
			{
				for (const auto& [_id, _data] : m_data)
				{
					m_recvdatafinishfun(_data);
				}				
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
			m_actor->log_error()->print(
				"nsp_client register {}:{}", tools::type_name<TDerived>(), tools::type_name<T>()
			);
			auto pro = std::make_shared<np_channel_register<T>>();
			pro->m_actorid = m_actor->id_guid();
			actor::static_send_actor(m_nspserver, nguid::make(), pro);
		}
	};

	template <typename TDerived, typename T>
	std::map<i64_actorid, T> nsp_client<TDerived, T>::m_data;

	template <typename TDerived, typename T>
	i64_actorid nsp_client<TDerived, T>::m_nspserver = -1;

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