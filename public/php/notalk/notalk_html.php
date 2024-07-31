<html>
	<h2>禁言</h2>
	<form action="./notalk.php" method="post" accept-charset="UTF-8">
		<?php require_once "../serverls.php";?>
		玩家id:<input name="roleid" type="text"/><br/>
		持续时间:<input name="duration" type="text"/><br/>
		<input type="submit" value="禁言"/><br/>
	</form>
</body>
</html>