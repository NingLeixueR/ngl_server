local ngldata = require("ngldata").getInstance()
require("ngldata")

function handle(amsgname, amsgjson)
	-- ��λ�ȡdb����:��ȷ�����Ƿ��޸�
	-- �����޸�����ʹ�� ngldata:getconst("��������")
	-- ���޸�����ʹ�� ngldata:get("��������")

	-- ��η������ݸ�client,actor
	-- nguid:actor_type#areaid#dataid
	--send_client("guid", "msgname", "msgjson")
	--send_actor("guid", "msgname", "msgjson")

	local tab = ngldata:get("tab_servers","1")
	if tab ~= nil then
		ngldata:print_table(tab)
	end
	--ngldata:get("kkk","777")

	-- ����ͨ��[amsgname]��[amsgjson]������Ϣ����
	print("msg name:" .. amsgname)
	print(amsgjson)

	

	if amsgname == "np_actormodule_forward<PROBUFF_NET_TESTLUA>" then
		local testlua = ngldata:get("db_testlua", nguidstr2int64str("actor_testlua#1#1"))
		--ngldata:print_table(testlua)
		if testlua ~= nil then
			ngldata:print_table(testlua["mvalue"])
			table.remove(testlua["mvalue"], 1)
			table.remove(testlua["mvalue"], 3)
			table.remove(testlua["mvalue"], 5)
			ngldata:print_table(testlua["mvalue"])
		end
	elseif amsgname == "np_testlua" then
		local ret, data = ngldata:check_outdata("db_testlua", "-1")
		if ret then
			print(data)
		end
	end
end
