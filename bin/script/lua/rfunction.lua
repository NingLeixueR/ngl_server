local ngldata = require("ngldata").getInstance()

-- ��CPP����ѹ������lua
-- 1�����ݿ����ݣ����check_outdata��check_outdata_all
-- 2��Csv������
-- 3��Config����
function push_data(adbname, aactorid, adatajson)
    ngldata:push_data(adbname, aactorid, adatajson)
end

function check_outdata(adbname, aactorid)
    return ngldata:check_outdata(adbname, aactorid)
end

function check_outdata_all(adbname)
    return ngldata:check_outdata_all(adbname)
end
