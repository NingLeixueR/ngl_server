local ngldata = require("ngldata").getInstance()

-- json数据记录一些基本信息,如脚本对应的actor_id
function init_sysdata(asysjson)
    ngldata:init_sysdata(asysjson)
end

-- 将CPP数据压入数据lua
-- 1、数据库数据，配合check_outdata与check_outdata_all
-- 2、Csv表数据
-- 3、Config数据
-- parm 参数 
-- parm adbname     数据名称
-- parm adata_source 数据来源db,csv,dbnsp
-- parm adatajson   json数据
-- parm aedit       数据是否允许在lua中修改
function push_data(adbname, adata_source, adatajson, aedit)
    ngldata:push_data(adbname, adatajson, aedit)
end

-- 检查数据是否被修改
function check_outdata(adbname, aactorid)
    return ngldata:check_outdata(adbname, aactorid)
end

-- 检查数据是否被删除
function check_outdata_del(adbname, aactorid)
    return ngldata:check_outdata_del(adbname, aactorid)
end

-- 消息结束后会自动调用
function dbnsp_auto_save()
    return ngldata:dbnsp_auto_save()
end


