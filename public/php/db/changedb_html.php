<html>
	<h2>更新数据</h2>
	<form action="./changedb.php" method="post" accept-charset="UTF-8">
		<?php require_once "../serverls.php";?>
		<?php
			//enum ENUM_DB
			//{
			//	ENUM_DB_ACCOUNT = 0;		// 账号数据
			//	ENUM_DB_ROLE = 1;			// 玩家数据
			//	ENUM_DB_BRIEF = 2;			// 玩家简要数据
			//	ENUM_DB_BAG = 3;			// 背包数据
			//	ENUM_DB_KEYVALUE = 4;		// 键值数据
			//	ENUM_DB_MAIL = 5;			// 邮件数据
			//	ENUM_DB_GUILD = 6;			// 工会数据
			//	ENUM_DB_NOTICE = 7;			// 公告数据
			//	ENUM_DB_RANKLIST = 8;		// 排行数据
			//	ENUM_DB_ACTIVITY = 9;		// 活动数据
			//	ENUM_DB_TASK = 10;			// 任务数据
			//	ENUM_DB_COUNT = 11;
			//};
		?>
		<select name="db">
			<option value="0">账号数据</option>
			<option value="1">玩家数据</option>
			<option value="2">玩家简要数据</option>
			<option value="3">背包数据</option>
			<option value="4">键值数据</option>
			<option value="5">邮件数据</option>
			<option value="6">工会数据</option>
			<option value="7">公告数据</option>
			<option value="8">排行数据</option>
			<option value="9">活动数据</option>
			<option value="10">任务数据</option>
		</select><br/>
		data:<br/>
		<textarea id="data" name="data" rows="50" cols="100"></textarea><br/>
		<input type="submit" value="查询"/><br/>
	</form>
</body>
</html>