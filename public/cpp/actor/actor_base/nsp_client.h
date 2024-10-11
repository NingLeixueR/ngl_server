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

		static actor*									m_actor;
		static std::map<i16_area, i64_actorid>			m_nspserver;
		static std::map<i16_area,bool>					m_register;
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

		static void log(const char* amessage)
		{
			m_actor->log_error()->print("{} {}:{}", amessage, type_name<TDerived>(), type_name<T>());
		}

		static void init(ENUM_ACTOR atype, TDerived* aactor, const std::set<i64_actorid>& adataid)
		{
			std::vector<i16_area> lvecarea = ttab_servers::get_arealist(nconfig::m_nodeid);
			Assert(lvecarea.empty() == false)
			for (i16_area area : lvecarea)
			{
				m_nspserver[area] = nguid::make(atype, area, nguid::none_actordataid());
				m_register[area] = false;
			}
			m_actor		= aactor;
			m_dataid	= adataid;
			// ��������
			actor::register_actor_s<
				EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_data<T>
			>([](TDerived*, message<np_channel_data<T>>& adata)
				{
					log("nsp_client np_channel_data");
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

			// ע��ظ�
			actor::register_actor_s<
				EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_register_reply<T>
			>([](TDerived*, message<np_channel_register_reply<T>>& adata)
				{
					log("nsp_client np_channel_register_reply");
					auto& recv = *adata.get_data();
					m_register[nguid::area(recv.m_actorid)] = true;
				});

			std::ranges::for_each(m_register, [](const auto& apair)
				{
					i16_area larea = apair.first;
					wheel_parm lparm
					{
						.m_ms = 1000,
						.m_intervalms = [](int64_t) {return 1000; } ,
						.m_count = 0x7fffffff,
						.m_fun = [larea](const wheel_node* anode)
						{
							if (m_register[larea])
							{
								twheel::wheel().removetimer(anode->m_timerid);
								return;
							}
							register_echannel(larea);
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
			auto itor = m_data.find(aactorid);
			if (itor == m_data.end())
				return;

			auto pro = std::make_shared<np_channel_data<T>>();
			pro->m_data.make();
			(*pro->m_data.m_data)[aactorid] = itor->second;
			actor::static_send_actor(m_nspserver[nguid::area(aactorid)], nguid::make(), pro);
		}

		// # ������ݲ��ָ��Ƶ�λ��ִ�����²���
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

		// # ������ݷ����仯
		static void set_changedata_fun(const std::function<void(int64_t, const T&)>& afun)
		{
			m_changedatafun = afun;
		}
	private:
		static void register_echannel(i16_area aarea)
		{
			log("nsp_client register");
			auto pro = std::make_shared<np_channel_register<T>>();
			pro->m_actorid = m_actor->id_guid();
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
	bool nsp_client<TDerived, T>::m_recvdatafinish = false;
	
	template <typename TDerived, typename T>
	std::function<void(const T&)> nsp_client<TDerived, T>::m_recvdatafinishfun = nullptr;

	template <typename TDerived, typename T>
	std::function<void(int64_t, const T&)>	nsp_client<TDerived, T>::m_changedatafun;
}//namespace ngl