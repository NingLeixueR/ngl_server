local ngldata = require("ngldata").getInstance()
require("ngldata")

function db_loadfinish()
    -- ���ݼ������
end

function handle(amsgname, amsgjson)
	-- ����ͨ��[amsgname]��[amsgjson]������Ϣ����
	print("actor_testlua2.lua")
	print("msg name:" .. amsgname)
	print(amsgjson)
end
