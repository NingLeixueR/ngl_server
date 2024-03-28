<html>
	<h2>添加公告</h2>
	<form action="./add_notice.php" method="post" accept-charset="UTF-8">
		<?php require_once "../serverls.php";?>
		公告内容:<input name="notice" type="text" size="32"/><br/>
		开始时间:<input name="starttime" type="text"/><br/>
		结束时间:<input name="finishtime" type="text"/><br/>
		<input type="submit" value="添加"/><br/>
	</form>
</body>
</html>