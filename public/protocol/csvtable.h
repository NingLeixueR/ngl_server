// 注意【rebuild.bat 工具生成文件，不要手动修改】
// 创建时间 // 创建时间 23-11-25 20:38:24
#ifndef _csvtable_H_
#define _csvtable_H_
#include "actor_define.h"
#include "csv.h"
#include "csvpair.h"
#include "protocol_define.h"
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
enum EPH
{
	EPH_BYTES = 0,	// 协议字节数
	EPH_TIME,	// 发送端的时间戳
	EPH_PROTOCOLNUM,	// 协议号
	EPH_PROTOCOLTYPE,	// 协议类型 EPROTOCOL_TYPE
	EPH_ACTOR_TYPEAREA,	// ACTOR_TYPE_AREA
	EPH_ACTOR_ID,	// ACTOR_ID
	EPH_REQUEST_ACTOR_TYPEAREA,	// Request REQUEST_ACTOR_TYPE_AREA
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
	FAIL = -1,	// 服务器类型错误
};
enum ENET_PROTOCOL
{
	ENET_TCP = 1,	
	ENET_UDP = 2,	
	ENET_WS = 3,	
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
	E_Null = 0,	
	E_Attack = 1,	// [攻击]
	E_Defense = 2,	// [防御]
	E_Hp = 3,	// [血量]
	E_Anger = 4,	// [怒气]
	E_Speed = 5,	// [速度]
	E_Count = 6,	
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
enum EActivity
{
	EActivityDrawCompliance = 1,	// 类似咸鱼之王的<<招募达标>>
};
enum ETask
{
	ETaskRoleLv = 1,	// 玩家等级
	ETaskRoleVip = 2,	// 玩家vip等级
	ETaskTaskId = 3,	// 完成某ID任务
};
enum ETaskType
{
	ETaskTypePrincipalLine,	// 主线任务
	ETaskTypeBranchLine,	// 支线任务
	ETaskTypeDaily,	// 每日任务
};
enum eobstacles
{
	eobstacles_pass = 0,	// 可以通过
	eobstacles_nopass = 1,	// 无法通过
};
struct tab_servers
{
/*********************************/
	int32_t		m_id;		
	std::string		m_name;		
	std::string		m_remarks;		
	int16_t		m_area;		// 区服
	NODE_TYPE		m_type;		// 服务器类型(1db2actorserver3game4gateway5login6robot7world8log9reloadcsv10reloadcsv_tools)
	int32_t		m_tcount;		// 同类型服务器的序号
	ENET_PROTOCOL		m_net;		// 服务器协议(1tcp2udp3ws)
	std::string		m_ip;		// ip
	int16_t		m_port;		// 端口
	int32_t		m_threadnum;		// socket线程数
	int32_t		m_actorthreadnum;		// actor线程池线程数
	bool		m_outernet;		// 是否允许外网访问
	int32_t		m_db;		// 连接的db进程id
	int32_t		m_reloadcsv;		// 连接的reloadcsv进程id
	int32_t		m_login;		// 连接的login进程id
	int16_t		m_crossarea;		// 跨服区服
	std::vector<int32_t>		m_actorserver;		// 连接的actorserver进程id(跨服需要填写多个actorserver)
/*********************************/
	tab_servers();
	// 序列化反序列化相关
	def_portocol(tab_servers, m_id, m_name, m_remarks, m_area, m_type, m_tcount, m_net, m_ip, m_port, m_threadnum, m_actorthreadnum, m_outernet, m_db, m_reloadcsv, m_login, m_crossarea, m_actorserver)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_area,m_type,m_tcount,m_net,m_ip,m_port,m_threadnum,m_actorthreadnum,m_outernet,m_db,m_reloadcsv,m_login,m_crossarea,m_actorserver)
};
struct tab_dbload
{
/*********************************/
	int32_t		m_id;		
	std::string		m_name;		
	std::string		m_remarks;		
	bool		m_isloadall;		// 是否需要启动加载全部数据
	bool		m_network;		// 是否允许通过网络分发
	int		m_sendmaxcount;		// 单次最多发送数量
/*********************************/
	tab_dbload();
	// 序列化反序列化相关
	def_portocol(tab_dbload, m_id, m_name, m_remarks, m_isloadall, m_network, m_sendmaxcount)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_isloadall,m_network,m_sendmaxcount)
};
struct tab_errormessage
{
/*********************************/
	int32_t		m_id;		
	std::string		m_name;		
	std::string		m_remarks;		
	std::string		m_message;		// 错误描述
/*********************************/
	tab_errormessage();
	// 序列化反序列化相关
	def_portocol(tab_errormessage, m_id, m_name, m_remarks, m_message)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_message)
};
struct tab_specialid
{
/*********************************/
	int32_t		m_id;		
	std::string		m_name;		
	std::string		m_remarks;		
	std::string		m_value;		
/*********************************/
	tab_specialid();
	// 序列化反序列化相关
	def_portocol(tab_specialid, m_id, m_name, m_remarks, m_value)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_value)
};
struct tab_attribute
{
/*********************************/
	int32_t		m_id;		
	std::string		m_name;		
	std::string		m_remarks;		
	float		m_fightcoefficient;		// 属性提供的战力系数
	int32_t		m_initvalue;		// 属性初始值
	int32_t		m_uplimit;		// 上限
	int32_t		m_lowlimit;		// 下限
/*********************************/
	tab_attribute();
	// 序列化反序列化相关
	def_portocol(tab_attribute, m_id, m_name, m_remarks, m_fightcoefficient, m_initvalue, m_uplimit, m_lowlimit)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_fightcoefficient,m_initvalue,m_uplimit,m_lowlimit)
};
struct tab_item
{
/*********************************/
	int32_t		m_id;		
	std::string		m_name;		
	std::string		m_remarks;		
	int32_t		m_ranks;		// 排序值
	EItemType		m_type;		// 物品类型EItemType(1装备,2材料,3金币,4银币)
	int32_t		m_quality;		// 物品品质(EQuality)
	bool		m_isstack;		// 是否可堆叠
/*********************************/
	tab_item();
	// 序列化反序列化相关
	def_portocol(tab_item, m_id, m_name, m_remarks, m_ranks, m_type, m_quality, m_isstack)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_ranks,m_type,m_quality,m_isstack)
};
struct attributevalue
{
/*********************************/
	int32_t		m_id;		// 属性id(EnumAttribute)
	bool		m_isratio;		// 是否万分比属性
	int32_t		m_value;		// 属性值
/*********************************/
	attributevalue();
	// 序列化反序列化相关
	def_portocol(attributevalue, m_id, m_isratio, m_value)
	// csv相关
	def_rcsv(m_id,m_isratio,m_value)
};
struct tab_equip
{
/*********************************/
	int32_t		m_id;		
	std::string		m_name;		
	std::string		m_remarks;		
	EEquip		m_position;		// 部位EEquip(1武器2头盔3衣服4护腿)
	std::vector<attributevalue>		m_attributevalues;		// 装备属性
/*********************************/
	tab_equip();
	// 序列化反序列化相关
	def_portocol(tab_equip, m_id, m_name, m_remarks, m_position, m_attributevalues)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_position,m_attributevalues)
};
struct tab_equiplv
{
/*********************************/
	int32_t		m_id;		
	std::string		m_name;		
	std::string		m_remarks;		
	int32_t		m_consume;		// 升级消耗id
	std::vector<attributevalue>		m_attributevalues;		// 装备属性
/*********************************/
	tab_equiplv();
	// 序列化反序列化相关
	def_portocol(tab_equiplv, m_id, m_name, m_remarks, m_consume, m_attributevalues)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_consume,m_attributevalues)
};
struct tab_card
{
/*********************************/
	int32_t		m_id;		
	std::string		m_name;		
	std::string		m_remarks;		
	EQuality		m_quality;		// 品质EQuality(0灰色1绿色2黄色3橙色4红色)
	std::vector<attributevalue>		m_attributevalues;		// 属性
/*********************************/
	tab_card();
	// 序列化反序列化相关
	def_portocol(tab_card, m_id, m_name, m_remarks, m_quality, m_attributevalues)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_quality,m_attributevalues)
};
struct trandom
{
/*********************************/
	int32_t		m_id;		
	int32_t		m_min;		
	int32_t		m_max;		
	int32_t		m_weight;		
/*********************************/
	trandom();
	// 序列化反序列化相关
	def_portocol(trandom, m_id, m_min, m_max, m_weight)
	// csv相关
	def_rcsv(m_id,m_min,m_max,m_weight)
};
struct tab_random
{
/*********************************/
	int32_t		m_id;		
	std::string		m_name;		
	std::string		m_remarks;		
	bool		m_exclusive;		// 排他性多次掉落时使用（true不会掉落出已掉落物品 false会掉落出已掉落物品）
	int32_t		m_count;		// 随机数量
	int32_t		m_calendarids;		// 是否关联日历表,只有日历触发才会执行此掉落
	std::vector<trandom>		m_randomdatas;		// trandom(id*min*max*weight)
	std::vector<int32_t>		m_childrandomids;		// 子随机
/*********************************/
	tab_random();
	// 序列化反序列化相关
	def_portocol(tab_random, m_id, m_name, m_remarks, m_exclusive, m_count, m_calendarids, m_randomdatas, m_childrandomids)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_exclusive,m_count,m_calendarids,m_randomdatas,m_childrandomids)
};
struct tconsume
{
/*********************************/
	int32_t		m_tid;		
	int32_t		m_count;		
/*********************************/
	tconsume();
	// 序列化反序列化相关
	def_portocol(tconsume, m_tid, m_count)
	// csv相关
	def_rcsv(m_tid,m_count)
};
struct tab_consume
{
/*********************************/
	int32_t		m_id;		
	std::string		m_name;		
	std::string		m_remarks;		
	std::vector<tconsume>		m_consumeitems;		// tid*count
/*********************************/
	tab_consume();
	// 序列化反序列化相关
	def_portocol(tab_consume, m_id, m_name, m_remarks, m_consumeitems)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_consumeitems)
};
struct tab_synthesis
{
/*********************************/
	int32_t		m_id;		
	std::string		m_name;		
	std::string		m_remarks;		
	int32_t		m_consume;		// 消耗列表
	int32_t		m_generate;		// 随机表(tab_rando)id 随机成功物品列表
/*********************************/
	tab_synthesis();
	// 序列化反序列化相关
	def_portocol(tab_synthesis, m_id, m_name, m_remarks, m_consume, m_generate)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_consume,m_generate)
};
struct tweek
{
/*********************************/
	int32_t		m_week;		// 周几
	std::string		m_opentime;		//  开启时间 HH:mm:ss
	std::string		m_closetime;		//  结束时间 HH:mm:ss
/*********************************/
	tweek();
	// 序列化反序列化相关
	def_portocol(tweek, m_week, m_opentime, m_closetime)
	// csv相关
	def_rcsv(m_week,m_opentime,m_closetime)
};
struct tserveropen
{
/*********************************/
	int32_t		m_beg;		// 开服后多少天开启
	std::string		m_opentime;		// 开启时间 HH:mm:ss
	int32_t		m_end;		// 开服后多少天结束
	std::string		m_closetime;		// 结束时间 HH:mm:ss
/*********************************/
	tserveropen();
	// 序列化反序列化相关
	def_portocol(tserveropen, m_beg, m_opentime, m_end, m_closetime)
	// csv相关
	def_rcsv(m_beg,m_opentime,m_end,m_closetime)
};
struct tregularslot
{
/*********************************/
	std::string		m_opentime;		// 开启时间 YYYY-MM-DD HH:mm:ss
	std::string		m_closetime;		// 结束时间 YYYY-MM-DD HH:mm:ss
/*********************************/
	tregularslot();
	// 序列化反序列化相关
	def_portocol(tregularslot, m_opentime, m_closetime)
	// csv相关
	def_rcsv(m_opentime,m_closetime)
};
struct tab_calendar
{
/*********************************/
	int32_t		m_id;		
	std::string		m_name;		
	std::string		m_remarks;		
	ECalendar		m_type;		//ECalendar(0:周几,1:以开服时间以来的天数,2:固定时间段)
	std::vector<tweek>		m_week;		//m_type=0,tweek(周几(1-7)*开启时间HH:mm:ss*结束时间HH:mm:ss)
	std::vector<tserveropen>		m_serveropen;		//m_type=1,tserveropen(开服后多少天开启*开启时间HH:mm:ss*开服后多少天结束*结束时间HH:mm:ss)
	std::vector<tregularslot>		m_tregularslot;		//m_type=2,tregularslot(开启时间YYYY/MM/DD HH:mm:ss*结束时间YYYY/MM/DD HH:mm:ss)
/*********************************/
	tab_calendar();
	// 序列化反序列化相关
	def_portocol(tab_calendar, m_id, m_name, m_remarks, m_type, m_week, m_serveropen, m_tregularslot)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_type,m_week,m_serveropen,m_tregularslot)
};
struct tab_mail
{
/*********************************/
	int32_t		m_id;		
	std::string		m_name;		
	std::string		m_remarks;		
	std::string		m_title;		// 邮件标题
	std::string		m_author;		// 邮件作者
	std::string		m_content;		// 邮件内容
/*********************************/
	tab_mail();
	// 序列化反序列化相关
	def_portocol(tab_mail, m_id, m_name, m_remarks, m_title, m_author, m_content)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_title,m_author,m_content)
};
struct tab_chat
{
/*********************************/
	int32_t		m_id;		
	std::string		m_name;		
	std::string		m_remarks;		
	int32_t		m_time;		// 聊天发言间隔
	int32_t		m_count;		// 聊天发言最大数量
/*********************************/
	tab_chat();
	// 序列化反序列化相关
	def_portocol(tab_chat, m_id, m_name, m_remarks, m_time, m_count)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_time,m_count)
};
struct tab_activity
{
/*********************************/
	int32_t		m_id;		// 活动id
	std::string		m_name;		
	std::string		m_remarks;		
	EActivity		m_type;		// 活动类型(1类似咸鱼之王的<<招募达标>>)
	int32_t		m_calendarid;		// 关联的脚本日历
/*********************************/
	tab_activity();
	// 序列化反序列化相关
	def_portocol(tab_activity, m_id, m_name, m_remarks, m_type, m_calendarid)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_type,m_calendarid)
};
struct tab_activity_drawcompliance
{
/*********************************/
	int32_t		m_id;		// 累计次数
	std::string		m_name;		
	std::string		m_remarks;		
	int32_t		m_dropid;		// 掉落id
	int32_t		m_mailid;		// 邮件id
/*********************************/
	tab_activity_drawcompliance();
	// 序列化反序列化相关
	def_portocol(tab_activity_drawcompliance, m_id, m_name, m_remarks, m_dropid, m_mailid)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_dropid,m_mailid)
};
struct task_receive
{
/*********************************/
	ETask		m_receivetype;		// ETask(1.玩家等级达到X 2.主公vip等级达到x 3.完成某ID任务)
	std::vector<int32_t>		m_parmint;		
/*********************************/
	task_receive();
	// 序列化反序列化相关
	def_portocol(task_receive, m_receivetype, m_parmint)
	// csv相关
	def_rcsv(m_receivetype,m_parmint)
};
struct task_complete
{
/*********************************/
	ETask		m_completetype;		// ETask(1.玩家等级达到X 2.主公vip等级达到x 3.完成某ID任务)
	std::vector<int32_t>		m_parmint;		
/*********************************/
	task_complete();
	// 序列化反序列化相关
	def_portocol(task_complete, m_completetype, m_parmint)
	// csv相关
	def_rcsv(m_completetype,m_parmint)
};
struct tab_task
{
/*********************************/
	int32_t		m_id;		// 任务id
	std::string		m_name;		
	std::string		m_remarks;		
	ETaskType		m_type;		
	int32_t		m_dropid;		// 任务奖励
	std::vector<task_receive>		m_taskreceive;		// 接收此任务的前提
	std::vector<task_complete>		m_taskcomplete;		// 完成此任务的条件
/*********************************/
	tab_task();
	// 序列化反序列化相关
	def_portocol(tab_task, m_id, m_name, m_remarks, m_type, m_dropid, m_taskreceive, m_taskcomplete)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_type,m_dropid,m_taskreceive,m_taskcomplete)
};
struct obstacles_data
{
/*********************************/
	std::vector<eobstacles>		m_data;		
/*********************************/
	obstacles_data();
	// 序列化反序列化相关
	def_portocol(obstacles_data, m_data)
	// csv相关
	def_rcsv(m_data)
};
struct tab_map
{
/*********************************/
	int32_t		m_id;		// 地图id
	std::string		m_name;		
	std::string		m_remarks;		
	int32_t		m_w;		// x轴宽度
	int32_t		m_l;		// y轴长度
	int32_t		m_nx;		// [视野]x轴格子数量
	int32_t		m_ny;		// [视野]y轴格子数量
	int32_t		m_obstaclenx;		// [寻路]x轴格子数量
	int32_t		m_obstacleny;		// [寻路]y轴格子数量
	std::vector<obstacles_data>		m_obstacles;		// 地图数据
/*********************************/
	tab_map();
	// 序列化反序列化相关
	def_portocol(tab_map, m_id, m_name, m_remarks, m_w, m_l, m_nx, m_ny, m_obstaclenx, m_obstacleny, m_obstacles)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_w,m_l,m_nx,m_ny,m_obstaclenx,m_obstacleny,m_obstacles)
};
struct tab_matching
{
/*********************************/
	int32_t		m_id;		// 玩法类型 pbnet::eplays
	std::string		m_name;		
	std::string		m_remarks;		
	int32_t		m_count;		// 玩法达到此人数可以开启玩法
	int32_t		m_time;		// 匹配最长时间
	bool		m_iswaitconfirm;		// 是否需要等待确认
	int32_t		m_waitconfirmtime;		// 等待确认的时间
/*********************************/
	tab_matching();
	// 序列化反序列化相关
	def_portocol(tab_matching, m_id, m_name, m_remarks, m_count, m_time, m_iswaitconfirm, m_waitconfirmtime)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_count,m_time,m_iswaitconfirm,m_waitconfirmtime)
};
struct tab_playes
{
/*********************************/
	int32_t		m_id;		// 玩法
	std::string		m_name;		
	std::string		m_remarks;		
	int32_t		m_type;		// 玩法类型 pbnet::eplays
	int32_t		m_mapid;		// 在哪个地图应用该玩法
	int32_t		m_preparation_tm;		// 准备阶段时间
	int32_t		m_play_tm;		// 玩法时间
	int32_t		m_settlement_tm;		// 结算时间
/*********************************/
	tab_playes();
	// 序列化反序列化相关
	def_portocol(tab_playes, m_id, m_name, m_remarks, m_type, m_mapid, m_preparation_tm, m_play_tm, m_settlement_tm)
	// csv相关
	def_rcsv(m_id,m_name,m_remarks,m_type,m_mapid,m_preparation_tm,m_play_tm,m_settlement_tm)
};
}//namespace ngl
#endif //_csvtable_H_
