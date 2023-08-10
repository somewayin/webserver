

WebServer
==============
**项目简介:**  

学习webserver的练手项目.  
1.使用 **线程池 + 非阻塞socket + epoll(ET和LT均实现) + 事件处理(Reactor和模拟Proactor均实现)** 的并发模型  
2.使用**状态机**解析HTTP请求报文，支持解析**GET和POST**请求  
3.访问服务器数据库实现web端用户**注册、登录**功能，可以请求服务器**图片和视频文件**  
4.实现**同步/异步日志系统**，记录服务器运行状态  
5.经Webbench压力测试可以实现**上万的并发连接**数据交换

**项目改进:**  
1.最小堆取代双向链表  
2.循环接收队列取代固定的数组  
3.添加内存池


**项目参考:**  
https://github.com/qinguoyi/TinyWebServer  
https://github.com/shangguanyongshi/WebFileServer






