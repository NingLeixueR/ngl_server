<?php require_once __DIR__ . '/../auth.php'; check_login(); ?>
<html>
	<h2>删除邮件</h2>
	<form action="./del_mail.php" method="post" accept-charset="UTF-8">
		<?php require_once "../serverls.php";?>
		玩家id:<input name="m_roleid" type="text"/><br/>
		邮件id:<input name="m_mailid" type="text"/><br/>
		<input type="submit" value="删除"/><br/>
	</form>
</body>
</html>