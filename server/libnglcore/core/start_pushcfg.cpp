#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/auto/nactor_auto.h"
#include "tools/tab/xml/xml.h"
#include "init_dbdata.h"
#include "start_node.h"
#include "start_db.h"

struct json_net
{
	std::string m_ip;
	std::string m_nip;
	uint16_t m_port;

	DPROTOCOL(json_net, m_ip, m_nip, m_port)
};

bool build_push_cfg(const ngl::tab_servers& asrv, std::string& aparam)
{
	aparam.clear();
	const std::string lname = ngl::tools::url_encode(asrv.m_name);

	// The endpoint expects the scalar fields as flat query params.
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

		json_net lnet_val;
		lnet_val.m_ip = lnet.m_ip;
		lnet_val.m_nip = lnet.m_nip;
		lnet_val.m_port = lnet.m_port;
		// The network list is encoded as a small JSON object and then URL-escaped.
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

startup_error start_pushcfg([[maybe_unused]] int* atcp_port)
{
	// This process only pushes tab_servers data to the external GM service.
	ngl::xarg_info* linfo = nconfig.info();
	if (linfo == nullptr)
	{
		return startup_error::node_start_failed;
	}

	std::string lgm_url;
	if (!linfo->find("gmurl", lgm_url))
	{
		return startup_error::node_start_failed;
	}

	std::string lapi;
	if (!linfo->find("push_server_config", lapi))
	{
		return startup_error::node_start_failed;
	}

	std::string lurl = std::format("{}/{}", lgm_url, lapi);
	bool lbad_net = false;

	// Push every configured server entry independently so one bad row does not block logging
	// for the rest of the batch.
	ngl::ttab_servers::instance().foreach_server([&lurl, &lbad_net](ngl::tab_servers* aserver)
		{
			auto lhttp = ngl::ncurl::http();
			ngl::ncurl::set_mode(lhttp, ngl::ENUM_MODE_HTTP);
			ngl::ncurl::set_type(lhttp, ngl::ENUM_TYPE_GET);
			ngl::ncurl::set_url(lhttp, lurl);

			std::string lparam;
			if (!build_push_cfg(*aserver, lparam))
			{
				lbad_net = true;
				ngl::log_error()->print(
					"[pushserverconfig] invalid network type server:{} type:{}",
					aserver->m_id,
					static_cast<int32_t>(aserver->m_type));
				return;
			}

			ngl::ncurl::set_param(lhttp, lparam);

			ngl::ncurl::set_callback(lhttp, [lsend = std::move(lparam)](int, ngl::http_parm& ahttp)
				{
					ngl::log_error()->print("[{}]->[{}]", lsend, ahttp.m_recvdata);
				});
			ngl::ncurl::send(lhttp);
		});
	return lbad_net ? startup_error::node_start_failed : startup_error::ok;
}
