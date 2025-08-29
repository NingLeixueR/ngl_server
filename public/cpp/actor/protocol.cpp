#include "manage_curl.h"
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

		static std::map<i32_protocolnum, impl_protocol::pfun>				m_protocolfun;
		static std::shared_mutex											m_mutex;

		static const char* name(i32_protocolnum aprotocolnum)
		{
			const char* lname = em<eprotocol_tar>::get_name((eprotocol_tar)(aprotocolnum));
			return lname != nullptr ? lname : "none";
		}

		static void print(const char* amsg, i32_protocolnum aprotocolnum)
		{
			log_error()->print(
				"protocol::push msg:{} protocolnum:{} name:{}"
				, amsg
				, aprotocolnum
				, name(aprotocolnum)
			);
		}

		static pfun* find(i32_protocolnum aprotocolnum)
		{
			lock_read(m_mutex);
			auto itor = m_protocolfun.find(aprotocolnum);
			if (itor == m_protocolfun.end())
			{
				print("protocol num none", aprotocolnum);
				return nullptr;
			}
			return &itor->second;
		}
	public:
		// # �����������ݰ�[net pack],�������ϲ��߼� 
		static void push(std::shared_ptr<pack>& apack)
		{
			i32_protocolnum lprotocolnum = apack->m_head.get_protocolnumber();
			pfun* lpfun = find(lprotocolnum);
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
				for (std::pair<const ENUM_ACTOR, protocol::fun_run>& item : lpfun->m_runfun)
				{
					item.second(apack, lptrpram);
				}
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

		// # ע������Э��
		// parm i32_protocolnum aprotocolnumber		Э���
		// parm ENUM_ACTOR aenumactor				actor����
		// parm const fun_pack& apackfun			����ص�
		// parm const fun_run& arunfun				�߼��ص�
		// parm const char* aname					debug name
		static void register_protocol(
			int aprotocolnumber
			, ENUM_ACTOR aenumactor
			, const protocol::fun_pack& apackfun
			, const protocol::fun_run& arunfun
			, const char* aname
		)
		{
			lock_write(m_mutex);
			pfun& lprotocol = m_protocolfun[aprotocolnumber];
			lprotocol.m_packfun = apackfun;
			lprotocol.m_runfun[aenumactor] = arunfun;
			em<eprotocol_tar>::set((eprotocol_tar)aprotocolnumber, aname);
		}
	};

	std::map<i32_protocolnum, impl_protocol::pfun> impl_protocol::m_protocolfun;
	std::shared_mutex impl_protocol::m_mutex;

	void protocol::push(std::shared_ptr<pack>& apack)
	{
		impl_protocol::push(apack);
	}

	void protocol::register_protocol(
		i32_protocolnum aprotocolnumber, ENUM_ACTOR aenumactor
		, const protocol::fun_pack& apackfun, const protocol::fun_run& arunfun
		, const char* aname
	)
	{
		impl_protocol::register_protocol(aprotocolnumber, aenumactor, apackfun, arunfun, aname);
	}

	class cmd_admin
	{
		static std::set<int> m_adminsocket;
		static std::shared_mutex	m_mutex;
	public:
		static bool login(int asocket, const std::string_view& auser, const std::string& apassworld)
		{
			monopoly_shared_lock(m_mutex);
			if (auser != xmlnode::m_telnet.m_telnetarg.m_account)
			{
				return false;
			}
			if (ngl::tools::md5(apassworld) != xmlnode::m_telnet.m_telnetarg.m_passworld)
			{
				return false;
			}
			m_adminsocket.insert(asocket);
			return true;
		}
		static bool check(int asocket)
		{
			monopoly_shared_lock(m_mutex);
			if (m_adminsocket.contains(asocket))
			{
				return true;
			}
			return false;
		}
	};

	std::set<int> cmd_admin::m_adminsocket;
	std::shared_mutex cmd_admin::m_mutex;

	void protocol::cmd(const std::shared_ptr<pack>& apack)
	{
		std::vector<std::string> lvec;
		if (tools::splite(apack->m_buff, " ", lvec) == false)
		{
			return;
		}
		std::string& lkey = lvec[0];
		std::ranges::transform(lkey, lkey.begin(), tolower);

		if (cmd_admin::check(apack->m_id) == false)
		{
			if (lvec[0] == "/login" && lvec.size() >= 3)
			{
				if (cmd_admin::login(apack->m_id, lvec[1], lvec[2]))
				{
					std::string lstr = std::format("{}:��½�ɹ�\n", lvec[1]);
					nets::sendmsg(apack->m_id, lstr);
				}
				else
				{
					std::string lstr = std::format("{}:��½ʧ��\n", lvec[1]);
					nets::sendmsg(apack->m_id, lstr);
				}
			}
			return;
		}
		using handle_cmd = ngl::cmd<protocol, std::string, const std::shared_ptr<pack>&, const std::vector<std::string>&>;
		if (handle_cmd::empty())
		{
			handle_cmd::add("/actor_count") = [](const std::shared_ptr<pack>& pack, const std::vector<std::string>&)
				{
					int32_t lcount = actor_manage::instance().actor_count();
					std::string lstr = std::format("actor count:{}\n", lcount);
					nets::sendmsg(pack->m_id, lstr);
				};

			handle_cmd::add("/print_guid") = [](const std::shared_ptr<pack>& pack, const std::vector<std::string>& avec)
				{
					ENUM_ACTOR ltype = em<ENUM_ACTOR>::get_enum(avec[1].c_str());
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
				};

			handle_cmd::add("/sendmail") = [](const std::shared_ptr<pack>& pack, const std::vector<std::string>& avec)
				{
					// �����ʼ��б�[�ʼ���ַ1:����1]....[�ʼ���ַn:����n] �ʼ����� �ʼ�����
					if (avec.size() < 4)
					{
						nets::sendmsg(pack->m_id, "��������");
						return;
					}
					std::vector<std::pair<std::string, std::string>> lmail;
					if (tools::splite_special(avec[1].c_str(), "\\[", "]", lmail) == false)
					{
						nets::sendmsg(pack->m_id, "��������");
						return;
					}
					ngl::test_mail(avec[2].c_str(), avec[3].c_str(), lmail);
				};

			handle_cmd::add("/week") = [](const std::shared_ptr<pack>& pack, const std::vector<std::string>& avec)
				{
					if (avec.size() < 2)
					{
						nets::sendmsg(pack->m_id, "��������");
						return;
					}
					time_t ltime = tools::lexical_cast<time_t>(avec[1]);
					if (ngl::localtime::issameweek(ngl::localtime::gettime(), ltime))
					{
						std::cout << "ͬһ��" << std::endl;
						nets::sendmsg(pack->m_id, "ͬһ��");
						return;
					}
					else
					{
						std::cout << "��ͬһ��" << std::endl;
						nets::sendmsg(pack->m_id, "��ͬһ��");
						return;
					}
				};

			// ������̨�������Ƿ���ͨ
			// ping serverid
			handle_cmd::add("/ping") = [](const std::shared_ptr<pack>& pack, const std::vector<std::string>& avec)
				{
					if (avec.size() < 2)
					{
						nets::sendmsg(pack->m_id, "��������");
						return;
					}
					i32_serverid lserverid = tools::lexical_cast<i32_serverid>(avec[1]);
					ttab_servers::instance().tab(lserverid);
					i32_session lsession = server_session::sessionid(lserverid);
					nets::sendmsg(lsession, "/login libo 123456");
					wheel_parm lparm
					{
						.m_ms = (int32_t)(2000),
						.m_intervalms = [](int64_t) {return 2000; },
						.m_count = 1,
						.m_fun = [lsession](const wheel_node* anode)
						{
							nets::sendmsg(lsession, "/each china");
						}
					};
					twheel::wheel().addtimer(lparm);
				};

			handle_cmd::add("/echo") = [](const std::shared_ptr<pack>& pack, const std::vector<std::string>& avec)
				{
					if (avec.size() < 2)
					{
						nets::sendmsg(pack->m_id, "��������");
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