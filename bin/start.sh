CURRENT_DIR=`pwd`  
nohup $CURRENT_DIR/node actor 1 1 > /dev/null 2>&1 &  
nohup $CURRENT_DIR/node game 1 1 > /dev/null 2>&1 &  
nohup $CURRENT_DIR/node db 1 1 > /dev/null 2>&1 &  
nohup $CURRENT_DIR/node login 1 1 > /dev/null 2>&1 &  
nohup $CURRENT_DIR/node gateway 1 1 > /dev/null 2>&1 &  
nohup $CURRENT_DIR/node world 1 1 > /dev/null 2>&1 &
echo Done!
