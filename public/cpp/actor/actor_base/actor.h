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
		int32_t			m_weight	= 0x7fffffff;	// Ȩ��:���λ�ȡ�̺߳�����Ϣ������
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

		nrfunbase* m_actorfun = nullptr;

#pragma region register // ��Ϣע��ӿ�
		template <typename TDerived>
		static nrfun<TDerived>& ninst()
		{
			return nrfun<TDerived>::instance();
		}

	public:
		template <typename TDerived>
		void init_rfun()
		{
			m_actorfun = &ninst<TDerived>();
			if (isbroadcast())
			{
				// # ע��㲥������
				register_actornonet<TDerived, np_actor_broadcast>(
						true, (Tfun<actor, np_actor_broadcast>) & actor::handle
					);
			}
			// # ע��actor close������
			register_actornonet<TDerived, np_actor_close>(
				true, (Tfun<actor, np_actor_close>) & actor::handle
			);
		}

		// # ע�ᶨʱ��
		template <typename TDerived>
		static void register_timer(Tfun<TDerived, np_timerparm> afun = &TDerived::timer_handle)
		{
			ninst<TDerived>().template rfun_nonet<TDerived, np_timerparm>(afun, false);
		}

#pragma region register_actor

		// # ����ע���������������ڶ�Ӧactor��
		template <typename TDerived, typename T>
		static void register_actor_s(const std::function<void(TDerived*, message<T>&)>& afun, bool aisload = true)
		{
			ninst<TDerived>().template rfun<TDerived, T>(afun, aisload);
		}

		// # ע��actor��Ա����(�����Ƿ�handle)
		template <typename TDerived, typename T>
		static void register_actor(bool aisload, T afun)
		{
			ninst<TDerived>().template rfun<TDerived, T>(afun, aisload);
		}

		template <typename TDerived, typename T, typename ...ARG>
		static void register_actor(bool aisload, T afun, ARG... argfun)
		{
			register_actor<TDerived, T>(aisload, afun);
			register_actor<TDerived, ARG...>(aisload, argfun...);
		}
	private:
		// # ע��actor��Աhandle����
		template <typename TDerived>
		struct register_actor_handle
		{
			template <typename T>
			static void func(bool aisload)
			{
				ninst<TDerived>().template rfun<TDerived, T>(
					(Tfun<TDerived, T>) & TDerived::handle, aisload
				);
			}
		};
	public:
		template <typename TDerived>
		using register_handle = template_arg<actor::register_actor_handle<TDerived>, bool>;
		
#pragma endregion 

		//# ע�� �ű�����Э��
		template <typename TDerived>
		struct cregister_actor_handle
		{
			template <typename T>
			static void func(bool aisload = false)
			{
				ninst<TDerived>().template rfun<actor, T>(
					(Tfun<actor, T>) & actor::handle_script<T>, aisload
				);
			}
		};
		
		template <typename TDerived>
		using register_script_handle = template_arg<actor::cregister_actor_handle<TDerived>, bool>;

#pragma region register_actornonet
		//# ��register_actor���� ֻ������ע�������
		template <typename TDerived, typename T>
		static void register_actornonet(bool aisload, const Tfun<TDerived, T> afun)
		{
			ninst<TDerived>().template rfun_nonet<TDerived, T>(afun, aisload);
		}
#pragma endregion 
	public:
		template <typename TDerived>
		struct c2g_forward_gateway_handle
		{
			template <typename T>
			static void func()
			{
				ninst<TDerived>().template rfun_c2g_gateway<T>(
					(Tfun<TDerived, np_actor_forward<T, forward_c2g<forward>>>)&TDerived::handle
				);
			}
		};
		template <typename TDerived>
		using register_forward_gateway_c2g = template_arg<c2g_forward_gateway_handle<TDerived>>;
		
		// # ����ת��
		template <typename TDerived, ENUM_ACTOR ACTOR>
		struct c2g_secondary_forward_handle
		{
			template <typename T>
			static void func()
			{
				ninst<TDerived>().template rfun<TDerived, T>(
					(Tfun<TDerived, T>) & TDerived::template handle_forward<ACTOR, T>
					, false
				);
			}
		};
		template <typename TDerived, ENUM_ACTOR ACTOR>
		using register_secondary_forward_c2g = template_arg<c2g_secondary_forward_handle<TDerived, ACTOR>>;

		template <typename TDerived>
		struct g2c_forward_gateway_handle
		{
			template <typename T>
			static void func()
			{
				ninst<TDerived>().template rfun_g2c_gateway<T>(
					(Tfun<TDerived, np_actor_forward<T, forward_g2c<forward>>>) & TDerived::handle
				);
			}
		};
		template <typename TDerived>
		using register_forward_gateway_g2c = template_arg<g2c_forward_gateway_handle<TDerived>>;

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

		// # �ű����Դ�����Ϣ
		template <typename TMESSAGE>
		bool handle_script(const message<TMESSAGE>& adata)
		{
			const TMESSAGE& ldata = *adata.get_data();
			return nscript_handle(ldata);
		}

		static i64_actorid tab2actor(ENUM_ACTOR atype, int32_t atabid)
		{
			return nguid::make(atype, nconfig::area(), atabid);
		}
	};
}//namespace ngl