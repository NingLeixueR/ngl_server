<?php

$aaa = array(
	'uid' => true,
	'role_id' => true,
	'server_id' => true,
	'cp_product_id' => true,
	'product_id' => true,
	'money' => true,
	'time' => true,
);

ksort($aaa);

foreach ($aaa as $key => $value) 
{
	echo "{$key}<br/>";		
}

?>