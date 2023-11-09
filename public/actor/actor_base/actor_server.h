#pragma once

#include "actor_address.h"
#include "actor_manage.h"
#include "xmlnode.h"
#include "net.h"

namespace ngl
{
	class actor_server : public actor
	{
		actor_server();
	public:
		friend class actor_instance<actor_server>;
		static actor_server& getInstance() 
		{ 
			return actor_instance<actor_server>::instance(); 
		}

		virtual ~actor_server();

		static void actor_register();

		enum { ACTOR_TYPE = ACTOR_ADDRESS_SERVER};

		// ---- [actor client -> actor server] register
		// ×¢²á½áµã
		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_node_register& adata);

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_node_update& adata);

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_gateway_id_updata& adata);

	private:
	};


}
