<html>
	<h2>修改军团名称</h2>
	<form action="./changname_family.php" method="post" accept-charset="UTF-8">
		<?php require_once "../serverls.php";?>
		军团id:<input name="familyid" type="text"/><br/>
		军团名称:<input name="familyname" type="text"/><br/>
		<input type="submit" value="修改"/><br/>
	</form>
</body>
</html>