# 读写分离和负载均衡

## 读写分离

```shell
    进行读写分离，因为写负载是不能够分担的，只能够在主数据库上进行写操作，而读操作在主数据库和从数据库都可以进行，为了减少主数据库的
    负载。
    
    读写分离的目的是要如何在复制集群的不同角色上，去执行不同的SQL语句
    
    1.读写分离方式
        (1) 有程序实现读写分离
                优点：
                    A.有开发人员控制什么样查询在从库上执行，比较灵活
                        由于主从复制机制原因，主从复制会存在延时，这个延迟是以ms为单位，对应实时性非常敏感的业务则不太适合在从库上
                        执行
                        
                    B.有程序直接连接数据库，所以性能损耗比较少
                缺点:
                    A. 增加开发的工作量，使得程序代码更加复杂
                    B. 人为控制，容易出现错误
                        
        (2) 有中间件来实现读写分离
                中间件有 mysql-proxy(解决读写分离，和负载均衡，但其性能和稳定性存在问题，不推荐使用)
                        maxScale(性能损耗比较小，提供读写分离，和多个从服务器读负载均衡)
                优点:
                    A.由中间件根据查询语法分析，自动完成读写分离
                        通过对SQL语法的分析是读操作还是写操作，如果是select读操作则会将其分配到从数据库上处理
                        
                    B.对程序透明，对于已有程序不用做任何调整
                        只要把数据库连接的方式改为通过中间件连接，不用对程序代码进行修改
                        
                缺点:
                    A.增加了中间层，对查询效率有损耗
                        其查询的处理能力就非常依赖以中间层的处理能力，特别是中间层中还要进行权限认证和语法分析的工作
                        
                    B.对于延迟敏感业务无法自动在主库执行
```

## 读的负载均衡

```shell
    读的负载均衡解决的是在相同角色的数据库，如何共同分担相同的负载，同样是读请求，如何分布到多台从数据库进行查询
    
    1.可以通过程序轮询的方式
        有程序来控制具体在哪个服务器上进行查询，这种方式每次增加或则修改从服务器时，则要对应的修改程序，这样不太灵活
    2.通过软件和硬件实现负载均衡
        软件：LVS，Haproxy，MaxScale
        硬件：F5
```

## 中间层(MaxScale)

```shell
    1.MaxScale插件
        (1) Authentication 认证插件
            提供了数据库用户登陆认证的功能，为了能验证用户连接，MaxScale会从后端数据库中读取MySQL的user表中的信息，并将
            此用户信息缓存到MaxScale的服务器端，当用户连接进来后会根据缓存的信息来判断是否通过验证，如果当前的缓存无此用户
            则会从后端数据库进行更新
            
        (2) Protocal 协议插件
            负责Maxscale与外部接口的协议，主要包括客户端到MaxScale的接口，和MaxScale到后端数据库的接口2个方面，目前提供2个协议插件
            一个是MySQL客户端协议插件，另外一个MySQL服务器端的协议插件(用于MaxScale连接后端数据库)
            
        (3) Router 路由插件
                其决定了把前端用户请求如何发送给后端数据库，我们所要实现的读写分离和读负载均衡主要由这个插件实现的，
                有2种路由模块，是readconnroute和readwritesplit，其中readconnroute路由是用来实现多台服务器读负载均衡的，
                而readwritesplit则是实现读写分离的
                
        (4) Monitor 监控插件
                用于MaxScale用于后端数据库进行实施监控，以便MaxScale可以将前段的请求发送到正确的后台数据库中，正确的后台数据库
                是指可以正常对外提供服务的数据库，通过这个模块对主从延迟的监控
                
        (5) Filter&Logging 日志和过滤插件
                提供了简单的数据库防火墙的功能，可以对某些SQL进行过滤和改写，可以进行简单的SQL容错和语句的自动转换
                
    2.安装
        (1) 下载安装包（在监控服务器上安装MaxScale）
                > wget https://downloads.mariadb.com/enterprise/fre6-c9jr/mariadb-maxscale/1.3.0/rhel/6/x86_64/
                              maxscale-1.3.0-1.rhel6.x86_64.rpm
                              
                > yum intall -y libaio.x86_64 libaio-devel.x86_64 novacom-server.x86_64
                > rpm -ivh maxscale-1.3.0-1.rhel6.x86_64.rpm
                
    3.实际部署
        (1) 节点1 192.168.3.100 从服务器
            节点2 192.168.3.101 主服务器
            节点3 192.168.3.102 从服务器
            监控服务器(安装MaxScale)
            
        A. 在主数据库上建立用于MaxScale监控模块的账号
             I.创建用户
                 mysql> create user ‘scale_monitor’ @ ‘192.168.3.%' identified by '123456';
             II.授权
                mysql> grant replication slave,replication client on *.* to 'scale_monitor’ @ ‘192.168.3.%';
                    其中 replication slave 是一种权限类型,需要完成主从复制的监控
                    
        B.在主数据库上建立用于MaxScale路由模块的账号
            I.创建用户
                mysql> create user ‘scale_route’ @ ‘192.168.3.%' identified by '123456';
            II.授权
                mysql> grant select on mysql.* to 'scale_monitor’ @ ‘192.168.3.%';
                
        可选
        在监控服务器上对密码进行加密
            I.生成加密秘钥
                > maxkeys
            II.对密码进行加密
                > maxpasswd (.secrets file 所在的目录) 要加密的密码
                例如：
                > maxpasswd /var/lib/maxscale/ 123456
                
        C.在监控服务器中修改maxscale配置文件
            > vim /etc/maxscale.cnf
                [maxscale]
                threads=4   (maxscale的线程数)
                
                [server1]
                type=server
                address=192.168.3.100
                port=3306
                protocol=MySQLBackend
                
                [server2]
                type=server
                address=192.168.3.101
                port=3306
                protocol=MySQLBackend
                
                [server3]
                type=server
                address=192.168.3.102
                port=3306
                protocol=MySQLBackend
                
                [MySQL Monitor]
                type=monitor
                module=mysqlmon
                servers=server1,server2,server3  (对服务器进行监控)
                user=scale_monitor
                passwd=123456  (这里是明文，如果有通过maxkeys，maxpasswd进行加密，则填写的是加密后的字符串)
                monitor_interval=10000   (监控的时间间隔，单位是ms)
                
                [Read-Write Service]
                type=server
                router=readwritesplit
                servers=server1,server2,server3
                user=scale_route
                passwd=123456  (这里是明文，如果有通过maxkeys，maxpasswd进行加密，则填写的是加密后的字符串)
                max_slave_connections=100% (将从服务器加入到读负载的最大范围，100%代表所有从服务器都进行读负载均衡)
                max_slave_replication_lag=60 (单位秒，如果延时超过60秒，则将该从服务器剔除读的负载均衡中)
                
                把[Read-Only Listener] 删掉
                把[Read-Only Service] 删掉
                
        D. 启动maxscale
            > maxscale --config=/etc/maxscale.cnf
            
        E.登陆maxscale管理界面登陆
            > maxadmin --user=admin --password=mariadb
            结果
                Maxscale>    (其maxscale管理界面命令)
                
                I 在maxscale管理界面中查看启动那些服务器
                    Maxscale>list servers;
                II. 
                    Maxscale>show dbusers "Read-Write Service";
                
                
         
                
          
```