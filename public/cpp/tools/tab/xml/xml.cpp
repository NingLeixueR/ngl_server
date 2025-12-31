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
#include "ttab_servers.h"
#include "nprotocol.h"
#include "tinyxml2.h"
#include "csvtable.h"
#include "xmlinfo.h"
#include "tools.h"
#include "type.h"
#include "nlog.h"
#include "xml.h"

#include "skill.h"

namespace ngl
{
	void xmlnode::init()
	{
		em<NODE_TYPE>::set(DB, "db");
		em<NODE_TYPE>::set(ACTORSERVER, "actor");
		em<NODE_TYPE>::set(GAME, "game");
		em<NODE_TYPE>::set(GATEWAY, "gateway");
		em<NODE_TYPE>::set(LOGIN, "login");
		em<NODE_TYPE>::set(ROBOT, "robot");
		em<NODE_TYPE>::set(WORLD, "world");
		em<NODE_TYPE>::set(LOG, "log");
		em<NODE_TYPE>::set(RELOADCSV, "reloadcsv");
		em<NODE_TYPE>::set(CROSS, "cross");
		em<NODE_TYPE>::set(PUSHSERVERCONFIG, "pushserverconfig");
		em<NODE_TYPE>::set(CROSSDB, "crossdb");
	}

	i16_area xmlnode::area()
	{
		auto ltab = ttab_servers::instance().tab(m_tid);
		if (ltab == nullptr)
		{
			tools::no_core_dump();
			return -1;
		}
		return ltab->m_area;
	}

	void xmlnode::set_server(const char* aservertypename)
	{
		NODE_TYPE lnodetype = em<NODE_TYPE>::get_enum(aservertypename);
		if (lnodetype == em<NODE_TYPE>::enum_null())
		{
			tools::no_core_dump();
		}
		m_nodename = aservertypename;
		m_nodetype = lnodetype;
	}

	void xmlnode::set_nodeid(int atid, int atcount)
	{
		m_nodeid = nnodeid::nodeid(atid, atcount);
		m_tid = nnodeid::tid(m_nodeid);
		m_tcount = nnodeid::tcount(m_nodeid);
	}

	void xmlnode::load(const std::string& axmlpath, const std::string& aname)
	{
		std::string lxmlname = std::format("{}/config/config_{}.xml", axmlpath, aname);
		if (tools::file_exists(lxmlname) == false)
		{
			lxmlname = std::format("{}/config/config.xml", axmlpath);
		}

		log_error()->print("begin xmlnode read [{}]", lxmlname);

		xml_pop(lxmlname.c_str());
		log_error()->print("finish xmlnode read [{}]", lxmlname);
	}

	xarg_info* xmlnode::publicconfig()
	{
		return &m_public;
	}

	int16_t xmlnode::tcount()
	{
		return m_tcount;
	}

	int16_t	xmlnode::tid()
	{
		return m_tid;
	}

	i32_serverid xmlnode::nodeid()
	{
		return m_nodeid;
	}

	NODE_TYPE xmlnode::nodetype()
	{
		return m_nodetype;
	}

	xarg_db& xmlnode::db()
	{
		return m_db;
	}

	xarg_db& xmlnode::crossdb()
	{
		return m_crossdb;
	}

	xarg_mail& xmlnode::mail()
	{
		return m_mail;
	}

	xarg_telnet& xmlnode::telnet()
	{
		return m_telnet;
	}

	void testxml()
	{
		ngl::xmlnode& ltempxml = ngl::xmlnode::instance();
		auto& lmap = ltempxml.publicconfig()->data();
		lmap["varint"] = "false";
		lmap["isxor"] = "true";
		lmap["sessionewait"] = "1";
		lmap["robot_test"] = "true";
		lmap["xor_str"] = "国际巅峰调酒大师、品酒大师，有酒神之称的李解冻，为了品尝出土的汉代美酒而醉死于而立之年。当他一觉醒来时，却发现自己来到了一个名叫五行大陆的世界，变成了一个叫做姬动的小乞丐。在这里，有一种极其特殊的职业，名叫阴阳魔师，奇异的十系魔技令人叹为观止。每一名阴阳魔师都有一顶属于自己的由本属性凝聚而成的阴阳冕。一次偶然的机遇，让他认识了地底世界的火焰女王烈焰，作为阴阳平衡体质的他，接受了阴阳极焰的传承，他的命运也从此走向了强者的道路。";
		lmap["gmurl"] = "http://127.0.0.1:800";
		lmap["push_server_config"] = "push_server_config.php";
		lmap["head_version"] = "1";
		lmap["kcpping"] = "10";
		lmap["kcppinginterval"] = "20";
		lmap["kcpsession"] = "abcdefghigklmn";
		lmap["log_flushtime"] = "10000000";
		lmap["logline"] = "10000";
		lmap["logwritelevel"] = "3";
		lmap["logconsolelevel"] = "3";
		lmap["consumings"] = "100";
		lmap["dbprotobinary"] = "true";
		lmap["lua"] = "../configure/script/lua/";
		lmap["version"] = "0";
		lmap["gateway_count"] = "2";
		lmap["game_count"] = "2";

		ngl::xmlnode::instance().db().m_account = "root";
		ngl::xmlnode::instance().db().m_passworld = "123456";
		ngl::xmlnode::instance().db().m_dbname = "lbtest";
		ngl::xmlnode::instance().db().m_ip = "127.0.0.1";
		ngl::xmlnode::instance().db().m_port = 3306;

		ngl::xmlnode::instance().crossdb() = ngl::xmlnode::instance().db();
		ngl::xmlnode::instance().crossdb().m_dbname = "lbtestcross";

		ngl::xmlnode::instance().mail().m_email = "15101192850@163.com";
		ngl::xmlnode::instance().mail().m_smtp = "smtp.163.com:25";
		ngl::xmlnode::instance().mail().m_password = "NChaNkmFgvJbzQ2i";
		ngl::xmlnode::instance().mail().m_name = "[libo1@youxigu.com:李博][348634371@qq.com:李博QQ]";

		ngl::xmlnode::instance().telnet().m_account = "libo";
		ngl::xmlnode::instance().telnet().m_password = "e10adc3949ba59abbe56e057f20f883e";
		if (!ngl::xmlnode::instance().xml_push("config.xml"))
		{
			std::cout << "error" << std::endl;
		}


		ngl::skill_template lskill;
		lskill.m_name = "技能名称";
		lskill.m_describe = "技能描述";
		lskill.m_maxlv = 3;
		lskill.m_cooldown[1] = 3000;
		lskill.m_cooldown[2] = 2000;
		lskill.m_cooldown[3] = 1000;

		lskill.m_consumeattr[1].m_data[ngl::E_Attack] = 100;
		lskill.m_consumeattr[2].m_data[ngl::E_Defense] = 200;
		lskill.m_consumeattr[3].m_data[ngl::E_Hp] = 300;

		lskill.m_consumeitem[1].m_data[1] = 3;
		lskill.m_consumeitem[2].m_data[2] = 4;
		lskill.m_consumeitem[3].m_data[3] = 5;

		lskill.m_setp.push_back(
			ngl::skill_template::setp
			{
				.m_lvs =
				{
					{ 1, 
						ngl::skill_template::setp::lv
						{
							.m_release = 1000,
							.m_buffids = 
							{
								ngl::skill_template::setp::lv::buff
								{
									.m_id = 1,
									.m_lv = 1,
								},
								ngl::skill_template::setp::lv::buff
								{
									.m_id = 2,
									.m_lv = 1,
								},
								ngl::skill_template::setp::lv::buff
								{
									.m_id = 3,
									.m_lv = 1,
								},
							}
						} 
					},
					{ 2,
						ngl::skill_template::setp::lv
						{
							.m_release = 900,
							.m_buffids =
							{
								ngl::skill_template::setp::lv::buff
								{
									.m_id = 1,
									.m_lv = 2,
								},
								ngl::skill_template::setp::lv::buff
								{
									.m_id = 2,
									.m_lv = 2,
								},
								ngl::skill_template::setp::lv::buff
								{
									.m_id = 3,
									.m_lv = 2,
								},
							}
						}
					},
					{ 3,
						ngl::skill_template::setp::lv
						{
							.m_release = 800,
							.m_buffids =
							{
								ngl::skill_template::setp::lv::buff
								{
									.m_id = 1,
									.m_lv = 3,
								},
								ngl::skill_template::setp::lv::buff
								{
									.m_id = 2,
									.m_lv = 3,
								},
								ngl::skill_template::setp::lv::buff
								{
									.m_id = 3,
									.m_lv = 3,
								},
							}
						}
					},
				}
			}
		);

		if (!lskill.xml_push("skill_template.xml"))
		{
			std::cout << "error" << std::endl;
		}
	}
}// namespace ngl