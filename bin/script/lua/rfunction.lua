local ngldata = require("ngldata").getInstance()

-- 将CPP数据压入数据lua
-- 1、数据库数据，配合check_outdata与check_outdata_all
-- 2、Csv表数据
-- 3、Config数据
function push_data(adbname, aactorid, adatajson)
    ngldata:push_data(adbname, aactorid, adatajson)
end

function check_outdata(adbname, aactorid)
    return ngldata:check_outdata(adbname, aactorid)
end

function check_outdata_all(adbname)
    return ngldata:check_outdata_all(adbname)
end
