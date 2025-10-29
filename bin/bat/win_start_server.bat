#一区
set /P area=区服id
start node_actor actor %area% 1
start node_game game %area% 1
#start node_game game %area% 2
start node_db db %area% 1
start node_login login  %area% 1
start node_log log  %area% 1
start node_gateway gateway %area% 1
#start node_gateway gateway %area% 2
start node_world world %area% 1




