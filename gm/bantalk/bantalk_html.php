<html>
	<h2>禁言</h2>
	<form action="./bantalk.php" method="post" accept-charset="UTF-8">
		<?php require_once "../serverls.php";?>
		玩家id:<input name="m_roleid" type="text"/><br/>
		持续时间:<input name="m_duration" type="text"/><br/>
		<input type="submit" value="禁言"/><br/>
	</form>
</body>
</html>