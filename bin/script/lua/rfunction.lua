local dbdata = require("dbdata").getInstance()

function push_data(adbname, aactorid, adatajson)
    dbdata:push_data(adbname, aactorid, adatajson)
end

function check_outdata(adbname, aactorid)
    return dbdata:check_outdata(adbname, aactorid)
end

function check_outdata_all(adbname)
    return dbdata:check_outdata_all(adbname)
end
