local ngldata = require("ngldata").getInstance()

-- json数据记录一些基本信息,如脚本对应的actor_id
function init_sysdata(asys)
    ngldata:init_sysdata(asys)
end

-- 将CPP数据压入数据lua
-- 1、数据库数据，配合check_outdata与check_outdata_all
-- 2、Csv表数据
-- 3、Config数据
-- parm 参数 
-- parm adbname     数据名称
-- parm adata_source 数据来源db,csv,dbnsp
-- parm adata       json数据
-- parm aedit       数据是否允许在lua中修改
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

-- 检查数据是否被修改
function data_checkout(aname, adataid)
    if adataid == "-1" then
        return ngldata:data_checkout(aname)
    else
        return ngldata:data_checkoutbyid(aname, adataid)
    end
end

-- 检查数据是否被删除
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


