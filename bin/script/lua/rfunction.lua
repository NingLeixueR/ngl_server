local dbdata = require("dbdata").getInstance()

function push_data(adbname, adatajson)
    dbdata:push_data(adbname, adatajson)
end

function check_outdata(adbname)
    return dbdata:check_outdata(adbname)
end
