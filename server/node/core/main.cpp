#include "actor_robot_manage.h"
#include "operator_file.h"
#include "template_arg.h"
#include "server_main.h"
#include "manage_curl.h"
#include "nactortype.h"
#include "tprotocol.h"
#include "asio_kcp.h"
#include "dumper.h"
#include "tools.h"
#include "ukcp.h"
#include "nlog.h"
#include "slist.h"
#include "tools.h"


#include "lua.hpp"

Dumper lDumper;

class script_base
{
public:

};


class script_lua
{
	lua_State* m_l = nullptr;
	std::string m_scriptpath;//�ű��ļ�����
public:
	void setupluapaths() 
	{
		lua_getglobal(m_l, "package");
		lua_getfield(m_l, -1, "path");
		std::string lpath = lua_tostring(m_l, -1);

		// ����Զ���·��������ԭ��·����
		lpath += ";./script/lua/?.lua"; 

		lua_pop(m_l, 1);
		lua_pushstring(m_l, lpath.c_str());
		lua_setfield(m_l, -2, "path");
		lua_pop(m_l, 1);
	}

	void init(const std::string& ascript)
	{
		m_l = luaL_newstate();
		luaL_openlibs(m_l);  // �򿪱�׼��
		setupluapaths();
		if (luaL_loadfile(m_l, "./script/lua/rfunction.lua") || lua_pcall(m_l, 0, 0, 0))
		{
			std::cout << std::format("can't run [{}] : {} !", m_scriptpath, lua_tostring(m_l, -1)) << std::endl;
			::ngl::log_error()->print("can't run [{}] : {} !", m_scriptpath, lua_tostring(m_l, -1));
			lua_close(m_l);
			m_l = nullptr;
			return;
		}
		m_scriptpath = std::format("./script/lua/{}", ascript);
		if (luaL_loadfile(m_l, m_scriptpath.c_str()) || lua_pcall(m_l, 0, 0, 0))
		{
			std::cout << std::format("can't run [{}] : {} !", m_scriptpath, lua_tostring(m_l, -1)) << std::endl;
			::ngl::log_error()->print("can't run [{}] : {} !", m_scriptpath, lua_tostring(m_l, -1));
			lua_close(m_l);
			m_l = nullptr;
			return;
		}
	}

	// # ��db����ѹ��
	bool push_data(const std::string& adbname, const std::string& adatajson)
	{
		if (m_l == nullptr)
		{
			return false;
		}
		int result = lua_getglobal(m_l, "push_data");
		if (result == LUA_TNIL)
		{
			std::cout << std::format("lua_getglobal get failure[{}.push_data]", m_scriptpath) << std::endl;
			::ngl::log_error()->print("lua_getglobal get failure[{}.push_data]", m_scriptpath);
			lua_pop(m_l, 1); // ����nilֵ
			return false;
		}
		lua_pushstring(m_l, adbname.c_str());
		lua_pushstring(m_l, adatajson.c_str());
		if (lua_pcall(m_l, 2, 0, 0) != LUA_OK)
		{
			std::cout << std::format("{}.push_data error [{}]", m_scriptpath, lua_tostring(m_l, -1)) << std::endl;
			::ngl::log_error()->print("{}.push_data error [{}]", m_scriptpath, lua_tostring(m_l, -1));
			lua_pop(m_l, 1); // ����������Ϣ
			return false;
		}
		return true;
	}

	// # ��Ϣ����
	bool handle(std::string& ajson)
	{
		if (m_l == nullptr)
		{
			return false;
		}
		int result = lua_getglobal(m_l, "handle");
		if (result == LUA_TNIL) 
		{
			std::cout << std::format("lua_getglobal get failure[{}.handle]", m_scriptpath) << std::endl;
			::ngl::log_error()->print("lua_getglobal get failure[{}.handle]", m_scriptpath);
			lua_pop(m_l, 1); // ����nilֵ
			return false;
		}
		lua_pushstring(m_l, ajson.c_str());
		if (lua_pcall(m_l, 1, 0, 0) != LUA_OK) 
		{
			std::cout << std::format("{}.handle error [{}]", m_scriptpath, lua_tostring(m_l, -1)) << std::endl;
			::ngl::log_error()->print("{}.handle error [{}]", m_scriptpath, lua_tostring(m_l, -1));
			lua_pop(m_l, 1); // ����������Ϣ
			return false;
		}

		return true;
	}

	// # ��ѯ�����Ƿ��޸�#������޸ľͼ�������
	bool check_outdata(const std::string& adbname, std::string& adatajson)
	{
		if (m_l == nullptr)
		{
			return false;
		}
		int result = lua_getglobal(m_l, "check_outdata");
		if (result == LUA_TNIL)
		{
			std::cout << std::format("lua_getglobal get failure[{}.check_outdata]", m_scriptpath) << std::endl;
			::ngl::log_error()->print("lua_getglobal get failure[{}.check_outdata]", m_scriptpath);
			lua_pop(m_l, 1);
			return false;
		}
		lua_pushstring(m_l, adbname.c_str());
		if (lua_pcall(m_l, 1, 0, 0) != LUA_OK)
		{
			std::cout << std::format("{}.check_outdata error [{}]", m_scriptpath, lua_tostring(m_l, -1)) << std::endl;
			::ngl::log_error()->print("{}.check_outdata error [{}]", m_scriptpath, lua_tostring(m_l, -1));
			lua_pop(m_l, 1);
			return false;
		}
		lua_pushstring(m_l, adbname.c_str());
		if (lua_pcall(m_l, 1, 2, 0) != LUA_OK)
		{
			std::cout << std::format("{}.check_outdata error [{}]", m_scriptpath, lua_tostring(m_l, -1)) << std::endl;
			::ngl::log_error()->print("{}.check_outdata error [{}]", m_scriptpath, lua_tostring(m_l, -1));
			lua_pop(m_l, 1);
			return false;
		}

		if (lua_gettop(m_l) < 2) 
		{
			std::cout << std::format("{}.check_outdata return count error", m_scriptpath) << std::endl;
			::ngl::log_error()->print("{}.check_outdata return count error", m_scriptpath);
			lua_settop(m_l, -lua_gettop(m_l));
			return false;
		}

		if (!lua_isboolean(m_l, -2))
		{
			std::cout << std::format("{}.check_outdata return error isboolean", m_scriptpath) << std::endl;
			::ngl::log_error()->print("{}.check_outdata return error isboolean", m_scriptpath);
			lua_settop(m_l, -lua_gettop(m_l));
			return false;
		}

		if (!lua_isstring(m_l, -1))
		{
			std::cout << std::format("{}.check_outdata return error isstring", m_scriptpath) << std::endl;
			::ngl::log_error()->print("{}.check_outdata return error isstring", m_scriptpath);
			lua_pop(m_l, 1);
			return false;
		}

		bool success = lua_toboolean(m_l, -2) != 0;
		if (success)
		{
			adatajson = lua_tostring(m_l, -1);
		}
		lua_pop(m_l, 2);

		return success;
	}
};


int main(int argc, char** argv)
{
	script_lua llua;
	llua.init("test.lua");

	pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE pro;
	pro.set_mgold(1000);
	pro.set_morderid("kcf");
	pro.set_mrechargeid(1234);
	(*pro.mutable_mitems())[123] = 456;
	(*pro.mutable_mitems())[456] = 789;
	std::string lstr;
	ngl::tools::proto2json(pro, lstr);

	llua.push_data("PROBUFF_NET_DELIVER_GOODS_RECHARGE", lstr);
	llua.handle(lstr);





	lua_State* L = luaL_newstate();
	luaL_openlibs(L);  // �򿪱�׼��
	luaL_dostring(L, "print('Hello from Lua!')");

	
	// luaд��json

	lua_pushstring(L, lstr.c_str());
	lua_setglobal(L, "json_str");

	// ִ��Lua�������cjson
	int status = luaL_dostring(L, R"(
		local cjson = require("cjson")
		
		print(json_str)
		local decodeData = cjson.decode(json_str)
		for k,v in pairs(decodeData) do
			print(type(v))
			if type(v) == "table" then
				for k1,v1 in pairs(v) do
					print(k1, v1)
				end
			else
				print(k, v)
			end
		end

		
    )");

	if (status != LUA_OK) {
		std::cerr << "����: " << lua_tostring(L, -1) << std::endl;
	}

	lua_close(L);
	if (argc <= 3)
	{
		std::cout << "��������:EXE name areaid tab_servers::tcount" << std::endl;
		return 0;
	}

	// # ����
	std::string lname = argv[1];

	// # ����id
	int32_t larea = ngl::tools::lexical_cast<int32_t>(argv[2]);

	// # ����id�¹��ܽ��̵����
	int32_t ltcount = ngl::tools::lexical_cast<int32_t>(argv[3]);

	// # ��ʼ������ö��NODE_TYPE���ַ���
	nconfig::init();

	nconfig::set_server(argv[1]);

	// # ����xml����
	nconfig::load("./config", std::format("{}_{}", lname, ltcount));

	// # ����csv����
	ngl::csvbase::set_path("./csv", lname);

	const ngl::tab_servers* tab = ngl::ttab_servers::instance().tab(argv[1], larea, ltcount);
	if (tab == nullptr)
	{
		return 0;
	}

	nconfig::set_nodeid(tab->m_id);

	std::string lnodename;
	if (larea < 0)
	{
		lnodename = std::format("node_{}__{}_{}", lname, -larea, ltcount);
	}
	else
	{
		lnodename = std::format("node_{}_{}_{}", lname, tab->m_area, ltcount);
	}

#ifdef WIN32
	// # ���ÿ���̨��������
	SetConsoleTitle(lnodename.c_str());
#endif

	Dumper::m_excname = lnodename;
	switch (nconfig::node_type())
	{
	case ngl::DB:
		start_db(argc, argv);
		break;
	case ngl::GAME:
		start_game();
		break;
	case ngl::ACTORSERVER:
		start_actor();
		break;
	case ngl::LOG:
		start_log();
		break;
	case ngl::GATEWAY:
		start_gateway();
		break;
	case ngl::LOGIN:
		start_login();
		break;
	case ngl::WORLD:
		start_world();
		break;
	case ngl::RELOADCSV:
		start_csvserver();
		break;
	case ngl::ROBOT:
		start_robot(argc, argv);
		break;
	case ngl::CROSS:
		start_cross();
		break;
	case ngl::CROSSDB:
		start_crossdb();
		break;
	case ngl::PUSHSERVERCONFIG:
		start_pushserverconfig();
		break;
	default:
		return 0;
	}

	while (1)
	{
		ngl::sleep::seconds(1);
	}
	return 0;
}