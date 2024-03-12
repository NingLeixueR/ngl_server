* 写入日志与BI(经分)<br/>
在[nlog.h]中定义了很多宏<br/>
根据日志级别区分有<br/>

```
enum ELOG
{
  ELOG_DEBUG,   // 测试信息
  ELOG_INFO,    // 打印
  ELOG_WARN,    // 警告
  ELOG_ERROR,   // 错误
  ELOG_NONE,    // 无
};
```

根据日志存储位置区分<br/>
本地与网络<br/>

日志打印分别有以下宏定义<br/>

```
// [测试信息]本地打印
# define LogDebug(FORMAT,...)			dlogmsg(ngl::ELOG_DEBUG, false, FORMAT,##__VA_ARGS__)
// [错误信息]本地打印
# define LogError(FORMAT,...)			dlogmsg(ngl::ELOG_ERROR, false, FORMAT,##__VA_ARGS__)
// [打印信息]本地打印
# define LogInfo(FORMAT,...)			dlogmsg(ngl::ELOG_INFO, false,FORMAT, ##__VA_ARGS__)
// [警告信息]本地打印
# define LogWarn(FORMAT,...)			dlogmsg(ngl::ELOG_WARN, false, FORMAT,##__VA_ARGS__)
// [测试信息]网络打印
#define LogLocalDebug(FORMAT,...)		dlogmsg(ngl::ELOG_DEBUG, true, FORMAT,##__VA_ARGS__)
// [错误信息]网络打印
#define LogLocalError(FORMAT,...)		dlogmsg(ngl::ELOG_ERROR, true, FORMAT,##__VA_ARGS__)
// [打印信息]网络打印
#define LogLocalInfo(FORMAT,...)		dlogmsg(ngl::ELOG_INFO, true,FORMAT,##__VA_ARGS__)
// [警告信息]网络打印
#define LogLocalWarn(FORMAT,...)		dlogmsg(ngl::ELOG_WARN, true, FORMAT,##__VA_ARGS__)
```
使用方法<br/>
```
LogDebug("[测试信息]本地打印")                // 输出：[测试信息]本地打印
LogDebug("[%]%","测试信息",本地打印)          // 输出：[测试信息]本地打印
LogDebug("[%]%-%","测试信息",本地打印，1989)  // 输出：[测试信息]本地打印-1989
```
对try...catch也进行了封装<br/>
```
Try
{
  xxxx
}Catch
```
这样进行捕获会将错误信息自动打印到控制台和文件<br/>

说说本地与网络日志的区分<br/>
就其实现而言分别是创建两个actor_log对象<br/>
每个进程都有一个actor_log，即本地actor_log其guid为nguid::make(ACTOR_LOG, tab_self_area, ELOG_LOCAL);<br/>
当然网络actor_log就可以部署在任意一个进程或者单独为其创建一个进程,其guid为nguid::make(ACTOR_LOG, tab_self_area, ELOG_NETWORK);<br/>
