# 基础知识

```shell
    1. QPS:每秒钟的查询量
    2. DML（data manipulation language）：用来对数据库里的数据进行操作的语言，有SELECT、UPDATE、INSERT、DELETE
    3. DDL（data definition language）：
            用在定义或改变表（TABLE）的结构，数据类型，表之间的链接和约束等初始化工作上，他们大多在建立表时使用。
            主要的命令有CREATE、ALTER、DROP以及创建索引等。
    4. DCL（Data Control Language）：
            是数据库控制功能。是用来设置或更改数据库用户或角色权限的语句
            包括（grant,deny,revoke等）语句。在默认状态下，只有sysadmin,dbcreator,db_owner或db_securityadmin等人员
            才有权力执行DCL
    5. 事务：是一组具有原子性的SQL语句，要么都执行完这些SQL语句，要么都不执行。
            有4个特性 
                (1) 原子性：事务被视为不可分割的最小工作单位，要么commit成功，要么回滚到原来的
                (2) 一致性：操作的对象(数据)在数据库中是存在的，不能消失。比如银行的例子，一个转账事务有若干个SQL语句构成，
                            第一个SQL语句从A中扣掉1000元，第二个SQL语句从B中加上1000元。这两个是一起执行的，一致性是1000元在
                            数据库中不能消失。
                (3) 隔离性：一个事务对数据库中数据的修改，在未提交完成前对其他事务是不可见的。
                           数据库命令(mysql> show variables like '%iso%'; 或者 > SELECT @@global.tx_isolation)
                                tx_isolation ： READ-COMMITTED
                                1.未提交读(read uncommited)
                                    会读到脏数据，在一个事务1中(开始begin;)进行select读表A的数据，在另外一个事务2中(begin;)向
                                    表A中插入数据data_new(事务2还没有commit),如果这时事务1再进行select读表A，那么会读到data_new
                                    
                                2.已提交读(read commited)
                                    在一个事务1中(开始begin;)进行select读表A的数据，在另外一个事务2中(begin;)向
                                    表A中插入数据data_new(事务2还没有commit),这时事务1再进行select读表A，不会读到data_new，
                                    只有当事务2进行commit后，事务1进行select读表A才会读到data_new.这个时候会有问题,是在事务中
                                    不能重复读，因为在一个事务中读到的数据随时可以会发生变化(另一个事务对同一张表进行操作)
                                    
                                3.可重复读(repeatable read)(mysql默认隔离级别)
                                     解决已提交读问题，在一个事务中不管读多少次，数据都不会发生变化(即使有另一个事务对同一张表进行操作)
                                     
                                4.可串行化（Serializable）
                                    完全串行化的读，每次读都需要获得表级共享锁，读写相互都会阻塞(一般不使用)
                                    
                                   
                (4) 持久性：一旦事务被提交，所做的修改会永久的保存到数据库中。
                
    6.MySQL体系结构
        
        
        客户端(C api)  --->  MySQL服务层(连接管理器，查询解析，查询缓存，查询优化器)  ---> 存储引擎层
        
        mysql定义了一系列的存储引擎接口(innodb,myisam),插件式存储引擎相当灵活。
        
    7.MySQL是单进程下使用的。
    
                                    
```

# 影响数据库的性能

```shell
    1. 大量的并发和超高的CPU使用率
            大量的并发会使得数据库的连接数被占满(max_connections),超高的CPU使用率可能会宕机.
            
    2.网络IO
        避免无法连接到数据库
            (1) 减少从服务器的数量
            (2) 进行分级缓存
            (3) 避免使用"select *" 查询
            
    3.大表
        大表：(1)单表的记录超过千万行.(2)表数据文件超过10GB
        
        问题：
           1.大表会导致查询效率低，如果某个表中包含千万条记录且某一列中只有4中情况条件,如果要查询筛选出1种情况,那么会导致区分度低，
             有大量的数据被读出，大量的磁盘IO从而导致减低了磁盘的效率.
           2.对DDL操作的影响
                (1) 大表使得建立索引需要很长的时间，mysql版本<5.5(建立索引会长时间的锁表)。mysql版本>=5.5(建立索引虽然不会长时间锁表
                    但会造成主从数据同步长时间延迟)
                (2) 修改表结构需要长时间的锁表。这样主数据库修改完后从数据库也要修改而且时同步的，只有当主数据库修改完表结构相应的
                    从数据库才能进行修改，会长时间的造成主从延时。这时候如果有对该表进行数据操作时，那么得等主从数据库表都修改完
                    表结构才能进行，这时操作会阻塞导致数据库的连接数剧增。
        
        解决方法：
            1.可以对大表进行历史数据归档
                    要考虑到归档时间点的选择，如何进行归档操作(可以会导致主从延时和其他业务数据查询的阻塞)
               
    4.大事务(运行时间长,操作的数据量大)
        
        问题：
            (1) 锁定大量的数据,造成大量的阻塞和锁超时
            (2) 回滚所需要的时间较长.
            (3) 会造成主从延时,只有当事务在主数据库中操作完成,将操作记录到binlog中，从数据库服务器根据binlog进行相同的事务操作.
            
        解决方法:
            (1) 避免一次处理太多的数据,可以进行分批次进行处理，一个事务可以只处理定量的数据，然后分成多个事务.
            (2) 在事务中移除不必要的select操作
            
    5.数据库参数的配置
    
    6.数据库表结构设计和SQL语句(这个最重要)
        (1) 过分的反范式化为表建立太多的列
        (2) 过分的范式化造成太多的表的关联
        (3) 在OLTP环境中使用不恰当的分区表，分区表是在同一个数据库中逻辑上使用同一张表，而物理存储在不同小表中。而
            分库分表则是在不同的数据库以及不同的表进行拆分
        (4) 过度的使用外键保证数据的完整性
    
    7.服务器硬件
        (1) mysql一定要运行在64位操作系统中,对于并发量比较高的场景CPU的数量要比频率重要。对于CPU密集型的场景和复杂的SQL其CPU的频率要
            比CPU的数量重要
        (2) 增加内存对数据库有好处,读数据可以通过缓存来读,而写数据更新到磁盘(可以先写到内存中，再一次性批量写入到磁盘中,减少写入的次数)
        磁盘：
           RAID(磁盘阵列)
            RAID0:只是简单的磁盘软件串联，没有提供冗余和错误修复能力，如果是有2个磁盘构成的，则读取的速度是单个的2倍，但数据丢失的成本是
                  单个的2个倍,因为任何一个磁盘损坏则数据库就不能用，RAID0适用于数据安全性不高的。
                  
            RAID1：磁盘镜像，就是把一个磁盘的数据镜像到另外一个磁盘中，2个磁盘的数据是相同的。对于读数据比较适合(比如读日志等)
            RAID5：分布式奇偶校验磁盘阵列，通过分布式奇偶校验块将数据分布到各个磁盘中，如果只有一块磁盘数据丢失则可以恢复，但有2块
                   磁盘数据丢失则无法恢复。对于读数据比较适合，适合从数据库服务器中。
            RAID10：分片的镜像，是RAID0和RAID1复合(比较适合读写频繁)
            
           固态硬盘(例如SSD)
                适用场景：
                    1.适用于大量 随机 I/O的场景
                    2.解决单线程负载的I/O瓶颈，例如只有一块SSD固态磁盘，应该要配置在从数据库服务器上,因为主服务器一般是多线程写入数据，
                      并且读写频繁导致SSD固态硬盘易损坏(写数据到固态硬盘时要先擦除原先的数据)，而主服务器同步到从服务器时只有一个线程。
                      
    8.linux系统参数优化
        (1) net.core.somaxconn = 65535 
            net.core.netdev_max_backlog = 65535
            net.ipv4.tcp_max_syn_backlog = 65535
            
            加快tcp socket的回收
        (2) net.ipv4.tcp_fin_timeout = 10
            net.ipv4.tcp_tw_reuse = 1
            net.ipv4.tcp_tw_recycle = 1 ((服务器不建议开启 tcp_tw_recycle 快速回收，会导致大局域网用户访问失败))
            
        (3) net.core.rmem_default = 87380  (表示套接字接收缓冲区大小的缺省值)(单位字节)
            net.core.rmem_max = 16777216   (表示套接字接收缓冲区大小的最大值)(单位字节)
            net.core.wmem_default = 87380  (表示套接字发送缓冲区大小的缺省值)(单位字节)
            net.core.wmem_max = 16777216   (表示套接字发送缓冲区大小的最大值)(单位字节)
            
        (4) net.ipv4.tcp_keepalive_time = 120
            net.ipv4.tcp_keepalive_intvl = 30
            net.ipv4.tcp_keepalive_probes = 3
            
        (5) kernel.shmmax = 4294967295  (byte)
            vm.swappiness = 0  (0表示最大限度使用物理内存,然后才是 swap空间)
            
        (6  /etc/security/limit.conf
            * soft nofile 65535
            * hard nofile 65535
            
        (7) 磁盘调度策略(/sys/block/磁盘存储设备名/queue/scheduler) 例如(/sys/block/sda/queue/scheduler)
            
            > cat /sys/block/sda/queue/scheduler
            结果：
                noop deadline [cfq]
            
            noop(电梯式调度策略):实现了一个FIFO队列，当有一个新的请求到来时，它将请求合并到最近的请求之后，以此保证请求同一介质，
                               NOOP倾向饿死读利于写，因此NOOP对于闪存，RAM及嵌入式系统是最好的选择。
                               
            deadline(截止时间调度策略):Daadline确保在一个截止时间内进行服务请求，这个截止时间是可调整的，而默认读期限是短于写期限的。
                                    Deadline对数据库类应用是最好的选择。
                                    
            改变磁盘调度策略：
                echo deadline > /sys/block/sda/queue/scheduler
               
        
        (8) 文件系统对性能的影响 EXT3/4系统的挂载(/etc/fstab)       
               /dev/sda1/ext4 noatime,nodiratime,data=writeback 1 1
               
               在 Linux 下面挂载文件系统的时候设置 noatime 可以显著提高文件系统的性能。默认情况下，
               Linux ext2/ext3 文件系统在文件被访问、创建、修改等的时候记录下了文件的一些时间戳，比如：文件创建时间、
               最近一次修改时间和最近一次访问时间。因为系统运行的时候要访问大量文件，如果能减少一些动作（比如减少时间戳的记录次数等）
               将会显著提高磁盘 IO 的效率、提升文件系统的性能。Linux 提供了 noatime 这个参数来禁止记录最近一次访问时间戳
               
            
        
        

```

# 性能优化的顺序

```shell
    1.数据库结构设计和SQL语句
    2.数据库存储引擎的选择和参数配置
    3.系统选择和优化
    4.硬件升级
```

# 数据库实战经验

```shell
    1. 最好不要在数据库主库(master)上进行数据库备份远程同步计划任务，大型活动前取消这类计划任务.因为这些同步计划任务会导致服务器的性能
        下降,使得其他正常的服务有堵塞现象.
        
    2.大多数数据库性能的问题可以通过对sql查询的优化来解决(慢查询问题)
```

