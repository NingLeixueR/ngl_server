<?php require_once dirname(__FILE__) . '/../auth.php'; check_action(1502); ?>
<html>
	<h2>Socket缓冲池状态</h2>
	<form action="./socket_pool_stats.php" method="post" accept-charset="UTF-8">
		<?php require_once "../serverls.php";?>
		<?php require_once "../servertype.php";?>
		<input type="submit" value="查询"/><br/>
	</form>
</body>
</html>