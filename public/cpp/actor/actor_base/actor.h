#pragma once

#include "template_arg.h"
#include "nactor_type.h"
#include "nactortype.h"
#include "actor_base.h"
#include "nrfun.h"
#include "impl.h"

namespace ngl
{
	struct actorparm
	{
		actorparmbase	m_parm;
		int				m_weight	= 10;			// 权重:单次获取线程后处理消息的数量		
		bool			m_broadcast	= false;		// 是否支持广播
	};

	template <typename T>
	constexpr T* null = (T*)nullptr;

	class actor : 
		public actor_base
	{
		struct impl_actor;
		ngl::impl<impl_actor> m_impl_actor;

		std::array<nrfunbase*, EPROTOCOL_TYPE_COUNT> m_actorfun;
	public:

#pragma region register // 消息注册接口
		template <typename TDerived, EPROTOCOL_TYPE TYPE>
		static nrfun<TDerived, TYPE>& ninst()
		{
			return nrfun<TDerived, TYPE>::instance();
		}

		template <typename TDerived>
		void init_rfun()
		{
			m_actorfun[EPROTOCOL_TYPE_CUSTOM]
				= &ninst<TDerived, EPROTOCOL_TYPE_CUSTOM>();
			m_actorfun[EPROTOCOL_TYPE_PROTOCOLBUFF]
				= &ninst<TDerived, EPROTOCOL_TYPE_PROTOCOLBUFF>();
		
			if (isbroadcast())
			{
				register_actornonet<EPROTOCOL_TYPE_CUSTOM, TDerived>(
					true, 
					(Tfun<actor, np_actor_broadcast>) & actor::handle
				);
			}
		}

		//# 注册定时器
		template <typename TDerived>
		static void register_timer(Tfun<TDerived, timerparm> afun = &TDerived::timer_handle)
		{
			ninst<TDerived, EPROTOCOL_TYPE_CUSTOM>().
				template rfun_nonet<TDerived, timerparm>(afun, false);
		}

		template <pbdb::ENUM_DB DBTYPE, typename TDBTAB>
		class db_pair{};

		//# 注册db加载
		template <EPROTOCOL_TYPE TYPE, typename TDerived, pbdb::ENUM_DB DBTYPE, typename TDBTAB>
		static void register_db(const db_pair<DBTYPE, TDBTAB>*)
		{
			using tloaddb = np_actordb_load_response<TYPE, DBTYPE, TDBTAB>;
			auto lpfun = &actor_base::template handle<TYPE, DBTYPE, TDBTAB, TDerived>;

			ninst<TDerived, TYPE>().template rfun<actor_base, tloaddb>(lpfun, true);
		}

		template <EPROTOCOL_TYPE TYPE, typename TDerived, pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename ...ARG>
		static void register_db(const db_pair<DBTYPE, TDBTAB>* ap, const ARG*... arg)
		{
			register_db<TYPE, TDerived>(ap);
			register_db<TYPE, TDerived>(arg...);
		}

		//# 用来注册匿名函数挂载在对应actor上
		template <EPROTOCOL_TYPE TYPE, typename TDerived, typename T>
		static void register_actor_s(const std::function<void(TDerived*, T&)>& afun)
		{
			ninst<TDerived, TYPE>().template rfun<TDerived, T>(afun);
		}

#pragma region register_actor

		//# 简化[handle]方法注册
		#define dregister_fun_handle(TDerived,T)		(Tfun<TDerived, T>)&TDerived::handle
		#define dregister_fun(TDerived, T, Fun)			(Tfun<TDerived, T>)&TDerived::Fun

		//# 注册actor成员函数
		template <EPROTOCOL_TYPE TYPE , typename TDerived , typename T>
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

		template <EPROTOCOL_TYPE TYPE, typename TDerived>
		class register_actor_handle
		{
		public:
			template <typename T>
			static void func(bool aisload)
			{
				ninst<TDerived, TYPE>().rfun((Tfun<TDerived, T>) & TDerived::handle, aisload);
			}
		};

		template <EPROTOCOL_TYPE TYPE, typename TDerived>
		using type_register_actor_handle = template_arg<actor::register_actor_handle<TYPE, TDerived>, bool>;

#pragma endregion 

#pragma region register_actornonet
		//# 与register_actor类似 只不过不注册网络层
		template <EPROTOCOL_TYPE TYPE, typename TDerived, typename T>
		static void register_actornonet(bool aisload, T afun)
		{
			ninst<TDerived, TYPE>().rfun_nonet(afun, aisload);
		}

		template <EPROTOCOL_TYPE TYPE, typename TDerived, typename T, typename ...ARG>
		static void register_actornonet(bool aisload, T apdata, ARG... arg)
		{
			register_actornonet<TYPE, TDerived>(aisload, apdata);
			register_actornonet<TYPE, TDerived>(aisload, arg...);
		}
#pragma endregion 
	private:
		friend class nforward;

		//# 注册 [forward:转发协议]
		template <EPROTOCOL_TYPE TYPE, bool IsForward, typename TDerived, typename T>
		static void register_forward(T afun)
		{
			ninst<TDerived, TYPE>().
				template rfun_forward<IsForward>(afun, nactor_type<TDerived>::type(), false);
		}

		template <EPROTOCOL_TYPE TYPE, bool IsForward, typename TDerived, typename T, typename ...ARG>
		static void register_forward(T ap, ARG... arg)
		{
			register_forward<TYPE, IsForward, TDerived, T>(ap);
			register_forward<TYPE, IsForward, TDerived, ARG...>(arg...);
		}

		template <EPROTOCOL_TYPE TYPE, bool IsForward, typename TDerived>
		class register_forward_handle
		{
		public:
			template <typename T>
			static void func()
			{
				ninst<TDerived, TYPE>().
					template rfun_forward<IsForward>((Tfun<TDerived, np_actor_forward<T, TYPE, false, ngl::forward>>) & TDerived::handle, nactor_type<TDerived>::type(), false);
			}
		};

		template <EPROTOCOL_TYPE TYPE, bool IsForward, typename TDerived>
		using type_register_forward_handle = template_arg<actor::register_forward_handle<TYPE, IsForward, TDerived>>;


		//# 注册 [forward:转发协议] recvforward
		template <EPROTOCOL_TYPE TYPE, typename TDerived, typename T>
		static void register_recvforward(T afun)
		{
			ninst<TDerived, TYPE>().rfun_recvforward(afun, false);
		}

		template <EPROTOCOL_TYPE TYPE, typename TDerived, typename T, typename ...ARG>
		static void register_recvforward(T ap, ARG... arg)
		{
			register_recvforward<TYPE, TDerived>(ap);
			register_recvforward<TYPE, TDerived>(arg...);
		}

		template <EPROTOCOL_TYPE TYPE, ENUM_ACTOR ACTOR, typename TDerived>
		class register_recvforward_handle
		{
		public:
			template <typename T>
			static void func()
			{
				Tfun<TDerived, T> lfun = &TDerived::template handle_forward<ACTOR, T>;
				ninst<TDerived, TYPE>().rfun_recvforward(lfun, false);
			}
		};

		template <EPROTOCOL_TYPE TYPE, ENUM_ACTOR ACTOR, typename TDerived>
		using type_register_recvforward_handle = template_arg<actor::register_recvforward_handle<TYPE, ACTOR, TDerived>>;

#pragma endregion 
	public:
		explicit actor(const actorparm& aparm);

		virtual ~actor();

		virtual actor_stat get_activity_stat() final;

		virtual void set_activity_stat(actor_stat astat) final;
	private:
		virtual void release() final;

		virtual bool list_empty() final;

		virtual void push(handle_pram& apram) final;

		virtual void clear_task() final;

		virtual void actor_handle(i32_threadid athreadid) final;
	public:
#pragma region ActorBroadcast
		// ############# Start[Actor 全员广播] ############# 
		// ## 间隔一段时间发起的全员(所有actor)广播
		// ## 可以在这个广播里推送一些需要处理的任务,例如 保存数据
		// ## 与actor_base::start_broadcast() 相呼应
		// ## 重载此方法实现actor_base::m_broadcast毫秒触发事件
		virtual void broadcast() {}
		// ## 广播处理函数
		bool handle(message<np_actor_broadcast>& adata);
		// ############# End[Actor 全员广播] ############# 
#pragma endregion
	};
}//namespace ngl