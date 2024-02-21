namespace ngl
{
	enum ENET_CONFIG
	{
		net_config_send_buff_maxbyte = 8129,	
		net_config_recv_buff_maxbyte = 8129,	
		net_config_logic_pthread_max_size = 8,	
		net_config_socket_pthread_max_size = 8,	
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<ENET_CONFIG> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((ENET_CONFIG)ltemp);
			}
			return true;
		}
	}
	enum EUDP_OPERATOR
	{
		EUDP_OPERATOR_ACK,	// 确认包
		EUDP_OPERATOR_DATA,	// 传输子数据包
		EUDP_OPERATOR_HEARTBEAT,	// 心跳主要用来确认对端是否在线
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<EUDP_OPERATOR> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((EUDP_OPERATOR)ltemp);
			}
			return true;
		}
	}
	enum EPH_HEAD_VAL
	{
		EPH_HEAD_VERSION_SUCCESS = 1,	// 版本一致
		EPH_HEAD_VERSION_FAIL = 2,	// 版本不一致
		EPH_HEAD_VERSION_FOLLOW = 3,	// 无法对比版本,数据没有接收完成
		EPH_HEAD_FOLLOW = 4,	// 包头数据没有接收完成
		EPH_HEAD_SUCCESS = 5,	// 包头数据已接收完成
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<EPH_HEAD_VAL> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((EPH_HEAD_VAL)ltemp);
			}
			return true;
		}
	}
	enum EPH
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
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<EPH> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((EPH)ltemp);
			}
			return true;
		}
	}
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
		FAIL = -1,	// 服务器类型错误
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<NODE_TYPE> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((NODE_TYPE)ltemp);
			}
			return true;
		}
	}
	enum ENET_PROTOCOL
	{
		ENET_TCP = 1,	
		ENET_UDP = 2,	
		ENET_WS = 3,	
		ENET_KCP = 4,	
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<ENET_PROTOCOL> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((ENET_PROTOCOL)ltemp);
			}
			return true;
		}
	}
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
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<EnumModule> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((EnumModule)ltemp);
			}
			return true;
		}
	}
	enum EnumAttribute
	{
		E_Null = 0,	
		E_Attack = 1,	// [攻击]
		E_Defense = 2,	// [防御]
		E_Hp = 3,	// [血量]
		E_Anger = 4,	// [怒气]
		E_Speed = 5,	// [速度]
		E_Count = 6,	
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<EnumAttribute> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((EnumAttribute)ltemp);
			}
			return true;
		}
	}
	enum EItemSrc
	{
		EItemSrcNoraml = 0,	
		EItemSrcGM = 1,	
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<EItemSrc> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((EItemSrc)ltemp);
			}
			return true;
		}
	}
	enum EItemConsume
	{
		EItemConsumeNoraml = 0,	
		EItemConsumeGM = 1,	
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<EItemConsume> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((EItemConsume)ltemp);
			}
			return true;
		}
	}
	enum EItemType
	{
		EEquipment = 1,	// 装备
		EMaterial = 2,	// 材料
		EMoneyGold = 3,	// 金币
		EMoneySilver = 4,	// 银币
		ECard = 5,	// 卡牌
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<EItemType> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((EItemType)ltemp);
			}
			return true;
		}
	}
	enum EQuality
	{
		EQualityGrey = 0,	//灰色
		EQualityGreen = 1,	//绿色
		EQualityYellow = 2,	//黄色
		EQualityOrange = 3,	//橙色
		EQualityRed = 4,	//红色
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<EQuality> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((EQuality)ltemp);
			}
			return true;
		}
	}
	enum EEquip
	{
		EEquipWeapon = 1,	// 武器
		EEquipHelmet = 2,	// 头盔
		EEquipCloth = 3,	// 衣服
		EEquipPants = 4,	// 护腿
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<EEquip> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((EEquip)ltemp);
			}
			return true;
		}
	}
	enum ECalendar
	{
		Week = 0,	// 周几
		ServerOpen = 1,	// 以开服时间以来的天数
		RegularSlot = 2,	// 固定时间段
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<ECalendar> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((ECalendar)ltemp);
			}
			return true;
		}
	}
	enum EActivity
	{
		EActivityDrawCompliance = 1,	// 类似咸鱼之王的<<招募达标>>
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<EActivity> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((EActivity)ltemp);
			}
			return true;
		}
	}
	enum ETask
	{
		ETaskRoleLv = 1,	// 玩家等级
		ETaskRoleVip = 2,	// 玩家vip等级
		ETaskTaskId = 3,	// 完成某ID任务
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<ETask> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((ETask)ltemp);
			}
			return true;
		}
	}
	enum ETaskType
	{
		ETaskTypePrincipalLine,	// 主线任务
		ETaskTypeBranchLine,	// 支线任务
		ETaskTypeDaily,	// 每日任务
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<ETaskType> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((ETaskType)ltemp);
			}
			return true;
		}
	}
	enum eobstacles
	{
		eobstacles_pass = 0,	// 可以通过
		eobstacles_nopass = 1,	// 无法通过
	}
	partial class rcsv
	{
		public static bool readcsv(csvpair apair, List<eobstacles> avec)
		{
			string ltempstr = read(apair);
			csvpair lpair = new csvpair();
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; !isok(lpair);)
			{
				Int32 ltemp = 0;
				if (readcsv(lpair, ref ltemp))
					avec.Add((eobstacles)ltemp);
			}
			return true;
		}
	}
	class tab_servers : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		
		public string		m_name;		
		public string		m_remarks;		
		public Int16		m_area;		// 区服
		public NODE_TYPE		m_type;		// 服务器类型(1db2actorserver3game4gateway5login6robot7world8log9reloadcsv10reloadcsv_tools)
		public Int32		m_tcount;		// 同类型服务器的序号
		public ENET_PROTOCOL		m_net;		// 服务器协议(1tcp2udp3ws)
		public string		m_ip;		// ip
		public string		m_nip;		// 内网ip
		public Int16		m_port;		// 端口
		public Int32		m_threadnum;		// socket线程数
		public Int32		m_actorthreadnum;		// actor线程池线程数
		public bool		m_outernet;		// 是否允许外网访问
		public Int32		m_db;		// 连接的db进程id
		public Int32		m_reloadcsv;		// 连接的reloadcsv进程id
		public Int32		m_login;		// 连接的login进程id
		public Int16		m_crossarea;		// 跨服区服
		public bool		m_isopenkcp;		// 是否开启kcp(m_port+10000)
		public List<Int32>		m_actorserver = new List<Int32>();		// 连接的actorserver进程id(跨服需要填写多个actorserver)
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_area) == false)
				return false;
			Int32 lm_type = 0;
			if(rcsv.readcsv(apair, ref lm_type) == false)
				return false;
			m_type = (NODE_TYPE)lm_type;
			if(rcsv.readcsv(apair, ref m_tcount) == false)
				return false;
			Int32 lm_net = 0;
			if(rcsv.readcsv(apair, ref lm_net) == false)
				return false;
			m_net = (ENET_PROTOCOL)lm_net;
			if(rcsv.readcsv(apair, ref m_ip) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_nip) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_port) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_threadnum) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_actorthreadnum) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_outernet) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_db) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_reloadcsv) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_login) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_crossarea) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_isopenkcp) == false)
				return false;
			if(rcsv.readcsv(apair, m_actorserver) == false)
				return false;
			return true;
		}
	}
	class tab_dbload : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		
		public string		m_name;		
		public string		m_remarks;		
		public bool		m_isloadall;		// 是否需要启动加载全部数据
		public bool		m_network;		// 是否允许通过网络分发
		public Int32		m_sendmaxcount;		// 单次最多发送数量
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_isloadall) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_network) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_sendmaxcount) == false)
				return false;
			return true;
		}
	}
	class tab_errormessage : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		
		public string		m_name;		
		public string		m_remarks;		
		public string		m_message;		// 错误描述
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_message) == false)
				return false;
			return true;
		}
	}
	class tab_specialid : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		
		public string		m_name;		
		public string		m_remarks;		
		public string		m_value;		
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_value) == false)
				return false;
			return true;
		}
	}
	class tab_attribute : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		
		public string		m_name;		
		public string		m_remarks;		
		public float		m_fightcoefficient;		// 属性提供的战力系数
		public Int32		m_initvalue;		// 属性初始值
		public Int32		m_uplimit;		// 上限
		public Int32		m_lowlimit;		// 下限
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_fightcoefficient) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_initvalue) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_uplimit) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_lowlimit) == false)
				return false;
			return true;
		}
	}
	class tab_item : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		
		public string		m_name;		
		public string		m_remarks;		
		public Int32		m_ranks;		// 排序值
		public EItemType		m_type;		// 物品类型EItemType(1装备,2材料,3金币,4银币)
		public Int32		m_quality;		// 物品品质(EQuality)
		public bool		m_isstack;		// 是否可堆叠
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_ranks) == false)
				return false;
			Int32 lm_type = 0;
			if(rcsv.readcsv(apair, ref lm_type) == false)
				return false;
			m_type = (EItemType)lm_type;
			if(rcsv.readcsv(apair, ref m_quality) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_isstack) == false)
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
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_isratio) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_value) == false)
				return false;
			return true;
		}
	}
	class tab_equip : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		
		public string		m_name;		
		public string		m_remarks;		
		public EEquip		m_position;		// 部位EEquip(1武器2头盔3衣服4护腿)
		public List<attributevalue>		m_attributevalues = new List<attributevalue>();		// 装备属性
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			Int32 lm_position = 0;
			if(rcsv.readcsv(apair, ref lm_position) == false)
				return false;
			m_position = (EEquip)lm_position;
			if(rcsv.readcsv(apair, m_attributevalues) == false)
				return false;
			return true;
		}
	}
	class tab_equiplv : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		
		public string		m_name;		
		public string		m_remarks;		
		public Int32		m_consume;		// 升级消耗id
		public List<attributevalue>		m_attributevalues = new List<attributevalue>();		// 装备属性
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_consume) == false)
				return false;
			if(rcsv.readcsv(apair, m_attributevalues) == false)
				return false;
			return true;
		}
	}
	class tab_card : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		
		public string		m_name;		
		public string		m_remarks;		
		public EQuality		m_quality;		// 品质EQuality(0灰色1绿色2黄色3橙色4红色)
		public List<attributevalue>		m_attributevalues = new List<attributevalue>();		// 属性
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			Int32 lm_quality = 0;
			if(rcsv.readcsv(apair, ref lm_quality) == false)
				return false;
			m_quality = (EQuality)lm_quality;
			if(rcsv.readcsv(apair, m_attributevalues) == false)
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
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_min) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_max) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_weight) == false)
				return false;
			return true;
		}
	}
	class tab_random : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		
		public string		m_name;		
		public string		m_remarks;		
		public bool		m_exclusive;		// 排他性多次掉落时使用（true不会掉落出已掉落物品 false会掉落出已掉落物品）
		public Int32		m_count;		// 随机数量
		public Int32		m_calendarids;		// 是否关联日历表,只有日历触发才会执行此掉落
		public List<trandom>		m_randomdatas = new List<trandom>();		// trandom(id*min*max*weight)
		public List<Int32>		m_childrandomids = new List<Int32>();		// 子随机
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_exclusive) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_count) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_calendarids) == false)
				return false;
			if(rcsv.readcsv(apair, m_randomdatas) == false)
				return false;
			if(rcsv.readcsv(apair, m_childrandomids) == false)
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
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_tid) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_count) == false)
				return false;
			return true;
		}
	}
	class tab_consume : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		
		public string		m_name;		
		public string		m_remarks;		
		public List<tconsume>		m_consumeitems = new List<tconsume>();		// tid*count
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, m_consumeitems) == false)
				return false;
			return true;
		}
	}
	class tab_synthesis : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		
		public string		m_name;		
		public string		m_remarks;		
		public Int32		m_consume;		// 消耗列表
		public Int32		m_generate;		// 随机表(tab_rando)id 随机成功物品列表
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_consume) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_generate) == false)
				return false;
			return true;
		}
	}
	class tweek : ICsvRead
	{
		/*********************************/
		public Int32		m_week;		// 周几
		public string		m_opentime;		//  开启时间 HH:mm:ss
		public string		m_closetime;		//  结束时间 HH:mm:ss
		/*********************************/
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_week) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_opentime) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_closetime) == false)
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
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_beg) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_opentime) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_end) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_closetime) == false)
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
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_opentime) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_closetime) == false)
				return false;
			return true;
		}
	}
	class tab_calendar : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		
		public string		m_name;		
		public string		m_remarks;		
		public ECalendar		m_type;		//ECalendar(0:周几,1:以开服时间以来的天数,2:固定时间段)
		public List<tweek>		m_week = new List<tweek>();		//m_type=0,tweek(周几(1-7)*开启时间HH:mm:ss*结束时间HH:mm:ss)
		public List<tserveropen>		m_serveropen = new List<tserveropen>();		//m_type=1,tserveropen(开服后多少天开启*开启时间HH:mm:ss*开服后多少天结束*结束时间HH:mm:ss)
		public List<tregularslot>		m_tregularslot = new List<tregularslot>();		//m_type=2,tregularslot(开启时间YYYY/MM/DD HH:mm:ss*结束时间YYYY/MM/DD HH:mm:ss)
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			Int32 lm_type = 0;
			if(rcsv.readcsv(apair, ref lm_type) == false)
				return false;
			m_type = (ECalendar)lm_type;
			if(rcsv.readcsv(apair, m_week) == false)
				return false;
			if(rcsv.readcsv(apair, m_serveropen) == false)
				return false;
			if(rcsv.readcsv(apair, m_tregularslot) == false)
				return false;
			return true;
		}
	}
	class tab_mail : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		
		public string		m_name;		
		public string		m_remarks;		
		public string		m_title;		// 邮件标题
		public string		m_author;		// 邮件作者
		public string		m_content;		// 邮件内容
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_title) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_author) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_content) == false)
				return false;
			return true;
		}
	}
	class tab_chat : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		
		public string		m_name;		
		public string		m_remarks;		
		public Int32		m_time;		// 聊天发言间隔
		public Int32		m_count;		// 聊天发言最大数量
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_time) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_count) == false)
				return false;
			return true;
		}
	}
	class tab_activity : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// 活动id
		public string		m_name;		
		public string		m_remarks;		
		public EActivity		m_type;		// 活动类型(1类似咸鱼之王的<<招募达标>>)
		public Int32		m_calendarid;		// 关联的脚本日历
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			Int32 lm_type = 0;
			if(rcsv.readcsv(apair, ref lm_type) == false)
				return false;
			m_type = (EActivity)lm_type;
			if(rcsv.readcsv(apair, ref m_calendarid) == false)
				return false;
			return true;
		}
	}
	class tab_activity_drawcompliance : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// 累计次数
		public string		m_name;		
		public string		m_remarks;		
		public Int32		m_dropid;		// 掉落id
		public Int32		m_mailid;		// 邮件id
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_dropid) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_mailid) == false)
				return false;
			return true;
		}
	}
	class task_receive : ICsvRead
	{
		/*********************************/
		public ETask		m_receivetype;		// ETask(1.玩家等级达到X 2.主公vip等级达到x 3.完成某ID任务)
		public List<Int32>		m_parmint = new List<Int32>();		
		/*********************************/
		public bool Read(csvpair apair)
		{
			Int32 lm_receivetype = 0;
			if(rcsv.readcsv(apair, ref lm_receivetype) == false)
				return false;
			m_receivetype = (ETask)lm_receivetype;
			if(rcsv.readcsv(apair, m_parmint) == false)
				return false;
			return true;
		}
	}
	class task_complete : ICsvRead
	{
		/*********************************/
		public ETask		m_completetype;		// ETask(1.玩家等级达到X 2.主公vip等级达到x 3.完成某ID任务)
		public List<Int32>		m_parmint = new List<Int32>();		
		/*********************************/
		public bool Read(csvpair apair)
		{
			Int32 lm_completetype = 0;
			if(rcsv.readcsv(apair, ref lm_completetype) == false)
				return false;
			m_completetype = (ETask)lm_completetype;
			if(rcsv.readcsv(apair, m_parmint) == false)
				return false;
			return true;
		}
	}
	class tab_task : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// 任务id
		public string		m_name;		
		public string		m_remarks;		
		public ETaskType		m_type;		
		public Int32		m_dropid;		// 任务奖励
		public List<task_receive>		m_taskreceive = new List<task_receive>();		// 接收此任务的前提
		public List<task_complete>		m_taskcomplete = new List<task_complete>();		// 完成此任务的条件
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			Int32 lm_type = 0;
			if(rcsv.readcsv(apair, ref lm_type) == false)
				return false;
			m_type = (ETaskType)lm_type;
			if(rcsv.readcsv(apair, ref m_dropid) == false)
				return false;
			if(rcsv.readcsv(apair, m_taskreceive) == false)
				return false;
			if(rcsv.readcsv(apair, m_taskcomplete) == false)
				return false;
			return true;
		}
	}
	class obstacles_data : ICsvRead
	{
		/*********************************/
		public List<eobstacles>		m_data = new List<eobstacles>();		
		/*********************************/
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, m_data) == false)
				return false;
			return true;
		}
	}
	class tab_map : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// 地图id
		public string		m_name;		
		public string		m_remarks;		
		public Int32		m_w;		// x轴宽度
		public Int32		m_l;		// y轴长度
		public Int32		m_nx;		// [视野]x轴格子数量
		public Int32		m_ny;		// [视野]y轴格子数量
		public Int32		m_obstaclenx;		// [寻路]x轴格子数量
		public Int32		m_obstacleny;		// [寻路]y轴格子数量
		public List<obstacles_data>		m_obstacles = new List<obstacles_data>();		// 地图数据
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_w) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_l) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_nx) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_ny) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_obstaclenx) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_obstacleny) == false)
				return false;
			if(rcsv.readcsv(apair, m_obstacles) == false)
				return false;
			return true;
		}
	}
	class tab_matching : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// 玩法类型 pbnet::eplays
		public string		m_name;		
		public string		m_remarks;		
		public Int32		m_count;		// 玩法达到此人数可以开启玩法
		public Int32		m_time;		// 匹配最长时间
		public bool		m_iswaitconfirm;		// 是否需要等待确认
		public Int32		m_waitconfirmtime;		// 等待确认的时间
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_count) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_time) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_iswaitconfirm) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_waitconfirmtime) == false)
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
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_x) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_y) == false)
				return false;
			return true;
		}
	}
	class tab_plays : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// 玩法
		public string		m_name;		
		public string		m_remarks;		
		public Int32		m_type;		// 玩法类型 pbnet::eplays
		public Int32		m_mapid;		// 在哪个地图应用该玩法
		public Int32		m_preparation_tm;		// 准备阶段时间
		public Int32		m_play_tm;		// 玩法时间
		public Int32		m_settlement_tm;		// 结算时间
		public List<Vector2>		m_birthpoint = new List<Vector2>();		// 出生点Vector2(m_x:int32_t,m_y:int32_t)	
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_type) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_mapid) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_preparation_tm) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_play_tm) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_settlement_tm) == false)
				return false;
			if(rcsv.readcsv(apair, m_birthpoint) == false)
				return false;
			return true;
		}
	}
	class tab_recharge : ICsvRead, ICsv
	{
		/*********************************/
		public Int32		m_id;		// 充值id
		public string		m_name;		
		public string		m_remarks;		
		public Int32		m_price;		// 价格
		public Int32		m_gold;		// 获得的金币数量
		public Int32		m_bonus;		// 奖励的金币数量
		public Int32		m_firstbonus;		// 首充奖励的金币数量
		public Int32		m_dropid;		// 掉落
		public Int32		m_vipexp;		// 充值该档位赠送的vip经验
		public Int32		m_count;		// 是否限制充值次数
		public List<Int32>		m_activityid = new List<Int32>();		// 必须指定活动开启时才能充值这一档
		/*********************************/
		public Int32 Id(){return m_id;}
		public bool Read(csvpair apair)
		{
			if(rcsv.readcsv(apair, ref m_id) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_name) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_remarks) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_price) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_gold) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_bonus) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_firstbonus) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_dropid) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_vipexp) == false)
				return false;
			if(rcsv.readcsv(apair, ref m_count) == false)
				return false;
			if(rcsv.readcsv(apair, m_activityid) == false)
				return false;
			return true;
		}
	}
}
