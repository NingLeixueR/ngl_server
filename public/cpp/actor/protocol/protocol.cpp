/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for protocol.


#include "actor/protocol/protocol.h"
#include "tools/tab/xml/xmlinfo.h"
#include "tools/tools/tools_cmd.h"
#include "tools/tab/xml/xml.h"
#include "tools/curl/ncurl.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	std::map<i32_protocolnum, protocol::pfun> protocol::m_protocolfun;
	std::shared_mutex protocol::m_mutex;

	const char* protocol::name(i32_protocolnum aprotocol/* Protocol id */)
	{
		const char* lname = tools::em<eprotocol_tar>::name((eprotocol_tar)(aprotocol));
		return lname != nullptr ? lname : "none";
	}

	void protocol::print(const char* amsg, i32_protocolnum aprotocol)
	{
		log_error()->print("protocol::push msg:{} protocolnum:{} name:{}", amsg, aprotocol, name(aprotocol));
	}

	void protocol::registers(
		int aprotocol, ENUM_ACTOR aenumactor, const protocol::fun_pack& apackfun, const protocol::fun_run& arunfun, const char* aname
	)
	{
		lock_write(m_mutex);
		// A protocol id may be shared by multiple actor types, but it uses one common decoder.
		pfun& lprotocol = m_protocolfun[aprotocol];
		lprotocol.m_packfun = apackfun;
		lprotocol.m_runfun[aenumactor] = arunfun;
		tools::em<eprotocol_tar>::set((eprotocol_tar)aprotocol, aname);
	}

	protocol::pfun* protocol::find(i32_protocolnum aprotocol)
	{
		lock_read(m_mutex);
		auto lpfun = tools::findmap(m_protocolfun, aprotocol);
		if (lpfun == nullptr)
		{
			print("protocol num none", aprotocol);
			return nullptr;
		}
		return lpfun;
	}

	void protocol::push(std::shared_ptr<pack>& apack)
	{
		i32_protocolnum lprotocolnum = apack->m_head.get_protocolnumber();
		pfun* lpfun = find(lprotocolnum);
		if (lpfun == nullptr)
		{
			return;
		}
		std::shared_ptr<void> lptrpram = lpfun->m_packfun(apack);
		if (lptrpram == nullptr)
		{
			return;
		}

		auto lactortype = (ENUM_ACTOR)apack->m_head.get_actortype();
		if (lactortype != nguid::none<ENUM_ACTOR>())
		{
			// Fast path: the sender already encoded the target actor type in the pack header.
			auto lprun = tools::findmap(lpfun->m_runfun, lactortype);
			if (lprun != nullptr)
			{
				(*lprun)(apack, lptrpram);
				return;
			}
			else 
			{
				if (nconfig.nodetype() != ROBOT)
				{
					return;
				}
				// Robot traffic can fall back to fan-out dispatch when the header actor type is unset.
				apack->m_head.set_actor(nguid::make(), nguid::make());
			}
		}
		// Fallback path: run every actor-type binding registered for this protocol.
		for (std::pair<const ENUM_ACTOR, protocol::fun_run>& item : lpfun->m_runfun)
		{
			item.second(apack, lptrpram);
		}
		return;
	}

	class telnet_cmd_admin
	{
		static std::set<int>		m_adminsocket;
		static std::shared_mutex	m_mutex;
	public:
		static bool login(int asocket, const std::string_view& auser, const std::string& apassworld)
		{
			lock_write(m_mutex);
			if (auser != nconfig.telnet().m_account)
			{
				return false;
			}
			if (ngl::tools::md5(apassworld) != nconfig.telnet().m_password)
			{
				return false;
			}
			m_adminsocket.insert(asocket);
			return true;
		}
		static bool check(int asocket)
		{
			lock_read(m_mutex);
			if (m_adminsocket.contains(asocket))
			{
				return true;
			}
			return false;
		}
	};

	std::set<int> telnet_cmd_admin::m_adminsocket;
	std::shared_mutex telnet_cmd_admin::m_mutex;

	void protocol::telnet_cmd(const std::shared_ptr<pack>& apack)
	{
		std::string lcmdstr = apack->m_buff;
		tools::erase_repeat(lcmdstr, ' ');

		std::vector<std::string> lvec;
		if (!tools::splite(lcmdstr.c_str(), " ", lvec))
		{
			return;
		}
		std::string& lkey = lvec[0];
		std::ranges::transform(lkey, lkey.begin(), tolower);
		if (!telnet_cmd_admin::check(apack->m_id))
		{
			// Until login succeeds, only /login is accepted on this socket.
			if (lvec[0] == "/login" && lvec.size() >= 3)
			{
				if (telnet_cmd_admin::login(apack->m_id, lvec[1], lvec[2]))
				{
					std::string lstr = std::format("{}:登陆成功\n", lvec[1]);
					ntcp::instance().send_msg(apack->m_id, lstr);
				}
				else
				{
					std::string lstr = std::format("{}:登陆失败\n", lvec[1]);
					ntcp::instance().send_msg(apack->m_id, lstr);
				}
			}
			return;
		}
		using handle_cmd = tools::cmd<protocol, std::string, const std::shared_ptr<pack>&, const std::vector<std::string>&>;
		if (handle_cmd::empty())
		{
			// Command handlers are registered lazily on first use.
			handle_cmd::add("/actor_count") = [](const std::shared_ptr<pack>& pack, const std::vector<std::string>&)
				{
					int32_t lcount = actor_manage::instance().actor_count();
					std::string lstr = std::format("actor count:{}\n", lcount);
					ntcp::instance().send_msg(pack->m_id, lstr);
				};

			handle_cmd::add("/print_guid") = [](const std::shared_ptr<pack>& pack, const std::vector<std::string>& avec)
				{
					ENUM_ACTOR ltype = tools::em<ENUM_ACTOR>::get_enum(avec[1].c_str());
					if (ltype == tools::em<ENUM_ACTOR>::enum_null())
					{
						return;
					}

					auto lmap = naddress::get_actorserver_map();
					std::string lstr;
					for (auto [lguid, serverid] : lmap)
					{
						if (lguid.type() == ltype)
						{
							lstr += std::format("\r\n{}==={}", lguid, serverid);
						}
					}
					ntcp::instance().send_msg(pack->m_id, lstr);
				};

			handle_cmd::add("/sendmail") = [](const std::shared_ptr<pack>& pack, const std::vector<std::string>& avec)
				{
					// Maillist[mailaddress1: 1]....[mailaddressn: n] mail mailcontent
					if (avec.size() < 4)
					{
						ntcp::instance().send_msg(pack->m_id, "参数错误");
						return;
					}
					std::vector<std::pair<std::string, std::string>> lmail;
					if (tools::splite_special(avec[1].c_str(), "\\[", "]", lmail) == false)
					{
						ntcp::instance().send_msg(pack->m_id, "参数错误");
						return;
					}
					ngl::test_mail(avec[2].c_str(), avec[3].c_str(), lmail);
				};

			handle_cmd::add("/week") = [](const std::shared_ptr<pack>& pack, const std::vector<std::string>& avec)
				{
						if (avec.size() < 3)
					{
						ntcp::instance().instance().send_msg(pack->m_id, "参数错误");
						return;
					}
					time_t ltime = tools::lexical_cast<time_t>(avec[1]);
					if (tools::localtime::issameweek(tools::localtime::gettime(), ltime))
					{
						std::cout << "同一周" << std::endl;
						ntcp::instance().send_msg(pack->m_id, "同一周");
						return;
					}
					else
					{
						std::cout << "非同一周" << std::endl;
						ntcp::instance().send_msg(pack->m_id, "非同一周");
						return;
					}
				};

			// Test serverwhether
			// ping serverid
			handle_cmd::add("/ping") = [](const std::shared_ptr<pack>& pack, const std::vector<std::string>& avec)
				{
					if (avec.size() < 2)
					{
						ntcp::instance().send_msg(pack->m_id, "参数错误");
						return;
					}
						const int16_t ltid = tools::lexical_cast<int16_t>(avec[1]);
						const int16_t ltcount = tools::lexical_cast<int16_t>(avec[2]);
					i32_session lsession = server_session::sessionid(nnodeid::nodeid(ltid, ltcount));
					ntcp::instance().send_msg(lsession, "/login libo 123456");
					wheel_parm lparm
					{
						.m_ms = (int32_t)(2000),
						.m_intervalms = [](int64_t) {return 2000; },
						.m_count = 1,
							.m_fun = [lsession](const wheel_node*)
						{
							ntcp::instance().send_msg(lsession, "/each china");
						}
					};
					twheel::wheel().addtimer(lparm);
				};

			handle_cmd::add("/echo") = [](const std::shared_ptr<pack>& pack, const std::vector<std::string>& avec)
				{
					if (avec.size() < 2)
					{
						ntcp::instance().send_msg(pack->m_id, "参数错误");
						return;
					}
					std::cout << std::format("###{}###",avec[1]) << std::endl;
				};
		}
		if (handle_cmd::function(lkey, apack, lvec) == false)
		{
			log_error()->print("protocol::cmd [{}] ERROR", lkey);
		}
		return;
	}
}// namespace ngl
