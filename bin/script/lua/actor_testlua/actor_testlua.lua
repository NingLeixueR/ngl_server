local ngldata = require("ngldata").getInstance()
require("ngldata")


function db_loadfinish()
    -- ���ݼ������
end

function handle(amsgname, amsgjson)
	-- ��λ�ȡdb����:��ȷ�����Ƿ��޸�
	-- �����޸�����ʹ�� ngldata:getconst("��������")
	-- ���޸�����ʹ�� ngldata:get("��������")

	-- ��η������ݸ�client,actor
	-- nguid:actor_type#areaid#dataid
	--send_client("guid", "msgname", "msgjson")
	--send_actor("guid", "msgname", "msgjson")
	
	-- ��Ϊ tab_servers ��ֻ���� ���Բ���������޸ģ��޷�ͨ��get��ȡ
	print("ngldata:get ############################")
	local tab = ngldata:get("tab_servers","1")
	if tab ~= nil then
		ngldata:print_table(tab)
	end
	print("############################")
	print("ngldata:getconst ############################")
	local tabconst = ngldata:getconst("tab_servers","1")
	if tabconst ~= nil then
		print("tabconst.m_threadnum = "..tabconst.m_threadnum)
	end
	print("############################")

	-- ����ͨ��[amsgname]��[amsgjson]������Ϣ����
	print("msg name:" .. amsgname)
	print(amsgjson)

	if amsgname == "np_actormodule_forward<PROBUFF_NET_TESTLUA>" then
		-- �˴���db_testlua���ݽ������޸�
		local testlua = ngldata:get("db_testlua", nguidstr2int64str("actor_testlua#1#1"))
		if testlua ~= nil then
			ngldata:print_table(testlua["mvalue"])
			table.remove(testlua["mvalue"], 1)
			ngldata:print_table(testlua["mvalue"])
		end
	elseif amsgname == "np_testlua" then
		local ret, data = ngldata:data_checkout("db_testlua", "-1")
		if ret then
			print(data)
		end
		ngldata:data_del("db_testlua", "34359803960")
	end

	local temp =  ngldata:get("db_brief", nguidstr2int64str("actor_brief#1#1"));
	print(temp)
	print("mlv:"..temp["mlv"])
	temp["mlv"] = 999
	print("mlv:"..temp["mlv"])

	send_actor("actor_testlua2#1#-1", amsgname, amsgjson)
end
