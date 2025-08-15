local ngldata = require("ngldata").getInstance()

-- json���ݼ�¼һЩ������Ϣ,��ű���Ӧ��actor_id
function init_sysdata(asys)
    ngldata:init_sysdata(asys)
end

-- ��CPP����ѹ������lua
-- 1�����ݿ����ݣ����check_outdata��check_outdata_all
-- 2��Csv������
-- 3��Config����
-- parm ���� 
-- parm adbname     ��������
-- parm adata_source ������Դdb,csv,dbnsp
-- parm adata       json����
-- parm aedit       �����Ƿ�������lua���޸�
function data_push(aname, asource, adata, aedit)
    print(aname)
    print(asource)
    print(adata)
    print(aedit)
    ngldata:data_push(aname, asource, adata, aedit)
end

function data_del(aname, adataid)
    ngldata:data_del(aname, adataid, true)
end

-- ��������Ƿ��޸�
function data_checkout(aname, adataid)
    if adataid == "-1" then
        return ngldata:data_checkout(aname)
    else
        return ngldata:data_checkoutbyid(aname, adataid)
    end
end

-- ��������Ƿ�ɾ��
function data_checkdel(aname, adataid)
     if adataid == "-1" then
        return ngldata:data_checkdel(aname)
    else
        return ngldata:data_checkdelbyid(aname, adataid)
    end
end

function test(adata)
    ngldata:print_table(adata)
    return adata
end


