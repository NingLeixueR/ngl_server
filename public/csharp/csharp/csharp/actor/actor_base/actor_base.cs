using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ngl
{
    class actorparmbase
    {
        ENUM_ACTOR m_type       = actor_guid.none_type();               // actor类型
        Int16 m_area            = -1;//tab_self_area;                        // 区服
        Int32 m_id              = actor_guid.none_actordataid();        // 数据id
        bool m_manage_dbclient  = false;                                // 是否有数据库依赖
    };
    internal class actor_base
    {
    }
}
