/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "template_arg.h"
#include "nactortype.h"
#include "actor_base.h"
#include "tprotocol.h"
#include "nrfun.h"
#include "slist.h"
#include "nlog.h"

#include <deque>
#include <queue>

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

	public:
#define STL_MESSAGELIST
#ifdef STL_MESSAGELIST
		template <typename T>
		using tls = std::deque<T>;
		template <typename T>
		using trunls = std::deque<T>;
#else
		template <typename T>
		using tls = slist_production<T>;
		template <typename T>
		using trunls = slist_consumption<T>;
#endif//STL_MESSAGELIST
	private:
		tls<handle_pram>					m_list;							// 待处理消息列表
		std::map<int32_t, tls<handle_pram>>	m_hightlist;					// 待处理消息列表(高特权)			
		trunls<handle_pram>					m_locallist;					// 正在处理消息列表
		std::map<int32_t, tls<handle_pram>>	m_localhightlist;				// 正在处理消息列表(高特权)	
		actor_stat							m_stat = actor_stat_init;		// actor状态
		std::shared_mutex					m_mutex;						// 锁:[m_list:待处理消息列表]
		int32_t								m_weight = 0;					// 权重
		int32_t								m_timeout = 0;					// 超时:(当actor处理消息超过此时间)
		bool								m_release = false;				// 释放将忽略权重和超时
		nrfunbase*							m_actorfun = nullptr;			// 注册可处理函数

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
					nready::e_ready_all, (Tfun<actor, np_actor_broadcast>) & actor::handle
				);
			}
			// # 注册actor close处理函数
			register_actornonet<TDerived, np_actor_close>(
				nready::e_ready_all, (Tfun<actor, np_actor_close>) & actor::handle
			);
		}

		// # 注册定时器
		template <typename TDerived>
		static void register_timer(Tfun<TDerived, np_timerparm> afun = &TDerived::timer_handle)
		{
			ninst<TDerived>().template rfun_nonet<TDerived, np_timerparm>(afun, nready::e_ready_all);
		}

#pragma region register_actor

		// # 用来注册匿名函数挂载在对应actor上
		template <typename TDerived, typename T>
		static void register_actor_s(const std::function<void(TDerived*, message<T>&)>& afun, int32_t aready/*nready::enum_ready*/)
		{
			ninst<TDerived>().template rfun<TDerived, T>(afun, aready);
		}

		// # 注册actor成员函数(可以是非handle)
		template <typename TDerived, typename T>
		static void register_actor(int32_t aready/*nready::enum_ready*/, T afun)
		{
			ninst<TDerived>().template rfun<TDerived, T>(afun, aready);
		}

		template <typename TDerived, typename T, typename ...ARG>
		static void register_actor(int32_t aready/*nready::enum_ready*/, T afun, ARG... argfun)
		{
			register_actor<TDerived, T>(aready, afun);
			register_actor<TDerived, ARG...>(aready, argfun...);
		}
	private:
		// # 注册actor成员handle函数
		template <typename TDerived>
		struct register_actor_handle
		{
			template <typename T>
			static void func(int32_t aready/*nready::enum_ready*/)
			{
				ninst<TDerived>().template rfun<TDerived, T>(
					(Tfun<TDerived, T>) & TDerived::handle, aready
				);
			}
		};
	public:
		template <typename TDerived>
		using register_handle = template_arg<actor::register_actor_handle<TDerived>, int32_t>;
		
#pragma endregion 

		//# 注册 脚本处理协议
		template <typename TDerived>
		struct cregister_actor_handle
		{
			template <typename T>
			static void func(int32_t aready/*nready::enum_ready*/)
			{
				ninst<TDerived>().template rfun<actor, T>(
					(Tfun<actor, T>) & actor::handle_script<T>, aready
				);
			}
		};
		
		template <typename TDerived>
		using register_script_handle = template_arg<actor::cregister_actor_handle<TDerived>, int32_t>;

#pragma region register_actornonet
		//# 与register_actor类似 只不过不注册网络层
		template <typename TDerived, typename T>
		static void register_actornonet(nready::enum_ready aready, const Tfun<TDerived, T> afun)
		{
			ninst<TDerived>().template rfun_nonet<TDerived, T>(afun, aready);
		}
#pragma endregion 
	public:
#pragma region 	register_gateway
		//# gateway注册接收转发协议处理协议
		template <typename TDerived>
		struct c2g_forward_handle
		{
			template <typename T>
			static void func()
			{
				ninst<TDerived>().template rfun_c2g<T>(
					(Tfun<TDerived, np_actor_forward<T, forward_c2g<forward>>>)&TDerived::handle
				);
			}
		};
		template <typename TDerived>
		using register_forward_c2g = template_arg<c2g_forward_handle<TDerived>>;
		
		template <typename TDerived>
		struct g2c_forward_handle
		{
			template <typename T>
			static void func()
			{
				ninst<TDerived>().template rfun_g2c<T>(
					(Tfun<TDerived, np_actor_forward<T, forward_g2c<forward>>>) & TDerived::handle
				);
			}
		};
		template <typename TDerived>
		using register_forward_g2c = template_arg<g2c_forward_handle<TDerived>>;
#pragma endregion 

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
#pragma endregion 
	public:
		explicit actor(const actorparm& aparm);

		virtual ~actor();

		// # 获取actor的状态
		actor_stat get_activity_stat() final;

		// # 设置actor的状态
		void set_activity_stat(actor_stat astat) final;

		// # 等待ready
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

		bool ahandle(i32_threadid athreadid, handle_pram& aparm);

		// # 设置kcp
		void set_kcp(const handle_pram& aparm);
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