#pragma once

#include "ndb_modular.h"

namespace ngl
{
	template <echannel ENUM, typename TDerived, typename T>
	class nsp_client
	{
	public:
		static std::map<i64_actorid, T> m_data;
		static i64_actorid				m_nspserver;
		static actor* m_actor;
		static bool						m_register;

		// # aactor nsp_serverµÄactor id
		static void init(i64_actorid anspserver, TDerived* aactor)
		{
			m_nspserver = anspserver;
			m_actor = aactor;
			actor::register_actor_s<
				EPROTOCOL_TYPE_CUSTOM,
				TDerived,
				np_channel_data<T>
			>([](TDerived* apTDerived, np_channel_data<T>& ainfo)
				{
					m_actor->log_error()->print(
						"nsp_client {}:{}",
						tools::type_name<TDerived>(),
						tools::type_name<T>()
					);
					std::map<int64_t, T>& lmap = *ainfo.m_data.m_data;
					for (std::pair<const int64_t, T>& lpair : lmap)
					{
						m_data[lpair.first] = lpair.second;
					}
				});

			// m_register
			actor::register_actor_s<
				EPROTOCOL_TYPE_CUSTOM,
				TDerived,
				np_channel_register_reply
			>([](TDerived* apTDerived, np_channel_register_reply& ainfo)
				{
					m_actor->log_error()->print(
						"nsp_client register reply {}:{}",
						tools::type_name<TDerived>(),
						tools::type_name<T>()
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

		static const T* get(i64_actorid aactorid)
		{
			T* lp = tools::findmap(m_data, aactorid);
			return lp;
		}
	private:
		static void register_echannel()
		{
			m_actor->log_error()->print(
				"nsp_client register {}:{}",
				tools::type_name<TDerived>(),
				tools::type_name<T>()
			);
			auto pro = std::make_shared<np_channel_register>();
			pro->m_actorid = m_actor->id_guid();
			actor::static_send_actor(m_nspserver, nguid::make(), pro);
		}
	};

	template <echannel ENUM, typename TDerived, typename T>
	std::map<i64_actorid, T> nsp_client<ENUM, TDerived, T>::m_data;

	template <echannel ENUM, typename TDerived, typename T>
	i64_actorid nsp_client<ENUM, TDerived, T>::m_nspserver = -1;

	template <echannel ENUM, typename TDerived, typename T>
	actor* nsp_client<ENUM, TDerived, T>::m_actor = nullptr;

	template <echannel ENUM, typename TDerived, typename T>
	bool nsp_client<ENUM, TDerived, T>::m_register = false;
}