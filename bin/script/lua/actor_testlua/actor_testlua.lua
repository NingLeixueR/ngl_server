local ngldata = require("ngldata").getInstance()
require("ngldata")


function db_loadfinish()
    -- 数据加载完成
end

function handle(amsgname, amsgjson)
	-- 如何获取db数据:明确需求是否修改
	-- 无需修改数据使用 ngldata:getconst("数据名称")
	-- 需修改数据使用 ngldata:get("数据名称")

	-- 如何发送数据给client,actor
	-- nguid:actor_type#areaid#dataid
	--send_client("guid", "msgname", "msgjson")
	--send_actor("guid", "msgname", "msgjson")
	
	-- 因为 tab_servers 是只读的 所以不允许玩家修改，无法通过get获取
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

	-- 可以通过[amsgname]和[amsgjson]进行消息处理
	print("msg name:" .. amsgname)
	print(amsgjson)

	if amsgname == "np_actormodule_forward<PROBUFF_NET_TESTLUA>" then
		-- 此处对db_testlua数据进行了修改
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
