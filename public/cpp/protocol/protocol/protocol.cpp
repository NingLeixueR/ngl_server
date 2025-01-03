#include "protocol.h"
#include "xmlinfo.h"
#include "xml.h"
#include "cmd.h"

namespace ngl
{
	enum eprotocol_tar
	{};

	class impl_protocol
	{
		struct pfun
		{
			protocol::fun_pack								m_packfun;
			std::map<ENUM_ACTOR, protocol::fun_run>			m_runfun;
		};

		static std::map<EPROTOCOL_TYPE, std::map<i32_protocolnum, pfun>>	m_protocolfun;
		static std::shared_mutex											m_mutex;

		static const char* name(EPROTOCOL_TYPE aprotocoltype, i32_protocolnum aprotocolnum)
		{
			const char* lname = em<eprotocol_tar>::get_name((eprotocol_tar)(aprotocolnum), aprotocoltype);
			return lname != nullptr ? lname : "none";
		}

		static void print(const char* amsg, EPROTOCOL_TYPE aprotocoltype, i32_protocolnum aprotocolnum)
		{
			log_error()->print("protocol::push msg:{} protocolnum:{} name:{}", amsg, aprotocolnum, name(aprotocoltype, aprotocolnum));
		}

		static pfun* find(EPROTOCOL_TYPE aprotocoltype, i32_protocolnum aprotocolnum)
		{
			lock_read(m_mutex);
			auto itor1 = m_protocolfun.find(aprotocoltype);
			if (itor1 == m_protocolfun.end())
			{
				print("protocol type none", aprotocoltype, aprotocolnum);
				return nullptr;
			}
			auto itor2 = itor1->second.find(aprotocolnum);
			if (itor2 == itor1->second.end())
			{
				print("protocol num none", aprotocoltype, aprotocolnum);
				return nullptr;
			}
			return &itor2->second;
		}
	public:
		// # 解析网络数据包[net pack],交付给上层逻辑 
		static void push(std::shared_ptr<pack>& apack)
		{
			EPROTOCOL_TYPE lprotocoltype = apack->m_head.get_protocoltype();
			i32_protocolnum lprotocolnum = apack->m_head.get_protocolnumber();
			pfun* lpfun = find(lprotocoltype, lprotocolnum);
			if(lpfun == nullptr)
			{
				return;
			}
			std::shared_ptr<void> lptrpram = lpfun->m_packfun(apack);
			if (lptrpram == nullptr)
			{
				return;
			}
			
			auto lactortype = (ENUM_ACTOR)apack->m_head.get_actortype();
			if (lactortype == nguid::none<ENUM_ACTOR>())
			{
				std::ranges::for_each(lpfun->m_runfun, [&apack,&lptrpram](const auto& item)
					{
						item.second(apack, lptrpram);
					});
			}
			else
			{
				auto itorrun = lpfun->m_runfun.find(lactortype);
				if (itorrun != lpfun->m_runfun.end())
				{
					itorrun->second(apack, lptrpram);
				}
			}
			return;
		}

		// # 注册网络协议
		static void register_protocol(
			EPROTOCOL_TYPE atype					// 协议类型
			, int aprotocolnumber					// 协议号
			, ENUM_ACTOR aenumactor					// 处理此网络协议的actor类型
			, const protocol::fun_pack& apackfun	// 解析数据包函数
			, const protocol::fun_run& arunfun		// 交付给actor_manage的函数
			, const char* aname						// 用于调试
		)
		{
			lock_write(m_mutex);
			pfun& lprotocol = m_protocolfun[atype][aprotocolnumber];
			lprotocol.m_packfun				= apackfun;
			lprotocol.m_runfun[aenumactor]	= arunfun;
			em<eprotocol_tar>::set((eprotocol_tar)aprotocolnumber, aname, atype);
		}
	};

	std::map<EPROTOCOL_TYPE, std::map<i32_protocolnum, impl_protocol::pfun>> impl_protocol::m_protocolfun;
	std::shared_mutex impl_protocol::m_mutex;

	void protocol::push(std::shared_ptr<pack>& apack)
	{
		impl_protocol::push(apack);
	}

	void protocol::register_protocol(
		EPROTOCOL_TYPE atype
		, i32_protocolnum aprotocolnumber
		, ENUM_ACTOR aenumactor
		, const protocol::fun_pack& apackfun
		, const protocol::fun_run& arunfun
		, const char* aname
	)
	{
		impl_protocol::register_protocol(atype, aprotocolnumber, aenumactor, apackfun, arunfun, aname);
	}

	void protocol::cmd(const std::shared_ptr<pack>& apack)
	{
		std::vector<std::string> lvec;
		if (tools::splite(apack->m_buff, " ", lvec) == false)
		{
			return;
		}
		using handle_cmd = ngl::cmd<protocol, std::string, const std::shared_ptr<pack>&, const std::vector<std::string>&>;
		if (handle_cmd::empty())
		{
			handle_cmd::push("/actor_count", [](const std::shared_ptr<pack>& pack, const std::vector<std::string>&)
				{
					int32_t lcount = actor_manage::getInstance().actor_count();
					std::string lstr = std::format("actor count:{}\n", lcount);
					nets::sendmsg(pack->m_id, lstr);
				}
			);
			handle_cmd::push("/print_guid", [](const std::shared_ptr<pack>& pack, const std::vector<std::string>& avec)
				{
					ENUM_ACTOR ltype;
					ltype = em<ENUM_ACTOR>::get_enum(avec[1].c_str());
					if (ltype == em<ENUM_ACTOR>::enum_null())
					{
						return;
					}

					auto lmap = naddress::get_actorserver_map();
					std::string lstr;
					for (const auto& [lguid, serverid] : lmap)
					{
						if (lguid.type() == ltype)
						{
							lstr += std::format("\r\n{}==={}", lguid, serverid);
						}
					}
					nets::sendmsg(pack->m_id, lstr);
				}
			);
		}
		std::string& lkey = lvec[0];
		std::ranges::transform(lkey, lkey.begin(), tolower);
		if (handle_cmd::function(lkey, apack, lvec) == false)
		{
			log_error()->print("protocol::cmd [{}] ERROR", lkey);
		}
		return;
	}
}// namespace ngl