local ngldata = require("ngldata").getInstance()

-- json���ݼ�¼һЩ������Ϣ,��ű���Ӧ��actor_id
function init_sysdata(asysjson)
    ngldata:init_sysdata(asysjson)
end

-- ��CPP����ѹ������lua
-- 1�����ݿ����ݣ����check_outdata��check_outdata_all
-- 2��Csv������
-- 3��Config����
-- parm ���� 
-- parm adbname     ��������
-- parm adatajson   json����
-- parm aedit       �����Ƿ�������lua���޸�
function push_data(adbname, adatajson, aedit)
    ngldata:push_data(adbname, adatajson, aedit)
end

function check_outdata(adbname, aactorid)
    return ngldata:check_outdata(adbname, aactorid)
end
