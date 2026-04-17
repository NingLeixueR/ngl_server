<?php require_once __DIR__ . '/../auth.php'; check_login(); ?>
<html>
	<h2>删除公告</h2>
	<form action="./del_notice.php" method="post" accept-charset="UTF-8">
		<?php require_once "../serverls.php";?>
		id:<input name="m_id" type="text"/><br/>
		<input type="submit" value="删除"/><br/>
	</form>
</body>
</html>