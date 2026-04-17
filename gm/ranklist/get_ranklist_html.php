<html>
	<h2>查询排行榜</h2>
	<form action="./get_ranklist.php" method="post" accept-charset="UTF-8">
		<?php require_once "../serverls.php";?>
		排行榜类型(0:lv):<input name="m_type" type="text"/><br/>
		页码(1-n):<input name="m_page" type="text"/><br/>
		<input type="submit" value="查看"/><br/>
	</form>
</body>
</html>