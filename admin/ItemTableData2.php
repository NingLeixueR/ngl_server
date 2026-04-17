<?php 

function items()
{
	//require_once "../controller/action/gmaction/ItemTableData2.php";
	$path ="C:\\Users\\Administrator\\Documents\\GitHub\\ngl_gm\\php\\ItemTableData.csv";
	$myfile = fopen($path, "r") or die("Unable to open file!".$path);
	$datastr = fread($myfile,2000000);
	fclose($myfile);
	$array = explode("\n", $datastr);
	print_r($array);
	
}
items();
?>
