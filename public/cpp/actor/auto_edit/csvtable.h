// 注意【IDL 工具生成文件，不要手动修改】
// 创建时间 // 创建时间 25-06-26 20:09:09
#pragma once

#include "csv.h"
#include "ndefine.h"
#include "serialize.h"
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
	EUDP_OPERATOR_ACK,	// 确认包
	EUDP_OPERATOR_DATA,	// 传输子数据包
	EUDP_OPERATOR_HEARTBEAT,	// 心跳主要用来确认对端是否在线
};
enum EPH_HEAD_VAL
{
	EPH_HEAD_MASK_SUCCESS = 1,	// MASK一致
	EPH_HEAD_MASK_FAIL = 2,	// MASK不一致
	EPH_HEAD_FOLLOW = 3,	// 包头数据没有接收完成
	EPH_HEAD_SUCCESS = 4,	// 包头数据已接收完成
};
enum EPH
{
	EPH_MASK_VALUE = 0xff,	
	EPH_MASK_COUNT = 2,	
	EPH_MASK = 0,	// 用于确认是否使用包
	EPH_BYTES = EPH_MASK_COUNT,	// 协议字节数
	EPH_TIME,	// 发送端的时间戳
	EPH_PROTOCOLNUM,	// 协议号
	EPH_PROTOCOLTYPE,	// 协议类型 EPROTOCOL_TYPE
	EPH_ACTOR_TYPEAREA,	// ACTOR_TYPE_AREA
	EPH_ACTOR_ID,	// ACTOR_ID
	EPH_REQUEST_ACTOR_TYPEAREA,	// Request ACTOR_TYPE_AREA
	EPH_REQUEST_ACTOR_ID,	// Request ACTOR_ID
	EPH_SUM,	
};
enum NODE_TYPE
{
	DB = 1,	// 数据库服务器
	ACTORSERVER = 2,	// actor 服务器
	GAME = 3,	// 逻辑服务器
	GATEWAY = 4,	// 网关服务器
	LOGIN = 5,	// 登录服务器
	ROBOT = 6,	// 测试机器人服务器
	WORLD = 7,	// 世界服务器
	LOG = 8,	// 日志服务器
	RELOADCSV = 9,	// 重载分发csv服务器
	CROSS = 10,	// 跨服服务器
	CROSSDB = 11,	// 跨服数据库服务器
	PUSHSERVERCONFIG = 12,	// 将服务器配置上传lbgmsys
	NODE_TYPE_COUNT,	
	FAIL = -1,	// 服务器类型错误
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
	E_ModuleNull = 0,	// 空	
	E_ModuleRoot = 1,	// 卡牌(链尾)
	E_ModuleBuff = 2,	// 玩家buff
	E_ModuleEquip = 3,	// 装备
	E_ModuleEquipWeapon = 4,	// 装备武器
	E_ModuleEquipHelmet = 5,	// 装备头盔
	E_ModuleEquipCloth = 6,	// 装备衣服
	E_ModuleEquipPants = 7,	// 装备护腿
	E_ModuleCount = 8,	// 数量 	
};
enum EnumAttribute
{
	E_Attack,	// [攻击]
	E_Defense,	// [防御]
	E_Hp,	// [血量]
	E_Anger,	// [怒气]
	E_Speed,	// [速度]
	E_Count,	
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
	EEquipment = 1,	// 装备
	EMaterial = 2,	// 材料
	EMoneyGold = 3,	// 金币
	EMoneySilver = 4,	// 银币
	ECard = 5,	// 卡牌
};
enum EQuality
{
	EQualityGrey = 0,	//灰色
	EQualityGreen = 1,	//绿色
	EQualityYellow = 2,	//黄色
	EQualityOrange = 3,	//橙色
	EQualityRed = 4,	//红色
};
enum EEquip
{
	EEquipWeapon = 1,	// 武器
	EEquipHelmet = 2,	// 头盔
	EEquipCloth = 3,	// 衣服
	EEquipPants = 4,	// 护腿
};
enum ECalendar
{
	Week = 0,	// 周几
	ServerOpen = 1,	// 以开服时间以来的天数
	RegularSlot = 2,	// 固定时间段
};
enum ECalendarType
{
	ECalendarTypeNull,	// 无
	ECalendarTypeActivity,	// 活动开启与关闭
	ECalendarTypeCount,	
};
enum EActivity
{
	EActivityDrawCompliance = 1,	// 类似咸鱼之王的<<招募达标>>
	EActivityTopRank = 2,	// 冲榜
};
enum EActivityOpen
{
	EActivityOpenGm = 0,	// Gm开启
	EActivityOpenAlways = 1,	// 常驻活动
};
enum EActivityAlways
{
	EActivityAlwaysNull = 0,	
	EActivityAlwaysWeek = 1,	// 周
	EActivityAlwaysMonth = 2,	// 月
	EActivityAlwaysFixed = 3,	// 间隔固定时间
};
enum ETask
{
	ETaskRoleLv = 0,	// 玩家等级
	ETaskRoleVip = 1,	// 玩家vip等级
	ETaskTaskId = 2,	// 完成某ID任务
	ETaskRoleLogin = 3,	// 累计登陆
	ETaskConsumeGold = 4,	// 累计消耗金币
	ETaskCount,	
};
enum ETaskCondition
{
	ETaskConditionMore = 0,	// 大于等于
	ETaskConditionLess = 1,	// 小于等于
	ETaskConditionEqual = 2,	// 等于
	ETaskConditionCount,	
};
enum ETaskType
{
	ETaskTypePrincipalLine = 0,	// 主线任务
	ETaskTypeBranchLine = 1,	// 支线任务
	ETaskTypeDaily = 2,	// 每日任务
	ETaskTypeRepeat = 3,	// 可重复完成的任务
};
enum eobstacles
{
	eobstacles_pass = 0,	// 可以通过
	eobstacles_nopass = 1,	// 无法通过
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
	def_portocol(net_works, m_type, m_ip, m_nip, m_port)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_type,m_ip,m_nip,m_port);
	}
};
struct tab_servers
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int16_t                          m_area                          ; // [index:3][load:y] 区服
	NODE_TYPE                        m_type                          ; // [index:4][load:y] 服务器类型(1、DB(数据库服务器)2、ACTORSERVER(actor 服务器))3、GAME(逻辑服务器)4、GATEWAY(网关服务器)5、LOGIN(登录服务器)6、ROBOT(测试机器人服务器)7、WORLD(世界服务器)8、LOG(日志服务器)9、RELOADCSV(重载分发csv服务器)10、CROSS(跨服服务器)11、CROSSDB(跨服数据库服务器)12、PUSHSERVERCONFIG(将服务器配置上传lbgmsys))
	int32_t                          m_tcount                        ; // [index:5][load:y] 同类型服务器的序号
	int32_t                          m_threadnum                     ; // [index:6][load:y] socket线程数
	int32_t                          m_actorthreadnum                ; // [index:7][load:y] actor线程池线程数
	bool                             m_outernet                      ; // [index:8][load:y] 是否允许外网访问
	int32_t                          m_db                            ; // [index:9][load:y] 连接的db进程id
	int32_t                          m_reloadcsv                     ; // [index:10][load:y] 连接的reloadcsv进程id
	int32_t                          m_login                         ; // [index:11][load:y] 连接的login进程id
	int16_t                          m_crossarea                     ; // [index:12][load:y] 跨服区服
	std::vector<int32_t>             m_actorserver                   ; // [index:13][load:y] 连接的actorserver进程id(跨服需要填写多个actorserver)
	std::vector<net_works>           m_net                           ; // [index:14][load:y] 服务器网络相关(net_works:m_type(0tcp1ws2kcp),m_ip,m_nip,m_port)
	/*********************************/
	tab_servers();
	// 序列化反序列化相关
	def_portocol(tab_servers, m_id, m_name, m_area, m_type, m_tcount, m_threadnum, m_actorthreadnum, m_outernet, m_db, m_reloadcsv, m_login, m_crossarea, m_actorserver, m_net)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_area,m_type,m_tcount,m_threadnum,m_actorthreadnum,m_outernet,m_db,m_reloadcsv,m_login,m_crossarea,m_actorserver,m_net);
	}
};
struct tab_dbload
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	bool                             m_isloadall                     ; // [index:3][load:y] 是否需要启动加载全部数据
	bool                             m_network                       ; // [index:4][load:y] 是否允许通过网络分发
	int32_t                          m_sendmaxcount                  ; // [index:5][load:y] 单次最多发送数量
	int32_t                          m_dbcacheintervalms             ; // [index:6][load:y] 数据缓存入库间隔
	/*********************************/
	tab_dbload();
	// 序列化反序列化相关
	def_portocol(tab_dbload, m_id, m_name, m_isloadall, m_network, m_sendmaxcount, m_dbcacheintervalms)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_isloadall,m_network,m_sendmaxcount,m_dbcacheintervalms);
	}
};
struct tab_errormessage
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	std::string                      m_message                       ; // [index:3][load:y] 错误描述
	/*********************************/
	tab_errormessage();
	// 序列化反序列化相关
	def_portocol(tab_errormessage, m_id, m_name, m_message)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_message);
	}
};
struct tab_specialid
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1[load:y]] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	std::string                      m_value                         ; // [index:3][load:y] 值
	/*********************************/
	tab_specialid();
	// 序列化反序列化相关
	def_portocol(tab_specialid, m_id, m_name, m_value)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_value);
	}
};
struct tab_attribute
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	float                            m_fightcoefficient              ; // [index:3][load:y] 属性提供的战力系数
	int32_t                          m_initvalue                     ; // [index:4][load:y] 属性初始值
	int32_t                          m_uplimit                       ; // [index:5][load:y] 上限
	int32_t                          m_lowlimit                      ; // [index:6][load:y] 下限
	/*********************************/
	tab_attribute();
	// 序列化反序列化相关
	def_portocol(tab_attribute, m_id, m_name, m_fightcoefficient, m_initvalue, m_uplimit, m_lowlimit)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_fightcoefficient,m_initvalue,m_uplimit,m_lowlimit);
	}
};
struct tab_item
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int32_t                          m_ranks                         ; // [index:3][load:y] 排序值
	EItemType                        m_type                          ; // [index:4][load:y] 物品类型EItemType(1装备,2材料,3金币,4银币)
	int32_t                          m_quality                       ; // [index:5][load:y] 物品品质(EQuality)
	bool                             m_isstack                       ; // [index:6][load:y] 是否可堆叠
	/*********************************/
	tab_item();
	// 序列化反序列化相关
	def_portocol(tab_item, m_id, m_name, m_ranks, m_type, m_quality, m_isstack)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_ranks,m_type,m_quality,m_isstack);
	}
};
struct attributevalue
{
	/*********************************/
	int32_t                          m_id                            ; // 属性id(EnumAttribute)
	bool                             m_isratio                       ; // 是否万分比属性
	int32_t                          m_value                         ; // 属性值
	/*********************************/
	attributevalue();
	// 序列化反序列化相关
	def_portocol(attributevalue, m_id, m_isratio, m_value)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_id,m_isratio,m_value);
	}
};
struct tab_equip
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	EEquip                           m_position                      ; // [index:3][load:y] 部位EEquip(1武器2头盔3衣服4护腿)
	std::vector<attributevalue>      m_attributevalues               ; // [index:4][load:y] 装备属性
	/*********************************/
	tab_equip();
	// 序列化反序列化相关
	def_portocol(tab_equip, m_id, m_name, m_position, m_attributevalues)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_position,m_attributevalues);
	}
};
struct tab_equiplv
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int32_t                          m_consume                       ; // [index:3][load:y] 升级消耗id
	std::vector<attributevalue>      m_attributevalues               ; // [index:4][load:y] 装备属性
	/*********************************/
	tab_equiplv();
	// 序列化反序列化相关
	def_portocol(tab_equiplv, m_id, m_name, m_consume, m_attributevalues)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_consume,m_attributevalues);
	}
};
struct tab_card
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	EQuality                         m_quality                       ; // [index:3][load:y] 品质EQuality(0灰色1绿色2黄色3橙色4红色)
	std::vector<attributevalue>      m_attributevalues               ; // [index:4][load:y] 属性
	/*********************************/
	tab_card();
	// 序列化反序列化相关
	def_portocol(tab_card, m_id, m_name, m_quality, m_attributevalues)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_quality,m_attributevalues);
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
	def_portocol(trandom, m_id, m_min, m_max, m_weight)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_id,m_min,m_max,m_weight);
	}
};
struct tab_random
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	std::vector<trandom>             m_randomdatas                   ; // [index:3][load:y] trandom(id*min*max*weight)
	bool                             m_exclusive                     ; // [index:4][load:y] 排他性多次掉落时使用（true不会掉落出已掉落物品 false会掉落出已掉落物品）
	int32_t                          m_count                         ; // [index:5][load:y] 随机数量
	std::set<int32_t>                m_activityids                   ; // [index:6][load:y] 只有活动开启才会触发此掉落
	std::vector<int32_t>             m_childrandomids                ; // [index:7][load:y] 子随机
	/*********************************/
	tab_random();
	// 序列化反序列化相关
	def_portocol(tab_random, m_id, m_name, m_randomdatas, m_exclusive, m_count, m_activityids, m_childrandomids)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_randomdatas,m_exclusive,m_count,m_activityids,m_childrandomids);
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
	def_portocol(tconsume, m_tid, m_count)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_tid,m_count);
	}
};
struct tab_consume
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	std::vector<tconsume>            m_consumeitems                  ; // [index:3][load:y] tid*count
	/*********************************/
	tab_consume();
	// 序列化反序列化相关
	def_portocol(tab_consume, m_id, m_name, m_consumeitems)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_consumeitems);
	}
};
struct tab_synthesis
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int32_t                          m_consume                       ; // [index:3][load:y] 消耗列表
	int32_t                          m_generate                      ; // [index:4][load:y] 随机表(tab_rando)id 随机成功物品列表
	/*********************************/
	tab_synthesis();
	// 序列化反序列化相关
	def_portocol(tab_synthesis, m_id, m_name, m_consume, m_generate)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_consume,m_generate);
	}
};
struct tweek
{
	/*********************************/
	int32_t                          m_weekstart                     ; // 周几开始
	std::string                      m_opentime                      ; // 开启时间 HH:mm:ss
	int32_t                          m_weekfinish                    ; // 周几结束
	std::string                      m_closetime                     ; // 结束时间 HH:mm:ss
	/*********************************/
	tweek();
	// 序列化反序列化相关
	def_portocol(tweek, m_weekstart, m_opentime, m_weekfinish, m_closetime)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_weekstart,m_opentime,m_weekfinish,m_closetime);
	}
};
struct tserveropen
{
	/*********************************/
	int32_t                          m_beg                           ; // 开服后多少天开启
	std::string                      m_opentime                      ; // 开启时间 HH:mm:ss
	int32_t                          m_end                           ; // 开服后多少天结束
	std::string                      m_closetime                     ; // 结束时间 HH:mm:ss
	/*********************************/
	tserveropen();
	// 序列化反序列化相关
	def_portocol(tserveropen, m_beg, m_opentime, m_end, m_closetime)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_beg,m_opentime,m_end,m_closetime);
	}
};
struct tregularslot
{
	/*********************************/
	std::string                      m_opentime                      ; // 开启时间 YYYY-MM-DD HH:mm:ss
	std::string                      m_closetime                     ; // 结束时间 YYYY-MM-DD HH:mm:ss
	/*********************************/
	tregularslot();
	// 序列化反序列化相关
	def_portocol(tregularslot, m_opentime, m_closetime)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_opentime,m_closetime);
	}
};
struct tab_mail
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	std::string                      m_title                         ; // [index:3][load:y] 邮件标题
	std::string                      m_author                        ; // [index:4][load:y] 邮件作者
	std::string                      m_content                       ; // [index:5][load:y] 邮件内容
	/*********************************/
	tab_mail();
	// 序列化反序列化相关
	def_portocol(tab_mail, m_id, m_name, m_title, m_author, m_content)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_title,m_author,m_content);
	}
};
struct tab_chat
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int32_t                          m_time                          ; // [index:3][load:y] 聊天发言间隔
	int32_t                          m_count                         ; // [index:4][load:y] 聊天发言最大数量
	/*********************************/
	tab_chat();
	// 序列化反序列化相关
	def_portocol(tab_chat, m_id, m_name, m_time, m_count)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_time,m_count);
	}
};
struct activity_task
{
	/*********************************/
	int32_t                          m_begday                        ; // 第几天(从1开始)接收
	int32_t                          m_endday                        ; // 第几天(从1开始)移除(-1代表活动结束)
	std::vector<int32_t>             m_taskids                       ; // 任务列表
	/*********************************/
	activity_task();
	// 序列化反序列化相关
	def_portocol(activity_task, m_begday, m_endday, m_taskids)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_begday,m_endday,m_taskids);
	}
};
struct tab_activity
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	EActivity                        m_type                          ; // [index:3][load:y] 活动类型(1:类似咸鱼之王的<<招募达标>>2:冲榜)
	EActivityOpen                    m_open                          ; // [index:4][load:y] 开启方式(0:GM开启1:常驻活动)
	std::vector<int32_t>             m_task                          ; // [index:5][load:y] 活动期间接取任务
	std::vector<activity_task>       m_taskday                       ; // [index:6][load:y] 活动期间接取任务(第几天接收任务)
	std::vector<int32_t>             m_ranktype                      ; // [index:7][load:y] 支持的活动排行列表(eranklist.activity_lv:1000000活动期间玩家等级变化值,eranklist.activity_gold:2000000活动期间玩家金币增加值)
	/*********************************/
	tab_activity();
	// 序列化反序列化相关
	def_portocol(tab_activity, m_id, m_name, m_type, m_open, m_task, m_taskday, m_ranktype)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_type,m_open,m_task,m_taskday,m_ranktype);
	}
};
struct tab_activityopenserver
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int                              m_openday                       ; // [index:3][load:y] 开服第几天开启（开服当天算为第一天）
	int                              m_openhour                      ; // [index:4][load:y] 时
	int                              m_openminute                    ; // [index:5][load:y] 分
	int                              m_opensecond                    ; // [index:6][load:y] 秒
	int                              m_closeday                      ; // [index:7][load:y] 开服第几天开启（开服当天算为第一天）
	int                              m_closehour                     ; // [index:8][load:y] 时
	int                              m_closeminute                   ; // [index:9][load:y] 分
	int                              m_closesecond                   ; // [index:10][load:y] 秒
	/*********************************/
	tab_activityopenserver();
	// 序列化反序列化相关
	def_portocol(tab_activityopenserver, m_id, m_name, m_openday, m_openhour, m_openminute, m_opensecond, m_closeday, m_closehour, m_closeminute, m_closesecond)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_openday,m_openhour,m_openminute,m_opensecond,m_closeday,m_closehour,m_closeminute,m_closesecond);
	}
};
struct tab_activityalways
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	EActivityAlways                  m_type                          ; // [index:3][load:y] 常驻活动的周期类型
	int                              m_wbday                         ; // [index:4][load:y] 周几(1-7)
	int                              m_wbhour                        ; // [index:5][load:y] 时
	int                              m_wbminute                      ; // [index:6][load:y] 分
	int                              m_wbsecond                      ; // [index:7][load:y] 秒
	int                              m_weday                         ; // [index:8][load:y] 周几(1-7)
	int                              m_wehour                        ; // [index:9][load:y] 时
	int                              m_weminute                      ; // [index:10][load:y] 分
	int                              m_wesecond                      ; // [index:11][load:y] 秒
	int                              m_mbday                         ; // [index:12][load:y] 每月几号(1-31)
	int                              m_mbhour                        ; // [index:13][load:y] 时
	int                              m_mbminute                      ; // [index:14][load:y] 分
	int                              m_mbsecond                      ; // [index:15][load:y] 秒
	int                              m_meday                         ; // [index:16][load:y] 每月几号(1-31)
	int                              m_mehour                        ; // [index:17][load:y] 时
	int                              m_meminute                      ; // [index:18][load:y] 分
	int                              m_mesecond                      ; // [index:19][load:y] 秒
	int                              m_fixedday                      ; // [index:20][load:y] 持续到第几天
	int                              m_fbhour                        ; // [index:21][load:y] 时
	int                              m_fbminute                      ; // [index:22][load:y] 分
	int                              m_fbsecond                      ; // [index:23][load:y] 秒
	int                              m_fehour                        ; // [index:24][load:y] 时
	int                              m_feminute                      ; // [index:25][load:y] 分
	int                              m_fesecond                      ; // [index:26][load:y] 秒
	/*********************************/
	tab_activityalways();
	// 序列化反序列化相关
	def_portocol(tab_activityalways, m_id, m_name, m_type, m_wbday, m_wbhour, m_wbminute, m_wbsecond, m_weday, m_wehour, m_weminute, m_wesecond, m_mbday, m_mbhour, m_mbminute, m_mbsecond, m_meday, m_mehour, m_meminute, m_mesecond, m_fixedday, m_fbhour, m_fbminute, m_fbsecond, m_fehour, m_feminute, m_fesecond)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_type,m_wbday,m_wbhour,m_wbminute,m_wbsecond,m_weday,m_wehour,m_weminute,m_wesecond,m_mbday,m_mbhour,m_mbminute,m_mbsecond,m_meday,m_mehour,m_meminute,m_mesecond,m_fixedday,m_fbhour,m_fbminute,m_fbsecond,m_fehour,m_feminute,m_fesecond);
	}
};
struct tab_activity_drawcompliance
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int32_t                          m_dropid                        ; // [index:3][load:y] 掉落id
	int32_t                          m_mailid                        ; // [index:4][load:y] 邮件id
	/*********************************/
	tab_activity_drawcompliance();
	// 序列化反序列化相关
	def_portocol(tab_activity_drawcompliance, m_id, m_name, m_dropid, m_mailid)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_dropid,m_mailid);
	}
};
struct tab_activity_toprank
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int32_t                          m_activityid                    ; // [index:3][load:y] 活动id
	int32_t                          m_rank                          ; // [index:3][load:y] 排名
	int32_t                          m_dropid                        ; // [index:4][load:y] 掉落id
	int32_t                          m_mailid                        ; // [index:5][load:y] 邮件id
	/*********************************/
	tab_activity_toprank();
	// 序列化反序列化相关
	def_portocol(tab_activity_toprank, m_id, m_name, m_activityid, m_rank, m_dropid, m_mailid)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_activityid,m_rank,m_dropid,m_mailid);
	}
};
struct task_condition
{
	/*********************************/
	ETask                            m_type                          ; // ETask(0.玩家等级达到X 1.主公vip等级达到x 2.完成某ID任务)
	ETaskCondition                   m_condition                     ; 
	int32_t                          m_parmint                       ; 
	/*********************************/
	task_condition();
	// 序列化反序列化相关
	def_portocol(task_condition, m_type, m_condition, m_parmint)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_type,m_condition,m_parmint);
	}
};
struct tab_task
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	ETaskType                        m_type                          ; // [index:3][load:y] 任务类型(0:主线任务1:支线任务2:每日任务3:可重复完成的任务)
	std::string                      m_typeparm                      ; // [index:4][load:y] (m_type=2||=3 : 可完成次数)
	std::vector<task_condition>      m_taskreceive                   ; // [index:5][load:y] 接收此任务的前提(ETask(0.玩家等级达到X 1.主公vip等级达到x 2.完成某ID任务)*ETaskCondition(0.大于等于1.小于等于2.等于)*int32_t(值))
	std::vector<task_condition>      m_taskcomplete                  ; // [index:6][load:y] 完成此任务的条件(ETask(0.玩家等级达到X 1.主公vip等级达到x 2.完成某ID任务)*ETaskCondition(0.大于等于1.小于等于2.等于)*int32_t(值))
	int32_t                          m_dropid                        ; // [index:7][load:y] 任务奖励
	bool                             m_autoreceive                   ; // [index:8][load:y] 是否自动领取
	int32_t                          m_mailid                        ; // [index:9][load:y] 自动领取后是否发送邮件的邮件id(自动领取的邮件id:m_autoreceive == true,当m_autoreceive为ture可以为-1)
	/*********************************/
	tab_task();
	// 序列化反序列化相关
	def_portocol(tab_task, m_id, m_name, m_type, m_typeparm, m_taskreceive, m_taskcomplete, m_dropid, m_autoreceive, m_mailid)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_type,m_typeparm,m_taskreceive,m_taskcomplete,m_dropid,m_autoreceive,m_mailid);
	}
};
struct obstacles_data
{
	/*********************************/
	std::vector<eobstacles>          m_data                          ; 
	/*********************************/
	obstacles_data();
	// 序列化反序列化相关
	def_portocol(obstacles_data, m_data)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_data);
	}
};
struct tab_map
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int32_t                          m_w                             ; // [index:3][load:y] x轴宽度
	int32_t                          m_l                             ; // [index:4][load:y] y轴长度
	int32_t                          m_nx                            ; // [index:5][load:y] [视野]x轴格子数量
	int32_t                          m_ny                            ; // [index:6][load:y] [视野]y轴格子数量
	int32_t                          m_obstaclenx                    ; // [index:7][load:y] [寻路]x轴格子数量
	int32_t                          m_obstacleny                    ; // [index:8][load:y] [寻路]y轴格子数量
	std::vector<obstacles_data>      m_obstacles                     ; // [index:9][load:y] 地图数据
	/*********************************/
	tab_map();
	// 序列化反序列化相关
	def_portocol(tab_map, m_id, m_name, m_w, m_l, m_nx, m_ny, m_obstaclenx, m_obstacleny, m_obstacles)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_w,m_l,m_nx,m_ny,m_obstaclenx,m_obstacleny,m_obstacles);
	}
};
struct tab_matching
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int32_t                          m_count                         ; // [index:3][load:y] 玩法达到此人数可以开启玩法
	int32_t                          m_time                          ; // [index:4][load:y] 匹配最长时间
	bool                             m_iswaitconfirm                 ; // [index:5][load:y] 是否需要等待确认
	int32_t                          m_waitconfirmtime               ; // [index:6][load:y] 等待确认的时间
	/*********************************/
	tab_matching();
	// 序列化反序列化相关
	def_portocol(tab_matching, m_id, m_name, m_count, m_time, m_iswaitconfirm, m_waitconfirmtime)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_count,m_time,m_iswaitconfirm,m_waitconfirmtime);
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
	def_portocol(Vector2, m_x, m_y)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_x,m_y);
	}
};
struct tab_plays
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int32_t                          m_type                          ; // [index:3][load:y] 玩法类型 pbnet::eplays
	int32_t                          m_mapid                         ; // [index:4][load:y] 在哪个地图应用该玩法
	int32_t                          m_preparation_tm                ; // [index:5][load:y] 准备阶段时间
	int32_t                          m_play_tm                       ; // [index:6][load:y] 玩法时间
	int32_t                          m_settlement_tm                 ; // [index:7][load:y] 结算时间
	std::vector<Vector2>             m_birthpoint                    ; // [index:8][load:y] 出生点Vector2(m_x:int32_t,m_y:int32_t)	
	/*********************************/
	tab_plays();
	// 序列化反序列化相关
	def_portocol(tab_plays, m_id, m_name, m_type, m_mapid, m_preparation_tm, m_play_tm, m_settlement_tm, m_birthpoint)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_type,m_mapid,m_preparation_tm,m_play_tm,m_settlement_tm,m_birthpoint);
	}
};
struct tab_recharge
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int32_t                          m_price                         ; // [index:3][load:y] 价格
	int32_t                          m_gold                          ; // [index:4][load:y] 获得的金币数量
	int32_t                          m_bonus                         ; // [index:5][load:y] 奖励的金币数量
	int32_t                          m_firstbonus                    ; // [index:6][load:y] 首充奖励的金币数量
	int32_t                          m_dropid                        ; // [index:7][load:y] 掉落
	int32_t                          m_vipexp                        ; // [index:8][load:y] 充值该档位赠送的vip经验
	int32_t                          m_count                         ; // [index:9][load:y] 是否限制充值次数
	std::vector<int32_t>             m_activityid                    ; // [index:10][load:y] 必须指定活动开启时才能充值这一档
	int32_t                          m_mailid                        ; // [index:11][load:y] 邮件id
	/*********************************/
	tab_recharge();
	// 序列化反序列化相关
	def_portocol(tab_recharge, m_id, m_name, m_price, m_gold, m_bonus, m_firstbonus, m_dropid, m_vipexp, m_count, m_activityid, m_mailid)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_price,m_gold,m_bonus,m_firstbonus,m_dropid,m_vipexp,m_count,m_activityid,m_mailid);
	}
};
struct tab_familylv
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] 军团等级 
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int32_t                          m_exp                           ; // [index:3][load:y] 升级需要的经验
	int32_t                          m_maxmembers                    ; // [index:4][load:y] 最多军团人数
	/*********************************/
	tab_familylv();
	// 序列化反序列化相关
	def_portocol(tab_familylv, m_id, m_name, m_exp, m_maxmembers)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_exp,m_maxmembers);
	}
};
struct tab_mergearea
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] 区服id
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int32_t                          m_mergeid                       ; // [index:3][load:y] 合并到的区服id
	/*********************************/
	tab_mergearea();
	// 序列化反序列化相关
	def_portocol(tab_mergearea, m_id, m_name, m_mergeid)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_mergeid);
	}
};
struct tab_ranklist
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] eranklist(0:等级排行)
	std::string                      m_name                          ; // [index:1][load:y] 名字 
//	std::string                      m_remarks                       ; // [index:2][load:n] 备注
	int32_t                          m_maxitem                       ; // [index:3][load:y] 最大排行结点数量
	int32_t                          m_showitem                      ; // [index:4][load:y] 展示排行结点数量
	int32_t                          m_everypagecount                ; // [index:5][load:y] 每页展示结点数量
	int32_t                          m_minvalue                      ; // [index:6][load:y] 最小入榜值
	/*********************************/
	tab_ranklist();
	// 序列化反序列化相关
	def_portocol(tab_ranklist, m_id, m_name, m_maxitem, m_showitem, m_everypagecount, m_minvalue)
	// csv相关
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_maxitem,m_showitem,m_everypagecount,m_minvalue);
	}
};
}//namespace ngl
