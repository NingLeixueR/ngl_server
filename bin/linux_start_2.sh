CURRENT_DIR=`pwd`  
nohup $CURRENT_DIR/node actor 2 1 > /dev/null 2>&1 &  
nohup $CURRENT_DIR/node game 2 1 > /dev/null 2>&1 &  
nohup $CURRENT_DIR/node db 2 1 > /dev/null 2>&1 &  
nohup $CURRENT_DIR/node login 2 1 > /dev/null 2>&1 &  
nohup $CURRENT_DIR/node gateway 2 1 > /dev/null 2>&1 &  
nohup $CURRENT_DIR/node world 2 1 > /dev/null 2>&1 &
echo Done!

