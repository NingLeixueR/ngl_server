local dbdata = require("dbdata").getInstance()
require("dbdata")

function handle(data)
	-- ��λ�ȡdb����:��ȷ�����Ƿ��޸�
	-- �����޸�����ʹ�� dbdata:getconst("��������")
	-- ���޸�����ʹ�� dbdata:get("��������")

	-- ��η������ݸ�client,actor
	-- nguid:actor_type#areaid#dataid
	send_client("guid", "msgname", "msgjson")
	send_actor("guid", "msgname", "msgjson")

	dbdata:get("kkk","1")
	dbdata:get("kkk","777")
end
