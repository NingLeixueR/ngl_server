// File overview: Provides lightweight startup/test helpers for ngl_test shared builds.

#include "../libnglcore/core/bootstrap/start_pushcfg.h"
#include "../libnglcore/core/bootstrap/start_world.h"
#include "../libnglcore/core/bootstrap/start_robot.h"
#include "../libnglcore/core/bootstrap/start_login.h"
#include "../libnglcore/core/bootstrap/start_log.h"
#include "../libnglcore/core/bootstrap/start_gateway.h"
#include "../libnglcore/core/bootstrap/start_game.h"
#include "../libnglcore/core/bootstrap/start_db.h"
#include "../libnglcore/core/bootstrap/start_csvserver.h"
#include "../libnglcore/core/bootstrap/start_crossdb.h"
#include "../libnglcore/core/bootstrap/start_cross.h"
#include "../libnglcore/core/bootstrap/start_actor.h"

#include "actor/tab/csvtable.h"
#include "tools/curl/ncurl.h"
#include "tools/tab/json/njson.h"

#include <array>
#include <string>

namespace
{
	struct json_net_stub
	{
		std::string m_ip;
		std::string m_nip;
		uint16_t m_port = 0;

		DPROTOCOL(json_net_stub, m_ip, m_nip, m_port)
	};
}

bool build_push_cfg(const ngl::tab_servers& asrv, std::string& aparam)
{
	aparam.clear();
	const std::string lname = ngl::tools::url_encode(asrv.m_name);

	ngl::ncurl::param(aparam, "id", asrv.m_id);
	ngl::ncurl::param(aparam, "name", lname);
	ngl::ncurl::param(aparam, "area", asrv.m_area);
	ngl::ncurl::param(aparam, "type", static_cast<int32_t>(asrv.m_type));

	constexpr std::array<const char*, ngl::ENET_COUNT> knet_names = { "tcp", "ws", "kcp" };
	ngl::ncjson lnet_json;
	for (const ngl::net_works& lnet : asrv.m_net)
	{
		const auto ltype_idx = static_cast<std::size_t>(lnet.m_type);
		if (ltype_idx >= knet_names.size())
		{
			aparam.clear();
			return false;
		}

		json_net_stub lnet_val;
		lnet_val.m_ip = lnet.m_ip;
		lnet_val.m_nip = lnet.m_nip;
		lnet_val.m_port = lnet.m_port;
		if (!ngl::njson::push(lnet_json, { knet_names[ltype_idx] }, lnet_val))
		{
			aparam.clear();
			return false;
		}
	}

	const std::string lnet_txt = lnet_json.nonformat_str();
	ngl::ncurl::param(aparam, "net", ngl::tools::url_encode(lnet_txt));
	return true;
}

startup_error start_db([[maybe_unused]] int aargc, [[maybe_unused]] char** aargv, [[maybe_unused]] int* atcp_port)
{
	return startup_error::ok;
}

startup_error start_game([[maybe_unused]] int* atcp_port)
{
	return startup_error::ok;
}

startup_error start_actor([[maybe_unused]] int* atcp_port)
{
	return startup_error::ok;
}

startup_error start_log([[maybe_unused]] int* atcp_port)
{
	return startup_error::ok;
}

startup_error start_gateway([[maybe_unused]] int* atcp_port)
{
	return startup_error::ok;
}

startup_error start_login([[maybe_unused]] int* atcp_port)
{
	return startup_error::ok;
}

startup_error start_world([[maybe_unused]] int* atcp_port)
{
	return startup_error::ok;
}

startup_error start_csvserver([[maybe_unused]] int* atcp_port)
{
	return startup_error::ok;
}

startup_error start_robot([[maybe_unused]] int aargc, [[maybe_unused]] char** aargv, [[maybe_unused]] int* atcp_port)
{
	return startup_error::ok;
}

startup_error start_cross([[maybe_unused]] int* atcp_port)
{
	return startup_error::ok;
}

startup_error start_crossdb([[maybe_unused]] int* atcp_port)
{
	return startup_error::ok;
}

startup_error start_pushcfg([[maybe_unused]] int* atcp_port)
{
	return startup_error::ok;
}
