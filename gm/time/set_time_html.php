<html>
	<h2>设置服务器时间</h2>
	<form action="./set_time.php" method="post" accept-charset="UTF-8">
		<?php require_once "../serverls.php";?>
		<?php require_once "../servertype.php";?>
		年:<input name="year" type="text"/><br/>
		月:<input name="mon" type="text"/><br/>
		日:<input name="day" type="text"/><br/>
		时:<input name="hour" type="text"/><br/>
		分:<input name="min" type="text"/><br/>
		秒:<input name="sec" type="text"/><br/>
		<input type="submit" value="设置"/><br/>
	</form>
</body>
</html>