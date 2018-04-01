# Mysql 高可用架构设计

## 概念

```shell
    1.高可用： HA(high availability)指的是尽量缩短因日常维护操作和突发系统崩溃所导致的停机时间，以提高系统和应用的可用性。
              可能在MySQL中存在严重的主从延迟，主从复制中断，锁引起的大量阻塞这些情况下，其对于应用程序来说已经不能正常使用
              数据库

```

## 如何实现高可用

```shell
    1.避免导致系统不可用的因素，减少系统不可用的时间
            系统不可用的因素
                (1) 服务器磁盘空间耗尽
                        备份或则各种查询日志突增导致的磁盘空间被占满。MySQL由于无法记录记录二进制日志，无法处理新的请求
                        而产生的系统不可用的故障
                (2) 性能糟糕的SQL
                (3) 表结构和索引没有优化
                (4) 主从数据不一致
                (5) 人为的操作失误
                
            解决方案:
                (1) 建立完善的监控和报警系统
                        对于监控和报警最重要的是避免错报和漏报，太多的错误容易使人们忽略报警消息
                (2) 对备份数据进行恢复测试
                        有很多原因造成备份数据的损坏，比如远程传输中由于网络的原因造成文件损坏，这样导致我们需要备份数据进行
                        数据库还原时，造成无法还原数据库的问题，造成大量系统不可用的时间，所以很有必要对备份数据进行恢复测试，
                        特别是远程传输的备份文件
                        
                (3) 正确配置数据库环境
                        在主从复制中，从服务器一定要配置为 只读 模式，这样可以减少很多主从不一致而造成系统不可用的故障。
                (4) 对不需要的数据进行归档和清理
                        可以减少数据库中数据量的大小来增加查询的性能，还能减少磁盘的消耗，比如把存储中innodb的历史数据
                        归档到Archive存储引擎中，就可以节约大量的存储空间，而前提条件是一定要对innodb采用独立表空间，
                        否则清理了不可用的历史数据也很难对系统表空间进行收缩。
                        
    2.增加系统冗余，保证发生系统不可用时可以尽快恢复
            解决方案:
                (1) 避免存在单点故障
                        单纯的增加从服务的数据并不能达到增加系统冗余的效果，因为在主从架构中增加从服务器只能避免
                        从数据库出现单点故障，但是对于主数据库则存在单点故障
                        
                (2) 主从切换及故障转移
                        这包括如何从多个从库中选出新的主数据库，并把其他的从数据库对新的主数据库进行同步，和如何通知上层应用
                        新的主数据库的相关信息(如ip地址)，使得上层的应用能够正确得连接到新的主数据库，进行正常的业务交互，
                       
```

## 单点故障

```shell
    1.概念
        单点故障是指一个系统中提供相同功能的组件只有一个，如果这个组件失效了，就会影响整个系统功能的正常使用，组成应用系统
        的各个组件都有可能成为单点。
        
    2.解决方案:
        (1) 利用SUN共享存储或则DRDB磁盘复制解决MySQL单点故障
                在SUN使用共享存储时
                    特点:
                        2台服务器能够正常挂载相同的文件系统，但是只有一台服务器提供服务，如果当前提供服务服务器宕机了，
                        那么备用的服务器则向外提供服务，其可以避免除存储外其他任何组件失效所引起的数据丢失，
                        同时也可以解决非存储组件的
                单点问题
                    缺点：
                        使用存储共享本身是一个单点，如果共享存储恢复需要更长的时间，并且共享存储对应随机IO的性能并不理想。
                        SUN使用共享存储并不是好的解决单点故障的方式
                
                DRDB磁盘复制
                    特点
                         一个以linux的内核模块方式实现块级别的同步复制技术，它通过网卡将主服务器的每一个块
                         复制到另外一个服务器的块设备上，并且在主设备提交 块之前就会记录下，当主设备失效时，
                         就会把备用设备提升为主设备，从而实现故障转移的方式。这种方式与SUN共享存储很相似，都有一个热备的机器，
                         开始提供服务时，会使用和故障机器相同的数据，不同的是DRDB对于存储进行镜像，而不是共享存储，
                         所以当使用DRDB时，会获得一份相同的复制数据，也就是说使用DRDB是数据会有冗余的，
                         所以存储与数据本身都不会存在单点的问题，
                         
                    缺点：
                        DRDB也有其本身的问题，如果故障转移时所需要的时间比较长备用服务器处理被动模式，
                        而不能提供对外服务包括读服务，所以成本比较高。另外磁盘由于本身的bug造成了磁盘文件的损坏，导致
                        DRDB的主备设备的文件都会被损坏
                        
        (2) 利用多写集群或则NDB集群来解决MySQL单点故障
        
                多写集群
                    特点
                        目前提供的多写集群是Percona公司提供的pxc集群，在pxc集群中对于一个事务只有集群中的所有服务器全部提交后
                        任务才能被提交，如果集群中有一台服务器不能提交事务，那么所在的集群的服务器上都会对这个事务进行回滚操作
                        从这点上可以看出pxc集群的性能实际上是取决于集群中服务器性能最差的性能
                        
                    优点：
                        集群中所有的服务的数据都是同步的，不存在数据的延时问题，集群中任意一台服务器损坏都不会造成单点的问题
                        
                    缺点:
                        整个集群的性能取决于集群中服务器性能最差的，就算是集群中所有服务器的性能一样，pxc集群时MySQL的写入性能
                        也比单台MySQL服务器的性能要差，另外pxc集群只支持innodb存储引擎，如果我们的数据库中还使用其他的存储引擎
                        就不能使用pxc集群的方式
                        
                NDB集群(很少被用于生产环境中)
                    特点
                        在NDB集群中所有的节点都会同步得进行主主复制，也就是说我们可以在任何节点上写入，在NDB集群中所有的节点
                        都有相同的读写能力，并且每一行数据都是冗余存储的，因此就算一个节点损坏也不会丢失数据
                        
                    缺点
                        目前的NDB集群中所有的数据都要求存储在内存中，如果内存不足，NDB集群的性能就会非常差
                        
        (3) 利用MySQL的主从复制来解决MySQL的单点复制
                关键是如何解决主服务器的单点问题
                    (1) 主服务器切换后，如何通知应用 新的主服务器的IP地址
                    (2) 如何检测MySQL主服务器是否可用
                    (3) 如何处理从服务器和新的主服务器之间的那种复制关系
                
                可以通过复制管理组件解决主服务器的单点问题，有MMM架构和MHA架构
```

## MMM架构

```shell
    1.概念：
        MMM(Multi-Master Replication Manager) 其用于管理MySQL主主同步架构的工具集

        MMM架构主要作用
            (1) 监控和管理MySQL的主主复制拓扑，并在当前的主服务器失效时，进行主和 主备服务器 之间的主从切换和故障转移等工作。
            
        MMM架构提供的功能
            (1) 监控MySQL主从复制，主主同步健康情况
                    在MMM架构下只有一台主服务器向外提供服务(写操作)，另外一台主数据库只能进行只读模式不能进行写操作，在MMM架构
                    下出现主从延时时，或则主从复制链路中断时，会对主数据库进行故障转移，并且将相应的虚拟ip迁移到新的主数据库上，
                    这样对于上层应用来说不会受到主数据库宕机的影响
                    
            (2) 在主库宕机是进行故障转移并 自动配置其它从数据库对新主数据库的复制
                    需要进行的工作
                        A 如何找到从库需要进行数据同步时对应的新的主库日志同步点
                        B 如果存在多个从库出现数据不一致的情况如何处理
                    MMM架构对于以上2点的工作做的并不安全，它只是简单得找到新的主库的当前的日志点，然后使所有的从库日志点进行同步
                    所以在一个繁忙的系统中使用MMM架构很有可能对数据造成丢失的情况
                    
            (3) 提供主数据库的一个写虚拟ip，多个读的虚拟ip，在主数据库出现问题时可以自动迁移虚拟ip。
                    写的虚拟ip只能在2个主数据库之间进行切换，而读虚拟ip则可以在集群的所有主从服务器上进行切换
                    
        MMM架构的实际部署
            (1)
                由于MMM架构只能用于 主备模式的主主复制,处理活动的主数据库外，其他的服务器都要设置为只读模式，
                MMM架构中增加了一个用于监控各个服务器状态的监控服务器，在进行MySQL安装时我们要在MMM集群中每一台服务器上
                安装一个监控代理的软件，MMM监控服务器通过和各个服务器的代理软件进行交互来完成MMM集群中各个服务器状态的监控和
                角色迁移，为了能够部署MMM集群，我们需要部署MySQL主主复制之外还需要额外的资源
                
            (2) MMM部署所需要的资源
                    A. 主DB服务器  数量(2个) 用于主备模式的主主复制配置,这2台服务器的配置要相同，其cpu，内存以及数据库MySQL
                                           的版本要一致，否则经过故障转移后很可能由于配置不同而造成性能下降的问题，
                    B. 从DB服务器  数量(0-N) 可以配置0台或则多台从服务器，但不建议太多，这些从数据库服务器可以分担主服务器的
                                            读负载，不需要配置太多的从数据库因为在主从切换时可能出现的概率越大。
                    C. 监控服务器  数量(1个) 用来监控MySQL复制集群，主要用来监控主从服务器的情况，和主从链路的情况，在主数据库
                                           出现宕机或则主从延时大于某个阀值时，会对虚拟的ip进行切换，在一个监控服务器上可以
                                           监控多个MMM集群，同时监控服务器最好是可以使用独立的服务器资源，也可以把监控服务器
                                           安装在任意一个从服务器上，目前MMM架构的监控服务器存在单点问题，也就是说MMM集群
                                           中只能有一个监控服务器，不过监控服务器的宕机不会影响正常的业务，只是无法进行
                                           故障转移和主从切换
                    D. ip地址     数量(2*(n+1)) n为MySQL服务器的数量，每一个数据库服务器上都会有2个ip，一个是服务器本身的物理ip
                                               另外一个是读虚拟ip，还需要一个写虚拟ip资源，写虚拟ip只会在主数据库之间进行切换
                                               当活动的主数据库服务器无法访问时，MMM监控服务器会把写虚拟ip切换到主备服务器上。
                                               同时设置原来的主数据库服务器为只读模式
                    E. 监控用户   数量(1个) 用于监控数据库状态的MySQL用户(replication client)
                    F. 代理用户   数量(1个) 用于MMM代理的MySQL用户(super,replication client,process)
                    G. 复制用户   数量(1个) 用户配置MySQL复制的MySQL用户(replication slave)
                    
            (3) 部署步骤
                    A. 配置主主复制及主从同步集群
                    B. 安装主从节点所需要的支持包
                    C. 安装及配置MMM工具集
                    D. 运行MMM监控服务
                    E. 测试
                            是否可以进行主从切换，以及是否进行ip漂移等
                            
                    例子：
                        MMM演示拓扑
                            节点1：活动主数据库服务器 192.168.3.100
                            节点2: 备用主数据库服务器 192.168.3.101
                            节点3: 从数据库服务器    192.168.3.102
                            
                        1.对节点1(活动主)进行操作
                            (1) 在节点1上建立复制账号
                                    I.创建用户
                                        mysql> create user ‘jame’ @ ‘192.168.3.%' identified by '123456';
                                    II.授权
                                        mysql> grant replication slave on *.* to 'jame’ @ ‘192.168.3.%';
                                        其中 replication slave 是一种权限类型
                           (2)  配置节点1服务器(在/etc/my.cnf)
                                    I.  log-bin=mysql-bin  
                                        (用来启动mysql二进制日志，指定mysql二进制日志名字规范 /home/mysql/log/mysql-bin)
                                    II. server_id = 100 (在所有数据库中要确保唯一)
                                    可选：
                                        max_binlog_size = 1000M
                                        binlog_format = row
                                        expire_logs_days = 7
                                        sync_binlog = 1
                                        
                           (3) 在节点1中进行mysqldump备份
                                  > mysqldump --master-data=2 -single-transaction --triggers --routines 
                                    --all-databases -uroot -p >> all.mysql
                                    
                        下面2个步骤是配置主主相互同步操作
                        2.对节点2(备用主数据库)进行操作(进行主从复制)
                            (1) 对节点2(备用主数据库)进行数据恢复
                                    > mysql -uroot -p < all.mysql     (将备份数据库文件进行导入)
                                    
                            (2) 配置节点2服务器(在/etc/my.cnf)
                                    I.  log-bin=mysql-bin  
                                        (用来启动mysql二进制日志，指定mysql二进制日志名字规范 /home/mysql/log/mysql-bin)
                                    II. server_id = 101 (在所有数据库中要确保唯一)
                                    可选：
                                        max_binlog_size = 1000M
                                        binlog_format = row
                                        expire_logs_days = 7
                                        sync_binlog = 1
                                    
                            (3) 配置复制链路
                                    mysql> change master to master_host='192.168.1.100',master_user='jame',
                                           master_password='123456'，master_log_file='mysql-bin.000003',
                                           master_log_pos=1893;
                                                            
                                    其中master_log_file='mysql-bin.000003',master_log_pos=1893;
                                    可以查看all.mysql(备份文件)的内容
                                    
                            (4) 启动复制
                                     mysql> start slave;
                                     
                        3.配置节点1(活动主)的主从复制
                            (1) 对节点1(活动主)进行数据恢复 (因当时节点1是所有数据的源，所以这步可以省略)
                                   > mysql -uroot -p < all.mysql     (将备份数据库文件进行导入)
                                   
                            (2) 查看节点2(备用主数据库) 当前的日志点信息
                                    mysql> show master status \G;  (这个命令是在节点2(备用主数据库)进行的)
                                    
                                    结果
                                        File: mysql-bin.000002
                                        Position:1412692
                                   
                            (3) 配置复制链路
                                    mysql> change master to master_host='192.168.1.101',master_user='jame',
                                           master_password='123456'，master_log_file='mysql-bin.000002',
                                           master_log_pos=1412692;
                                           
                                    注意这个时候 master_log_file和master_log_pos的值是备用主数据库当前的日志点信息
                                    
                            (4) 启动复制
                                     mysql> start slave;
                            (5) 查看复制情况
                                    mysql> show slave status \G;
                                    
                        以上配置完成后则节点1和节点2互为主从关系，节点1的主数据库是节点2，节点2的主数据是节点1
                        
                        4.对节点3(从数据库服务器)操作
                            (1) 对节点3(从数据库服务器)进行数据恢复
                                    > mysql -uroot -p < all.mysql     (将备份数据库文件进行导入)
                                    
                            (2) 配置节点3服务器(在/etc/my.cnf)
                                    I. log-bin = mysql-bin(在从数据库配置binlog方便以后进行主从迁移，故障转移，进行连路复制)
                                    II. server_id = 103
                                    III. relay_log = mysql_relay_bin 
                                         (一定要进行固定，默认是按主机名字进行命名 /home/mysql/log/mysql-relay-bin)
                                    IV. log_slave_update = on [可选] (如果以后要将该从服务器当做主服务器使用则一定要设置为on)
                                    V. read_only = on 
                                    
                            (3) 配置复制链路
                                    mysql> change master to master_host='192.168.1.100',master_user='jame',
                                           master_password='123456'，master_log_file='mysql-bin.000003',
                                           master_log_pos=1893;
                                                            
                                    其中master_log_file='mysql-bin.000003',master_log_pos=1893;
                                    可以查看all.mysql(备份文件)的内容
                                    
                            (4) 启动复制
                                     mysql> start slave;
                                     
                        5.对每个服务器安装MMM架构需要的软件
                            (1) > wget http://mirrors.opencas.cn/epel/epel-release-latest-6.noarch.rpm
                            (2) > wget http://rpms.famillecollet.com/enterprise/remi-release-6.rpm
                            (3) > rpm -ivh epel-release-latest-6.noarch.rpm
                            (4) > rpm -ivh remi-release-6.rpm
                            (5) > vim /etc/yum.repos.d/remi.repo
                                    将[remi]
                                        enabled = 0 
                                    
                                    修改为
                                    [remi]
                                        enabled = 1
                                         
                            (6) > vim /etc/yum.repos.d/epel.repo
                                            将[epel]
                                               # baseurl=http://......
                                                 mirrorlist=http://
                                            
                                            去注释和注释
                                            [remi]
                                                baseurl=http://......   
                                                #mirrorlist=http://
                            (7) 安装监控服务
                                    A.查看MMM有那些安装包
                                        > rpm search mmm
                                        结果：
                                            mysql-mmm-agent.noarch
                                            mysql-mmm-monitor.noarch
                                            mysql-mmm-tools.noarch
                                            mysql-mmm-noarch
                                    B.yum -y install  mysql-mmm-agent.noarch (进行代理软件的安装)
                                    
                        6.在监控服务器上安装对应的软件
                            > yum -y install mysql-mmm*
                            
                        7.在节点1(活动主数据库)创建账号(会同步到备份主数据库和从数据库中)
                            (1) 创建MMM监控账号(检查数据库健康状况)
                                    mysql> grant replication client on *.* to 'mmm_monitor’ @ ‘192.168.3.%' 
                                            identified by '123456';
                                            
                            (2) 创建MMM代理服务账号   
                                    为了改变只读模式或则改变从服务器为主服务器，目的是故障转移或则主从切换，所以需要的权限
                                    比较大
                                    
                                    mysql> grant super,replication client,process on *.* to 'mmm_agent’ 
                                           @ ‘192.168.3.%' identified by '123456';
                                           
                            (3) 创建MMM复制账号（就是主从复制(同步)时的账号）
                            
                        8.在每一个数据库服务器中配置MMM的mmm_common.conf
                            > vim /etc/mysql-mmm/mmm_common.conf
                            需要修改
                                <host default>
                                    cluster_interface   eth0   (自己本地的网络接口)
                                    replication_user   jame   (MMM复制账号)
                                    replication_password 123456
                                    agent_user mmm_agent   (步骤7中创建MMM代理服务账号)
                                    agent_password 123456
                                </host>
                                
                                <host db1>          (活动主数据库)
                                    ip     192.168.3.100
                                    mode   master
                                    peer   db2
                                </host>
                                
                                <host db2>          (备份主数据库)
                                    ip     192.168.3.101
                                    mode   master
                                    peer   db1
                                </host>
                                                                    
                                <host db3>          (从数据库)
                                    ip     192.168.3.102
                                    mode   slave
                                </host>                            
 
                                <role writer>     (数据库具备写功能)
                                    hosts   db1，db2
                                    ips     192.168.3.90   (虚拟ip，刚开始该ip作用与db1，当db1出现宕机时，vip漂移到db2)
                                    mode   exclusive
                                </role>     
                                
                                <role reader>     (数据库具备读功能)
                                    hosts   db1，db2, db3
                                    ips     192.168.3.91,  192.168.3.92,  192.168.3.93 
                                    mode   balanced
                                </role>     
```