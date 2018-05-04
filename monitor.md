# MySQL 监控

## 概要
```shell
    1.监控的对象
        (1) 对数据库服务可用性进行监控
                数据库进程或则端口存在 并不意味数据库就是可用的，通过网络连接到数据库并且在数据库上执行一些简单脚本(SQL语句)
                确定数据库是可以对外提供服务的
        (2) 对数据库性能的监控
                A. QPS和TPS 的监控
                B. 并发线程数量的监控(并不等于连接数，而是指数据库同时处理的SQL请求的数量)，要远远小于数据库的连接数
        (3) 对innodb阻塞和死锁进行监控
        (4) 对主从复制进行监控
                A. 主从复制链路状态的监控
                B. 主从复制延迟的监控
                C. 定期的确认主从复制的数据是否一致
                
        (5) 对服务器资源的监控
                A. 磁盘空间
                     服务器磁盘空间大并不意味着MySQL数据库服务能够使用的空间足够大
                B. 其他
                     CPU的使用情况，内存的使用情况，Swap分区的使用情况，以及网络的IO的情况
```

## 数据库可用性监控

```shell
    1.如何确认数据库是否可以通过网络连接
        通过MySQL本地的SQL文件来连接数据库服务器，并不意味着可以通过网络连接(有防火墙等设置)，MySQL所在的tcp连接可能被占满
            的情况，只能通过远程的服务器连接请求数据库
            
        (1) > mysqladmin -umonitor_user -p密码 -h服务器地址 ping
        
        (2) > telnet ip db_port 
        
        (3) 使用程序通过网络建立数据库连接
        
    2.如何确认数据库是否可读写
        (1) 检查数据库的read_only参数是否为off
                定期的对主从服务器的主数据库的参数进行检查(可能是在进行主从切换后，新的主数据库保持read_only)
        (2) 建立监控表并对表中数据进行更新
                可以对监控表进行简单的数据操作
        (3) 执行简单的查询 mysql> select @@version;
        
    3.如何监控数据库的连接数
        造成连接数突然增长：(1) 出现阻塞像Memcahe这样的缓存失效
        
        A. mysql> show variables like 'max_connections';  获取系统中最大的连接数的值
        B. mysql> show global status like 'Threads_connected'; 记录了当前系统的连接数量
            结果:
               +-------------------+-------+
               | Variable_name     | Value |
               +-------------------+-------+
               | Threads_connected | 1     |
               +-------------------+-------+
               
        C. 在监控工具中 定义 Threads_connected/ max_connections > 0.8 进行报警

```

## 数据库性能监控

```shell
    1.记录性能监控过程中所采集到的数据库的状态
    2.如何计算QPS和TPS
        QPS:每秒钟数据库所执行的查询数量，这个查询并不指select语句的数量，包括数据库所有的请求
        TPS：每秒钟所处理的事务的数量
    3.如何监控数据库的并发请求的数量
        (1) 数据库系统的性能会随着并发处理请求数量的增加而下降，需要同CPU的使用率来分析。
        (2) 并发处理请求数量通常 远小于 同一时间连接到数据库的线程的数量
        (3) 并发处理请求数量通常比较稳定，如果在某个时间出现突然增大，则考虑该数据库是否有异常，例如数据库出现大量的阻塞。
        
        A. mysql> show global status like 'Threads_running';
        结果:
               +-----------------+-------+
               | Variable_name   | Value |
               +-----------------+-------+
               | Threads_running | 1     |
               +-----------------+-------+
               
    4.如何 对innodb阻塞和死锁进行监控
      
       记录的锁的情况以及事务的情况，筛选出阻塞时间大于60秒
        select b.trx_mysql_thread_id as '被阻塞线程',
        	   b.trx_query as '被阻塞SQL',
        	   c.trx_mysql_thread_id as '阻塞线程',
        	   c.trx_query as '阻塞SQL',
        	   (UNIX_TIMESTAMP()-UNIX_TIMESTAMP(c.trx_started)) as '阻塞时间'
        	   from information_schema.innodb_lock_waits a
        	   join information_schema.innodb_trx b ON a.requesting_trx_id=b.trx_id
        	   join information_schema.innodb_trx c ON a.blocking_trx_id=c.trx_id
        	     
        	   where (UNIX_TIMESTAMP()-UNIX_TIMESTAMP(c.trx_started)) > 60
        	          修改部分
        	          
    5.演示实例
        A.2个mysql连接
        B.连接1 
            (1) 显示连接1的连接id
                mysql> select connection_id();
                结果:
                    +-----------------+
                    | connection_id() |
                    +-----------------+
                    |               2 |
                    +-----------------+
                    
            (2) 修改innodb锁的超时时间
                    mysql> set global innodb_lock_wait_timeout=180;
            (3) 
                A.切换到test数据库
                    mysql> use test;
                B.开始事务    
                    mysql> begin；
                C.进行select操作(对t表加一个排它锁)
                    mysql> select * from t for update; 
                D.
                    
                    
        C.连接2
            (1) 显示连接2的连接id
                mysql> select connection_id();
                结果:
                    +-----------------+
                    | connection_id() |
                    +-----------------+
                    |               3 |
                    +-----------------+
            (2) 
                A.切换到test数据库
                    mysql> use test;
                B.开始事务    
                    mysql> begin；
                C.进行select操作(对t表加一个排它锁)
                    mysql> select * from t for update; 
                    这时候进行阻塞
        D.这时候运行检测阻塞SQL
            注意:
                可以抓到线程被阻塞的SQL，但是无法抓到导致阻塞线程的阻塞SQL，因为产生阻塞SQL已经执行完了
                
            mysql> select b.trx_mysql_thread_id as '被阻塞线程',
                ->            b.trx_query as '被阻塞SQL',
                ->            c.trx_mysql_thread_id as '阻塞线程',
                ->            c.trx_query as '阻塞SQL',
                ->            (UNIX_TIMESTAMP()-UNIX_TIMESTAMP(c.trx_started)) as '阻塞时间'
                ->            from information_schema.innodb_lock_waits a
                ->            join information_schema.innodb_trx b ON a.requesting_trx_id=b.trx_id
                ->            join information_schema.innodb_trx c ON a.blocking_trx_id=c.trx_id;
            +-----------------+----------------------------+--------------+-----------+--------------+
            | 被阻塞线程      | 被阻塞SQL                  | 阻塞线程     | 阻塞SQL   | 阻塞时间     |
            +-----------------+----------------------------+--------------+-----------+--------------+
            |               3 | select * from t for update |            2 | NULL      |          157 |
            +-----------------+----------------------------+--------------+-----------+--------------+
```

