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
-- parm adata_source ������Դdb,csv,dbnsp
-- parm adatajson   json����
-- parm aedit       �����Ƿ�������lua���޸�
function push_data(adbname, adata_source, adatajson, aedit)
    ngldata:push_data(adbname, adatajson, aedit)
end

-- ��������Ƿ��޸�
function check_outdata(adbname, aactorid)
    return ngldata:check_outdata(adbname, aactorid)
end

-- ��������Ƿ�ɾ��
function check_outdata_del(adbname, aactorid)
    return ngldata:check_outdata_del(adbname, aactorid)
end

-- ��Ϣ��������Զ�����
function dbnsp_auto_save()
    return ngldata:dbnsp_auto_save()
end


