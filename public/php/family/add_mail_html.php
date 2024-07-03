<html>
	<h2>发送邮件</h2>
	<form action="./add_mail.php" method="post" accept-charset="UTF-8">
		<?php require_once "../serverls.php";?>
		玩家id:<input name="roleid" type="text"/><br/>
		邮件内容:<br/>
		<textarea id="content" name="content" rows="10" cols="100"></textarea><br/>
		邮件附件(物品id*物品数量 单行*号分割):<br/>
		<textarea id="items" name="items" rows="5" cols="100"></textarea><br/>
		<input type="submit" value="添加"/><br/>
	</form>
</body>
</html>