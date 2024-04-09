#pragma once

#include "define_json.h"
#include "conversion.h"
#include "actor.h"

namespace ngl
{
	template <typename T>
	struct gcmd
	{
		std::string m_operator;
		T m_data;
		int id;

		~gcmd()
		{
			ngl::ijson lwrite;
			lwrite.write("operator", m_operator);
			lwrite.write("data", m_data);

			auto pro = std::make_shared<mforward<ngl::np_gm_response>>(id);
			std::string ljson;
			lwrite.get(ljson);
			ngl::conversion::to_utf8(ljson, pro->add_data()->m_json);			
			actor::static_send_actor(nguid::make_self(ACTOR_GM), -1, pro);
		}

	};
}