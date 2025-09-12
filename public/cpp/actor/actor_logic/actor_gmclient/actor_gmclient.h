#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "manage_curl.h"
#include "ndb_modular.h"
#include "manage_csv.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "tprotocol.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "ntimer.h"
#include "notice.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	struct protocols
	{
		std::map<i32_protocolnum, std::string> m_data;

		static std::vector<const char*>& parms(const char* astr = nullptr) {
			static std::vector<const char*> tempvec; if (astr == nullptr) {
				return tempvec;
			} static std::string tempstr(astr); static std::atomic<bool> lregister = true; if (lregister.exchange(false) && !tempstr.empty()) {
				tempvec = tools::split_str(&tempstr[0], tempstr.size());
			} return tempvec;
		} inline void json_write(ngl::njson_write& ijsn, const char* akey) {
			ngl::njson_write ltemp; json_write(ltemp); ngl::njson::write(ijsn, akey, ltemp.nofree());
		} inline void json_write(ngl::njson_write& ijsn) {
			help_writejson ltemp(parms("m_data"), ijsn); ltemp.fun(0, m_data);
		} inline bool json_read(ngl::njson_read& ijsn, const char* akey) {
			ngl::njson_read ltemp; if (!ngl::njson::read(ijsn, akey, ltemp.json())) {
				return false;
			} return json_read(ltemp);
		} inline bool json_read(ngl::njson_read& ijsn) {
			help_readjson ltemp(parms("m_data"), ijsn); return ltemp.fun(0, m_data);
		} bool push_format(ngl::ser::serialize_push* aserialize)const {
			return ngl::ser::nserialize::push(aserialize, m_data);
		} bool pop_format(ngl::ser::serialize_pop* aserialize) {
			return ngl::ser::nserialize::pop(aserialize, m_data);
		} void bytes_format(ngl::ser::serialize_byte* aserialize)const {
			ngl::ser::nserialize::bytes(aserialize, m_data);
		} void nlua_push(lua_State* aL, const char* aname = nullptr)const {
			help_nlua<false> ltemp(aL, aname, parms("m_data")); ltemp.push(m_data);
		} bool nlua_pop(lua_State* aL, const char* aname = nullptr) {
			help_nlua<true> ltemp(aL, aname, parms("m_data")); return ltemp.pop(m_data);
		}
	};

	class actor_gmclient : 
		public actor
	{
		actor_gmclient(const actor_gmclient&) = delete;
		actor_gmclient& operator=(const actor_gmclient&) = delete;

		actor_gmclient();
	public:
		friend class actor_instance<actor_gmclient>;
		static actor_gmclient& instance()
		{
			return actor_instance<actor_gmclient>::instance();
		}

		virtual ~actor_gmclient() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(i16_area area);

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		static void get_allprotocol(protocols& apro);

		// # GM²Ù×÷
		using handle_cmd = cmd<actor_gmclient, std::string, int, ngl::njson_read&>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<mforward<np_gm>>& adata);
	};
}// namespace ngl
