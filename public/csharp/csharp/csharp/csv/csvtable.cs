namespace ngl
{
	public enum ENET_CONFIG
	{
		net_config_send_buff_maxbyte = 8129,	
		net_config_recv_buff_maxbyte = 8129,	
		net_config_logic_pthread_max_size = 8,	
		net_config_socket_pthread_max_size = 8,	
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<ENET_CONFIG> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((ENET_CONFIG)ltemp);
			}
			return true;
		}
	}
	public enum EUDP_OPERATOR
	{
		EUDP_OPERATOR_ACK,	// 确认包
		EUDP_OPERATOR_DATA,	// 传输子数据包
		EUDP_OPERATOR_HEARTBEAT,	// 心跳主要用来确认对端是否在线
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<EUDP_OPERATOR> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((EUDP_OPERATOR)ltemp);
			}
			return true;
		}
	}
	public enum EPH_HEAD_VAL
	{
		EPH_HEAD_VERSION_SUCCESS = 1,	// 版本一致
		EPH_HEAD_VERSION_FAIL = 2,	// 版本不一致
		EPH_HEAD_VERSION_FOLLOW = 3,	// 无法对比版本,数据没有接收完成
		EPH_HEAD_FOLLOW = 4,	// 包头数据没有接收完成
		EPH_HEAD_SUCCESS = 5,	// 包头数据已接收完成
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<EPH_HEAD_VAL> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((EPH_HEAD_VAL)ltemp);
			}
			return true;
		}
	}
	public enum EPH
	{
		EPH_VERSION = 0,	// 协议版本号
		EPH_BYTES,	// 协议字节数
		EPH_TIME,	// 发送端的时间戳
		EPH_PROTOCOLNUM,	// 协议号
		EPH_PROTOCOLTYPE,	// 协议类型 EPROTOCOL_TYPE
		EPH_ACTOR_TYPEAREA,	// ACTOR_TYPE_AREA
		EPH_ACTOR_ID,	// ACTOR_ID
		EPH_REQUEST_ACTOR_TYPEAREA,	// Request REQUEST_ACTOR_TYPE_AREA
		EPH_REQUEST_ACTOR_ID,	// Request ACTOR_ID
		EPH_SUM,	
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<EPH> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((EPH)ltemp);
			}
			return true;
		}
	}
	public enum NODE_TYPE
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
		FAIL = -1,	// 服务器类型错误
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<NODE_TYPE> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((NODE_TYPE)ltemp);
			}
			return true;
		}
	}
	public enum ENET_PROTOCOL
	{
		ENET_NULL = -1,	
		ENET_TCP = 0,	
		ENET_WS = 1,	
		ENET_KCP = 2,	
		ENET_COUNT = 3,	
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<ENET_PROTOCOL> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((ENET_PROTOCOL)ltemp);
			}
			return true;
		}
	}
	public enum EnumModule
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
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<EnumModule> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((EnumModule)ltemp);
			}
			return true;
		}
	}
	public enum EnumAttribute
	{
		E_Attack,	// [攻击]
		E_Defense,	// [防御]
		E_Hp,	// [血量]
		E_Anger,	// [怒气]
		E_Speed,	// [速度]
		E_Count,	
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<EnumAttribute> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((EnumAttribute)ltemp);
			}
			return true;
		}
	}
	public enum EItemSrc
	{
		EItemSrcNoraml = 0,	
		EItemSrcGM = 1,	
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<EItemSrc> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((EItemSrc)ltemp);
			}
			return true;
		}
	}
	public enum EItemConsume
	{
		EItemConsumeNoraml = 0,	
		EItemConsumeGM = 1,	
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<EItemConsume> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((EItemConsume)ltemp);
			}
			return true;
		}
	}
	public enum EItemType
	{
		EEquipment = 1,	// 装备
		EMaterial = 2,	// 材料
		EMoneyGold = 3,	// 金币
		EMoneySilver = 4,	// 银币
		ECard = 5,	// 卡牌
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<EItemType> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((EItemType)ltemp);
			}
			return true;
		}
	}
	public enum EQuality
	{
		EQualityGrey = 0,	//灰色
		EQualityGreen = 1,	//绿色
		EQualityYellow = 2,	//黄色
		EQualityOrange = 3,	//橙色
		EQualityRed = 4,	//红色
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<EQuality> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((EQuality)ltemp);
			}
			return true;
		}
	}
	public enum EEquip
	{
		EEquipWeapon = 1,	// 武器
		EEquipHelmet = 2,	// 头盔
		EEquipCloth = 3,	// 衣服
		EEquipPants = 4,	// 护腿
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<EEquip> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((EEquip)ltemp);
			}
			return true;
		}
	}
	public enum ECalendar
	{
		Week = 0,	// 周几
		ServerOpen = 1,	// 以开服时间以来的天数
		RegularSlot = 2,	// 固定时间段
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<ECalendar> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((ECalendar)ltemp);
			}
			return true;
		}
	}
	public enum ECalendarType
	{
		ECalendarTypeEveryDay,	// 每日定时刷新
		ECalendarTypeActivity,	// 活动开启与关闭
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<ECalendarType> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((ECalendarType)ltemp);
			}
			return true;
		}
	}
	public enum EActivity
	{
		EActivityDrawCompliance = 1,	// 类似咸鱼之王的<<招募达标>>
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<EActivity> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((EActivity)ltemp);
			}
			return true;
		}
	}
	public enum ETask
	{
		ETaskRoleLv = 0,	// 玩家等级
		ETaskRoleVip = 1,	// 玩家vip等级
		ETaskTaskId = 2,	// 完成某ID任务
		ETaskCount,	
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<ETask> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((ETask)ltemp);
			}
			return true;
		}
	}
	public enum ETaskCondition
	{
		ETaskConditionMore = 0,	// 大于等于
		ETaskConditionLess = 1,	// 小于等于
		ETaskConditionEqual = 2,	// 等于
		ETaskConditionCount,	
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<ETaskCondition> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((ETaskCondition)ltemp);
			}
			return true;
		}
	}
	public enum ETaskType
	{
		ETaskTypePrincipalLine,	// 主线任务
		ETaskTypeBranchLine,	// 支线任务
		ETaskTypeDaily,	// 每日任务
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<ETaskType> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((ETaskType)ltemp);
			}
			return true;
		}
	}
	public enum eobstacles
	{
		eobstacles_pass = 0,	// 可以通过
		eobstacles_nopass = 1,	// 无法通过
	}
	partial class RCsv
	{
		public static bool ReadCsv(CsvPair apair, List<eobstacles> avec)
		{
			string ltempstr = Read(apair);
			CsvPair lpair = new CsvPair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !IsOk(lpair);)
			{
				Int32 ltemp = 0;
				if (ReadCsv(lpair, ref ltemp))
					avec.Add((eobstacles)ltemp);
			}
			return true;
		}
	}
	class net_works : ICsvRead
	{
		/*********************************/
		public ENET_PROTOCOL		m_type;		
		public string		m_ip;		
		public string		m_nip;		
		public UInt16		m_port;		
		/*********************************/
		public bool Read(CsvPair apair)
		{
			Int32 lm_type = 0;
			if(RCsv.ReadCsv(apair, ref lm_type) == false)
				return false;
			m_type = (ENET_PROTOCOL)lm_type;
			if(RCsv.ReadCsv(apair, ref m_ip) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_nip) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_port) == false)
				return false;
			return true;
		}
	}
	class tab_servers : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public Int16		m_area;		// [index:3][load:y] 区服
		public NODE_TYPE		m_type;		// [index:4][load:y] 服务器类型(1db2actorserver3game4gateway5login6robot7world8log9reloadcsv10reloadcsv_tools)
		public Int32		m_tcount;		// [index:5][load:y] 同类型服务器的序号
		public Int32		m_threadnum;		// [index:6][load:y] socket线程数
		public Int32		m_actorthreadnum;		// [index:7][load:y] actor线程池线程数
		public bool		m_outernet;		// [index:8][load:y] 是否允许外网访问
		public Int32		m_db;		// [index:9][load:y] 连接的db进程id
		public Int32		m_reloadcsv;		// [index:10][load:y] 连接的reloadcsv进程id
		public Int32		m_login;		// [index:11][load:y] 连接的login进程id
		public Int16		m_crossarea;		// [index:12][load:y] 跨服区服
		public List<Int32>		m_actorserver = new List<Int32>();		// [index:13][load:y] 连接的actorserver进程id(跨服需要填写多个actorserver)
		public List<net_works>		m_net = new List<net_works>();		// [index:14][load:y] 服务器网络相关(net_works:m_type(0tcp1ws2kcp),m_ip,m_nip,m_port)
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_area) == false)
				return false;
			Int32 lm_type = 0;
			if(RCsv.ReadCsv(apair, ref lm_type) == false)
				return false;
			m_type = (NODE_TYPE)lm_type;
			if(RCsv.ReadCsv(apair, ref m_tcount) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_threadnum) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_actorthreadnum) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_outernet) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_db) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_reloadcsv) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_login) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_crossarea) == false)
				return false;
			if(RCsv.ReadCsv(apair, m_actorserver) == false)
				return false;
			if(RCsv.ReadCsv(apair, m_net) == false)
				return false;
			return true;
		}
	}
	class tab_dbload : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public bool		m_isloadall;		// [index:3][load:y] 是否需要启动加载全部数据
		public bool		m_network;		// [index:4][load:y] 是否允许通过网络分发
		public Int32		m_sendmaxcount;		// [index:5][load:y] 单次最多发送数量
		public Int32		m_dbcacheintervalms;		// [index:6][load:y] 数据缓存入库间隔
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_isloadall) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_network) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_sendmaxcount) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_dbcacheintervalms) == false)
				return false;
			return true;
		}
	}
	class tab_errormessage : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public string		m_message;		// [index:3][load:y] 错误描述
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_message) == false)
				return false;
			return true;
		}
	}
	class tab_specialid : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1[load:y]] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public string		m_value;		// [index:3][load:y] 值
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_value) == false)
				return false;
			return true;
		}
	}
	class tab_attribute : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public float		m_fightcoefficient;		// [index:3][load:y] 属性提供的战力系数
		public Int32		m_initvalue;		// [index:4][load:y] 属性初始值
		public Int32		m_uplimit;		// [index:5][load:y] 上限
		public Int32		m_lowlimit;		// [index:6][load:y] 下限
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_fightcoefficient) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_initvalue) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_uplimit) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_lowlimit) == false)
				return false;
			return true;
		}
	}
	class tab_item : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public Int32		m_ranks;		// [index:3][load:y] 排序值
		public EItemType		m_type;		// [index:4][load:y] 物品类型EItemType(1装备,2材料,3金币,4银币)
		public Int32		m_quality;		// [index:5][load:y] 物品品质(EQuality)
		public bool		m_isstack;		// [index:6][load:y] 是否可堆叠
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_ranks) == false)
				return false;
			Int32 lm_type = 0;
			if(RCsv.ReadCsv(apair, ref lm_type) == false)
				return false;
			m_type = (EItemType)lm_type;
			if(RCsv.ReadCsv(apair, ref m_quality) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_isstack) == false)
				return false;
			return true;
		}
	}
	class attributevalue : ICsvRead
	{
		/*********************************/
		public Int32		m_id;		// 属性id(EnumAttribute)
		public bool		m_isratio;		// 是否万分比属性
		public Int32		m_value;		// 属性值
		/*********************************/
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_isratio) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_value) == false)
				return false;
			return true;
		}
	}
	class tab_equip : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public EEquip		m_position;		// [index:3][load:y] 部位EEquip(1武器2头盔3衣服4护腿)
		public List<attributevalue>		m_attributevalues = new List<attributevalue>();		// [index:4][load:y] 装备属性
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			Int32 lm_position = 0;
			if(RCsv.ReadCsv(apair, ref lm_position) == false)
				return false;
			m_position = (EEquip)lm_position;
			if(RCsv.ReadCsv(apair, m_attributevalues) == false)
				return false;
			return true;
		}
	}
	class tab_equiplv : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public Int32		m_consume;		// [index:3][load:y] 升级消耗id
		public List<attributevalue>		m_attributevalues = new List<attributevalue>();		// [index:4][load:y] 装备属性
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_consume) == false)
				return false;
			if(RCsv.ReadCsv(apair, m_attributevalues) == false)
				return false;
			return true;
		}
	}
	class tab_card : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public EQuality		m_quality;		// [index:3][load:y] 品质EQuality(0灰色1绿色2黄色3橙色4红色)
		public List<attributevalue>		m_attributevalues = new List<attributevalue>();		// [index:4][load:y] 属性
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			Int32 lm_quality = 0;
			if(RCsv.ReadCsv(apair, ref lm_quality) == false)
				return false;
			m_quality = (EQuality)lm_quality;
			if(RCsv.ReadCsv(apair, m_attributevalues) == false)
				return false;
			return true;
		}
	}
	class trandom : ICsvRead
	{
		/*********************************/
		public Int32		m_id;		
		public Int32		m_min;		
		public Int32		m_max;		
		public Int32		m_weight;		
		/*********************************/
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_min) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_max) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_weight) == false)
				return false;
			return true;
		}
	}
	class tab_random : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public List<trandom>		m_randomdatas = new List<trandom>();		// [index:3][load:y] trandom(id*min*max*weight)
		public bool		m_exclusive;		// [index:4][load:y] 排他性多次掉落时使用（true不会掉落出已掉落物品 false会掉落出已掉落物品）
		public Int32		m_count;		// [index:5][load:y] 随机数量
		public Int32		m_calendarids;		// [index:6][load:y] 是否关联日历表,只有日历触发才会执行此掉落
		public List<Int32>		m_childrandomids = new List<Int32>();		// [index:7][load:y] 子随机
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, m_randomdatas) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_exclusive) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_count) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_calendarids) == false)
				return false;
			if(RCsv.ReadCsv(apair, m_childrandomids) == false)
				return false;
			return true;
		}
	}
	class tconsume : ICsvRead
	{
		/*********************************/
		public Int32		m_tid;		
		public Int32		m_count;		
		/*********************************/
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_tid) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_count) == false)
				return false;
			return true;
		}
	}
	class tab_consume : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public List<tconsume>		m_consumeitems = new List<tconsume>();		// [index:3][load:y] tid*count
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, m_consumeitems) == false)
				return false;
			return true;
		}
	}
	class tab_synthesis : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public Int32		m_consume;		// [index:3][load:y] 消耗列表
		public Int32		m_generate;		// [index:4][load:y] 随机表(tab_rando)id 随机成功物品列表
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_consume) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_generate) == false)
				return false;
			return true;
		}
	}
	class tweek : ICsvRead
	{
		/*********************************/
		public Int32		m_weekstart;		// 周几开始
		public string		m_opentime;		// 开启时间 HH:mm:ss
		public Int32		m_weekfinish;		// 周几结束
		public string		m_closetime;		// 结束时间 HH:mm:ss
		/*********************************/
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_weekstart) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_opentime) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_weekfinish) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_closetime) == false)
				return false;
			return true;
		}
	}
	class tserveropen : ICsvRead
	{
		/*********************************/
		public Int32		m_beg;		// 开服后多少天开启
		public string		m_opentime;		// 开启时间 HH:mm:ss
		public Int32		m_end;		// 开服后多少天结束
		public string		m_closetime;		// 结束时间 HH:mm:ss
		/*********************************/
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_beg) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_opentime) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_end) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_closetime) == false)
				return false;
			return true;
		}
	}
	class tregularslot : ICsvRead
	{
		/*********************************/
		public string		m_opentime;		// 开启时间 YYYY-MM-DD HH:mm:ss
		public string		m_closetime;		// 结束时间 YYYY-MM-DD HH:mm:ss
		/*********************************/
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_opentime) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_closetime) == false)
				return false;
			return true;
		}
	}
	class tab_calendar : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public ECalendar		m_type;		// [index:3][load:y] ECalendar(0:周几,1:以开服时间以来的天数,2:固定时间段)
		public List<tweek>		m_week = new List<tweek>();		// [index:4][load:y] m_type=0,tweek(周几开始(1-7)*开启时间HH:mm:ss*周几结束(1-7)*结束时间HH:mm:ss)
		public List<tserveropen>		m_serveropen = new List<tserveropen>();		// [index:5][load:y] m_type=1,tserveropen(开服后多少天开启*开启时间HH:mm:ss*开服后多少天结束*结束时间HH:mm:ss)
		public List<tregularslot>		m_tregularslot = new List<tregularslot>();		// [index:6][load:y] m_type=2,tregularslot(开启时间YYYY/MM/DD HH:mm:ss*结束时间YYYY/MM/DD HH:mm:ss)
		public ECalendarType		m_carendar;		// [index:7][load:y] (0.每日定时刷新1.活动开启与关闭)
		public List<Int32>		m_carendarparm = new List<Int32>();		// [index:8][load:y] (m_carendar == ECalendarTypeActivity:活动id)
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			Int32 lm_type = 0;
			if(RCsv.ReadCsv(apair, ref lm_type) == false)
				return false;
			m_type = (ECalendar)lm_type;
			if(RCsv.ReadCsv(apair, m_week) == false)
				return false;
			if(RCsv.ReadCsv(apair, m_serveropen) == false)
				return false;
			if(RCsv.ReadCsv(apair, m_tregularslot) == false)
				return false;
			Int32 lm_carendar = 0;
			if(RCsv.ReadCsv(apair, ref lm_carendar) == false)
				return false;
			m_carendar = (ECalendarType)lm_carendar;
			if(RCsv.ReadCsv(apair, m_carendarparm) == false)
				return false;
			return true;
		}
	}
	class tab_mail : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public string		m_title;		// [index:3][load:y] 邮件标题
		public string		m_author;		// [index:4][load:y] 邮件作者
		public string		m_content;		// [index:5][load:y] 邮件内容
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_title) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_author) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_content) == false)
				return false;
			return true;
		}
	}
	class tab_chat : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public Int32		m_time;		// [index:3][load:y] 聊天发言间隔
		public Int32		m_count;		// [index:4][load:y] 聊天发言最大数量
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_time) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_count) == false)
				return false;
			return true;
		}
	}
	class tab_activity : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public EActivity		m_type;		// [index:3][load:y] 活动类型(1类似咸鱼之王的<<招募达标>>)
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			Int32 lm_type = 0;
			if(RCsv.ReadCsv(apair, ref lm_type) == false)
				return false;
			m_type = (EActivity)lm_type;
			return true;
		}
	}
	class tab_activity_drawcompliance : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public Int32		m_dropid;		// [index:3][load:y] 掉落id
		public Int32		m_mailid;		// [index:4][load:y] 邮件id
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_dropid) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_mailid) == false)
				return false;
			return true;
		}
	}
	class task_condition : ICsvRead
	{
		/*********************************/
		public ETask		m_type;		// ETask(0.玩家等级达到X 1.主公vip等级达到x 2.完成某ID任务)
		public ETaskCondition		m_condition;		
		public Int32		m_parmint;		
		/*********************************/
		public bool Read(CsvPair apair)
		{
			Int32 lm_type = 0;
			if(RCsv.ReadCsv(apair, ref lm_type) == false)
				return false;
			m_type = (ETask)lm_type;
			Int32 lm_condition = 0;
			if(RCsv.ReadCsv(apair, ref lm_condition) == false)
				return false;
			m_condition = (ETaskCondition)lm_condition;
			if(RCsv.ReadCsv(apair, ref m_parmint) == false)
				return false;
			return true;
		}
	}
	class tab_task : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public ETaskType		m_type;		// [index:3][load:y] 
		public List<task_condition>		m_taskreceive = new List<task_condition>();		// [index:4][load:y] 接收此任务的前提(ETask(0.玩家等级达到X 1.主公vip等级达到x 2.完成某ID任务)*ETaskCondition(0.大于等于1.小于等于2.等于)*int32_t(值))
		public List<task_condition>		m_taskcomplete = new List<task_condition>();		// [index:5][load:y] 完成此任务的条件(ETask(0.玩家等级达到X 1.主公vip等级达到x 2.完成某ID任务)*ETaskCondition(0.大于等于1.小于等于2.等于)*int32_t(值))
		public Int32		m_dropid;		// [index:6][load:y] 任务奖励
		public bool		m_autoreceive;		// [index:7][load:y] 是否自动领取
		public Int32		m_mailid;		// [index:8][load:y] 自动领取后是否发送邮件的邮件id(自动领取的邮件id:m_autoreceive == true,当m_autoreceive为ture可以为-1)
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			Int32 lm_type = 0;
			if(RCsv.ReadCsv(apair, ref lm_type) == false)
				return false;
			m_type = (ETaskType)lm_type;
			if(RCsv.ReadCsv(apair, m_taskreceive) == false)
				return false;
			if(RCsv.ReadCsv(apair, m_taskcomplete) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_dropid) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_autoreceive) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_mailid) == false)
				return false;
			return true;
		}
	}
	class obstacles_data : ICsvRead
	{
		/*********************************/
		public List<eobstacles>		m_data = new List<eobstacles>();		
		/*********************************/
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, m_data) == false)
				return false;
			return true;
		}
	}
	class tab_map : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public Int32		m_w;		// [index:3][load:y] x轴宽度
		public Int32		m_l;		// [index:4][load:y] y轴长度
		public Int32		m_nx;		// [index:5][load:y] [视野]x轴格子数量
		public Int32		m_ny;		// [index:6][load:y] [视野]y轴格子数量
		public Int32		m_obstaclenx;		// [index:7][load:y] [寻路]x轴格子数量
		public Int32		m_obstacleny;		// [index:8][load:y] [寻路]y轴格子数量
		public List<obstacles_data>		m_obstacles = new List<obstacles_data>();		// [index:9][load:y] 地图数据
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_w) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_l) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_nx) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_ny) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_obstaclenx) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_obstacleny) == false)
				return false;
			if(RCsv.ReadCsv(apair, m_obstacles) == false)
				return false;
			return true;
		}
	}
	class tab_matching : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public Int32		m_count;		// [index:3][load:y] 玩法达到此人数可以开启玩法
		public Int32		m_time;		// [index:4][load:y] 匹配最长时间
		public bool		m_iswaitconfirm;		// [index:5][load:y] 是否需要等待确认
		public Int32		m_waitconfirmtime;		// [index:6][load:y] 等待确认的时间
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_count) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_time) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_iswaitconfirm) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_waitconfirmtime) == false)
				return false;
			return true;
		}
	}
	class Vector2 : ICsvRead
	{
		/*********************************/
		public Int32		m_x;		
		public Int32		m_y;		
		/*********************************/
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_x) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_y) == false)
				return false;
			return true;
		}
	}
	class tab_plays : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public Int32		m_type;		// [index:3][load:y] 玩法类型 pbnet::eplays
		public Int32		m_mapid;		// [index:4][load:y] 在哪个地图应用该玩法
		public Int32		m_preparation_tm;		// [index:5][load:y] 准备阶段时间
		public Int32		m_play_tm;		// [index:6][load:y] 玩法时间
		public Int32		m_settlement_tm;		// [index:7][load:y] 结算时间
		public List<Vector2>		m_birthpoint = new List<Vector2>();		// [index:8][load:y] 出生点Vector2(m_x:int32_t,m_y:int32_t)	
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_type) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_mapid) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_preparation_tm) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_play_tm) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_settlement_tm) == false)
				return false;
			if(RCsv.ReadCsv(apair, m_birthpoint) == false)
				return false;
			return true;
		}
	}
	class tab_recharge : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// [index:0][load:y] id 
		public string		m_name;		// [index:1][load:y] 名字 
		public string		m_remarks;		// [index:2][load:n] 备注
		public Int32		m_price;		// [index:3][load:y] 价格
		public Int32		m_gold;		// [index:4][load:y] 获得的金币数量
		public Int32		m_bonus;		// [index:5][load:y] 奖励的金币数量
		public Int32		m_firstbonus;		// [index:6][load:y] 首充奖励的金币数量
		public Int32		m_dropid;		// [index:7][load:y] 掉落
		public Int32		m_vipexp;		// [index:8][load:y] 充值该档位赠送的vip经验
		public Int32		m_count;		// [index:9][load:y] 是否限制充值次数
		public List<Int32>		m_activityid = new List<Int32>();		// [index:10][load:y] 必须指定活动开启时才能充值这一档
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(CsvPair apair)
		{
			if(RCsv.ReadCsv(apair, ref m_id) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_name) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_remarks) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_price) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_gold) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_bonus) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_firstbonus) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_dropid) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_vipexp) == false)
				return false;
			if(RCsv.ReadCsv(apair, ref m_count) == false)
				return false;
			if(RCsv.ReadCsv(apair, m_activityid) == false)
				return false;
			return true;
		}
	}
}
