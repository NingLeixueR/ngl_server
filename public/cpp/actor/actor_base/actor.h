#pragma once

#include "template_arg.h"
#include "nactortype.h"
#include "actor_base.h"
#include "tprotocol.h"
#include "nrfun.h"
#include "impl.h"
#include "nlog.h"

namespace ngl
{
	struct actorparm
	{
		actorparmbase	m_parm;
		int32_t			m_weight	= 10;			// Ȩ��:���λ�ȡ�̺߳�����Ϣ������
		int32_t			m_timeout	= 0x7fffffff;	// ��ʱ:(��actor������Ϣ������ʱ��)
		bool			m_broadcast	= false;		// �Ƿ�֧�ֹ㲥(�����Ҫ����dbclient����Ҫ֧�ֹ㲥)
	};

	template <typename T>
	const T* null = (T*)nullptr;

	class actor : 
		public actor_base
	{
		actor() = delete;
		actor(const actor&) = delete;
		actor& operator=(const actor&) = delete;

		struct impl_actor;
		ngl::impl<impl_actor> m_impl_actor;

		std::array<nrfunbase*, EPROTOCOL_TYPE_COUNT> m_actorfun = { nullptr };

#pragma region register // ��Ϣע��ӿ�
		template <typename TDerived, EPROTOCOL_TYPE TYPE>
		static nrfun<TDerived, TYPE>& ninst()
		{
			return nrfun<TDerived, TYPE>::instance();
		}

		template <typename TDerived, EPROTOCOL_TYPE PTYPE>
		void init_array()
		{
			m_actorfun[PTYPE] = &ninst<TDerived, PTYPE>();
		}
	public:
		template <typename TDerived>
		void init_rfun()
		{
			init_array<TDerived, EPROTOCOL_TYPE_CUSTOM>();
			init_array<TDerived, EPROTOCOL_TYPE_PROTOCOLBUFF>();
			if (isbroadcast())
			{
				// # ע��㲥������
				register_actornonet<EPROTOCOL_TYPE_CUSTOM, TDerived>(true, (Tfun<actor, np_actor_broadcast>) & actor::handle);
			}
			// # ע��actor close������
			register_actornonet<EPROTOCOL_TYPE_CUSTOM, TDerived>(true, (Tfun<actor, np_actor_close>) & actor::handle);
		}

		// # ע�ᶨʱ��
		template <typename TDerived>
		static void register_timer(Tfun<TDerived, np_timerparm> afun = &TDerived::timer_handle)
		{
			ninst<TDerived, EPROTOCOL_TYPE_CUSTOM>().template rfun_nonet<TDerived, np_timerparm>(afun, false);
		}

#pragma region register_db
		// # ע��db����
		template <pbdb::ENUM_DB DBTYPE, typename TDBTAB>
		class db_pair{};

		template <typename TDerived, pbdb::ENUM_DB DBTYPE, typename TDBTAB>
		static void register_db(const db_pair<DBTYPE, TDBTAB>*)
		{
			auto lfun = &actor_base::template handle<DBTYPE, TDBTAB, TDerived>;
			ninst<TDerived, EPROTOCOL_TYPE_PROTOCOLBUFF>().template rfun<actor_base, np_actordb_load_response<DBTYPE, TDBTAB>>(lfun, true);
		}

		template <typename TDerived, pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename ...ARG>
		static void register_db(const db_pair<DBTYPE, TDBTAB>* ap, const ARG*... arg)
		{
			register_db<TDerived>(ap);
			register_db<TDerived>(arg...);
		}
#pragma endregion 

#pragma region register_actor

		// # ����ע���������������ڶ�Ӧactor��
		template <EPROTOCOL_TYPE TYPE, typename TDerived, typename T>
		static void register_actor_s(const std::function<void(TDerived*, message<T>&)>& afun)
		{
			ninst<TDerived, TYPE>().template rfun<TDerived, T>(afun);
		}

		// # ע��actor��Ա����(�����Ƿ�handle)
		template <EPROTOCOL_TYPE TYPE, typename TDerived, typename T>
		static void register_actor(bool aisload, T afun)
		{
			ninst<TDerived, TYPE>().rfun(afun, aisload);
		}

		template <EPROTOCOL_TYPE TYPE, typename TDerived, typename T, typename ...ARG>
		static void register_actor(bool aisload, T afun, ARG... argfun)
		{
			register_actor<TYPE, TDerived>(aisload, afun);
			register_actor<TYPE, TDerived, ARG...>(aisload, argfun...);
		}
	private:
		// # ע��actor��Աhandle����
		template <EPROTOCOL_TYPE TYPE, typename TDerived>
		class register_actor_handle
		{
		public:
			template <typename T>
			static void func(bool aisload)
			{
				std::string lname = tools::type_name<TDerived>();
				ninst<TDerived, TYPE>().rfun((Tfun<TDerived, T>) & TDerived::handle, aisload);
			}
		};
	public:
		template <EPROTOCOL_TYPE TYPE, typename TDerived>
		using register_handle = template_arg<actor::register_actor_handle<TYPE, TDerived>, bool>;

		template <typename TDerived>
		using register_handle_custom = register_handle<EPROTOCOL_TYPE_CUSTOM, TDerived>;

		template <typename TDerived>
		using register_handle_proto = register_handle<EPROTOCOL_TYPE_PROTOCOLBUFF, TDerived>;
#pragma endregion 

#pragma region register_actornonet
		//# ��register_actor���� ֻ������ע�������
		template <EPROTOCOL_TYPE TYPE, typename TDerived, typename T>
		static void register_actornonet(bool aisload, T afun)
		{
			ninst<TDerived, TYPE>().rfun_nonet(afun, aisload);
		}
#pragma endregion 

	private:
		friend class nforward;

		//# ע�� [forward:ת��Э��]
		template <EPROTOCOL_TYPE TYPE, bool IsForward, typename TDerived>
		class cregister_forward_handle
		{
		public:
			template <typename T>
			static void func()
			{
				using type_np_actor_forward = Tfun<TDerived, np_actor_forward<T, TYPE, IsForward, ngl::forward>>;
				ninst<TDerived, TYPE>().template rfun_forward<IsForward>((type_np_actor_forward)&TDerived::handle, nactor_type<TDerived>::type(), false);
			}
		};

		template <EPROTOCOL_TYPE TYPE, bool IsForward, typename TDerived>
		using register_forward_handle = template_arg<actor::cregister_forward_handle<TYPE, IsForward, TDerived>>;

		template <typename T1, typename T2>
		static bool isforward()
		{
			return tprotocol::isforward<ngl::np_actor_forward<T1, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T2>>()
				&& tprotocol::isforward<ngl::np_actor_forward<T1, EPROTOCOL_TYPE_PROTOCOLBUFF, false, T2>>();
		}

		//# ע�� [forward:ת��Э��] recvforward
		template <EPROTOCOL_TYPE TYPE, typename TDerived>
		class cregister_recvforward_handle
		{
		public:
			template <typename T>
			static void func()
			{
				static bool lisforward = isforward<T, ngl::forward>() && isforward<T, T>();
				if (lisforward == false)
				{
					std::cout << std::format("tprotocol_forward_pb:{}", tools::type_name<T>()) << std::endl;
					assert(false);
				}
				ninst<TDerived, TYPE>().rfun_recvforward((Tfun<TDerived, T>) & TDerived::handle, false);
			}
		};

		template <EPROTOCOL_TYPE TYPE, typename TDerived>
		using register_recvforward_handle = template_arg<actor::cregister_recvforward_handle<TYPE, TDerived>>;

		//# �����ڶ���ת��
		template <EPROTOCOL_TYPE TYPE, ENUM_ACTOR ACTOR, typename TDerived>
		class cregister_recvforward_handle2
		{
		public:
			template <typename T>
			static void func()
			{
				static bool lisforward = isforward<T, ngl::forward>() && isforward<T, T>();
				if (lisforward == false)
				{
					std::cout << std::format("tprotocol_forward_pb:{}", tools::type_name<T>()) << std::endl;
					assert(false);
				}
				// ����쳣 ˵����Ҫ��[tprotocol_forward_pb]��ע��
				assert(lisforward);
				ninst<TDerived, TYPE>().rfun_recvforward((Tfun<TDerived, T>) & TDerived::template handle_forward<ACTOR, T>, false);
			}
		};

		template <EPROTOCOL_TYPE TYPE, ENUM_ACTOR ACTOR, typename TDerived>
		using register_recvforward_handle2 = template_arg<actor::cregister_recvforward_handle2<TYPE, ACTOR, TDerived>>;
#pragma endregion 
	public:
		explicit actor(const actorparm& aparm);

		virtual ~actor();

		actor_stat get_activity_stat() final;

		void set_activity_stat(actor_stat astat) final;

		void wait_ready(const nguid& aguid);
	private:
		// # �ͷ�actor��Ϣ�б�
		void release() final;

		// # ��Ϣ�б��Ƿ�Ϊ��
		bool list_empty() final;

		// # ����Ϣ�б�������µ���Ϣ
		void push(handle_pram& apram) final;

		// # ���߳���������������Ϣ
		void actor_handle(i32_threadid athreadid) final;
	public:
#pragma region ActorBroadcast
		// ############# [�㲥] ############# 
		// # ���һ��ʱ�䷢��Ĺ㲥
		// # 1���������� 
		// # 2������broadcast����
		// # ��actor_base::start_broadcast() ���Ӧ
		// # ���ش˷���ʵ��actor_base::m_broadcast���봥���¼�
		virtual void broadcast() {}
		// # �㲥������
		bool handle(const message<np_actor_broadcast>& adata);
		// ############# [�㲥] ############# 
#pragma endregion
		
		// # �رմ�actor
		bool handle(const message<np_actor_close>&)
		{
			erase_actor();
			return true;
		}
	};
}//namespace ngl