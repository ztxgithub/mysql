# SQL 查询优化

## 简要

```shell
   查询优化，索引优化，库表结构优化需要齐头并进 
```

## 获取有性能问题SQL

```shell
   1.通过用户反馈获取存在性能问题的SQL
        当应用上线后，或则在测试过程中，某些应用或则页面打开比较慢
        
   2.通过慢日志获取存在性能问题的SQL
        慢日志是由MySQL服务器层记录的，和其数据库的存储引擎是无关的，无论是MyISAM存储引擎还是innodb存储引擎，其SQL语句都会记录
        到慢日志中，通过分析慢日志可以知道存在性能问题的SQL，可能会存在延迟
        
        (1) MySQL的慢查询日志是一种性能开销比较低的 获取性能问题SQL的解决方案，其主要的开销来之于磁盘IO和存储日志所需要的磁盘空间。
            对于磁盘IO来说，写日志是顺序存储的，其开销可以忽略不计。主要的开销是存储日志所需要的磁盘空间，为了能够对慢查询日志进行控制
                    A.slow_query_log 启动停止记录慢查询日志(默认情况下是启动慢查询日志) 开启为on
                    B.通过脚本定时的开关
                    C.slow_query_log_file 指定慢查询日志的存储路径及文件，在默认情况下保存在MySQL的数据目录中
                                          建议指定路径，将日志存储和数据存储 分开进行存储
                    D.long_query_time 指定记录慢查询日志SQL执行时间的阈值(单位秒，默认为10秒)，如果想要记录执行时间超过100s的SQL，
                                      则将 long_query_time 置为 100，其会记录所有符合条件的SQL，包括查询语句，数据修改语句，
                                      已经回滚的SQL。 改为 0.001秒较为合适
                    E.log_queries_not_using_indexes 是否记录未使用索引的SQL，不管该SQL有没有超过long_query_time，只要该SQL 
                                                    没有使用索引就会被记录到慢查询日志中，可以帮助我们找到当前查询记录比较小，
                                                    而性能良好的，但是确实是未使用索引的SQL，提前对这些SQL进行优化
                                                    
        (2) 演示
                A. mysql> set global slow_query_log=on;
                B.在慢查询日志的内容
                    # User@Host: sbtest[sbtest] @ localhost [] Id:17] (记录的用户信息和线程id)
                    # Query_time: 0.000233 (SQL执行所用的时间)
                    # Lock_time: 0.000120 (查询SQL使用锁的时间 )
                    # Rows_sent: 1 (返回数据行)
                    # Rows_examined:1 (扫描数据的行数，采用主键索引查找)
                    SET timestamp=1458612917 (执行该SQL的时间戳)
                    select c from sbtest1 where id=4969;
                    
        (3) 常用的慢查询的分析工具
                A.mysqldumpslow
                    汇总除查询条件外其它完全相同的SQL，并将分析结果按照参数中所指定的顺序输出
                    > mysqldumpslow -s r -t 10 slow-mysql.log
                    参数:
                        -s order(c,t,l,r,at,al,ar) 表示输出结果时排序方式
                                c: 总次数，表示是以总次数排序
                                t:总时间
                                l:锁的时间
                                r:总数据行
                                at,al,ar ：t,l,r平均数 
                                at:总时间/总次数
                                
                        -t ：top，指定取前几条作为结束输出
                        
                B.pt-query-digest (推荐使用)
                    > pt-query-digest --explain h=127.0.0.1,u=root,p=p@ssW0rd slow-mysql.log
                    > pt-query-digest --explain h=127.0.0.1 slow-mysql.log
                        参数:
                            --explain :是否进行查询计划
                            
            
        
   3.实时获取存在性能问题的SQL
        当前数据库服务器压力突增，通过MySQL的information_schema数据库PROCESSLIST表来实现实时发现性能问题的SQL。
        
        查询当前服务器执行时间超过60秒的SQL
        mysql> select id,user,host,DB,command,time,state,info from information_schema.processlist
               where time >= 60
        
        
```

## SQL的解析预处理及生成执行计划

```shell
   1.MySQL对SQL语句的执行步骤
        A.客户端发送SQL请求给服务器
        B.服务器检测是否可以在查询缓存中命中该SQL
            如果可以命中SQL则立即返回存储在查询缓存的结果
        C.如果未能在查询缓存中命中该SQL，则服务器端进行SQL解析，预处理，再由优化器生成对应的执行计划
        D.根据执行计划，调用存储引擎API来查询数据
            存储引擎就会返回我们所需要的数据给MySQL的服务器层
        E.MySQL服务器层将结果返回给客户端
        
   2.查询缓存对SQL性能的影响
        (1) 优先检查 这个查询 是否 命中在查询缓存中的数据 
            
```