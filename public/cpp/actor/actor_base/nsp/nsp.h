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

#include "actor/protocol/nprotocol_template.h"
#include "actor/tab/ttab_servers.h"
#include "actor/actor_base/nguid.h"
#include "actor/actor_base/actor.h"
#include "tools/pb_field.h"

namespace ngl
{
	// # nsp client 包括nsp_read与nsp_write
	// # nsp client 加载状况
	class nsp_regload
	{
		// # 需要连接的nsp服务器列表
		std::map<i16_area, i64_actorid>		m_nspserver;
		// # 注册状态
		std::map<i16_area, bool>			m_register;
		// # 加载状态
		std::map<i16_area, bool>			m_loadfinish;
	public:
		void init(i64_actorid aactorid);

		// # 设置[area]完成注册
		void set_register(i16_area aarea);

		// # [area]是否完成注册
		bool is_register(i16_area aarea)const;

		// # 所有[area]是否都完成注册
		bool is_register()const;

		// # 设置[area]加载数据完成
		void set_loadfinish(i16_area aarea);

		// # [area]是否加载数据完成
		bool is_loadfinish(i16_area aarea)const;

		// # 所有[area]是否都加载数据完成
		bool is_loadfinish()const;

		// # 根据[area]获取nsp server
		i64_actorid nspserid(i16_area aarea)const;

		// # 遍历所有需要连接的nsp server
		void foreach_nspser(const std::function<void(i16_area, i64_actorid)>& afun)const;
	};

	// # 对"数据字段"进行封装
	class operator_field
	{
		// # 只有nep server设置时不检查"因为同一字段设置读/写，写优先于读，进行读覆盖写的检测"
		bool m_nspserver = false;
		std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>> m_node_fieldnumbers;
	public:
		// # 初始化
		void init(bool anspserver);

		// # 设置"数据字段"
		void set_field(i16_actortype atype, const std::map<i32_fieldnumber, epb_field>& anode_fieldnumbers);

		// # 设置
		void set_field(const std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>>& anode_fieldnumbers);

		// # 添加
	private:
		void nspser_add_field(std::map<i32_fieldnumber, epb_field>& afieldmap, i32_fieldnumber afieldnumber, epb_field afieldtype);
		void nspcli_add_field(std::map<i32_fieldnumber, epb_field>& afieldmap, i32_fieldnumber afieldnumber, epb_field afieldtype);
	public:
		void add_field(i16_actortype atype, i32_fieldnumber afieldnumber, epb_field afieldtype);
	
		template <typename T>
		void add_field(i16_actortype atype, epb_field afieldtype, const std::set<i32_fieldnumber>& afield)
		{
			if (afieldtype == epb_field_read && afield.empty())
			{//当[awritefieldnumbers]为空,则认为其读全部字段
				std::set<i32_fieldnumber> lreadfield;
				pb_field::field_numbers<T>(lreadfield);
				for (i32_fieldnumber field : lreadfield)
				{
					add_field(atype, field, afieldtype);
				}
			}
			else
			{
				if (afield.empty())
				{//afieldtype == epb_field_write 不允许[afield]为空
					tools::no_core_dump();
				}
				for (i32_fieldnumber field : afield)
				{
					add_field(atype, field, afieldtype);
				}
			}
		}

		template <typename T>
		void add_field(i16_actortype atype, const std::set<i32_fieldnumber>& areadfield, const std::set<i32_fieldnumber>& awritefield)
		{
			std::set<i32_fieldnumber> lreadfield;
			if (areadfield.empty())
			{//当[awritefieldnumbers]为空,则认为其读全部字段
				pb_field::field_numbers<T>(lreadfield);
			}
			for (i32_fieldnumber field : awritefield)
			{
				add_field(atype, field, epb_field_write);
			}
			for (i32_fieldnumber field : (lreadfield.empty() ? areadfield : lreadfield))
			{
				add_field(atype, field, epb_field_read);
			}
		}

		// # 根据类型获取
		std::map<i32_fieldnumber, epb_field>* get_field(i16_actortype atype);

		// # 根据字段类型进行数据拷贝
		template <typename T>
		bool field_copy(i16_actortype atypesource, i16_actortype atypetarget, const T& asource, T& atarget, bool amessage)
		{
			std::map<i32_fieldnumber, epb_field>* lpsource = get_field(atypesource);
			std::map<i32_fieldnumber, epb_field>* lptarget = get_field(atypetarget);
			if (lpsource == nullptr || lptarget == nullptr)
			{
				return false;
			}
			pb_field::copy(asource, &atarget, *lpsource, *lptarget, amessage);
			return true;
		}

		// # 根据字段类型进行数据拷贝
		template <typename T>
		bool field_copy(i16_actortype atype, const T& asource, T& atarget, bool amessage)
		{
			std::map<i32_fieldnumber, epb_field>* lp = get_field(atype);
			if (lp == nullptr)
			{
				return false;
			}
			pb_field::copy(asource, &atarget, *lp, amessage);
			return true;
		}

		// # 获取所有区服的数据字段
		std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>>& field_numbers();
	};

	// # 关注哪些数据
	class care_data
	{
		nsp_care m_core;
	public:
		// # "全部读,全部写" 构造
		void init(bool aread);

		// # "部分读" 构造
		void init(const std::set<i64_actorid>& aids);

		// # "部分读,部分写" 构造
		void init(const std::set<i64_actorid>& areadids, const std::set<i64_actorid>& awriteids);

		// # "全部读,部分写" 构造
		void init(bool aread, const std::set<i64_actorid>& awriteids);

		// # 使用nsp_care数据初始化
		void init(const nsp_care& acore);

		// # 是否关心
		bool is_care(i64_actorid adataid)const;

		// # 是否可读
		bool is_read(i64_actorid adataid)const;

		// # 是否可写(既然可写 当然可读)
		bool is_write(i64_actorid adataid)const;

		// # 是否可读全部数据
		bool is_readall()const;

		// # 是否可写全部数据
		bool is_writeall()const;

		// # 可读列表
		std::set<i64_actorid>& readids();

		// # 可写列表
		std::set<i64_actorid>& writeids();

		// # 获取关注数据
		const nsp_care& get_core()const;
	};

	template <typename T>
	class nsp_instance
	{
	private:
		static std::map<i64_actorid, std::shared_ptr<T>>	m_instance;
		static std::shared_mutex							m_mutex;
	public:
		static T* nclient(i64_actorid aactorid, bool adump)
		{
			lock_read(m_mutex);
			auto lpinstance = tools::findmap(m_instance, aactorid);
			if (lpinstance == nullptr)
			{
				if (adump)
				{
					tools::no_core_dump();
				}
				return nullptr;
			}
			return lpinstance->get();
		}

		static T* init(i64_actorid aactorid, std::shared_ptr<T>& athis)
		{
			lock_write(m_mutex);
			if (m_instance.contains(aactorid))
			{
				tools::no_core_dump();
			}
			m_instance[aactorid] = athis;
			return athis.get();
		}

		static void exit(i64_actorid aactorid)
		{
			lock_write(m_mutex);
			m_instance.erase(aactorid);
		}

		template <typename TDerived, typename ...TMESSAGES>
		static void register_handle()
		{
			(actor::register_actor_s<TDerived, TMESSAGES>(e_ready_db, [](TDerived* aacotor, const message<TMESSAGES>& adata)
				{
					T* lpclass = nclient(aacotor->id_guid(), true);
					if (lpclass != nullptr)
					{
						lpclass->handle(aacotor, adata);
					}
				} 
			), ...);
		}
	};

	// nsp client 注册回调
	template <typename T>
	class nsp_callback
	{
		template <typename TDATA>
		struct tcallback
		{
			// [回调] 当数据发生变化
			std::function<void(int64_t, const TDATA&, bool)>				m_changedatafun = nullptr;
			// [回调] 当数据被删除
			std::function<void(int64_t)>									m_deldatafun = nullptr;
			// [回调] 数据加载完成
			std::function<void()>											m_loadfinishfun = nullptr;
		};
		tcallback<T> m_call;
	public:
		// # 设置数据被修改或首次获取数据的回调
		inline void set_changedatafun(const std::function<void(int64_t, const T&, bool)>& afun)
		{
			m_call.m_changedatafun = afun;
		}

		// # 设置数据被删除的回调
		inline void set_deldatafun(const std::function<void(int64_t)>& afun)
		{
			m_call.m_deldatafun = afun;
		}

		// # 设置数据加载完成的回调
		inline void set_loadfinishfun(const std::function<void()>& afun)
		{
			m_call.m_loadfinishfun = afun;
		}

		// # 调用数据被修改或首次获取数据的回调
		inline void changedatafun(int64_t aid, const T& adata, bool afrist)
		{
			if (m_call.m_changedatafun != nullptr)
			{
				m_call.m_changedatafun(aid, adata, afrist);
			}
		}

		// # 调用数据被删除的回调
		inline void deldatafun(int64_t aid)
		{
			if (m_call.m_deldatafun != nullptr)
			{
				m_call.m_deldatafun(aid);
			}
		}

		// # 调用数据加载完成的回调
		inline void loadfinishfun()
		{
			if (m_call.m_loadfinishfun != nullptr)
			{
				m_call.m_loadfinishfun();
			}
		}
	};

	template <typename TDerived>
	class nsp_handle_print
	{
	public:
		template <typename TRECV>
		static void print(const char* aname, const TDerived* aactor, const TRECV* arecv)
		{
			std::string lcustomstr;
			tools::custom2json(*arecv, lcustomstr);
			log_error()->print("{}::handle<{}>( actor({}) : {} )", aname, tools::type_name<TRECV>(), nguid(aactor->id_guid()), lcustomstr);
		}

		template <typename TACTOR, typename TX>
		static void msg_info(TX& adata)
		{
			adata.m_msg = std::format("{}:{}:{}", tools::type_name<TDerived>(), tools::type_name<TACTOR>(), tools::type_name<typename TX::T>());
		}
	};
}//namespace ngl

namespace ngl
{
	template <typename T>
	std::map<i64_actorid, std::shared_ptr<T>> nsp_instance<T>::m_instance;

	template <typename T>
	std::shared_mutex nsp_instance<T>::m_mutex;
}//namespace ngl