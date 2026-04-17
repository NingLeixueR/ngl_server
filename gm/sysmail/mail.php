<?php

$title = filter_input(INPUT_GET, "title");
$msg = filter_input(INPUT_GET, "msg");

if (!isset($title) ||
    !isset($msg)) {
    die('Error From');
    return;
}

$u_msg = strToUtf8($msg);
$u_title = strToUtf8($title);

$set_charset = "export LANG=en_US.UTF-8;";

$mailList = fopen("mail.txt", "r") or die("Unable to open mail.txt!");
while ($line = fgets($mailList)) {
    $mailAddr = trim($line);
    system($set_charset . "echo " . $u_msg . " | mail -s " . $u_title . " " . $mailAddr, $result);
    sleep(1);
}
echo 0;

function strToUtf8($str){
    $encode = mb_detect_encoding($str, array("ASCII",'UTF-8',"GB2312","GBK",'BIG5'));
    if($encode == 'UTF-8'){
        return $str;
    }else{
        return mb_convert_encoding($str, 'UTF-8', $encode);
    }
}
?>