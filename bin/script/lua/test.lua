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
end
