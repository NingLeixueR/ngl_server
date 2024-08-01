<html>
	<h2>封号</h2>
	<form action="./roleban.php" method="post" accept-charset="UTF-8">
		<?php require_once "../serverls.php";?>
		玩家id:<input name="roleid" type="text"/><br/>
		<select name="stat">
		<option value=0>解封</option>
		<option value=1>封号</option>
		</select>
		<br/>
		<input type="submit" value="确定"/><br/>
	</form>
</body>
</html>