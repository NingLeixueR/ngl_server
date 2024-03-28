<html>
	<h2>gm充值</h2>
	<form action="./gmpay.php" method="post" accept-charset="UTF-8">
		<?php 
			require_once "../serverls.php";
		?>
		充值id:<input name="rechangeid" type="text" size="32"/><br/>
		玩家id:<input name="roleid" type="text" size="32"/><br/>
		<input type="submit" value="添加"/><br/>
	</form>
</body>
</html>