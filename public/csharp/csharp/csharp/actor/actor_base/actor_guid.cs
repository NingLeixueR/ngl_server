using System;
using System.IO;

namespace ngl
{
    enum ENUM_ACTOR
    {
        ACTOR_NONE = -1,
        //#######################  非单例  #####################
        ACTOR_ROLE = 1,
        ACTOR_ROBOT,                                        // 机器人
        ACTOR_GUILD,                                        // 工会	
        ACTOR_PLAYS,                                        // 玩法
        ACTOR_PLAYS_GO_UNDERGROUNDPALACE = ACTOR_PLAYS,     // 玩法[勇闯地宫]

        //ACTOR_PLAYS_END = ACTOR_PLAYS + pbnet::eplays_count,
        //#######################  单例  #######################
        ACTOR_SIGNLE_START,
        ACTOR_ADDRESS_SERVER = ACTOR_SIGNLE_START,              // 单例 actor address server  管理分发地址的中心
        ACTOR_ADDRESS_CLIENT,                                   // 单例 actor address client 
        ACTOR_LOGIN,
        ACTOR_GATEWAY,                                          // 网关
        ACTOR_GATEWAY_CLIENT2GAME,                              // 网关 [Client]->[Game]
        ACTOR_GATEWAY_GAME2CLIENT,                              // 网关 [Game]	->[Client]
        ACTOR_CREATE,                                           // 在指定[Server]上创建[Actor]
        ACTOR_LOG,
        ACTOR_MANAGE_ROLE,
        ACTOR_MANAGE_ROBOT,
        ACTOR_MANAGE_GUILD,                                     // 工会管理器
        ACTOR_RELOADCSV,                                        // 接收分发然后重载csv
        ACTOR_RELOADCSV_DISTRIBUTE,                             // 分发csv
        ACTOR_NOTICE,                                           // 公告
        ACTOR_GM,                                               // GM
        ACTOR_MAIL,                                             // 邮件
        ACTOR_CHAT,                                             // 聊天
        ACTOR_RANKLIST,                                         // 排行榜
        ACTOR_ACTIVITY_MANAGE,                                  // 活动管理
        ACTOR_BRIEF,                                            // brief简要信息
        ACTOR_CROSS,                                            // 跨服
        ACTOR_CROSSDB,                                          // 跨服数据库
        ACTOR_CROSSCHAT,                                        // 跨服聊天
        ACTOR_MANAGE_PLAYS,                                     // 创建玩法
        ACTOR_MATCHING,                                         // 匹配
        ACTOR_DB,                                               // 必须是最后一个
        ACTOR_SIGNLE_FINISH = ACTOR_DB + Pbdb.ENUM_DB.Count,
        ACTOR_COUNT = ACTOR_SIGNLE_FINISH,
    }

    // actor唯一id(64bit)
    // 16位			类型ENUM_ACTOR
    // 16位			区服id	
    // 32位			数据id
    // ############ 64 bit ######### //
    // #actor_type###areaid###id#### //
    // #16bit########16bit####32bit# //
    class actor_guid
    {
        private Byte[] m_data   = new Byte[sizeof(Int64)];
              
        public static Int16 none_area()
        {
            return -1;
        }

        public static ENUM_ACTOR none_type()
        {
            return (ENUM_ACTOR)(-1);
        }

        public static Int32 none_actordataid()
        {
            return -1;
        }

        public actor_guid()
        {
        }

        public actor_guid(Int64 aid)
        {
            Buffer.BlockCopy(BitConverter.GetBytes(aid), 0, m_data, 0, sizeof(Int64));
        }

        public actor_guid(ENUM_ACTOR atype, Int16 aareaid, Int32 aid)
        {
            Int16 ltype = (Int16)atype;
            Buffer.BlockCopy(BitConverter.GetBytes(ltype) , 0, m_data, 0, sizeof(Int16));
            Buffer.BlockCopy(BitConverter.GetBytes(aareaid), 0, m_data, sizeof(Int16), sizeof(Int16));
            Buffer.BlockCopy(BitConverter.GetBytes(aid), 0, m_data, sizeof(Int32), sizeof(Int32));
        }

        // 获取guidid值
        public Int64 id()
        {
            return BitConverter.ToInt64(m_data, 0);
        }

        // 获取ENUM_ACTOR类型
        public ENUM_ACTOR type()
        {
            return (ENUM_ACTOR)BitConverter.ToInt16(m_data, 0);
        }

        // 获取数据id
        public Int32 actordataid()
        {
            return BitConverter.ToInt32(m_data, sizeof(Int32));
        }

        // 获取区服
        public Int16 area()
        {
            return BitConverter.ToInt16(m_data, sizeof(Int16));
        }

        // 根据类型、区服、数据id创建一个i64_actorid
        public static Int64 make(ENUM_ACTOR atype, Int16 aareaid, Int32 aid)
        {
            actor_guid lguid = new actor_guid(atype, aareaid, aid);
            return lguid.id();
        }

        // 根据类型、数据id、区服为none_area()创建一个i64_actorid
        public static Int64 make(ENUM_ACTOR atype, Int32 aid)
        {
            actor_guid lguid = new actor_guid(atype, none_area(), aid);
            return lguid.id();
        }

        // 根据类型、数据id为none_actordataid()、区服为none_area()创建一个i64_actorid
        public static Int64 make(ENUM_ACTOR atype)
        {
            actor_guid lguid = new actor_guid(atype, none_area(), none_actordataid());
            return lguid.id();
        }

        // 根据类型、区服为ttab_servers::tab()->m_area none_actordataid()、数据id为none_actordataid创建一个i64_actorid
        public static Int64 make_self(ENUM_ACTOR atype)
        {
            //actor_guid lguid(atype, ttab_servers::tab()->m_area, none_actordataid());
            // return (i64_actorid)lguid;
            return 0;
        }

        // 根据类型none_type()、区服为none_area()数据id为none_actordataid()创建一个i64_actorid
        public static Int64 make()
        {
            actor_guid lguid = new actor_guid(none_type(), none_area(), none_actordataid());
            return lguid.id();
        }

        // ### 替换type
        public static Int64 make_type(Int64 aactorid, ENUM_ACTOR atype)
        {
            actor_guid lguid1 = new actor_guid(aactorid);
            actor_guid lguid2 = new actor_guid(atype, lguid1.area(), lguid1.actordataid());
            return lguid2.id();
        }

        // ### 替换area
        public static Int64 make_area(Int64 aactorid, Int16 aareaid)
        {
            actor_guid lguid1 = new actor_guid(aactorid);
            actor_guid lguid2 = new actor_guid(lguid1.type(), aareaid, lguid1.actordataid());
            return lguid2.id();
        }

        // ### 替换actordataid
        public static Int64 make_actordataid(Int64 aactorid, Int32 aid)
        {
            actor_guid lguid1 = new actor_guid(aactorid);
            actor_guid lguid2 = new actor_guid(lguid1.type(), lguid1.area(), aid);
            return lguid2.id();
        }

        // ### 替换type
        public Int64 make_type(ENUM_ACTOR atype)
        {
            Int16 ltype = (Int16)atype;
            Buffer.BlockCopy(BitConverter.GetBytes(ltype), 0, m_data, 0, sizeof(Int16));
            return id();
        }

        // ### 替换area
        public Int64 make_area(Int16 aareaid)
        {
            Buffer.BlockCopy(BitConverter.GetBytes(aareaid), 0, m_data, sizeof(Int16), sizeof(Int16));
            return id();
        }

        // ### 替换actordataid
        public Int64 make_actordataid(Int32 aid)
        {
            Buffer.BlockCopy(BitConverter.GetBytes(aid), 0, m_data, sizeof(Int32), sizeof(Int32));
            return id();
        }

        // ### 获取actordataid
        public static Int32 actordataid(Int64 aactorid)
        {
            actor_guid lguid = new actor_guid(aactorid);
            return lguid.actordataid();
        }

        // ### 获取type
        public static ENUM_ACTOR type(Int64 aactorid)
        {
            actor_guid lguid = new actor_guid(aactorid);
            return lguid.type();
        }

        // ### 获取area
        public static Int16 area(Int64 aactorid)
        {
            actor_guid lguid = new actor_guid(aactorid);
            return lguid.area();
        }

        // sendmore 发送给同类型的所有actor
        public static Int64 moreactor(ENUM_ACTOR atype)
        {
            return make(atype, none_area(), none_actordataid());
        }

        // 和无参make()一致
        public static Int64 moreactor()
        {
            return make(none_type(), none_area(), none_actordataid());
        }

        // 和无参make()一致
        public void none()
        {
            Int16 ltype = (Int16)none_type();
            Buffer.BlockCopy(BitConverter.GetBytes(ltype), 0, m_data, 0, sizeof(Int16));
            Buffer.BlockCopy(BitConverter.GetBytes(none_area()), 0, m_data, sizeof(Int16), sizeof(Int16));
            Buffer.BlockCopy(BitConverter.GetBytes(none_actordataid()), 0, m_data, sizeof(Int32), sizeof(Int32));
        }

        public bool is_moreactor(ENUM_ACTOR atype)
        {
            actor_guid lguid = new actor_guid(atype, none_area(), none_actordataid());
            return id() == lguid.id();
        }

        // 是否发给同类型的所有actor
        public static bool is_moreactor(Int64 actorid, ENUM_ACTOR atype)
        {
            actor_guid lguid = new actor_guid(actorid);
            return lguid.is_moreactor(atype);
        }

        // actor type 是否无效
        public static bool is_actortypenone(Int64 actorid)
        {
            actor_guid lguid = new actor_guid(actorid);
            return lguid.is_actortypenone();
        }

        public bool is_actortypenone()
        {
            return type() == none_type();
        }

        //actor area 是否无效
        public static bool is_actorareanone(Int64 actorid)
        {
            actor_guid lguid = new actor_guid(actorid);
            return lguid.is_actorareanone();
        }

        public bool is_actorareanone()
        {
            return area() == none_area();
        }

        //actor id 是否无效
        public static bool is_actoridnone(Int64 actorid)
        {
            actor_guid lguid = new actor_guid(actorid);
            return lguid.is_actoridnone();
        }

        public bool is_actoridnone()
        {
            return actordataid() == none_actordataid();
        }

        public static bool is_single(ENUM_ACTOR atype)
        {
            return atype >= ENUM_ACTOR.ACTOR_SIGNLE_START && atype < ENUM_ACTOR.ACTOR_SIGNLE_FINISH;
        }
    }
}