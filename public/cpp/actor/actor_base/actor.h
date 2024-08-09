#pragma once

#include "template_arg.h"
#include "nactortype.h"
#include "actor_base.h"
#include "nrfun.h"
#include "impl.h"
#include "nlog.h"

namespace ngl
{
	struct actorparm
	{
		actorparmbase	m_parm;
		int				m_weight	= 10;			// 权重:单次获取线程后处理消息的数量		
		bool			m_broadcast	= false;		// 是否支持广播(如果需要加载dbclient，需要支持广播)
	};

	template <typename T>
	constexpr T* null = (T*)nullptr;

	class actor : 
		public actor_base
	{
		actor() = delete;
		actor(const actor&) = delete;
		actor& operator=(const actor&) = delete;

		struct impl_actor;
		ngl::impl<impl_actor> m_impl_actor;

		std::array<nrfunbase*, EPROTOCOL_TYPE_COUNT> m_actorfun;

#pragma region register // 消息注册接口

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
				// # 注册广播处理函数
				register_actornonet<EPROTOCOL_TYPE_CUSTOM, TDerived>(
					true, 
					(Tfun<actor, np_actor_broadcast>) & actor::handle
				);
			}
			// # 注册actor close处理函数
			register_actornonet<EPROTOCOL_TYPE_CUSTOM, TDerived>(
				true,
				(Tfun<actor, np_actor_close>) & actor::handle
			);
		}

		// # 注册定时器
		template <typename TDerived>
		static void register_timer(
			Tfun<TDerived, timerparm> afun = &TDerived::timer_handle
		)
		{
			ninst<TDerived, EPROTOCOL_TYPE_CUSTOM>().
				template rfun_nonet<TDerived, timerparm>(afun, false);
		}

#pragma region register_db
		// # 注册db加载
		template <pbdb::ENUM_DB DBTYPE, typename TDBTAB>
		class db_pair{};

		template <typename TDerived, pbdb::ENUM_DB DBTYPE, typename TDBTAB>
		static void register_db(const db_pair<DBTYPE, TDBTAB>*)
		{
			auto lfun = &actor_base::template handle<DBTYPE, TDBTAB, TDerived>;
			ninst<TDerived, EPROTOCOL_TYPE_PROTOCOLBUFF>().
				template rfun<actor_base, np_actordb_load_response<DBTYPE, TDBTAB>>(lfun, true);
		}

		template <typename TDerived, pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename ...ARG>
		static void register_db(const db_pair<DBTYPE, TDBTAB>* ap, const ARG*... arg)
		{
			register_db<TDerived>(ap);
			register_db<TDerived>(arg...);
		}
#pragma endregion 

#pragma region register_actor

		// # 用来注册匿名函数挂载在对应actor上
		template <EPROTOCOL_TYPE TYPE, typename TDerived, typename T>
		static void register_actor_s(const std::function<void(TDerived*, message<T>&)>& afun)
		{
			ninst<TDerived, TYPE>().template rfun<TDerived, T>(afun);
		}

		// # 注册actor成员函数(可以是非handle)
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
		// # 注册actor成员handle函数
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
	public:
		template <EPROTOCOL_TYPE TYPE, typename TDerived>
		using register_handle = template_arg<
			actor::register_actor_handle<TYPE, TDerived>, bool
		>;

		template <typename TDerived>
		using register_handle_custom = register_handle<
			EPROTOCOL_TYPE_CUSTOM, TDerived
		>;

		template <typename TDerived>
		using register_handle_proto = register_handle<
			EPROTOCOL_TYPE_PROTOCOLBUFF, TDerived
		>;
#pragma endregion 

#pragma region register_actornonet
		//# 与register_actor类似 只不过不注册网络层
		template <EPROTOCOL_TYPE TYPE, typename TDerived, typename T>
		static void register_actornonet(bool aisload, T afun)
		{
			ninst<TDerived, TYPE>().rfun_nonet(afun, aisload);
		}
#pragma endregion 

	private:
		friend class nforward;

		//# 注册 [forward:转发协议]
		template <EPROTOCOL_TYPE TYPE, bool IsForward, typename TDerived>
		class cregister_forward_handle
		{
		public:
			template <typename T>
			static void func()
			{
				using type_np_actor_forward = Tfun<
					TDerived, np_actor_forward<T, TYPE, IsForward, ngl::forward>
				>;
				ninst<TDerived, TYPE>().
					template rfun_forward<IsForward>(
						(type_np_actor_forward)&TDerived::handle
						, nactor_type<TDerived>::type()
						, false
					);
			}
		};

		template <EPROTOCOL_TYPE TYPE, bool IsForward, typename TDerived>
		using register_forward_handle = template_arg<
			actor::cregister_forward_handle<TYPE, IsForward, TDerived>
		>;

		//# 注册 [forward:转发协议] recvforward
		template <EPROTOCOL_TYPE TYPE, typename TDerived>
		class cregister_recvforward_handle
		{
		public:
			template <typename T>
			static void func()
			{
				ninst<TDerived, TYPE>().
					rfun_recvforward((Tfun<TDerived, T>) & TDerived::handle, false);
			}
		};

		template <EPROTOCOL_TYPE TYPE, typename TDerived>
		using register_recvforward_handle = template_arg<
			actor::cregister_recvforward_handle<TYPE, TDerived>
		>;

		//# 服务于二次转发
		template <EPROTOCOL_TYPE TYPE, ENUM_ACTOR ACTOR, typename TDerived>
		class cregister_recvforward_handle2
		{
		public:
			template <typename T>
			static void func()
			{
				ninst<TDerived, TYPE>()
					.rfun_recvforward(
						(Tfun<TDerived, T>) & TDerived::template handle_forward<ACTOR, T>
						, false
					);
			}
		};

		template <EPROTOCOL_TYPE TYPE, ENUM_ACTOR ACTOR, typename TDerived>
		using register_recvforward_handle2 = template_arg<
			actor::cregister_recvforward_handle2<TYPE, ACTOR, TDerived>
		>;
#pragma endregion 
	public:
		explicit actor(const actorparm& aparm);

		virtual ~actor()
		{}

		virtual actor_stat get_activity_stat() final;

		virtual void set_activity_stat(actor_stat astat) final;
	private:
		// # 释放actor消息列表
		virtual void release() final;

		// # 消息列表是否为空
		virtual bool list_empty() final;

		// # 向消息列表中添加新的消息
		virtual void push(handle_pram& apram) final;

		// # 由线程主动调用消费消息
		virtual void actor_handle(i32_threadid athreadid) final;

	public:
#pragma region ActorBroadcast
		// ############# [广播] ############# 
		// # 间隔一段时间发起的广播
		// # 1、保存数据 
		// # 2、调用broadcast函数
		// # 与actor_base::start_broadcast() 相呼应
		// # 重载此方法实现actor_base::m_broadcast毫秒触发事件
		virtual void broadcast() {}
		// # 广播处理函数
		bool handle(message<np_actor_broadcast>& adata);
		// ############# [广播] ############# 
#pragma endregion
		
		// # 关闭此actor
		bool handle(message<np_actor_close>& adata)
		{
			erase_actor_byid();
			return true;
		}
	};
}//namespace ngl