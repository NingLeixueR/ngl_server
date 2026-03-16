-- File overview: Defines Lua script logic for rfunction.

local ngldata = require("ngldata").getInstance()

-- Jsondatarecord info, scriptcorresponding actor_id
function init_sysdata(asys)
    ngldata:init_sysdata(asys)
end

-- CPPdata datalua
-- 1, Databasedata, check_outdataandcheck_outdata_all
-- 2, CSVtabledata
-- 3, Configdata
-- Parm parameters
-- Parm adbname dataname
-- Parm adata_source datasourcedb,csv,dbnsp
-- Parm adata data
-- Parm aedit datawhether luain
function data_push(aname, asource, adata, aedit)
    ngldata:data_push(aname, asource, adata, aedit)
end

function data_del(aname, adataid)
    ngldata:data_del(aname, adataid, true)
end

-- Check whether the data has been modified
function data_checkout(aname, adataid)
    print(aname)
    print(adataid)
    if adataid == nil or adataid == -1 then
        return ngldata:data_checkout(aname)
    else
        return ngldata:data_checkoutbyid(aname, adataid)
    end
end

-- Check whether the data has been deleted
function data_checkdel(aname, adataid)
     if adataid == nil or adataid == -1 then
        return ngldata:data_checkdel(aname)
    else
        return ngldata:data_checkdelbyid(aname, adataid)
    end
end

function test(adata)
    ngldata:print_table(adata)
    return adata
end