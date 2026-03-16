REM File overview: Automates the start server initdb workflow on Windows.

set /P rebot_value1=ÇëÊäÈëÇø·ş
start node_actor actor %rebot_value1% 1
#start node_login login %rebot_value1% 1
#start node_db db %rebot_value1% 1
start node_db db %rebot_value1% 1 init
#start node_db db %rebot_value1% 1