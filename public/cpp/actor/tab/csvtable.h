// 注意【IDL 工具生成文件，不要手动修改】
// 创建时间 // 创建时间 26-03-19 15:36:56
#pragma once

#include "tools/serialize/ndefine.h"
#include "tools/serialize/nserialize.h"
#include "tools/tab/csv/csv.h"
namespace ngl{
enum ENET_CONFIG
{
	net_config_send_buff_maxbyte = 8129,	
	net_config_recv_buff_maxbyte = 8129,	
	net_config_logic_pthread_max_size = 8,	
	net_config_socket_pthread_max_size = 8,	
};
enum EUDP_OPERATOR
{
	EUDP_OPERATOR_ACK,	// Confirmpack
	EUDP_OPERATOR_DATA,	// Datapack
	EUDP_OPERATOR_HEARTBEAT,	// Heartbeat used toconfirm whetheronline
};
enum EPH_HEAD_VAL
{
	EPH_HEAD_MASK_SUCCESS = 1,	// MASKconsistent
	EPH_HEAD_MASK_FAIL = 2,	// MASK consistent
	EPH_HEAD_FOLLOW = 3,	// Packet headerdata complete
	EPH_HEAD_SUCCESS = 4,	// Packet headerdata complete
};
enum EPH
{
	EPH_MASK_VALUE = 0xff,	
	EPH_MASK_COUNT = 2,	
	EPH_MASK = 0,	// Used toconfirmwhether pack
	EPH_BYTES = EPH_MASK_COUNT,	// Protocol
	EPH_TIME,	// Send time
	EPH_PROTOCOLNUM,	// Protocol id
	EPH_ACTOR_TYPEAREA,	// ACTOR_TYPE_AREA
	EPH_ACTOR_ID,	// ACTOR_ID
	EPH_REQUEST_ACTOR_TYPEAREA,	// Request ACTOR_TYPE_AREA
	EPH_REQUEST_ACTOR_ID,	// Request ACTOR_ID
	EPH_SUM,	
};
enum NODE_TYPE
{
	DB = 1,	// Databaseserver
	ACTORSERVER = 2,	// Actor server
	GAME = 3,	// Server
	GATEWAY = 4,	// Gatewayserver
	LOGIN = 5,	// Loginserver
	ROBOT = 6,	// Test server
	WORLD = 7,	// Server
	LOG = 8,	// Logserver
	RELOADCSV = 9,	// Csvserver
	CROSS = 10,	// Cross-serverserver
	CROSSDB = 11,	// Cross-serverdatabaseserver
	PUSHSERVERCONFIG = 12,	// Serverconfigon lbgmsys
	NODE_TYPE_COUNT,	
	FAIL = -1,	// Servertype
};
enum ENET_PROTOCOL
{
	ENET_NULL = -1,	
	ENET_TCP = 0,	
	ENET_WS = 1,	
	ENET_KCP = 2,	
	ENET_COUNT = 3,	
};
enum EnumModule
{
	E_ModuleNull = 0,	// Translated comment.
	E_ModuleRoot = 1,	// Card( )
	E_ModuleEquip = 3,	// Equipment
	E_ModuleEquipWeapon = 4,	// Equipment
	E_ModuleEquipHelmet = 5,	// Equipment
	E_ModuleEquipCloth = 6,	// Equipment
	E_ModuleEquipPants = 7,	// Equipment
	E_ModuleBuff = 100,	// buff
	E_ModuleBuffStart = 100,	// Playerbuff(100-200), 100
	E_ModuleBuffFinish = 200,	// Playerbuff(100-200), 100
	E_ModuleCount = 200,	// Translated comment.
};
enum EnumAttribute
{
	E_Attack,	// Translated comment.
	E_Defense,	// Translated comment.
	E_Hp,	// Translated comment.
	E_Anger,	// Translated comment.
	E_Speed,	// Translated comment.
	E_CooldownAbsolute,	// Translated comment.
	E_CooldownProp,	// Translated comment.
	E_Count,	
};
enum EnumFightStat
{
	E_FightMove,	// Translated comment.
	E_FightNormalAttack,	// Translated comment.
	E_FightReleaseSkill,	// Translated comment.
	E_FightCount,	
};
enum EItemSrc
{
	EItemSrcNoraml = 0,	
	EItemSrcGM = 1,	
};
enum EItemConsume
{
	EItemConsumeNoraml = 0,	
	EItemConsumeGM = 1,	
};
enum EItemType
{
	EEquipment = 1,	// Equipment
	EMaterial = 2,	// Translated comment.
	EMoneyGold = 3,	// Translated comment.
	EMoneySilver = 4,	// Translated comment.
	ECard = 5,	// Card
};
enum EQuality
{
	EQualityGrey = 0,	// Translated comment.
	EQualityGreen = 1,	// Translated comment.
	EQualityYellow = 2,	// Translated comment.
	EQualityOrange = 3,	// Translated comment.
	EQualityRed = 4,	// Translated comment.
};
enum EEquip
{
	EEquipWeapon = 1,	// Translated comment.
	EEquipHelmet = 2,	// Translated comment.
	EEquipCloth = 3,	// Translated comment.
	EEquipPants = 4,	// Translated comment.
};
enum ECalendar
{
	Week = 0,	// Translated comment.
	ServerOpen = 1,	// Server opentime
	RegularSlot = 2,	// Time
};
enum ECalendarType
{
	ECalendarTypeNull,	// Translated comment.
	ECalendarTypeActivity,	// Activitystartandclose
	ECalendarTypeCount,	
};
enum EActivity
{
	EActivityDrawCompliance = 1,	// Similar << >>
	EActivityTopRank = 2,	// Translated comment.
};
enum EActivityOpen
{
	EActivityOpenGm = 0,	// Gmstart
	EActivityOpenAlways = 1,	// Activity
};
enum EActivityAlways
{
	EActivityAlwaysNull = 0,	
	EActivityAlwaysWeek = 1,	// Translated comment.
	EActivityAlwaysMonth = 2,	// Translated comment.
	EActivityAlwaysFixed = 3,	// Time
};
enum ETask
{
	ETaskRoleLv = 0,	// Playerlevel
	ETaskRoleVip = 1,	// Playerviplevel
	ETaskTaskId = 2,	// Complete IDtask
	ETaskRoleLogin = 3,	// Translated comment.
	ETaskConsumeGold = 4,	// Translated comment.
	ETaskCount,	
};
enum ETaskCondition
{
	ETaskConditionMore = 0,	// Translated comment.
	ETaskConditionLess = 1,	// Translated comment.
	ETaskConditionEqual = 2,	// Translated comment.
	ETaskConditionCount,	
};
enum ETaskType
{
	ETaskTypePrincipalLine = 0,	// Task
	ETaskTypeBranchLine = 1,	// Task
	ETaskTypeDaily = 2,	// Task
	ETaskTypeRepeat = 3,	// Complete task
};
enum eobstacles
{
	eobstacles_pass = 0,	// Canthrough
	eobstacles_nopass = 1,	// Cannotthrough
};
struct net_works
{
	/*********************************/
	ENET_PROTOCOL                    m_type                          ; 
	std::string                      m_ip                            ; 
	std::string                      m_nip                           ; 
	uint16_t                         m_port                          ; 
	/*********************************/
	net_works();
	// 序列化反序列化相关
	DPROTOCOL(net_works, m_type, m_ip, m_nip, m_port)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_type,m_ip,m_nip,m_port);
	}
};
struct tab_servers
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	int16_t                          m_area                          ; // [Index:3][load:y] area
	NODE_TYPE                        m_type                          ; // [Index:4][load:y] servertype(1, DB(databaseserver)2, ACTORSERVER(actor server))3, GAME( server)4, GATEWAY(gatewayserver)5, LOGIN(loginserver)6, ROBOT(test server)7, WORLD( server)8, LOG(logserver)9, RELOADCSV( csvserver)10, CROSS(cross-serverserver)11, CROSSDB(cross-serverdatabaseserver)12, PUSHSERVERCONFIG( serverconfigon lbgmsys))
	int32_t                          m_tcount                        ; // [Index:5][load:y] typeserver
	int32_t                          m_threadnum                     ; // [Index:6][load:y] socketthread
	int32_t                          m_actorthreadnum                ; // [Index:7][load:y] actorthread thread
	bool                             m_outernet                      ; // [Index:8][load:y] whether
	int32_t                          m_db                            ; // [Index:9][load:y] connection db id
	int32_t                          m_reloadcsv                     ; // [Index:10][load:y] connection reloadcsv id
	int32_t                          m_login                         ; // [Index:11][load:y] connection login id
	int32_t                          m_log                           ; // [Index:13][load:y] connection log id
	int16_t                          m_crossarea                     ; // [Index:12][load:y] cross-serverarea
	std::vector<int32_t>             m_actorserver                   ; // [Index:13][load:y] connection actorserver id(cross-serverneed to actorserver)
	std::vector<net_works>           m_net                           ; // [Index:14][load:y] server related(net_works:m_type(0tcp1ws2kcp),m_ip,m_nip,m_port)
	/*********************************/
	tab_servers();
	// 序列化反序列化相关
	DPROTOCOL(tab_servers, m_id, m_name, m_area, m_type, m_tcount, m_threadnum, m_actorthreadnum, m_outernet, m_db, m_reloadcsv, m_login, m_log, m_crossarea, m_actorserver, m_net)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_area,m_type,m_tcount,m_threadnum,m_actorthreadnum,m_outernet,m_db,m_reloadcsv,m_login,m_log,m_crossarea,m_actorserver,m_net);
	}
};
struct tab_dbload
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	bool                             m_isloadall                     ; // [Index:3][load:y] whetherneed to loadalldata
	bool                             m_network                       ; // [Index:4][load:y] whether through
	int32_t                          m_sendmaxcount                  ; // [Index:5][load:y] send
	int32_t                          m_dbcacheintervalms             ; // [Index:6][load:y] datacache
	/*********************************/
	tab_dbload();
	// 序列化反序列化相关
	DPROTOCOL(tab_dbload, m_id, m_name, m_isloadall, m_network, m_sendmaxcount, m_dbcacheintervalms)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_isloadall,m_network,m_sendmaxcount,m_dbcacheintervalms);
	}
};
struct tab_errormessage
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	std::string                      m_message                       ; // [Index:3][load:y]
	/*********************************/
	tab_errormessage();
	// 序列化反序列化相关
	DPROTOCOL(tab_errormessage, m_id, m_name, m_message)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_message);
	}
};
struct tab_specialid
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1[load:y]]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	std::string                      m_value                         ; // [Index:3][load:y]
	/*********************************/
	tab_specialid();
	// 序列化反序列化相关
	DPROTOCOL(tab_specialid, m_id, m_name, m_value)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_value);
	}
};
struct tab_attribute
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	float                            m_fightcoefficient              ; // [Index:3][load:y] attribute
	int32_t                          m_initvalue                     ; // [Index:4][load:y] attribute
	int32_t                          m_uplimit                       ; // [Index:5][load:y] on
	int32_t                          m_lowlimit                      ; // [Index:6][load:y] under
	/*********************************/
	tab_attribute();
	// 序列化反序列化相关
	DPROTOCOL(tab_attribute, m_id, m_name, m_fightcoefficient, m_initvalue, m_uplimit, m_lowlimit)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_fightcoefficient,m_initvalue,m_uplimit,m_lowlimit);
	}
};
struct tab_item
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	int32_t                          m_ranks                         ; // [Index:3][load:y]
	EItemType                        m_type                          ; // [Index:4][load:y] itemtypeEItemType(1equipment,2,3,4 )
	int32_t                          m_quality                       ; // [Index:5][load:y] item (EQuality)
	bool                             m_isstack                       ; // [Index:6][load:y] whether
	/*********************************/
	tab_item();
	// 序列化反序列化相关
	DPROTOCOL(tab_item, m_id, m_name, m_ranks, m_type, m_quality, m_isstack)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_ranks,m_type,m_quality,m_isstack);
	}
};
struct attributevalue
{
	/*********************************/
	int32_t                          m_id                            ; // Attributeid(EnumAttribute)
	bool                             m_isratio                       ; // Whether attribute
	int32_t                          m_value                         ; // Attribute
	/*********************************/
	attributevalue();
	// 序列化反序列化相关
	DPROTOCOL(attributevalue, m_id, m_isratio, m_value)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_id,m_isratio,m_value);
	}
};
struct tab_equip
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	EEquip                           m_position                      ; // [Index:3][load:y] EEquip(1 2 3 4 )
	std::vector<attributevalue>      m_attributevalues               ; // [Index:4][load:y] equipmentattribute
	/*********************************/
	tab_equip();
	// 序列化反序列化相关
	DPROTOCOL(tab_equip, m_id, m_name, m_position, m_attributevalues)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_position,m_attributevalues);
	}
};
struct tab_equiplv
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	int32_t                          m_consume                       ; // [Index:3][load:y] id
	std::vector<attributevalue>      m_attributevalues               ; // [Index:4][load:y] equipmentattribute
	/*********************************/
	tab_equiplv();
	// 序列化反序列化相关
	DPROTOCOL(tab_equiplv, m_id, m_name, m_consume, m_attributevalues)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_consume,m_attributevalues);
	}
};
struct tab_card
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	EQuality                         m_quality                       ; // [Index:3][load:y] EQuality(0 1 2 3 4 )
	std::vector<attributevalue>      m_attributevalues               ; // [Index:4][load:y] attribute
	/*********************************/
	tab_card();
	// 序列化反序列化相关
	DPROTOCOL(tab_card, m_id, m_name, m_quality, m_attributevalues)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_quality,m_attributevalues);
	}
};
struct trandom
{
	/*********************************/
	int32_t                          m_id                            ; 
	int32_t                          m_min                           ; 
	int32_t                          m_max                           ; 
	int32_t                          m_weight                        ; 
	/*********************************/
	trandom();
	// 序列化反序列化相关
	DPROTOCOL(trandom, m_id, m_min, m_max, m_weight)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_id,m_min,m_max,m_weight);
	}
};
struct tab_random
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	bool                             m_exclusive                     ; // [Index:4][load:y] (true item false item)
	int32_t                          m_count                         ; // [Index:5][load:y]
	std::set<int32_t>                m_activityids                   ; // [Index:6][load:y] activitystart this
	std::vector<int32_t>             m_childrandomids                ; // [Index:7][load:y]
	std::vector<trandom>             m_randomdatas                   ; // [index:3][load:y] trandom(id*min*max*weight)
	/*********************************/
	tab_random();
	// 序列化反序列化相关
	DPROTOCOL(tab_random, m_id, m_name, m_exclusive, m_count, m_activityids, m_childrandomids, m_randomdatas)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_exclusive,m_count,m_activityids,m_childrandomids,m_randomdatas);
	}
};
struct tconsume
{
	/*********************************/
	int32_t                          m_tid                           ; 
	int32_t                          m_count                         ; 
	/*********************************/
	tconsume();
	// 序列化反序列化相关
	DPROTOCOL(tconsume, m_tid, m_count)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_tid,m_count);
	}
};
struct tab_consume
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	std::vector<tconsume>            m_consumeitems                  ; // [index:3][load:y] tid*count
	/*********************************/
	tab_consume();
	// 序列化反序列化相关
	DPROTOCOL(tab_consume, m_id, m_name, m_consumeitems)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_consumeitems);
	}
};
struct tab_synthesis
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	int32_t                          m_consume                       ; // [Index:3][load:y] list
	int32_t                          m_generate                      ; // [Index:4][load:y] table(tab_rando)id successfulitemlist
	/*********************************/
	tab_synthesis();
	// 序列化反序列化相关
	DPROTOCOL(tab_synthesis, m_id, m_name, m_consume, m_generate)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_consume,m_generate);
	}
};
struct tweek
{
	/*********************************/
	int32_t                          m_weekstart                     ; // Translated comment.
	std::string                      m_opentime                      ; // Starttime HH:mm:ss
	int32_t                          m_weekfinish                    ; // End
	std::string                      m_closetime                     ; // Endtime HH:mm:ss
	/*********************************/
	tweek();
	// 序列化反序列化相关
	DPROTOCOL(tweek, m_weekstart, m_opentime, m_weekfinish, m_closetime)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_weekstart,m_opentime,m_weekfinish,m_closetime);
	}
};
struct tserveropen
{
	/*********************************/
	int32_t                          m_beg                           ; // Server openafter start
	std::string                      m_opentime                      ; // Starttime HH:mm:ss
	int32_t                          m_end                           ; // Server openafter end
	std::string                      m_closetime                     ; // Endtime HH:mm:ss
	/*********************************/
	tserveropen();
	// 序列化反序列化相关
	DPROTOCOL(tserveropen, m_beg, m_opentime, m_end, m_closetime)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_beg,m_opentime,m_end,m_closetime);
	}
};
struct tregularslot
{
	/*********************************/
	std::string                      m_opentime                      ; // Starttime YYYY-MM-DD HH:mm:ss
	std::string                      m_closetime                     ; // Endtime YYYY-MM-DD HH:mm:ss
	/*********************************/
	tregularslot();
	// 序列化反序列化相关
	DPROTOCOL(tregularslot, m_opentime, m_closetime)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_opentime,m_closetime);
	}
};
struct tab_mail
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	std::string                      m_title                         ; // [Index:3][load:y] mail
	std::string                      m_author                        ; // [Index:4][load:y] mail
	std::string                      m_content                       ; // [Index:5][load:y] mailcontent
	/*********************************/
	tab_mail();
	// 序列化反序列化相关
	DPROTOCOL(tab_mail, m_id, m_name, m_title, m_author, m_content)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_title,m_author,m_content);
	}
};
struct tab_chat
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	int32_t                          m_time                          ; // [Index:3][load:y] chat
	int32_t                          m_count                         ; // [Index:4][load:y] chat
	/*********************************/
	tab_chat();
	// 序列化反序列化相关
	DPROTOCOL(tab_chat, m_id, m_name, m_time, m_count)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_time,m_count);
	}
};
struct activity_task
{
	/*********************************/
	int32_t                          m_begday                        ; // (From1 )
	int32_t                          m_endday                        ; // (From1 )remove(-1 tableactivityend)
	std::vector<int32_t>             m_taskids                       ; // Tasklist
	/*********************************/
	activity_task();
	// 序列化反序列化相关
	DPROTOCOL(activity_task, m_begday, m_endday, m_taskids)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_begday,m_endday,m_taskids);
	}
};
struct tab_activity
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	EActivity                        m_type                          ; // [Index:3][load:y] activitytype(1:similar << >>2: )
	EActivityOpen                    m_open                          ; // [Index:4][load:y] start (0:GMstart1: activity)
	std::vector<int32_t>             m_task                          ; // [Index:5][load:y] activityduring task
	std::vector<activity_task>       m_taskday                       ; // [Index:6][load:y] activityduring task( task)
	std::vector<int32_t>             m_ranktype                      ; // [Index:7][load:y] support activityranklist(eranklist.activity_lv:1000000activityduringplayerlevelchange,eranklist.activity_gold:2000000activityduringplayer )
	/*********************************/
	tab_activity();
	// 序列化反序列化相关
	DPROTOCOL(tab_activity, m_id, m_name, m_type, m_open, m_task, m_taskday, m_ranktype)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_type,m_open,m_task,m_taskday,m_ranktype);
	}
};
struct tab_activityopenserver
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	int                              m_openday                       ; // [Index:3][load:y] server open start(server open )
	int                              m_openhour                      ; // [Index:4][load:y]
	int                              m_openminute                    ; // [Index:5][load:y]
	int                              m_opensecond                    ; // [Index:6][load:y]
	int                              m_closeday                      ; // [Index:7][load:y] server open start(server open )
	int                              m_closehour                     ; // [Index:8][load:y]
	int                              m_closeminute                   ; // [Index:9][load:y]
	int                              m_closesecond                   ; // [Index:10][load:y]
	/*********************************/
	tab_activityopenserver();
	// 序列化反序列化相关
	DPROTOCOL(tab_activityopenserver, m_id, m_name, m_openday, m_openhour, m_openminute, m_opensecond, m_closeday, m_closehour, m_closeminute, m_closesecond)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_openday,m_openhour,m_openminute,m_opensecond,m_closeday,m_closehour,m_closeminute,m_closesecond);
	}
};
struct tab_activityalways
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	EActivityAlways                  m_type                          ; // [Index:3][load:y] activity type
	int                              m_wbday                         ; // [Index:4][load:y] (1-7)
	int                              m_wbhour                        ; // [Index:5][load:y]
	int                              m_wbminute                      ; // [Index:6][load:y]
	int                              m_wbsecond                      ; // [Index:7][load:y]
	int                              m_weday                         ; // [Index:8][load:y] (1-7)
	int                              m_wehour                        ; // [Index:9][load:y]
	int                              m_weminute                      ; // [Index:10][load:y]
	int                              m_wesecond                      ; // [Index:11][load:y]
	int                              m_mbday                         ; // [Index:12][load:y] (1-31)
	int                              m_mbhour                        ; // [Index:13][load:y]
	int                              m_mbminute                      ; // [Index:14][load:y]
	int                              m_mbsecond                      ; // [Index:15][load:y]
	int                              m_meday                         ; // [Index:16][load:y] (1-31)
	int                              m_mehour                        ; // [Index:17][load:y]
	int                              m_meminute                      ; // [Index:18][load:y]
	int                              m_mesecond                      ; // [Index:19][load:y]
	int                              m_fixedday                      ; // [Index:20][load:y] durationto
	int                              m_fbhour                        ; // [Index:21][load:y]
	int                              m_fbminute                      ; // [Index:22][load:y]
	int                              m_fbsecond                      ; // [Index:23][load:y]
	int                              m_fehour                        ; // [Index:24][load:y]
	int                              m_feminute                      ; // [Index:25][load:y]
	int                              m_fesecond                      ; // [Index:26][load:y]
	/*********************************/
	tab_activityalways();
	// 序列化反序列化相关
	DPROTOCOL(tab_activityalways, m_id, m_name, m_type, m_wbday, m_wbhour, m_wbminute, m_wbsecond, m_weday, m_wehour, m_weminute, m_wesecond, m_mbday, m_mbhour, m_mbminute, m_mbsecond, m_meday, m_mehour, m_meminute, m_mesecond, m_fixedday, m_fbhour, m_fbminute, m_fbsecond, m_fehour, m_feminute, m_fesecond)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_type,m_wbday,m_wbhour,m_wbminute,m_wbsecond,m_weday,m_wehour,m_weminute,m_wesecond,m_mbday,m_mbhour,m_mbminute,m_mbsecond,m_meday,m_mehour,m_meminute,m_mesecond,m_fixedday,m_fbhour,m_fbminute,m_fbsecond,m_fehour,m_feminute,m_fesecond);
	}
};
struct tab_activity_drawcompliance
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	int32_t                          m_dropid                        ; // [Index:3][load:y] id
	int32_t                          m_mailid                        ; // [Index:4][load:y] mailid
	/*********************************/
	tab_activity_drawcompliance();
	// 序列化反序列化相关
	DPROTOCOL(tab_activity_drawcompliance, m_id, m_name, m_dropid, m_mailid)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_dropid,m_mailid);
	}
};
struct tab_activity_toprank
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	int32_t                          m_activityid                    ; // [Index:3][load:y] activityid
	int32_t                          m_rank                          ; // [Index:3][load:y]
	int32_t                          m_dropid                        ; // [Index:4][load:y] id
	int32_t                          m_mailid                        ; // [Index:5][load:y] mailid
	/*********************************/
	tab_activity_toprank();
	// 序列化反序列化相关
	DPROTOCOL(tab_activity_toprank, m_id, m_name, m_activityid, m_rank, m_dropid, m_mailid)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_activityid,m_rank,m_dropid,m_mailid);
	}
};
struct task_condition
{
	/*********************************/
	ETask                            m_type                          ; // ETask(0.playerlevel toX 1. viplevel tox 2.complete IDtask)
	ETaskCondition                   m_condition                     ; 
	int32_t                          m_parmint                       ; 
	/*********************************/
	task_condition();
	// 序列化反序列化相关
	DPROTOCOL(task_condition, m_type, m_condition, m_parmint)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_type,m_condition,m_parmint);
	}
};
struct tab_task
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	ETaskType                        m_type                          ; // [Index:3][load:y] tasktype(0: task1: task2: task3: complete task)
	std::string                      m_typeparm                      ; // [Index:4][load:y] (m_type=2||=3: complete )
	int32_t                          m_dropid                        ; // [Index:7][load:y] taskreward
	bool                             m_autoreceive                   ; // [Index:8][load:y] whetherautomaticallyclaim
	int32_t                          m_mailid                        ; // [Index:9][load:y] automaticallyclaimafterwhethersendmail mailid(automaticallyclaim mailid:m_autoreceive == true, m_autoreceive turecan -1)
	std::vector<task_condition>      m_taskreceive                   ; // [Index:5][load:y] this task before (ETask(0.playerlevel toX 1. viplevel tox 2.complete IDtask)*ETaskCondition(0. 1. 2. )*int32_t( ))
	std::vector<task_condition>      m_taskcomplete                  ; // [Index:6][load:y] completethis task condition(ETask(0.playerlevel toX 1. viplevel tox 2.complete IDtask)*ETaskCondition(0. 1. 2. )*int32_t( ))
	/*********************************/
	tab_task();
	// 序列化反序列化相关
	DPROTOCOL(tab_task, m_id, m_name, m_type, m_typeparm, m_dropid, m_autoreceive, m_mailid, m_taskreceive, m_taskcomplete)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_type,m_typeparm,m_dropid,m_autoreceive,m_mailid,m_taskreceive,m_taskcomplete);
	}
};
struct obstacles_data
{
	/*********************************/
	std::vector<int32_t>             m_data                          ; //eobstacles
	/*********************************/
	obstacles_data();
	// 序列化反序列化相关
	DPROTOCOL(obstacles_data, m_data)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_data);
	}
};
struct tab_map
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	int32_t                          m_w                             ; // [Index:3][load:y] x
	int32_t                          m_l                             ; // [Index:4][load:y] y
	int32_t                          m_nx                            ; // [Index:5][load:y] [view range]x
	int32_t                          m_ny                            ; // [Index:6][load:y] [view range]y
	int32_t                          m_obstaclenx                    ; // [Index:7][load:y] [ ]x
	int32_t                          m_obstacleny                    ; // [Index:8][load:y] [ ]y
	std::vector<obstacles_data>      m_obstacles                     ; // [Index:9][load:y] mapdata
	/*********************************/
	tab_map();
	// 序列化反序列化相关
	DPROTOCOL(tab_map, m_id, m_name, m_w, m_l, m_nx, m_ny, m_obstaclenx, m_obstacleny, m_obstacles)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_w,m_l,m_nx,m_ny,m_obstaclenx,m_obstacleny,m_obstacles);
	}
};
struct tab_matching
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	int32_t                          m_count                         ; // [Index:3][load:y] gameplay tothis canstartgameplay
	int32_t                          m_time                          ; // [Index:4][load:y] time
	bool                             m_iswaitconfirm                 ; // [Index:5][load:y] whetherneed to confirm
	int32_t                          m_waitconfirmtime               ; // [Index:6][load:y] confirm time
	/*********************************/
	tab_matching();
	// 序列化反序列化相关
	DPROTOCOL(tab_matching, m_id, m_name, m_count, m_time, m_iswaitconfirm, m_waitconfirmtime)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_count,m_time,m_iswaitconfirm,m_waitconfirmtime);
	}
};
struct Vector2
{
	/*********************************/
	int32_t                          m_x                             ; 
	int32_t                          m_y                             ; 
	/*********************************/
	Vector2();
	// 序列化反序列化相关
	DPROTOCOL(Vector2, m_x, m_y)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_x,m_y);
	}
};
struct tab_plays
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	int32_t                          m_type                          ; // [Index:3][load:y] gameplaytype pbnet::eplays
	int32_t                          m_mapid                         ; // [Index:4][load:y] map this gameplay
	int32_t                          m_preparation_tm                ; // [Index:5][load:y] time
	int32_t                          m_play_tm                       ; // [Index:6][load:y] gameplaytime
	int32_t                          m_settlement_tm                 ; // [Index:7][load:y] time
	std::vector<Vector2>             m_birthpoint                    ; // [Index:8][load:y] Vector2(m_x:int32_t,m_y:int32_t)
	/*********************************/
	tab_plays();
	// 序列化反序列化相关
	DPROTOCOL(tab_plays, m_id, m_name, m_type, m_mapid, m_preparation_tm, m_play_tm, m_settlement_tm, m_birthpoint)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_type,m_mapid,m_preparation_tm,m_play_tm,m_settlement_tm,m_birthpoint);
	}
};
struct tab_recharge
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	int32_t                          m_price                         ; // [Index:3][load:y]
	int32_t                          m_gold                          ; // [Index:4][load:y]
	int32_t                          m_bonus                         ; // [Index:5][load:y] reward
	int32_t                          m_firstbonus                    ; // [Index:6][load:y] reward
	int32_t                          m_dropid                        ; // [Index:7][load:y]
	int32_t                          m_vipexp                        ; // [Index:8][load:y] this vip
	int32_t                          m_count                         ; // [Index:9][load:y] whether
	int32_t                          m_mailid                        ; // [Index:11][load:y] mailid
	std::vector<int32_t>             m_activityid                    ; // [Index:10][load:y] specifiedactivitystart this
	/*********************************/
	tab_recharge();
	// 序列化反序列化相关
	DPROTOCOL(tab_recharge, m_id, m_name, m_price, m_gold, m_bonus, m_firstbonus, m_dropid, m_vipexp, m_count, m_mailid, m_activityid)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_price,m_gold,m_bonus,m_firstbonus,m_dropid,m_vipexp,m_count,m_mailid,m_activityid);
	}
};
struct tab_familylv
{
	/*********************************/
	int32_t                          m_id                            ; // [Index:0][load:y] guildlevel
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	int32_t                          m_exp                           ; // [Index:3][load:y] need to
	int32_t                          m_maxmembers                    ; // [Index:4][load:y] guild
	/*********************************/
	tab_familylv();
	// 序列化反序列化相关
	DPROTOCOL(tab_familylv, m_id, m_name, m_exp, m_maxmembers)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_exp,m_maxmembers);
	}
};
struct tab_mergearea
{
	/*********************************/
	int32_t                          m_id                            ; // [Index:0][load:y] areaid
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	int32_t                          m_mergeid                       ; // [Index:3][load:y] andto areaid
	/*********************************/
	tab_mergearea();
	// 序列化反序列化相关
	DPROTOCOL(tab_mergearea, m_id, m_name, m_mergeid)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_mergeid);
	}
};
struct tab_ranklist
{
	/*********************************/
	int32_t                          m_id                            ; // [Index:0][load:y] eranklist(0:levelrank)
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	int32_t                          m_maxitem                       ; // [Index:3][load:y] ranknode
	int32_t                          m_showitem                      ; // [Index:4][load:y] ranknode
	int32_t                          m_everypagecount                ; // [Index:5][load:y] node
	int32_t                          m_minvalue                      ; // [Index:6][load:y]
	/*********************************/
	tab_ranklist();
	// 序列化反序列化相关
	DPROTOCOL(tab_ranklist, m_id, m_name, m_maxitem, m_showitem, m_everypagecount, m_minvalue)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_maxitem,m_showitem,m_everypagecount,m_minvalue);
	}
};
struct tab_filterword
{
	/*********************************/
	int32_t                          m_id                            ; // [Index:0][load:y] id
	std::string                      m_name                          ; // [Index:1][load:y]
//	std::string                      m_remarks                       ; // [Index:2][load:n] remarks
	std::string                      m_content                       ; // [Index:3][load:y]
	/*********************************/
	tab_filterword();
	// 序列化反序列化相关
	DPROTOCOL(tab_filterword, m_id, m_name, m_content)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_content);
	}
};
}//namespace ngl
