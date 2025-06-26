// ע�⡾IDL ���������ļ�����Ҫ�ֶ��޸ġ�
// ����ʱ�� // ����ʱ�� 25-06-26 20:09:09
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
	EUDP_OPERATOR_ACK,	// ȷ�ϰ�
	EUDP_OPERATOR_DATA,	// ���������ݰ�
	EUDP_OPERATOR_HEARTBEAT,	// ������Ҫ����ȷ�϶Զ��Ƿ�����
};
enum EPH_HEAD_VAL
{
	EPH_HEAD_MASK_SUCCESS = 1,	// MASKһ��
	EPH_HEAD_MASK_FAIL = 2,	// MASK��һ��
	EPH_HEAD_FOLLOW = 3,	// ��ͷ����û�н������
	EPH_HEAD_SUCCESS = 4,	// ��ͷ�����ѽ������
};
enum EPH
{
	EPH_MASK_VALUE = 0xff,	
	EPH_MASK_COUNT = 2,	
	EPH_MASK = 0,	// ����ȷ���Ƿ�ʹ�ð�
	EPH_BYTES = EPH_MASK_COUNT,	// Э���ֽ���
	EPH_TIME,	// ���Ͷ˵�ʱ���
	EPH_PROTOCOLNUM,	// Э���
	EPH_PROTOCOLTYPE,	// Э������ EPROTOCOL_TYPE
	EPH_ACTOR_TYPEAREA,	// ACTOR_TYPE_AREA
	EPH_ACTOR_ID,	// ACTOR_ID
	EPH_REQUEST_ACTOR_TYPEAREA,	// Request ACTOR_TYPE_AREA
	EPH_REQUEST_ACTOR_ID,	// Request ACTOR_ID
	EPH_SUM,	
};
enum NODE_TYPE
{
	DB = 1,	// ���ݿ������
	ACTORSERVER = 2,	// actor ������
	GAME = 3,	// �߼�������
	GATEWAY = 4,	// ���ط�����
	LOGIN = 5,	// ��¼������
	ROBOT = 6,	// ���Ի����˷�����
	WORLD = 7,	// ���������
	LOG = 8,	// ��־������
	RELOADCSV = 9,	// ���طַ�csv������
	CROSS = 10,	// ���������
	CROSSDB = 11,	// ������ݿ������
	PUSHSERVERCONFIG = 12,	// �������������ϴ�lbgmsys
	NODE_TYPE_COUNT,	
	FAIL = -1,	// ���������ʹ���
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
	E_ModuleNull = 0,	// ��	
	E_ModuleRoot = 1,	// ����(��β)
	E_ModuleBuff = 2,	// ���buff
	E_ModuleEquip = 3,	// װ��
	E_ModuleEquipWeapon = 4,	// װ������
	E_ModuleEquipHelmet = 5,	// װ��ͷ��
	E_ModuleEquipCloth = 6,	// װ���·�
	E_ModuleEquipPants = 7,	// װ������
	E_ModuleCount = 8,	// ���� 	
};
enum EnumAttribute
{
	E_Attack,	// [����]
	E_Defense,	// [����]
	E_Hp,	// [Ѫ��]
	E_Anger,	// [ŭ��]
	E_Speed,	// [�ٶ�]
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
	EEquipment = 1,	// װ��
	EMaterial = 2,	// ����
	EMoneyGold = 3,	// ���
	EMoneySilver = 4,	// ����
	ECard = 5,	// ����
};
enum EQuality
{
	EQualityGrey = 0,	//��ɫ
	EQualityGreen = 1,	//��ɫ
	EQualityYellow = 2,	//��ɫ
	EQualityOrange = 3,	//��ɫ
	EQualityRed = 4,	//��ɫ
};
enum EEquip
{
	EEquipWeapon = 1,	// ����
	EEquipHelmet = 2,	// ͷ��
	EEquipCloth = 3,	// �·�
	EEquipPants = 4,	// ����
};
enum ECalendar
{
	Week = 0,	// �ܼ�
	ServerOpen = 1,	// �Կ���ʱ������������
	RegularSlot = 2,	// �̶�ʱ���
};
enum ECalendarType
{
	ECalendarTypeNull,	// ��
	ECalendarTypeActivity,	// �������ر�
	ECalendarTypeCount,	
};
enum EActivity
{
	EActivityDrawCompliance = 1,	// ��������֮����<<��ļ���>>
	EActivityTopRank = 2,	// ���
};
enum EActivityOpen
{
	EActivityOpenGm = 0,	// Gm����
	EActivityOpenAlways = 1,	// ��פ�
};
enum EActivityAlways
{
	EActivityAlwaysNull = 0,	
	EActivityAlwaysWeek = 1,	// ��
	EActivityAlwaysMonth = 2,	// ��
	EActivityAlwaysFixed = 3,	// ����̶�ʱ��
};
enum ETask
{
	ETaskRoleLv = 0,	// ��ҵȼ�
	ETaskRoleVip = 1,	// ���vip�ȼ�
	ETaskTaskId = 2,	// ���ĳID����
	ETaskRoleLogin = 3,	// �ۼƵ�½
	ETaskConsumeGold = 4,	// �ۼ����Ľ��
	ETaskCount,	
};
enum ETaskCondition
{
	ETaskConditionMore = 0,	// ���ڵ���
	ETaskConditionLess = 1,	// С�ڵ���
	ETaskConditionEqual = 2,	// ����
	ETaskConditionCount,	
};
enum ETaskType
{
	ETaskTypePrincipalLine = 0,	// ��������
	ETaskTypeBranchLine = 1,	// ֧������
	ETaskTypeDaily = 2,	// ÿ������
	ETaskTypeRepeat = 3,	// ���ظ���ɵ�����
};
enum eobstacles
{
	eobstacles_pass = 0,	// ����ͨ��
	eobstacles_nopass = 1,	// �޷�ͨ��
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
	// ���л������л����
	def_portocol(net_works, m_type, m_ip, m_nip, m_port)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_type,m_ip,m_nip,m_port);
	}
};
struct tab_servers
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int16_t                          m_area                          ; // [index:3][load:y] ����
	NODE_TYPE                        m_type                          ; // [index:4][load:y] ����������(1��DB(���ݿ������)2��ACTORSERVER(actor ������))3��GAME(�߼�������)4��GATEWAY(���ط�����)5��LOGIN(��¼������)6��ROBOT(���Ի����˷�����)7��WORLD(���������)8��LOG(��־������)9��RELOADCSV(���طַ�csv������)10��CROSS(���������)11��CROSSDB(������ݿ������)12��PUSHSERVERCONFIG(�������������ϴ�lbgmsys))
	int32_t                          m_tcount                        ; // [index:5][load:y] ͬ���ͷ����������
	int32_t                          m_threadnum                     ; // [index:6][load:y] socket�߳���
	int32_t                          m_actorthreadnum                ; // [index:7][load:y] actor�̳߳��߳���
	bool                             m_outernet                      ; // [index:8][load:y] �Ƿ�������������
	int32_t                          m_db                            ; // [index:9][load:y] ���ӵ�db����id
	int32_t                          m_reloadcsv                     ; // [index:10][load:y] ���ӵ�reloadcsv����id
	int32_t                          m_login                         ; // [index:11][load:y] ���ӵ�login����id
	int16_t                          m_crossarea                     ; // [index:12][load:y] �������
	std::vector<int32_t>             m_actorserver                   ; // [index:13][load:y] ���ӵ�actorserver����id(�����Ҫ��д���actorserver)
	std::vector<net_works>           m_net                           ; // [index:14][load:y] �������������(net_works:m_type(0tcp1ws2kcp),m_ip,m_nip,m_port)
	/*********************************/
	tab_servers();
	// ���л������л����
	def_portocol(tab_servers, m_id, m_name, m_area, m_type, m_tcount, m_threadnum, m_actorthreadnum, m_outernet, m_db, m_reloadcsv, m_login, m_crossarea, m_actorserver, m_net)
	// csv���
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
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	bool                             m_isloadall                     ; // [index:3][load:y] �Ƿ���Ҫ��������ȫ������
	bool                             m_network                       ; // [index:4][load:y] �Ƿ�����ͨ������ַ�
	int32_t                          m_sendmaxcount                  ; // [index:5][load:y] ������෢������
	int32_t                          m_dbcacheintervalms             ; // [index:6][load:y] ���ݻ��������
	/*********************************/
	tab_dbload();
	// ���л������л����
	def_portocol(tab_dbload, m_id, m_name, m_isloadall, m_network, m_sendmaxcount, m_dbcacheintervalms)
	// csv���
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
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	std::string                      m_message                       ; // [index:3][load:y] ��������
	/*********************************/
	tab_errormessage();
	// ���л������л����
	def_portocol(tab_errormessage, m_id, m_name, m_message)
	// csv���
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
	std::string                      m_name                          ; // [index:1[load:y]] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	std::string                      m_value                         ; // [index:3][load:y] ֵ
	/*********************************/
	tab_specialid();
	// ���л������л����
	def_portocol(tab_specialid, m_id, m_name, m_value)
	// csv���
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
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	float                            m_fightcoefficient              ; // [index:3][load:y] �����ṩ��ս��ϵ��
	int32_t                          m_initvalue                     ; // [index:4][load:y] ���Գ�ʼֵ
	int32_t                          m_uplimit                       ; // [index:5][load:y] ����
	int32_t                          m_lowlimit                      ; // [index:6][load:y] ����
	/*********************************/
	tab_attribute();
	// ���л������л����
	def_portocol(tab_attribute, m_id, m_name, m_fightcoefficient, m_initvalue, m_uplimit, m_lowlimit)
	// csv���
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
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int32_t                          m_ranks                         ; // [index:3][load:y] ����ֵ
	EItemType                        m_type                          ; // [index:4][load:y] ��Ʒ����EItemType(1װ��,2����,3���,4����)
	int32_t                          m_quality                       ; // [index:5][load:y] ��ƷƷ��(EQuality)
	bool                             m_isstack                       ; // [index:6][load:y] �Ƿ�ɶѵ�
	/*********************************/
	tab_item();
	// ���л������л����
	def_portocol(tab_item, m_id, m_name, m_ranks, m_type, m_quality, m_isstack)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_ranks,m_type,m_quality,m_isstack);
	}
};
struct attributevalue
{
	/*********************************/
	int32_t                          m_id                            ; // ����id(EnumAttribute)
	bool                             m_isratio                       ; // �Ƿ���ֱ�����
	int32_t                          m_value                         ; // ����ֵ
	/*********************************/
	attributevalue();
	// ���л������л����
	def_portocol(attributevalue, m_id, m_isratio, m_value)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_id,m_isratio,m_value);
	}
};
struct tab_equip
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	EEquip                           m_position                      ; // [index:3][load:y] ��λEEquip(1����2ͷ��3�·�4����)
	std::vector<attributevalue>      m_attributevalues               ; // [index:4][load:y] װ������
	/*********************************/
	tab_equip();
	// ���л������л����
	def_portocol(tab_equip, m_id, m_name, m_position, m_attributevalues)
	// csv���
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
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int32_t                          m_consume                       ; // [index:3][load:y] ��������id
	std::vector<attributevalue>      m_attributevalues               ; // [index:4][load:y] װ������
	/*********************************/
	tab_equiplv();
	// ���л������л����
	def_portocol(tab_equiplv, m_id, m_name, m_consume, m_attributevalues)
	// csv���
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
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	EQuality                         m_quality                       ; // [index:3][load:y] Ʒ��EQuality(0��ɫ1��ɫ2��ɫ3��ɫ4��ɫ)
	std::vector<attributevalue>      m_attributevalues               ; // [index:4][load:y] ����
	/*********************************/
	tab_card();
	// ���л������л����
	def_portocol(tab_card, m_id, m_name, m_quality, m_attributevalues)
	// csv���
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
	// ���л������л����
	def_portocol(trandom, m_id, m_min, m_max, m_weight)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_id,m_min,m_max,m_weight);
	}
};
struct tab_random
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	std::vector<trandom>             m_randomdatas                   ; // [index:3][load:y] trandom(id*min*max*weight)
	bool                             m_exclusive                     ; // [index:4][load:y] �����Զ�ε���ʱʹ�ã�true���������ѵ�����Ʒ false�������ѵ�����Ʒ��
	int32_t                          m_count                         ; // [index:5][load:y] �������
	std::set<int32_t>                m_activityids                   ; // [index:6][load:y] ֻ�л�����Żᴥ���˵���
	std::vector<int32_t>             m_childrandomids                ; // [index:7][load:y] �����
	/*********************************/
	tab_random();
	// ���л������л����
	def_portocol(tab_random, m_id, m_name, m_randomdatas, m_exclusive, m_count, m_activityids, m_childrandomids)
	// csv���
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
	// ���л������л����
	def_portocol(tconsume, m_tid, m_count)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_tid,m_count);
	}
};
struct tab_consume
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	std::vector<tconsume>            m_consumeitems                  ; // [index:3][load:y] tid*count
	/*********************************/
	tab_consume();
	// ���л������л����
	def_portocol(tab_consume, m_id, m_name, m_consumeitems)
	// csv���
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
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int32_t                          m_consume                       ; // [index:3][load:y] �����б�
	int32_t                          m_generate                      ; // [index:4][load:y] �����(tab_rando)id ����ɹ���Ʒ�б�
	/*********************************/
	tab_synthesis();
	// ���л������л����
	def_portocol(tab_synthesis, m_id, m_name, m_consume, m_generate)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_consume,m_generate);
	}
};
struct tweek
{
	/*********************************/
	int32_t                          m_weekstart                     ; // �ܼ���ʼ
	std::string                      m_opentime                      ; // ����ʱ�� HH:mm:ss
	int32_t                          m_weekfinish                    ; // �ܼ�����
	std::string                      m_closetime                     ; // ����ʱ�� HH:mm:ss
	/*********************************/
	tweek();
	// ���л������л����
	def_portocol(tweek, m_weekstart, m_opentime, m_weekfinish, m_closetime)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_weekstart,m_opentime,m_weekfinish,m_closetime);
	}
};
struct tserveropen
{
	/*********************************/
	int32_t                          m_beg                           ; // ����������쿪��
	std::string                      m_opentime                      ; // ����ʱ�� HH:mm:ss
	int32_t                          m_end                           ; // ��������������
	std::string                      m_closetime                     ; // ����ʱ�� HH:mm:ss
	/*********************************/
	tserveropen();
	// ���л������л����
	def_portocol(tserveropen, m_beg, m_opentime, m_end, m_closetime)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_beg,m_opentime,m_end,m_closetime);
	}
};
struct tregularslot
{
	/*********************************/
	std::string                      m_opentime                      ; // ����ʱ�� YYYY-MM-DD HH:mm:ss
	std::string                      m_closetime                     ; // ����ʱ�� YYYY-MM-DD HH:mm:ss
	/*********************************/
	tregularslot();
	// ���л������л����
	def_portocol(tregularslot, m_opentime, m_closetime)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_opentime,m_closetime);
	}
};
struct tab_mail
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	std::string                      m_title                         ; // [index:3][load:y] �ʼ�����
	std::string                      m_author                        ; // [index:4][load:y] �ʼ�����
	std::string                      m_content                       ; // [index:5][load:y] �ʼ�����
	/*********************************/
	tab_mail();
	// ���л������л����
	def_portocol(tab_mail, m_id, m_name, m_title, m_author, m_content)
	// csv���
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
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int32_t                          m_time                          ; // [index:3][load:y] ���췢�Լ��
	int32_t                          m_count                         ; // [index:4][load:y] ���췢���������
	/*********************************/
	tab_chat();
	// ���л������л����
	def_portocol(tab_chat, m_id, m_name, m_time, m_count)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_time,m_count);
	}
};
struct activity_task
{
	/*********************************/
	int32_t                          m_begday                        ; // �ڼ���(��1��ʼ)����
	int32_t                          m_endday                        ; // �ڼ���(��1��ʼ)�Ƴ�(-1��������)
	std::vector<int32_t>             m_taskids                       ; // �����б�
	/*********************************/
	activity_task();
	// ���л������л����
	def_portocol(activity_task, m_begday, m_endday, m_taskids)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_begday,m_endday,m_taskids);
	}
};
struct tab_activity
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	EActivity                        m_type                          ; // [index:3][load:y] �����(1:��������֮����<<��ļ���>>2:���)
	EActivityOpen                    m_open                          ; // [index:4][load:y] ������ʽ(0:GM����1:��פ�)
	std::vector<int32_t>             m_task                          ; // [index:5][load:y] ��ڼ��ȡ����
	std::vector<activity_task>       m_taskday                       ; // [index:6][load:y] ��ڼ��ȡ����(�ڼ����������)
	std::vector<int32_t>             m_ranktype                      ; // [index:7][load:y] ֧�ֵĻ�����б�(eranklist.activity_lv:1000000��ڼ���ҵȼ��仯ֵ,eranklist.activity_gold:2000000��ڼ���ҽ������ֵ)
	/*********************************/
	tab_activity();
	// ���л������л����
	def_portocol(tab_activity, m_id, m_name, m_type, m_open, m_task, m_taskday, m_ranktype)
	// csv���
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
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int                              m_openday                       ; // [index:3][load:y] �����ڼ��쿪��������������Ϊ��һ�죩
	int                              m_openhour                      ; // [index:4][load:y] ʱ
	int                              m_openminute                    ; // [index:5][load:y] ��
	int                              m_opensecond                    ; // [index:6][load:y] ��
	int                              m_closeday                      ; // [index:7][load:y] �����ڼ��쿪��������������Ϊ��һ�죩
	int                              m_closehour                     ; // [index:8][load:y] ʱ
	int                              m_closeminute                   ; // [index:9][load:y] ��
	int                              m_closesecond                   ; // [index:10][load:y] ��
	/*********************************/
	tab_activityopenserver();
	// ���л������л����
	def_portocol(tab_activityopenserver, m_id, m_name, m_openday, m_openhour, m_openminute, m_opensecond, m_closeday, m_closehour, m_closeminute, m_closesecond)
	// csv���
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
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	EActivityAlways                  m_type                          ; // [index:3][load:y] ��פ�����������
	int                              m_wbday                         ; // [index:4][load:y] �ܼ�(1-7)
	int                              m_wbhour                        ; // [index:5][load:y] ʱ
	int                              m_wbminute                      ; // [index:6][load:y] ��
	int                              m_wbsecond                      ; // [index:7][load:y] ��
	int                              m_weday                         ; // [index:8][load:y] �ܼ�(1-7)
	int                              m_wehour                        ; // [index:9][load:y] ʱ
	int                              m_weminute                      ; // [index:10][load:y] ��
	int                              m_wesecond                      ; // [index:11][load:y] ��
	int                              m_mbday                         ; // [index:12][load:y] ÿ�¼���(1-31)
	int                              m_mbhour                        ; // [index:13][load:y] ʱ
	int                              m_mbminute                      ; // [index:14][load:y] ��
	int                              m_mbsecond                      ; // [index:15][load:y] ��
	int                              m_meday                         ; // [index:16][load:y] ÿ�¼���(1-31)
	int                              m_mehour                        ; // [index:17][load:y] ʱ
	int                              m_meminute                      ; // [index:18][load:y] ��
	int                              m_mesecond                      ; // [index:19][load:y] ��
	int                              m_fixedday                      ; // [index:20][load:y] �������ڼ���
	int                              m_fbhour                        ; // [index:21][load:y] ʱ
	int                              m_fbminute                      ; // [index:22][load:y] ��
	int                              m_fbsecond                      ; // [index:23][load:y] ��
	int                              m_fehour                        ; // [index:24][load:y] ʱ
	int                              m_feminute                      ; // [index:25][load:y] ��
	int                              m_fesecond                      ; // [index:26][load:y] ��
	/*********************************/
	tab_activityalways();
	// ���л������л����
	def_portocol(tab_activityalways, m_id, m_name, m_type, m_wbday, m_wbhour, m_wbminute, m_wbsecond, m_weday, m_wehour, m_weminute, m_wesecond, m_mbday, m_mbhour, m_mbminute, m_mbsecond, m_meday, m_mehour, m_meminute, m_mesecond, m_fixedday, m_fbhour, m_fbminute, m_fbsecond, m_fehour, m_feminute, m_fesecond)
	// csv���
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
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int32_t                          m_dropid                        ; // [index:3][load:y] ����id
	int32_t                          m_mailid                        ; // [index:4][load:y] �ʼ�id
	/*********************************/
	tab_activity_drawcompliance();
	// ���л������л����
	def_portocol(tab_activity_drawcompliance, m_id, m_name, m_dropid, m_mailid)
	// csv���
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
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int32_t                          m_activityid                    ; // [index:3][load:y] �id
	int32_t                          m_rank                          ; // [index:3][load:y] ����
	int32_t                          m_dropid                        ; // [index:4][load:y] ����id
	int32_t                          m_mailid                        ; // [index:5][load:y] �ʼ�id
	/*********************************/
	tab_activity_toprank();
	// ���л������л����
	def_portocol(tab_activity_toprank, m_id, m_name, m_activityid, m_rank, m_dropid, m_mailid)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_activityid,m_rank,m_dropid,m_mailid);
	}
};
struct task_condition
{
	/*********************************/
	ETask                            m_type                          ; // ETask(0.��ҵȼ��ﵽX 1.����vip�ȼ��ﵽx 2.���ĳID����)
	ETaskCondition                   m_condition                     ; 
	int32_t                          m_parmint                       ; 
	/*********************************/
	task_condition();
	// ���л������л����
	def_portocol(task_condition, m_type, m_condition, m_parmint)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_type,m_condition,m_parmint);
	}
};
struct tab_task
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	ETaskType                        m_type                          ; // [index:3][load:y] ��������(0:��������1:֧������2:ÿ������3:���ظ���ɵ�����)
	std::string                      m_typeparm                      ; // [index:4][load:y] (m_type=2||=3 : ����ɴ���)
	std::vector<task_condition>      m_taskreceive                   ; // [index:5][load:y] ���մ������ǰ��(ETask(0.��ҵȼ��ﵽX 1.����vip�ȼ��ﵽx 2.���ĳID����)*ETaskCondition(0.���ڵ���1.С�ڵ���2.����)*int32_t(ֵ))
	std::vector<task_condition>      m_taskcomplete                  ; // [index:6][load:y] ��ɴ����������(ETask(0.��ҵȼ��ﵽX 1.����vip�ȼ��ﵽx 2.���ĳID����)*ETaskCondition(0.���ڵ���1.С�ڵ���2.����)*int32_t(ֵ))
	int32_t                          m_dropid                        ; // [index:7][load:y] ������
	bool                             m_autoreceive                   ; // [index:8][load:y] �Ƿ��Զ���ȡ
	int32_t                          m_mailid                        ; // [index:9][load:y] �Զ���ȡ���Ƿ����ʼ����ʼ�id(�Զ���ȡ���ʼ�id:m_autoreceive == true,��m_autoreceiveΪture����Ϊ-1)
	/*********************************/
	tab_task();
	// ���л������л����
	def_portocol(tab_task, m_id, m_name, m_type, m_typeparm, m_taskreceive, m_taskcomplete, m_dropid, m_autoreceive, m_mailid)
	// csv���
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
	// ���л������л����
	def_portocol(obstacles_data, m_data)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_data);
	}
};
struct tab_map
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int32_t                          m_w                             ; // [index:3][load:y] x����
	int32_t                          m_l                             ; // [index:4][load:y] y�᳤��
	int32_t                          m_nx                            ; // [index:5][load:y] [��Ұ]x���������
	int32_t                          m_ny                            ; // [index:6][load:y] [��Ұ]y���������
	int32_t                          m_obstaclenx                    ; // [index:7][load:y] [Ѱ·]x���������
	int32_t                          m_obstacleny                    ; // [index:8][load:y] [Ѱ·]y���������
	std::vector<obstacles_data>      m_obstacles                     ; // [index:9][load:y] ��ͼ����
	/*********************************/
	tab_map();
	// ���л������л����
	def_portocol(tab_map, m_id, m_name, m_w, m_l, m_nx, m_ny, m_obstaclenx, m_obstacleny, m_obstacles)
	// csv���
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
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int32_t                          m_count                         ; // [index:3][load:y] �淨�ﵽ���������Կ����淨
	int32_t                          m_time                          ; // [index:4][load:y] ƥ���ʱ��
	bool                             m_iswaitconfirm                 ; // [index:5][load:y] �Ƿ���Ҫ�ȴ�ȷ��
	int32_t                          m_waitconfirmtime               ; // [index:6][load:y] �ȴ�ȷ�ϵ�ʱ��
	/*********************************/
	tab_matching();
	// ���л������л����
	def_portocol(tab_matching, m_id, m_name, m_count, m_time, m_iswaitconfirm, m_waitconfirmtime)
	// csv���
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
	// ���л������л����
	def_portocol(Vector2, m_x, m_y)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		def_rcsv2(m_x,m_y);
	}
};
struct tab_plays
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] id 
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int32_t                          m_type                          ; // [index:3][load:y] �淨���� pbnet::eplays
	int32_t                          m_mapid                         ; // [index:4][load:y] ���ĸ���ͼӦ�ø��淨
	int32_t                          m_preparation_tm                ; // [index:5][load:y] ׼���׶�ʱ��
	int32_t                          m_play_tm                       ; // [index:6][load:y] �淨ʱ��
	int32_t                          m_settlement_tm                 ; // [index:7][load:y] ����ʱ��
	std::vector<Vector2>             m_birthpoint                    ; // [index:8][load:y] ������Vector2(m_x:int32_t,m_y:int32_t)	
	/*********************************/
	tab_plays();
	// ���л������л����
	def_portocol(tab_plays, m_id, m_name, m_type, m_mapid, m_preparation_tm, m_play_tm, m_settlement_tm, m_birthpoint)
	// csv���
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
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int32_t                          m_price                         ; // [index:3][load:y] �۸�
	int32_t                          m_gold                          ; // [index:4][load:y] ��õĽ������
	int32_t                          m_bonus                         ; // [index:5][load:y] �����Ľ������
	int32_t                          m_firstbonus                    ; // [index:6][load:y] �׳佱���Ľ������
	int32_t                          m_dropid                        ; // [index:7][load:y] ����
	int32_t                          m_vipexp                        ; // [index:8][load:y] ��ֵ�õ�λ���͵�vip����
	int32_t                          m_count                         ; // [index:9][load:y] �Ƿ����Ƴ�ֵ����
	std::vector<int32_t>             m_activityid                    ; // [index:10][load:y] ����ָ�������ʱ���ܳ�ֵ��һ��
	int32_t                          m_mailid                        ; // [index:11][load:y] �ʼ�id
	/*********************************/
	tab_recharge();
	// ���л������л����
	def_portocol(tab_recharge, m_id, m_name, m_price, m_gold, m_bonus, m_firstbonus, m_dropid, m_vipexp, m_count, m_activityid, m_mailid)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_price,m_gold,m_bonus,m_firstbonus,m_dropid,m_vipexp,m_count,m_activityid,m_mailid);
	}
};
struct tab_familylv
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] ���ŵȼ� 
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int32_t                          m_exp                           ; // [index:3][load:y] ������Ҫ�ľ���
	int32_t                          m_maxmembers                    ; // [index:4][load:y] ����������
	/*********************************/
	tab_familylv();
	// ���л������л����
	def_portocol(tab_familylv, m_id, m_name, m_exp, m_maxmembers)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_exp,m_maxmembers);
	}
};
struct tab_mergearea
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] ����id
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int32_t                          m_mergeid                       ; // [index:3][load:y] �ϲ���������id
	/*********************************/
	tab_mergearea();
	// ���л������л����
	def_portocol(tab_mergearea, m_id, m_name, m_mergeid)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_mergeid);
	}
};
struct tab_ranklist
{
	/*********************************/
	int32_t                          m_id                            ; // [index:0][load:y] eranklist(0:�ȼ�����)
	std::string                      m_name                          ; // [index:1][load:y] ���� 
//	std::string                      m_remarks                       ; // [index:2][load:n] ��ע
	int32_t                          m_maxitem                       ; // [index:3][load:y] ������н������
	int32_t                          m_showitem                      ; // [index:4][load:y] չʾ���н������
	int32_t                          m_everypagecount                ; // [index:5][load:y] ÿҳչʾ�������
	int32_t                          m_minvalue                      ; // [index:6][load:y] ��С���ֵ
	/*********************************/
	tab_ranklist();
	// ���л������л����
	def_portocol(tab_ranklist, m_id, m_name, m_maxitem, m_showitem, m_everypagecount, m_minvalue)
	// csv���
	inline bool rcsv(ngl::csvpair& apair)
	{
		std::string lm_remarks;
		def_rcsv2(m_id,m_name,lm_remarks,m_maxitem,m_showitem,m_everypagecount,m_minvalue);
	}
};
}//namespace ngl
