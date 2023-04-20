## 网络攻防端口扫描程序

----

​	Windows平台，使用winsock2库，基于TCP全连接扫描检测目标主机端口是否开放

​	使用多线程加速

```bash
编译指令：gcc '.\scanner.c' -l wsock32 -o '.\scanner.exe'
使用方法：Usage: .\scanner.exe <host> <start_port> <end_port>
example:
'.\scanner.exe' 127.0.0.1 78 81
'.\scanner.exe' 127.0.0.1 1 10000 > res.txt
```

​	

程序流程图：

​		开启多线程，每个线程负责扫描一个端口

![多线程](https://github.com/dhcpack/port-scanner/blob/main/image/%E7%BA%BF%E7%A8%8B.jpg)

​		使用Winsock库进行TCP全连接扫描的流程图

![全连接扫描](https://github.com/dhcpack/port-scanner/blob/main/image/%E8%BF%9E%E6%8E%A5.jpg)