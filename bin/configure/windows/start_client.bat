REM File overview: Automates the start client workflow on Windows.

set /P rebot_value1=区服id
set /P rebot_value2=实例id
set /P rebot_value3=请输入玩家id
set /P rebot_value4=协议(ws,wss,tcp)
start node_robot robot %rebot_value1% %rebot_value2% libo %rebot_value3% %rebot_value3% %rebot_value4%
#robot 1 1 libo 7777 7777