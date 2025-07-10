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

		std::array<nrfunbase*, EPROTOCOL_TYPE_COUNT> m_actorfun = { nullptr };

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
				register_actornonet<EPROTOCOL_TYPE_CUSTOM, TDerived, np_actor_broadcast>(
						true, (Tfun<actor, np_actor_broadcast>) & actor::handle
					);
			}
			// # 注册actor close处理函数
			register_actornonet<EPROTOCOL_TYPE_CUSTOM, TDerived, np_actor_close>(
				true, (Tfun<actor, np_actor_close>) & actor::handle
			);
		}

		// # 注册定时器
		template <typename TDerived>
		static void register_timer(Tfun<TDerived, np_timerparm> afun = &TDerived::timer_handle)
		{
			ninst<TDerived, EPROTOCOL_TYPE_CUSTOM>().template 
				rfun_nonet<TDerived, np_timerparm, true>(afun, false);
			ninst<TDerived, EPROTOCOL_TYPE_CUSTOM>().template 
				rfun_nonet<TDerived, np_timerparm, false>(afun, false);
		}

#pragma region register_db
		// # 注册db加载
		template <pbdb::ENUM_DB DBTYPE, typename TDBTAB>
		class db_pair{};

		template <typename TDerived, pbdb::ENUM_DB DBTYPE, typename TDBTAB>
		static void register_db(const db_pair<DBTYPE, TDBTAB>*)
		{
			ninst<TDerived, EPROTOCOL_TYPE_CUSTOM>()
				.template rfun<actor_base, np_actordb_load_response<DBTYPE, TDBTAB>, false>(
					&actor_base::template handle<DBTYPE, TDBTAB, TDerived>, true
				);
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
		static void register_actor_s(const std::function<void(TDerived*, message<T>&)>& afun, bool aisload = true)
		{
			if constexpr (TYPE == EPROTOCOL_TYPE_CUSTOM)
			{
				ninst<TDerived, TYPE>().template rfun<TDerived, T, true>(afun, aisload);
			}
			ninst<TDerived, TYPE>().template rfun<TDerived, T, false>(afun, aisload);
		}

		// # 注册actor成员函数(可以是非handle)
		template <EPROTOCOL_TYPE TYPE, typename TDerived, typename T>
		static void register_actor(bool aisload,  T afun)
		{
			if constexpr (TYPE == EPROTOCOL_TYPE_CUSTOM)
			{
				ninst<TDerived, TYPE>().template rfun<TDerived, T, true>(afun, aisload);
			}
			ninst<TDerived, TYPE>().template rfun<TDerived, T, false>(afun, aisload);
		}

		template <EPROTOCOL_TYPE TYPE, typename TDerived, typename T, typename ...ARG>
		static void register_actor(bool aisload, T afun, ARG... argfun)
		{
			register_actor<TYPE, TDerived, T>(aisload, afun);
			register_actor<TYPE, TDerived, ARG...>(aisload, argfun...);
		}
	private:
		// # 注册actor成员handle函数
		template <EPROTOCOL_TYPE TYPE, typename TDerived>
		struct register_actor_handle
		{
			template <typename T>
			static void func(bool aisload)
			{
				std::string lname = tools::type_name<TDerived>();
				if constexpr (TYPE == EPROTOCOL_TYPE_CUSTOM)
				{
					ninst<TDerived, TYPE>().template rfun<TDerived, T, true>((Tfun<TDerived, T>) & TDerived::handle, aisload);
				}
				ninst<TDerived, TYPE>().template rfun<TDerived, T, false>((Tfun<TDerived, T>) & TDerived::handle, aisload);
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

		//# 注册 脚本处理协议
		template <EPROTOCOL_TYPE TYPE, typename TDerived>
		struct cregister_actor_handle
		{
			template <typename T>
			static void func(bool aisload = false)
			{
				if constexpr (TYPE == EPROTOCOL_TYPE_CUSTOM)
				{
					ninst<TDerived, TYPE>().template rfun<actor, T, true>(
						(Tfun<actor, T>) & actor::handle_script<TYPE, T>, aisload
					);
				}
				ninst<TDerived, TYPE>().template rfun<actor, T, false>(
					(Tfun<actor, T>) & actor::handle_script<TYPE, T>, aisload
				);
			}
		};
		
		template <EPROTOCOL_TYPE TYPE, typename TDerived>
		using register_script_handle = template_arg<actor::cregister_actor_handle<TYPE, TDerived>, bool>;


#pragma region register_actornonet
		//# 与register_actor类似 只不过不注册网络层
		template <EPROTOCOL_TYPE TYPE, typename TDerived, typename T>
		static void register_actornonet(bool aisload, const Tfun<TDerived, T> afun)
		{
			if constexpr (TYPE == EPROTOCOL_TYPE_CUSTOM)
			{
				ninst<TDerived, TYPE>().template rfun_nonet<TDerived, T, true>(afun, aisload);
			}
			ninst<TDerived, TYPE>().template rfun_nonet<TDerived, T, false>(afun, aisload);
		}
#pragma endregion 

	private:
		friend class nforward;

		//# 注册 [forward:转发协议]
		template <EPROTOCOL_TYPE TYPE, bool IsForward, typename TDerived>
		struct cregister_forward_handle
		{
			template <typename T>
			static void func()
			{
				using type_np_actor_forward = Tfun<TDerived, np_actor_forward<T, TYPE, IsForward, ngl::forward>>;
				ninst<TDerived, TYPE>().template rfun_forward<IsForward>(
					(type_np_actor_forward)&TDerived::handle, nactor_type<TDerived>::type(), false
				);
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

		//# 注册 [forward:转发协议] recvforward
		template <EPROTOCOL_TYPE TYPE, typename TDerived>
		struct cregister_recvforward_handle
		{
			template <typename T>
			static void func()
			{
				static bool lisforward = isforward<T, ngl::forward>() && isforward<T, T>();
				if (!lisforward)
				{
					std::cout << std::format("tprotocol_forward_pb:{}", tools::type_name<T>()) << std::endl;
					tools::no_core_dump();
				}
				ninst<TDerived, TYPE>().rfun_recvforward(
					(Tfun<TDerived, T>) & TDerived::handle, false
				);
			}
		};

		template <EPROTOCOL_TYPE TYPE, typename TDerived>
		using register_recvforward_handle = template_arg<actor::cregister_recvforward_handle<TYPE, TDerived>>;

		//# 服务于二次转发
		template <EPROTOCOL_TYPE TYPE, ENUM_ACTOR ACTOR, typename TDerived>
		struct cregister_recvforward_handle2
		{
			template <typename T>
			static void func()
			{
				static bool lisforward = isforward<T, ngl::forward>() && isforward<T, T>();
				if (!lisforward)
				{
					std::cout << std::format("tprotocol_forward_pb:{}", tools::type_name<T>()) << std::endl;
					tools::no_core_dump();
				}
				ninst<TDerived, TYPE>().rfun_recvforward(
					(Tfun<TDerived, T>) & TDerived::template handle_forward<ACTOR, T>, false
				);
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
		template <EPROTOCOL_TYPE TYPE, typename TMESSAGE>
		bool handle_script(const message<TMESSAGE>& adata)
		{
			nscript_handle<TYPE, TMESSAGE>(*adata.get_data());
			return true;
		}

		static i64_actorid tab2actor(ENUM_ACTOR atype, int32_t atabid)
		{
			return nguid::make(atype, nconfig::area(), atabid);
		}
	};
}//namespace ngl