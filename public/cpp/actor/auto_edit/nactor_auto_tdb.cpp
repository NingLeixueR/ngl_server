 // 注意【makeproto 工具生成文件，不要手动修改】
 // 创建时间【2025-07-02 20:24:31】
#include "auto_actor_enum.h"
#include "nprotocol_auto.h"
#include "nactor_auto.h"

namespace ngl
{
	void tdb::tdb_init(bool ainstance)
	{
		tdb_account::init(ainstance);
		tdb_activity::init(ainstance);
		tdb_activitytimes::init(ainstance);
		tdb_bag::init(ainstance);
		tdb_brief::init(ainstance);
		tdb_family::init(ainstance);
		tdb_familyer::init(ainstance);
		tdb_friends::init(ainstance);
		tdb_keyvalue::init(ainstance);
		tdb_mail::init(ainstance);
		tdb_notice::init(ainstance);
		tdb_ranklist::init(ainstance);
		tdb_role::init(ainstance);
		tdb_rolekv::init(ainstance);
		tdb_task::init(ainstance);
    }
}//namespace ngl