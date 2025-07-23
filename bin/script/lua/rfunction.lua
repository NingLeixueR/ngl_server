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
-- parm adatajson   json数据
-- parm aedit       数据是否允许在lua中修改
function push_data(adbname, adatajson, aedit)
    ngldata:push_data(adbname, adatajson, aedit)
end

function check_outdata(adbname, aactorid)
    return ngldata:check_outdata(adbname, aactorid)
end
