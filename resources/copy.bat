md ..\obj\server\server_db\config
md ..\obj\server\server_db\csv
md ..\obj\server\server_game\config
md ..\obj\server\server_game\csv
md ..\obj\server\server_gateway\config
md ..\obj\server\server_gateway\csv
md ..\obj\server\server_world\config
md ..\obj\server\server_world\csv
xcopy config\*.* ..\obj\server\server_db\config\ /s/y
xcopy config\*.* ..\obj\server\server_db\csv\ /s/y
xcopy config\*.* ..\obj\server\server_game\config\ /s/y
xcopy config\*.* ..\obj\server\server_game\csv\ /s/y
xcopy config\*.* ..\obj\server\server_gateway\config\ /s/y
xcopy config\*.* ..\obj\server\server_gateway\csv\ /s/y
xcopy config\*.* ..\obj\server\server_world\config\ /s/y
xcopy config\*.* ..\obj\server\server_world\csv\ /s/y
cd idl
idl ./idl


