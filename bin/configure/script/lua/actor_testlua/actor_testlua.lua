-- File overview: Defines Lua script logic for actor testlua.

local ngldata = require("ngldata").getInstance()
require("ngldata")

function db_loadfinish()
    -- Dataloadcomplete
end

function handle(amsgname, amsg)
	-- Getdbdata: whether
	-- Data ngldata:getconst("dataname")
	-- Data ngldata:get("dataname")

	-- Senddatatoclient,actor
	-- nguid:actor_type#areaid#dataid
	--send_client("guid", "kcp" or "tcp","msgname", "msgjson")
	--send_actor("guid", "msgname", "msgjson")
	
	-- Because tab_servers read-only do not allowplayer, cannotthroughgetget
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

	-- Canthrough[amsgname] [amsgjson] messagehandle
	print("msg name:" .. amsgname)
	print(amsg)

	local ret,id = nguidstr2int64("actor_brief#1#1")
	if ret == false then
		print("nguidstr2int64(\"actor_brief#1#1\") fail")
		return
	end

	if amsgname == "np_actormodule_forward<PROBUFF_NET_TESTLUA>" then
		-- This db_testluadata
		local testlua = ngldata:get("db_testlua", id)
		if testlua ~= nil then
			ngldata:print_table(testlua)
			print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
			ngldata:print_table(testlua["mvalue"])
			print("===================================")
			table.remove(testlua["mvalue"], 1)
			print("===================================")
			ngldata:print_table(testlua["mvalue"])
			print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
		end
	elseif amsgname == "np_testlua" then
		local ret, data = ngldata:data_checkout("db_testlua", "-1")
		if ret then
			print(data)
		end
		ngldata:data_del("db_testlua", "34359803960")
	end

	
	local brief =  ngldata:get("db_brief", id);
	if brief ~= nil then
		print(brief)
		print("mlv:"..brief["mlv"])
		brief["mlv"] = brief["mlv"] + 999
		print("mlv:"..brief["mlv"])
	end
	
	-- 
	send_actor("actor_testlua2#1#-1", amsgname, amsg)
end