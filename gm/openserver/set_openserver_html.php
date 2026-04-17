<html>
	<h2>开启服务器</h2>
	<form action="./set_openserver.php" method="post" accept-charset="UTF-8">
		<?php require_once "../serverls.php";?>
		<br/>
		开服时间:<input name="m_time" type="text"/>
		<br/>
		<a href="http://tool.chinaz.com/Tools/unixtime.aspx" target="_blank">获取utc时间的工具</a><br/>
		<input type="submit" value="确定"/><br/>
	</form>
</body>
</html>