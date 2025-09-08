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
		int32_t			m_weight	= 0x7fffffff;	// 权重:单次获取线程后处理消息的数量
		int32_t			m_timeout	= 0x7fffffff;	// 超时:(当actor处理消息超过此时间)
		bool			m_broadcast	= false;		// 是否支持广播(如果需要加载dbclient，需要支持广播)
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

#pragma region register // 消息注册接口
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
				// # 注册广播处理函数
				register_actornonet<TDerived, np_actor_broadcast>(
						true, (Tfun<actor, np_actor_broadcast>) & actor::handle
					);
			}
			// # 注册actor close处理函数
			register_actornonet<TDerived, np_actor_close>(
				true, (Tfun<actor, np_actor_close>) & actor::handle
			);
		}

		// # 注册定时器
		template <typename TDerived>
		static void register_timer(Tfun<TDerived, np_timerparm> afun = &TDerived::timer_handle)
		{
			ninst<TDerived>().template rfun_nonet<TDerived, np_timerparm>(afun, false);
		}

#pragma region register_actor

		// # 用来注册匿名函数挂载在对应actor上
		template <typename TDerived, typename T>
		static void register_actor_s(const std::function<void(TDerived*, message<T>&)>& afun, bool aisload = true)
		{
			ninst<TDerived>().template rfun<TDerived, T>(afun, aisload);
		}

		// # 注册actor成员函数(可以是非handle)
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
		// # 注册actor成员handle函数
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

		//# 注册 脚本处理协议
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
		//# 与register_actor类似 只不过不注册网络层
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
		
		// # 二次转发
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
		// # 释放actor消息列表
		void release() final;

		// # 消息列表是否为空
		bool list_empty() final;

		// # 向消息列表中添加新的消息
		void push(handle_pram& apram) final;

		// # 由线程主动调用消费消息
		void actor_handle(i32_threadid athreadid) final;
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
		bool handle(const message<np_actor_broadcast>& adata);
		// ############# [广播] ############# 
#pragma endregion
		
		// # 关闭此actor
		bool handle(const message<np_actor_close>&)
		{
			erase_actor();
			return true;
		}

		// # 脚本语言处理消息
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