<?php require_once dirname(__FILE__) . '/../auth.php'; check_action(1501); ?>
<html>
	<h2>关闭actor</h2>
	<form action="./close_actor.php" method="post" accept-charset="UTF-8">
		<?php require_once "../serverls.php";?>
		actor_name:<input name="actor_name" type="text"/><br/>
		area:<input name="area" type="text"/><br/>
		dataid:<input name="dataid" type="text"/><br/>
		<input type="submit" value="关闭"/><br/>
	</form>
</body>
</html>