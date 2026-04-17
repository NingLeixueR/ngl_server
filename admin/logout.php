<?php
require_once dirname(__FILE__) . '/auth.php';
do_logout();
header('Location: login.php');
exit;
?>
