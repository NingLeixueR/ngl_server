/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* é¡¹ç›®åç§°ï¼šngl_server
* é¡¹ç›®åœ°å€ï¼šhttps://github.com/NingLeixueR/ngl_server
* 
* æœ¬æ–‡ä»¶æ˜¯ ngl_server é¡¹ç›®çš„ä¸€éƒ¨åˆ†ï¼Œéµå¾ª MIT å¼€æºåè®®å‘å¸ƒã€‚
* æ‚¨å¯ä»¥æŒ‰ç…§åè®®è§„å®šè‡ªç”±ä½¿ç”¨ã€ä¿®æ”¹å’Œåˆ†å‘æœ¬é¡¹ç›®ï¼ŒåŒ…æ‹¬å•†ä¸šç”¨é€”ï¼Œ
* ä½†éœ€ä¿ç•™åŸå§‹ç‰ˆæƒå’Œè®¸å¯å£°æ˜ã€‚
* 
* è®¸å¯è¯¦æƒ…å‚è§é¡¹ç›®æ ¹ç›®å½•ä¸‹çš„ LICENSE æ–‡ä»¶ï¼š
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/

// æ³¨æ„ã€IDL å·¥å…·ç”Ÿæˆæ–‡ä»¶ï¼Œä¸è¦æ‰‹åŠ¨ä¿®æ”¹ã€‘
// åˆ›å»ºæ—¶é—´ // åˆ›å»ºæ—¶é—´ 26-02-12 16:08:09
#pragma once

#include "csv.h"
#include "ndefine.h"
#include "nserialize.h"
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
	EUDP_OPERATOR_ACK,	// È·ÈÏ°ü
	EUDP_OPERATOR_DATA,	// ´«Êä×ÓÊı¾İ°ü
	EUDP_OPERATOR_HEARTBEAT,	// ĞÄÌøÖ÷ÒªÓÃÀ´È·ÈÏ¶Ô¶ËÊÇ·ñÔÚÏß
};
enum EPH_HEAD_VAL
{
	EPH_HEAD_MASK_SUCCESS = 1,	// MASKÒ»ÖÂ
	EPH_HEAD_MASK_FAIL = 2,	// MASK²»Ò»ÖÂ
	EPH_HEAD_FOLLOW = 3,	// °üÍ·Êı¾İÃ»ÓĞ½ÓÊÕÍê³É
	EPH_HEAD_SUCCESS = 4,	// °üÍ·Êı¾İÒÑ½ÓÊÕÍê³É
};
enum EPH
{
	EPH_MASK_VALUE = 0xff,	
	EPH_MASK_COUNT = 2,	
	EPH_MASK = 0,	// ÓÃÓÚÈ·ÈÏÊÇ·ñÊ¹ÓÃ°ü
	EPH_BYTES = EPH_MASK_COUNT,	// Ğ­Òé×Ö½ÚÊı
	EPH_TIME,	// ·¢ËÍ¶ËµÄÊ±¼ä´Á
	EPH_PROTOCOLNUM,	// Ğ­ÒéºÅ
	EPH_ACTOR_TYPEAREA,	// ACTOR_TYPE_AREA
	EPH_ACTOR_ID,	// ACTOR_ID
	EPH_REQUEST_ACTOR_TYPEAREA,	// Request ACTOR_TYPE_AREA
	EPH_REQUEST_ACTOR_ID,	// Request ACTOR_ID
	EPH_SUM,	
};
enum NODE_TYPE
{
	DB = 1,	// Êı¾İ¿â·şÎñÆ÷
	ACTORSERVER = 2,	// actor ·şÎñÆ÷
	GAME = 3,	// Âß¼­·şÎñÆ÷
	GATEWAY = 4,	// Íø¹Ø·şÎñÆ÷
	LOGIN = 5,	// µÇÂ¼·şÎñÆ÷
	ROBOT = 6,	// ²âÊÔ»úÆ÷ÈË·şÎñÆ÷
	WORLD = 7,	// ÊÀ½ç·şÎñÆ÷
	LOG = 8,	// ÈÕÖ¾·şÎñÆ÷
	RELOADCSV = 9,	// ÖØÔØ·Ö·¢csv·şÎñÆ÷
	CROSS = 10,	// ¿ç·ş·şÎñÆ÷
	CROSSDB = 11,	// ¿ç·şÊı¾İ¿â·şÎñÆ÷
	PUSHSERVERCONFIG = 12,	// ½«·şÎñÆ÷ÅäÖÃÉÏ´«lbgmsys
	NODE_TYPE_COUNT,	
	FAIL = -1,	// ·şÎñÆ÷ÀàĞÍ´íÎó
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
	E_ModuleNull = 0,	// ¿Õ	
	E_ModuleRoot = 1,	// ¿¨ÅÆ(Á´Î²)
	E_ModuleEquip = 3,	// ×°±¸
	E_ModuleEquipWeapon = 4,	// ×°±¸ÎäÆ÷
	E_ModuleEquipHelmet = 5,	// ×°±¸Í·¿ø
	E_ModuleEquipCloth = 6,	// ×°±¸ÒÂ·ş
	E_ModuleEquipPants = 7,	// ×°±¸»¤ÍÈ
	E_ModuleBuff = 100,	// buff
	E_ModuleBuffStart = 100,	// Íæ¼Òbuff(100-200)£¬×î¶à100
	E_ModuleBuffFinish = 200,	// Íæ¼Òbuff(100-200)£¬×î¶à100
	E_ModuleCount = 200,	// ÊıÁ¿ 	
};
enum EnumAttribute
{
	E_Attack,	// [¹¥»÷]
	E_Defense,	// [·ÀÓù]
	E_Hp,	// [ÑªÁ¿]
	E_Anger,	// [Å­Æø]
	E_Speed,	// [ËÙ¶È]
	E_CooldownAbsolute,	// [ÀäÈ´Ëõ¼õ-¾ø¶ÔÖµ]
	E_CooldownProp,	// [ÀäÈ´Ëõ¼õ-Íò·Ö±È]
	E_Count,	
};
enum EnumFightStat
{
	E_FightMove,	// [ÒÆ¶¯]
	E_FightNormalAttack,	// [ÆÕÍ¨¹¥»÷]
	E_FightReleaseSkill,	// [ÊÍ·Å¼¼ÄÜ]
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
	EEquipment = 1,	// ×°±¸
	EMaterial = 2,	// ²ÄÁÏ
	EMoneyGold = 3,	// ½ğ±Ò
	EMoneySilver = 4,	// Òø±Ò
	ECard = 5,	// ¿¨ÅÆ
};
enum EQuality
{
	EQualityGrey = 0,	//»ÒÉ«
	EQualityGreen = 1,	//ÂÌÉ«
	EQualityYellow = 2,	//»ÆÉ«
	EQualityOrange = 3,	//³ÈÉ«
	EQualityRed = 4,	//ºìÉ«
};
enum EEquip
{
	EEquipWeapon = 1,	// ÎäÆ÷
	EEquipHelmet = 2,	// Í·¿ø
	EEquipCloth = 3,	// ÒÂ·ş
	EEquipPants = 4,	// »¤ÍÈ
};
enum ECalendar
{
	Week = 0,	// ÖÜ¼¸
	ServerOpen = 1,	// ÒÔ¿ª·şÊ±¼äÒÔÀ´µÄÌìÊı
	RegularSlot = 2,	// ¹Ì¶¨Ê±¼ä¶Î
};
enum ECalendarType
{
	ECalendarTypeNull,	// ÎŞ
	ECalendarTypeActivity,	// »î¶¯¿ªÆôÓë¹Ø±Õ
	ECalendarTypeCount,	
};
enum EActivity
{
	EActivityDrawCompliance = 1,	// ÀàËÆÏÌÓãÖ®ÍõµÄ<<ÕĞÄ¼´ï±ê>>
	EActivityTopRank = 2,	// ³å°ñ
};
enum EActivityOpen
{
	EActivityOpenGm = 0,	// Gm¿ªÆô
	EActivityOpenAlways = 1,	// ³£×¤»î¶¯
};
enum EActivityAlways
{
	EActivityAlwaysNull = 0,	
	EActivityAlwaysWeek = 1,	// ÖÜ
	EActivityAlwaysMonth = 2,	// ÔÂ
	EActivityAlwaysFixed = 3,	// ¼ä¸ô¹Ì¶¨Ê±¼ä
};
enum ETask
{
	ETaskRoleLv = 0,	// Íæ¼ÒµÈ¼¶
	ETaskRoleVip = 1,	// Íæ¼ÒvipµÈ¼¶
	ETaskTaskId = 2,	// Íê³ÉÄ³IDÈÎÎñ
	ETaskRoleLogin = 3,	// ÀÛ¼ÆµÇÂ½
	ETaskConsumeGold = 4,	// ÀÛ¼ÆÏûºÄ½ğ±Ò
	ETaskCount,	
};
enum ETaskCondition
{
	ETaskConditionMore = 0,	// ´óÓÚµÈÓÚ
	ETaskConditionLess = 1,	// Ğ¡ÓÚµÈÓÚ
	ETaskConditionEqual = 2,	// µÈÓÚ
	ETaskConditionCount,	
};
enum ETaskType
{
	ETaskTypePrincipalLine = 0,	// Ö÷ÏßÈÎÎñ
	ETaskTypeBranchLine = 1,	// Ö§ÏßÈÎÎñ
	ETaskTypeDaily = 2,	// Ã¿ÈÕÈÎÎñ
	ETaskTypeRepeat = 3,	// ¿ÉÖØ¸´Íê³ÉµÄÈÎÎñ
};
enum eobstacles
{
	eobstacles_pass = 0,	// ¿ÉÒÔÍ¨¹ı
	eobstacles_nopass = 1,	// ÎŞ·¨Í¨¹ı
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
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(net_works, m_type, m_ip, m_nip, m_port)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_type,m_ip,m_nip,m_port);
	}
};
struct tab_servers
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	int16_t                          m_area                          ; // [index:3][load:y] Çø·ş
	NODE_TYPE                        m_type                          ; // [index:4][load:y] ·şÎñÆ÷ÀàĞÍ(1¡¢DB(Êı¾İ¿â·şÎñÆ÷)2¡¢ACTORSERVER(actor ·şÎñÆ÷))3¡¢GAME(Âß¼­·şÎñÆ÷)4¡¢GATEWAY(Íø¹Ø·şÎñÆ÷)5¡¢LOGIN(µÇÂ¼·şÎñÆ÷)6¡¢ROBOT(²âÊÔ»úÆ÷ÈË·şÎñÆ÷)7¡¢WORLD(ÊÀ½ç·şÎñÆ÷)8¡¢LOG(ÈÕÖ¾·şÎñÆ÷)9¡¢RELOADCSV(ÖØÔØ·Ö·¢csv·şÎñÆ÷)10¡¢CROSS(¿ç·ş·şÎñÆ÷)11¡¢CROSSDB(¿ç·şÊı¾İ¿â·şÎñÆ÷)12¡¢PUSHSERVERCONFIG(½«·şÎñÆ÷ÅäÖÃÉÏ´«lbgmsys))
	int32_t                          m_tcount                        ; // [index:5][load:y] Í¬ÀàĞÍ·şÎñÆ÷µÄĞòºÅ
	int32_t                          m_threadnum                     ; // [index:6][load:y] socketÏß³ÌÊı
	int32_t                          m_actorthreadnum                ; // [index:7][load:y] actorÏß³Ì³ØÏß³ÌÊı
	bool                             m_outernet                      ; // [index:8][load:y] ÊÇ·ñÔÊĞíÍâÍø·ÃÎÊ
	int32_t                          m_db                            ; // [index:9][load:y] Á¬½ÓµÄdb½ø³Ìid
	int32_t                          m_reloadcsv                     ; // [index:10][load:y] Á¬½ÓµÄreloadcsv½ø³Ìid
	int32_t                          m_login                         ; // [index:11][load:y] Á¬½ÓµÄlogin½ø³Ìid
	int16_t                          m_crossarea                     ; // [index:12][load:y] ¿ç·şÇø·ş
	int32_t                          m_log                           ; // [index:13][load:y] Á¬½ÓµÄlog½ø³Ìid
	std::vector<int32_t>             m_actorserver                   ; // [index:13][load:y] Á¬½ÓµÄactorserver½ø³Ìid(¿ç·şĞèÒªÌîĞ´¶à¸öactorserver)
	std::vector<net_works>           m_net                           ; // [index:14][load:y] ·şÎñÆ÷ÍøÂçÏà¹Ø(net_works:m_type(0tcp1ws2kcp),m_ip,m_nip,m_port)
	/*********************************/
	tab_servers();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_servers, m_id, m_name, m_area, m_type, m_tcount, m_threadnum, m_actorthreadnum, m_outernet, m_db, m_reloadcsv, m_login, m_crossarea, m_log, m_actorserver, m_net)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_area,m_type,m_tcount,m_threadnum,m_actorthreadnum,m_outernet,m_db,m_reloadcsv,m_login,m_crossarea,m_log,m_actorserver,m_net);
	}
};
struct tab_dbload
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	bool                             m_isloadall                     ; // [index:3][load:y] ÊÇ·ñĞèÒªÆô¶¯¼ÓÔØÈ«²¿Êı¾İ
	bool                             m_network                       ; // [index:4][load:y] ÊÇ·ñÔÊĞíÍ¨¹ıÍøÂç·Ö·¢
	int32_t                          m_sendmaxcount                  ; // [index:5][load:y] µ¥´Î×î¶à·¢ËÍÊıÁ¿
	int32_t                          m_dbcacheintervalms             ; // [index:6][load:y] Êı¾İ»º´æÈë¿â¼ä¸ô
	/*********************************/
	tab_dbload();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_dbload, m_id, m_name, m_isloadall, m_network, m_sendmaxcount, m_dbcacheintervalms)
	// csvç›¸å…³
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
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	std::string                      m_message                       ; // [index:3][load:y] ´íÎóÃèÊö
	/*********************************/
	tab_errormessage();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_errormessage, m_id, m_name, m_message)
	// csvç›¸å…³
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
	std::string                      m_name                          ; // [index:1[load:y]] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	std::string                      m_value                         ; // [index:3][load:y] Öµ
	/*********************************/
	tab_specialid();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_specialid, m_id, m_name, m_value)
	// csvç›¸å…³
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
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	float                            m_fightcoefficient              ; // [index:3][load:y] ÊôĞÔÌá¹©µÄÕ½Á¦ÏµÊı
	int32_t                          m_initvalue                     ; // [index:4][load:y] ÊôĞÔ³õÊ¼Öµ
	int32_t                          m_uplimit                       ; // [index:5][load:y] ÉÏÏŞ
	int32_t                          m_lowlimit                      ; // [index:6][load:y] ÏÂÏŞ
	/*********************************/
	tab_attribute();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_attribute, m_id, m_name, m_fightcoefficient, m_initvalue, m_uplimit, m_lowlimit)
	// csvç›¸å…³
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
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	int32_t                          m_ranks                         ; // [index:3][load:y] ÅÅĞòÖµ
	EItemType                        m_type                          ; // [index:4][load:y] ÎïÆ·ÀàĞÍEItemType(1×°±¸,2²ÄÁÏ,3½ğ±Ò,4Òø±Ò)
	int32_t                          m_quality                       ; // [index:5][load:y] ÎïÆ·Æ·ÖÊ(EQuality)
	bool                             m_isstack                       ; // [index:6][load:y] ÊÇ·ñ¿É¶Ñµş
	/*********************************/
	tab_item();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_item, m_id, m_name, m_ranks, m_type, m_quality, m_isstack)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_ranks,m_type,m_quality,m_isstack);
	}
};
struct attributevalue
{
	/*********************************/
	int32_t                          m_id                            ; // ÊôĞÔid(EnumAttribute)
	bool                             m_isratio                       ; // ÊÇ·ñÍò·Ö±ÈÊôĞÔ
	int32_t                          m_value                         ; // ÊôĞÔÖµ
	/*********************************/
	attributevalue();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(attributevalue, m_id, m_isratio, m_value)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_id,m_isratio,m_value);
	}
};
struct tab_equip
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	EEquip                           m_position                      ; // [index:3][load:y] ²¿Î»EEquip(1ÎäÆ÷2Í·¿ø3ÒÂ·ş4»¤ÍÈ)
	std::vector<attributevalue>      m_attributevalues               ; // [index:4][load:y] ×°±¸ÊôĞÔ
	/*********************************/
	tab_equip();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_equip, m_id, m_name, m_position, m_attributevalues)
	// csvç›¸å…³
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
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	int32_t                          m_consume                       ; // [index:3][load:y] Éı¼¶ÏûºÄid
	std::vector<attributevalue>      m_attributevalues               ; // [index:4][load:y] ×°±¸ÊôĞÔ
	/*********************************/
	tab_equiplv();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_equiplv, m_id, m_name, m_consume, m_attributevalues)
	// csvç›¸å…³
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
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	EQuality                         m_quality                       ; // [index:3][load:y] Æ·ÖÊEQuality(0»ÒÉ«1ÂÌÉ«2»ÆÉ«3³ÈÉ«4ºìÉ«)
	std::vector<attributevalue>      m_attributevalues               ; // [index:4][load:y] ÊôĞÔ
	/*********************************/
	tab_card();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_card, m_id, m_name, m_quality, m_attributevalues)
	// csvç›¸å…³
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
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(trandom, m_id, m_min, m_max, m_weight)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_id,m_min,m_max,m_weight);
	}
};
struct tab_random
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	std::vector<trandom>             m_randomdatas                   ; // [index:3][load:y] trandom(id*min*max*weight)
	bool                             m_exclusive                     ; // [index:4][load:y] ÅÅËûĞÔ¶à´ÎµôÂäÊ±Ê¹ÓÃ£¨true²»»áµôÂä³öÒÑµôÂäÎïÆ· false»áµôÂä³öÒÑµôÂäÎïÆ·£©
	int32_t                          m_count                         ; // [index:5][load:y] Ëæ»úÊıÁ¿
	std::set<int32_t>                m_activityids                   ; // [index:6][load:y] Ö»ÓĞ»î¶¯¿ªÆô²Å»á´¥·¢´ËµôÂä
	std::vector<int32_t>             m_childrandomids                ; // [index:7][load:y] ×ÓËæ»ú
	/*********************************/
	tab_random();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_random, m_id, m_name, m_randomdatas, m_exclusive, m_count, m_activityids, m_childrandomids)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_randomdatas,m_exclusive,m_count,m_activityids,m_childrandomids);
	}
};
struct tconsume
{
	/*********************************/
	int32_t                          m_tid                           ; 
	int32_t                          m_count                         ; 
	/*********************************/
	tconsume();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tconsume, m_tid, m_count)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_tid,m_count);
	}
};
struct tab_consume
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	std::vector<tconsume>            m_consumeitems                  ; // [index:3][load:y] tid*count
	/*********************************/
	tab_consume();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_consume, m_id, m_name, m_consumeitems)
	// csvç›¸å…³
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
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	int32_t                          m_consume                       ; // [index:3][load:y] ÏûºÄÁĞ±í
	int32_t                          m_generate                      ; // [index:4][load:y] Ëæ»ú±í(tab_rando)id Ëæ»ú³É¹¦ÎïÆ·ÁĞ±í
	/*********************************/
	tab_synthesis();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_synthesis, m_id, m_name, m_consume, m_generate)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_consume,m_generate);
	}
};
struct tweek
{
	/*********************************/
	int32_t                          m_weekstart                     ; // ÖÜ¼¸¿ªÊ¼
	std::string                      m_opentime                      ; // ¿ªÆôÊ±¼ä HH:mm:ss
	int32_t                          m_weekfinish                    ; // ÖÜ¼¸½áÊø
	std::string                      m_closetime                     ; // ½áÊøÊ±¼ä HH:mm:ss
	/*********************************/
	tweek();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tweek, m_weekstart, m_opentime, m_weekfinish, m_closetime)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_weekstart,m_opentime,m_weekfinish,m_closetime);
	}
};
struct tserveropen
{
	/*********************************/
	int32_t                          m_beg                           ; // ¿ª·şºó¶àÉÙÌì¿ªÆô
	std::string                      m_opentime                      ; // ¿ªÆôÊ±¼ä HH:mm:ss
	int32_t                          m_end                           ; // ¿ª·şºó¶àÉÙÌì½áÊø
	std::string                      m_closetime                     ; // ½áÊøÊ±¼ä HH:mm:ss
	/*********************************/
	tserveropen();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tserveropen, m_beg, m_opentime, m_end, m_closetime)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_beg,m_opentime,m_end,m_closetime);
	}
};
struct tregularslot
{
	/*********************************/
	std::string                      m_opentime                      ; // ¿ªÆôÊ±¼ä YYYY-MM-DD HH:mm:ss
	std::string                      m_closetime                     ; // ½áÊøÊ±¼ä YYYY-MM-DD HH:mm:ss
	/*********************************/
	tregularslot();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tregularslot, m_opentime, m_closetime)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_opentime,m_closetime);
	}
};
struct tab_mail
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	std::string                      m_title                         ; // [index:3][load:y] ÓÊ¼ş±êÌâ
	std::string                      m_author                        ; // [index:4][load:y] ÓÊ¼ş×÷Õß
	std::string                      m_content                       ; // [index:5][load:y] ÓÊ¼şÄÚÈİ
	/*********************************/
	tab_mail();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_mail, m_id, m_name, m_title, m_author, m_content)
	// csvç›¸å…³
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
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	int32_t                          m_time                          ; // [index:3][load:y] ÁÄÌì·¢ÑÔ¼ä¸ô
	int32_t                          m_count                         ; // [index:4][load:y] ÁÄÌì·¢ÑÔ×î´óÊıÁ¿
	/*********************************/
	tab_chat();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_chat, m_id, m_name, m_time, m_count)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_time,m_count);
	}
};
struct activity_task
{
	/*********************************/
	int32_t                          m_begday                        ; // µÚ¼¸Ìì(´Ó1¿ªÊ¼)½ÓÊÕ
	int32_t                          m_endday                        ; // µÚ¼¸Ìì(´Ó1¿ªÊ¼)ÒÆ³ı(-1´ú±í»î¶¯½áÊø)
	std::vector<int32_t>             m_taskids                       ; // ÈÎÎñÁĞ±í
	/*********************************/
	activity_task();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(activity_task, m_begday, m_endday, m_taskids)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_begday,m_endday,m_taskids);
	}
};
struct tab_activity
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	EActivity                        m_type                          ; // [index:3][load:y] »î¶¯ÀàĞÍ(1:ÀàËÆÏÌÓãÖ®ÍõµÄ<<ÕĞÄ¼´ï±ê>>2:³å°ñ)
	EActivityOpen                    m_open                          ; // [index:4][load:y] ¿ªÆô·½Ê½(0:GM¿ªÆô1:³£×¤»î¶¯)
	std::vector<int32_t>             m_task                          ; // [index:5][load:y] »î¶¯ÆÚ¼ä½ÓÈ¡ÈÎÎñ
	std::vector<activity_task>       m_taskday                       ; // [index:6][load:y] »î¶¯ÆÚ¼ä½ÓÈ¡ÈÎÎñ(µÚ¼¸Ìì½ÓÊÕÈÎÎñ)
	std::vector<int32_t>             m_ranktype                      ; // [index:7][load:y] Ö§³ÖµÄ»î¶¯ÅÅĞĞÁĞ±í(eranklist.activity_lv:1000000»î¶¯ÆÚ¼äÍæ¼ÒµÈ¼¶±ä»¯Öµ,eranklist.activity_gold:2000000»î¶¯ÆÚ¼äÍæ¼Ò½ğ±ÒÔö¼ÓÖµ)
	/*********************************/
	tab_activity();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_activity, m_id, m_name, m_type, m_open, m_task, m_taskday, m_ranktype)
	// csvç›¸å…³
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
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	int                              m_openday                       ; // [index:3][load:y] ¿ª·şµÚ¼¸Ìì¿ªÆô£¨¿ª·şµ±ÌìËãÎªµÚÒ»Ìì£©
	int                              m_openhour                      ; // [index:4][load:y] Ê±
	int                              m_openminute                    ; // [index:5][load:y] ·Ö
	int                              m_opensecond                    ; // [index:6][load:y] Ãë
	int                              m_closeday                      ; // [index:7][load:y] ¿ª·şµÚ¼¸Ìì¿ªÆô£¨¿ª·şµ±ÌìËãÎªµÚÒ»Ìì£©
	int                              m_closehour                     ; // [index:8][load:y] Ê±
	int                              m_closeminute                   ; // [index:9][load:y] ·Ö
	int                              m_closesecond                   ; // [index:10][load:y] Ãë
	/*********************************/
	tab_activityopenserver();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_activityopenserver, m_id, m_name, m_openday, m_openhour, m_openminute, m_opensecond, m_closeday, m_closehour, m_closeminute, m_closesecond)
	// csvç›¸å…³
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
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	EActivityAlways                  m_type                          ; // [index:3][load:y] ³£×¤»î¶¯µÄÖÜÆÚÀàĞÍ
	int                              m_wbday                         ; // [index:4][load:y] ÖÜ¼¸(1-7)
	int                              m_wbhour                        ; // [index:5][load:y] Ê±
	int                              m_wbminute                      ; // [index:6][load:y] ·Ö
	int                              m_wbsecond                      ; // [index:7][load:y] Ãë
	int                              m_weday                         ; // [index:8][load:y] ÖÜ¼¸(1-7)
	int                              m_wehour                        ; // [index:9][load:y] Ê±
	int                              m_weminute                      ; // [index:10][load:y] ·Ö
	int                              m_wesecond                      ; // [index:11][load:y] Ãë
	int                              m_mbday                         ; // [index:12][load:y] Ã¿ÔÂ¼¸ºÅ(1-31)
	int                              m_mbhour                        ; // [index:13][load:y] Ê±
	int                              m_mbminute                      ; // [index:14][load:y] ·Ö
	int                              m_mbsecond                      ; // [index:15][load:y] Ãë
	int                              m_meday                         ; // [index:16][load:y] Ã¿ÔÂ¼¸ºÅ(1-31)
	int                              m_mehour                        ; // [index:17][load:y] Ê±
	int                              m_meminute                      ; // [index:18][load:y] ·Ö
	int                              m_mesecond                      ; // [index:19][load:y] Ãë
	int                              m_fixedday                      ; // [index:20][load:y] ³ÖĞøµ½µÚ¼¸Ìì
	int                              m_fbhour                        ; // [index:21][load:y] Ê±
	int                              m_fbminute                      ; // [index:22][load:y] ·Ö
	int                              m_fbsecond                      ; // [index:23][load:y] Ãë
	int                              m_fehour                        ; // [index:24][load:y] Ê±
	int                              m_feminute                      ; // [index:25][load:y] ·Ö
	int                              m_fesecond                      ; // [index:26][load:y] Ãë
	/*********************************/
	tab_activityalways();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_activityalways, m_id, m_name, m_type, m_wbday, m_wbhour, m_wbminute, m_wbsecond, m_weday, m_wehour, m_weminute, m_wesecond, m_mbday, m_mbhour, m_mbminute, m_mbsecond, m_meday, m_mehour, m_meminute, m_mesecond, m_fixedday, m_fbhour, m_fbminute, m_fbsecond, m_fehour, m_feminute, m_fesecond)
	// csvç›¸å…³
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
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	int32_t                          m_dropid                        ; // [index:3][load:y] µôÂäid
	int32_t                          m_mailid                        ; // [index:4][load:y] ÓÊ¼şid
	/*********************************/
	tab_activity_drawcompliance();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_activity_drawcompliance, m_id, m_name, m_dropid, m_mailid)
	// csvç›¸å…³
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
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	int32_t                          m_activityid                    ; // [index:3][load:y] »î¶¯id
	int32_t                          m_rank                          ; // [index:3][load:y] ÅÅÃû
	int32_t                          m_dropid                        ; // [index:4][load:y] µôÂäid
	int32_t                          m_mailid                        ; // [index:5][load:y] ÓÊ¼şid
	/*********************************/
	tab_activity_toprank();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_activity_toprank, m_id, m_name, m_activityid, m_rank, m_dropid, m_mailid)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_activityid,m_rank,m_dropid,m_mailid);
	}
};
struct task_condition
{
	/*********************************/
	ETask                            m_type                          ; // ETask(0.Íæ¼ÒµÈ¼¶´ïµ½X 1.Ö÷¹«vipµÈ¼¶´ïµ½x 2.Íê³ÉÄ³IDÈÎÎñ)
	ETaskCondition                   m_condition                     ; 
	int32_t                          m_parmint                       ; 
	/*********************************/
	task_condition();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(task_condition, m_type, m_condition, m_parmint)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_type,m_condition,m_parmint);
	}
};
struct tab_task
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	ETaskType                        m_type                          ; // [index:3][load:y] ÈÎÎñÀàĞÍ(0:Ö÷ÏßÈÎÎñ1:Ö§ÏßÈÎÎñ2:Ã¿ÈÕÈÎÎñ3:¿ÉÖØ¸´Íê³ÉµÄÈÎÎñ)
	std::string                      m_typeparm                      ; // [index:4][load:y] (m_type=2||=3 : ¿ÉÍê³É´ÎÊı)
	std::vector<task_condition>      m_taskreceive                   ; // [index:5][load:y] ½ÓÊÕ´ËÈÎÎñµÄÇ°Ìá(ETask(0.Íæ¼ÒµÈ¼¶´ïµ½X 1.Ö÷¹«vipµÈ¼¶´ïµ½x 2.Íê³ÉÄ³IDÈÎÎñ)*ETaskCondition(0.´óÓÚµÈÓÚ1.Ğ¡ÓÚµÈÓÚ2.µÈÓÚ)*int32_t(Öµ))
	std::vector<task_condition>      m_taskcomplete                  ; // [index:6][load:y] Íê³É´ËÈÎÎñµÄÌõ¼ş(ETask(0.Íæ¼ÒµÈ¼¶´ïµ½X 1.Ö÷¹«vipµÈ¼¶´ïµ½x 2.Íê³ÉÄ³IDÈÎÎñ)*ETaskCondition(0.´óÓÚµÈÓÚ1.Ğ¡ÓÚµÈÓÚ2.µÈÓÚ)*int32_t(Öµ))
	int32_t                          m_dropid                        ; // [index:7][load:y] ÈÎÎñ½±Àø
	bool                             m_autoreceive                   ; // [index:8][load:y] ÊÇ·ñ×Ô¶¯ÁìÈ¡
	int32_t                          m_mailid                        ; // [index:9][load:y] ×Ô¶¯ÁìÈ¡ºóÊÇ·ñ·¢ËÍÓÊ¼şµÄÓÊ¼şid(×Ô¶¯ÁìÈ¡µÄÓÊ¼şid:m_autoreceive == true,µ±m_autoreceiveÎªture¿ÉÒÔÎª-1)
	/*********************************/
	tab_task();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_task, m_id, m_name, m_type, m_typeparm, m_taskreceive, m_taskcomplete, m_dropid, m_autoreceive, m_mailid)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_type,m_typeparm,m_taskreceive,m_taskcomplete,m_dropid,m_autoreceive,m_mailid);
	}
};
struct obstacles_data
{
	/*********************************/
	std::vector<int32_t>             m_data                          ; //eobstacles
	/*********************************/
	obstacles_data();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(obstacles_data, m_data)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_data);
	}
};
struct tab_map
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	int32_t                          m_w                             ; // [index:3][load:y] xÖá¿í¶È
	int32_t                          m_l                             ; // [index:4][load:y] yÖá³¤¶È
	int32_t                          m_nx                            ; // [index:5][load:y] [ÊÓÒ°]xÖá¸ñ×ÓÊıÁ¿
	int32_t                          m_ny                            ; // [index:6][load:y] [ÊÓÒ°]yÖá¸ñ×ÓÊıÁ¿
	int32_t                          m_obstaclenx                    ; // [index:7][load:y] [Ñ°Â·]xÖá¸ñ×ÓÊıÁ¿
	int32_t                          m_obstacleny                    ; // [index:8][load:y] [Ñ°Â·]yÖá¸ñ×ÓÊıÁ¿
	std::vector<obstacles_data>      m_obstacles                     ; // [index:9][load:y] µØÍ¼Êı¾İ
	/*********************************/
	tab_map();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_map, m_id, m_name, m_w, m_l, m_nx, m_ny, m_obstaclenx, m_obstacleny, m_obstacles)
	// csvç›¸å…³
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
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	int32_t                          m_count                         ; // [index:3][load:y] Íæ·¨´ïµ½´ËÈËÊı¿ÉÒÔ¿ªÆôÍæ·¨
	int32_t                          m_time                          ; // [index:4][load:y] Æ¥Åä×î³¤Ê±¼ä
	bool                             m_iswaitconfirm                 ; // [index:5][load:y] ÊÇ·ñĞèÒªµÈ´ıÈ·ÈÏ
	int32_t                          m_waitconfirmtime               ; // [index:6][load:y] µÈ´ıÈ·ÈÏµÄÊ±¼ä
	/*********************************/
	tab_matching();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_matching, m_id, m_name, m_count, m_time, m_iswaitconfirm, m_waitconfirmtime)
	// csvç›¸å…³
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
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(Vector2, m_x, m_y)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		DEF_RCSV(m_x,m_y);
	}
};
struct tab_plays
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	int32_t                          m_type                          ; // [index:3][load:y] Íæ·¨ÀàĞÍ pbnet::eplays
	int32_t                          m_mapid                         ; // [index:4][load:y] ÔÚÄÄ¸öµØÍ¼Ó¦ÓÃ¸ÃÍæ·¨
	int32_t                          m_preparation_tm                ; // [index:5][load:y] ×¼±¸½×¶ÎÊ±¼ä
	int32_t                          m_play_tm                       ; // [index:6][load:y] Íæ·¨Ê±¼ä
	int32_t                          m_settlement_tm                 ; // [index:7][load:y] ½áËãÊ±¼ä
	std::vector<Vector2>             m_birthpoint                    ; // [index:8][load:y] ³öÉúµãVector2(m_x:int32_t,m_y:int32_t)	
	/*********************************/
	tab_plays();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_plays, m_id, m_name, m_type, m_mapid, m_preparation_tm, m_play_tm, m_settlement_tm, m_birthpoint)
	// csvç›¸å…³
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
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	int32_t                          m_price                         ; // [index:3][load:y] ¼Û¸ñ
	int32_t                          m_gold                          ; // [index:4][load:y] »ñµÃµÄ½ğ±ÒÊıÁ¿
	int32_t                          m_bonus                         ; // [index:5][load:y] ½±ÀøµÄ½ğ±ÒÊıÁ¿
	int32_t                          m_firstbonus                    ; // [index:6][load:y] Ê×³ä½±ÀøµÄ½ğ±ÒÊıÁ¿
	int32_t                          m_dropid                        ; // [index:7][load:y] µôÂä
	int32_t                          m_vipexp                        ; // [index:8][load:y] ³äÖµ¸ÃµµÎ»ÔùËÍµÄvip¾­Ñé
	int32_t                          m_count                         ; // [index:9][load:y] ÊÇ·ñÏŞÖÆ³äÖµ´ÎÊı
	std::vector<int32_t>             m_activityid                    ; // [index:10][load:y] ±ØĞëÖ¸¶¨»î¶¯¿ªÆôÊ±²ÅÄÜ³äÖµÕâÒ»µµ
	int32_t                          m_mailid                        ; // [index:11][load:y] ÓÊ¼şid
	/*********************************/
	tab_recharge();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_recharge, m_id, m_name, m_price, m_gold, m_bonus, m_firstbonus, m_dropid, m_vipexp, m_count, m_activityid, m_mailid)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_price,m_gold,m_bonus,m_firstbonus,m_dropid,m_vipexp,m_count,m_activityid,m_mailid);
	}
};
struct tab_familylv
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] ¾üÍÅµÈ¼¶ 
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	int32_t                          m_exp                           ; // [index:3][load:y] Éı¼¶ĞèÒªµÄ¾­Ñé
	int32_t                          m_maxmembers                    ; // [index:4][load:y] ×î¶à¾üÍÅÈËÊı
	/*********************************/
	tab_familylv();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_familylv, m_id, m_name, m_exp, m_maxmembers)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_exp,m_maxmembers);
	}
};
struct tab_mergearea
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] Çø·şid
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	int32_t                          m_mergeid                       ; // [index:3][load:y] ºÏ²¢µ½µÄÇø·şid
	/*********************************/
	tab_mergearea();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_mergearea, m_id, m_name, m_mergeid)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_mergeid);
	}
};
struct tab_ranklist
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] eranklist(0:µÈ¼¶ÅÅĞĞ)
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	int32_t                          m_maxitem                       ; // [index:3][load:y] ×î´óÅÅĞĞ½áµãÊıÁ¿
	int32_t                          m_showitem                      ; // [index:4][load:y] Õ¹Ê¾ÅÅĞĞ½áµãÊıÁ¿
	int32_t                          m_everypagecount                ; // [index:5][load:y] Ã¿Ò³Õ¹Ê¾½áµãÊıÁ¿
	int32_t                          m_minvalue                      ; // [index:6][load:y] ×îĞ¡Èë°ñÖµ
	/*********************************/
	tab_ranklist();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_ranklist, m_id, m_name, m_maxitem, m_showitem, m_everypagecount, m_minvalue)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_maxitem,m_showitem,m_everypagecount,m_minvalue);
	}
};
struct tab_filterword
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] ÆÁ±Î×Öid
	std::string                      m_name                          ; // [index:1][load:y] Ãû×Ö 
//	std::string                      m_remarks                       ; // [index:2][load:n] ±¸×¢
	std::string                      m_content                       ; // [index:3][load:y] ÆÁ±Î×Ö´® 
	/*********************************/
	tab_filterword();
	// åºåˆ—åŒ–ååºåˆ—åŒ–ç›¸å…³
	DPROTOCOL(tab_filterword, m_id, m_name, m_content)
	// csvç›¸å…³
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		DEF_RCSV(m_id,m_name,lm_remarks,m_content);
	}
};
}//namespace ngl
