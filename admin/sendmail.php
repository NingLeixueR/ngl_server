<?php
// SMTP 配置
$smtp_server = 'smtp.163.com';    // 你的SMTP服务器地址
$smtp_port = 25;                // SMTP端口（587为SMTP使用STARTTLS加密连接的端口）
$username = '15101192850@163.com'; // 你的邮箱地址
$password = 'NChaNkmFgvJbzQ2i'; // 你的邮箱授权码或密码

// 收件人和发件人信息
$from = '348634371@qq.com';
$to = 'libo1@youxigu.com';
$subject = 'Test Email';
$body = 'This is a test email sent using PHP and cURL!';

// 邮件头部
$headers = array(
    'From' => $from,
    'To' => $to,
    'Subject' => $subject,
    'Content-Type' => 'text/plain; charset=UTF-8'
);

// 构建邮件数据
$data = "Date: " . date("r") . "\r\n";
foreach ($headers as $key => $value) {
    $data .= "$key: $value\r\n";
}
$data .= "\r\n$body\r\n";

// 创建cURL会话
$ch = curl_init();

// 设置 cURL 参数
curl_setopt($ch, CURLOPT_URL, "smtp://$smtp_server:$smtp_port");  // 连接到SMTP服务器
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);  // 获取响应内容
curl_setopt($ch, CURLOPT_TIMEOUT, 30);  // 设置超时
curl_setopt($ch, CURLOPT_POST, true);  // 使用POST请求
curl_setopt($ch, CURLOPT_POSTFIELDS, $data);  // 邮件内容
curl_setopt($ch, CURLOPT_USERPWD, "$username:$password");  // 邮箱认证
curl_setopt($ch, CURLOPT_HTTPHEADER, array(
    'Content-Type: text/plain; charset=UTF-8',
));

// 执行 cURL 请求并获取返回结果
$response = curl_exec($ch);

// 检查是否有错误
if ($response === false) {
    echo "cURL Error: " . curl_error($ch);
} else {
    echo "Email sent successfully!";
}

// 关闭 cURL 会话
curl_close($ch);
?>