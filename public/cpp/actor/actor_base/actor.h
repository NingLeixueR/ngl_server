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
		std::list<handle_pram>						m_list;							// 待处理消息列表
		std::map<int32_t, std::list<handle_pram>>	m_hightlist;					// 待处理消息列表(高特权)
		std::map<int32_t, std::list<handle_pram>>	m_localhightlist;				// 正在处理消息列表(高特权)	
		actor_stat									m_stat = actor_stat_init;		// actor状态
		std::shared_mutex							m_mutex;						// 锁:[m_list:待处理消息列表]
		int32_t										m_weight = 0;					// 权重
		int32_t										m_timeout = 0;					// 超时:(当actor处理消息超过此时间)
		bool										m_release = false;				// 释放将忽略权重和超时
		nrfunbase*									m_actorfun = nullptr;			// 注册可处理函数
	public:
		template <typename TDerived>
		void init_rfun()
		{
			m_actorfun = &nrfun<TDerived>::instance();
			if (isbroadcast())
			{
				// # 注册广播处理函数
				register_actornonet<TDerived, np_actor_broadcast>(e_ready_all, (Tfun<actor, np_actor_broadcast>) & actor::handle);
			}
			// # 注册actor close处理函数
			register_actornonet<TDerived, np_actor_close>(e_ready_all, (Tfun<actor, np_actor_close>) & actor::handle);
		}

		// # 注册定时器
		template <typename TDerived>
		static void register_timer(Tfun<TDerived, np_timerparm> afun = &TDerived::timer_handle)
		{
			nrfun<TDerived>::instance().template rfun_nonet<TDerived, np_timerparm>(afun, e_ready_all);
		}

		// # 用来注册匿名函数挂载在对应actor上
		template <typename TDerived, typename T>
		static void register_actor_s(int32_t aready, const std::function<void(TDerived*, const message<T>&)>& afun)
		{
			nrfun<TDerived>::instance().template rfun<TDerived, T>(afun, aready);
		}

		// # 注册actor成员函数(可以是非handle)
		template <typename TDerived, typename ...ARG>
		static void register_actor(int32_t aready, ARG... afun)
		{
			(nrfun<TDerived>::instance().template rfun<TDerived, ARG>(afun, aready), ...);
		}

		// # 与register_actor类似 只不过不注册网络层
		template <typename TDerived, typename T>
		static void register_actornonet(enum_ready aready, const Tfun<TDerived, T> afun)
		{
			nrfun<TDerived>::instance().template rfun_nonet<TDerived, T>(afun, aready);
		}

		// # 注册actor handle函数
		template <typename TDerived, typename ...ARG>
		static void register_handle(enum_ready aready)
		{
			(nrfun<TDerived>::instance().template rfun<TDerived, ARG>((Tfun<TDerived, ARG>) & TDerived::handle, aready), ...);
		}

		// # 注册actor handle函数
		template <typename TDerived, typename ...ARG>
		static void register_script_handle(enum_ready aready)
		{
			(nrfun<TDerived>::instance().template rfun<actor, ARG>((Tfun<actor, ARG>) & actor::handle_script<ARG>, aready), ...);
		}

		// # actor_gateway_c2g 注册转发
		template <typename TDerived, typename ...ARG>
		static void register_forward_c2g()
		{
			(nrfun<TDerived>::instance().template rfun_c2g<ARG>((Tfun<TDerived, np_actor_forward<ARG, forward_c2g<forward>>>) & TDerived::handle), ...);
		}
		
		// # register_forward_g2c 注册转发
		template <typename TDerived, typename ...ARG>
		static void register_forward_g2c()
		{
			(nrfun<TDerived>::instance().template rfun_g2c<ARG>((Tfun<TDerived, np_actor_forward<ARG, forward_g2c<forward>>>) & TDerived::handle), ...);
		}

		// # actor_role 注册二次转发
		template <typename TDerived, ENUM_ACTOR ACTOR, typename ...ARG>
		static void register_secondary_forward_c2g()
		{
			(nrfun<TDerived>::instance().template rfun<TDerived, ARG>((Tfun<TDerived, ARG>) & TDerived::template handle_forward<ACTOR, ARG>, e_ready_all), ...);
		}

		explicit actor(const actorparm& aparm);

		// # 获取actor的状态
		actor_stat activity_stat() final;

		// # 设置actor的状态
		void set_activity_stat(actor_stat astat) final;

		// # 释放actor消息列表
		void release() final;

		// # 消息列表是否为空
		bool list_empty() final;

		// # 向消息列表中添加新的消息
		void push(handle_pram& apram) final;

		// # 由线程主动调用消费消息
		void actor_handle(i32_threadid athreadid) final;

		template <typename T>
		bool ahandle(const std::shared_ptr<T>& aparm)
		{
			if (m_actorfun == nullptr)
			{
				tools::no_core_dump();
				return false;
			}
			return m_actorfun->handle_switch(this, aparm);
		}

		bool ahandle(i32_threadid athreadid, handle_pram& aparm);

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
		
		// # 关闭此actor
		bool handle(const message<np_actor_close>&);

		// # 脚本语言处理消息
		template <typename TMESSAGE>
		bool handle_script(const message<TMESSAGE>& adata);
	};

	template <typename TMESSAGE>
	bool actor::handle_script(const message<TMESSAGE>& adata)
	{
		const TMESSAGE& ldata = *adata.get_data();
		return nscript_handle(ldata);
	}
}//namespace ngl