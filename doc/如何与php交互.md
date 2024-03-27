* 与php交互需要借助actor_gm<br/>
交互需要通过json<br/>
```
/*
   需要发送给服务器的字段包括
  actor_name   指定actor的类型   单例必须
  actor_id     操作制定actor     非单例必须
  operator     制定操作类型（自定义字符串）
  data         可选数据
*/
以添加公告为例
$arr = array(
 'actor_name' => 'ACTOR_NOTICE',
 'operator' => 'add_notice',
 'data' => array(
   'id' => '-1',
   'notice' => $_POST['notice'],
   'starttime' => intval($_POST['starttime']),
   'finishtime' => intval($_POST['finishtime']),
 )
);

// 此时我们只需要在【bool actor_notice::handle(message<mforward<np_gm>>& adata)】
// 实现对'add_notice'的处理
```
