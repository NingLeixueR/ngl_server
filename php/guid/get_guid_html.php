<html>
	<h2>获取GUID</h2>
	<form action="./get_guid.php" method="post" accept-charset="UTF-8">
		<?php require_once "../serverls.php";?>
		actor_name:<input name="actor_name" type="text"/><br/>
		area:<input name="area" type="text"/><br/>
		dataid:<input name="dataid" type="text"/><br/>
		<input type="submit" value="获取"/><br/>
	</form>
</body>
</html>