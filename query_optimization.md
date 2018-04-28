# SQL 查询优化

## 简要

```shell
   查询优化，索引优化，库表结构优化需要齐头并进 
   1.SQL查询优化的主要目的从根本上来说减少查询所消耗的时间加快查询的响应速度。
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
                通过一个对大小写敏感的哈希查找实现的
        (2) 如果当前的查找命中缓存中的数据，则MySQL会检查用户权限，这仍然是无需解析SQL数据的，在查询缓存中已经存放了当前查询需要
            表的信息，如果用户权限没有问题，MySQL会跳过所有的其他阶段，直接从缓存中拿到结果并返回给后端，这种情况下查询是不会被
            解析，也不会生成查询计划，每次在检查缓存数据都要对缓存进行加锁，对于一个读写频繁的系统使用查询缓存很可能会降低查询
            处理的效率，所以在读写频繁的系统建议大家不要使用查询缓存。
            
        在读写频繁的系统建议大家不要使用查询缓存
            
        系统参数:
            A.query_cache_type 设置查询缓存是否可用
                                (on,off,demand) 
                                demand:表示只有在查询语句中使用SQL_CACHE和SQL_NO_CACHE来控制是否需要查询缓存
                                
            B.query_cache_size 设置查询缓存内存大小，必须是1024的整数倍 （query_cache_type设置为off下，query_cache_size置为0）
            C.query_cache_limit 设置查询缓存可用存储的最大值，超过这个值不会被缓存
            D.query_cache_wlock_invalidate :设置数据表被锁后是否返回缓存中的数据，默认是关闭(也是建议值)
            E.query_cache_min_res_unit :设置查询缓存分配的内存块最小单位
            
   3.MySQL依照这个执行计划和存储引擎进行交互
        当不进行查询缓存，需要将一个SQL转化成一个执行计划，MySQL依照这个执行计划和存储引擎进行交互
        包括多个子过程: 解析SQL，预处理，优化SQL生成执行计划，在这个过程中出现任何错误，比如语法错误，都会终止该过程。
         (1) 解析SQL(语法解析阶段)
                通过关键字对MySQL语句进行解析，并生成一棵对应的"解析树"，
                MySQL解析器将使用MySQL语法规则验证和解析查询，包括检查语法是否使用了正确的关键字，关键字的顺序是否正确等
                
         (2) 预处理
                根据MySQL规则进一步检查解析树是否合法，检测查询中所涉及的数据库表和数据列是否存在，名字和别名是否存在歧义。
                语法检测全部通过，查询优化器就可以生成查询计划。一条查询可以有很多种查询方式，查询优化器对每一个查询可以用到的索引
                统计信息进行比较，以找到成本最低的方式来生成执行计划。如果可以使用的索引太多，会增加查询优化器生成查询计划的成本。
                
                有很多中因素造成MySQL的查询优化器生成查询计划错误
                    (1) 统计信息不准确
                            MySQL依赖与存储引擎提供的统计信息来评估成本，但是存储引擎提供的统计信息如果有很大的偏差，
                            对应innodb存储引擎来说，由于架构原因并不能维护一个数据表的行数精确的统计信息，取而代之则是
                            一个由抽样数据所估算出来的值，所以这就可能对MySQL的查询执行计划造成一定的影响
                            
                    (2) 执行计划中的成本估算 不等同于 实际的执行计划的成本
                            即使统计信息是完全精确的，优化器给出的执行计划也可能并不是最优的，例如一个查询可以通过2种
                            执行计划来完成，其中一种比另外一种需要更少的页面，则查询优化器则会选择较少的那种，如果
                            读取页面少的执行计划需要进行的是一些随机读取，而另外执行计划则是顺序读取，这样看来则是使用
                            读取页面多的执行计划较为合适。MySQL服务器层并不知道那些页面在内存中，那些页面在磁盘中，
                            那些页面需要随机读取，那些页面需要顺序读取，这就造成执行计划不准确的因素
                            
                    (3) MySQL优化器所认为的最优可能与你所认为的最优不一样
                            我们可能希望SQL执行的时间尽可能的短，但是MySQL只基于其成本模型选择最优的执行计划。有些时候并不是
                            最快的执行计划。
                            
                    (4) MySQL从不考虑其他并发的查询，这可能会影响当前查询的速度。
                            并发查询可能会涉及到锁的一些因素，但是MySQL查询优化器并不会考虑这些
                            
                    (5) MySQL有时候也会基于一些固定的规则来生成执行计划。
                            MySQL有时候不会基于成本进行优化，基于一些固定的规则
                            
                    (6) MySQL不会考虑不受其控制的成本
                            例如执行存储过程，用户自定义的函数的成本，都不在MySQL查询优化器的考虑范围内
                            
         (3) 优化SQL生成执行计划    
                 
                MySQL查询优化器可以自动对查询所做的一些优化，经过查询优化器改写后的SQL，查询优化器会对其生成执行计划，
                接下来MySQL服务器根据执行计划调用存储引擎的api，通过存储引擎获取所需要的数据
                MySQL优化器可优化的SQL类型
                    (1) 重新定义表的关联顺序
                            数据库表的关联顺序并不是总是按照我们在查询中指定的顺序来进行的，优化器会根据统计信息来
                            决定表的关联顺序
                            
                    (2) 将外连接转化为内连接
                            where条件和库表结构都可能让一个外连接等价于内连接，例如 a left join b,但是在where条件中
                            有b表的过滤条件，使外连接等价于内连接情况，当前MySQL版本是能够识别这种查询，使其重写优化。
                    (3) 使用等价交换规则
                            对where从句中一些过滤规则进行等价的变化，以使得表达式更容易理解，
                            例如在我们的where条件中出现 (5=5 and a > 5) 将被改写为 a > 5
                            
                    (4) 优化count(),min()和max()
                            还可以利用索引和列是否为空 来对count(),min()和max()来进行优化，例如B-tree索引按照顺序来存储的，
                            所以要找到列的最小值只需要查询对应的B-tree索引最左端的数据就可以了，因此MySQL可以直接获取一行
                            记录的最小值，这类会出现 select tables optimized away这样的提示信息，它表示优化器已经从
                            执行计划中移除了该表，并以一个常数取而代之
                            
                    (5) 将一个表达式转化为常数表达式
                    (6) 子查询优化
                            把子查询转换为关联查询，这样可以减少表的查询次数
                            
                    (7) 提前终止查询
                            当发现了不成立的条件，这时候MySQL立即返回空的结果，直接终止查询，而没有与存储引擎进行交互
                    (8) 对in()条件进行优化
                            将in中数据先进行排序，然后通过二分查找方式来确定列表中值是否满足条件         
```

## 如何确定查询处理各个阶段所消耗的时间

```shell
    1.使用profile
        步骤:
            A. mysql> set profiling =1  启动profiling功能，这是session级的配置。只有在当前session下才能有作用。
            B 执行想要优化的查询 (例如: mysql> select count(*) from film;)
            C mysql>  show profiles; 查看每一查询所消耗的总时间的信息
                结果:
                    +----------+------------+-----------------------------------+
                    | Query_ID | Duration   | Query                             |
                    +----------+------------+-----------------------------------+
                    |        1 | 0.00048075 | show variables like '%profiling%' |
                    |        2 | 0.00047650 | select count(*) from film         |
                    +----------+------------+-----------------------------------+
            D mysql> show profile for query N(SQL的query id值)
                结果:
                    mysql> show profile for query 1;
                    +----------------------+----------+
                    | Status               | Duration |
                    +----------------------+----------+
                    | starting             | 0.000063 |
                    | checking permissions | 0.000010 |
                    | Opening tables       | 0.000043 |
                    | init                 | 0.000013 |
                    | System lock          | 0.000005 |
                    | optimizing           | 0.000005 |
                    | statistics           | 0.000013 |
                    | preparing            | 0.000012 |
                    | executing            | 0.000252 |
                    | Sending data         | 0.000019 |
                    | end                  | 0.000005 |
                    | query end            | 0.000003 |
                    | closing tables       | 0.000003 |
                    | removing tmp table   | 0.000006 |
                    | closing tables       | 0.000004 |
                    | freeing items        | 0.000013 |
                    | cleaning up          | 0.000013 |
                    +----------------------+----------+
                    
            E  mysql> show profile cpu for query N(SQL的query id值)
                结果:
                    mysql> show profile cpu for query 1;
                    +----------------------+----------+----------+------------+
                    | Status               | Duration | CPU_user | CPU_system |
                    +----------------------+----------+----------+------------+
                    | starting             | 0.000063 | 0.000003 |   0.000045 |
                    | checking permissions | 0.000010 | 0.000001 |   0.000009 |
                    | Opening tables       | 0.000043 | 0.000002 |   0.000041 |
                    | init                 | 0.000013 | 0.000001 |   0.000011 |
                    | System lock          | 0.000005 | 0.000000 |   0.000005 |
                    | optimizing           | 0.000005 | 0.000000 |   0.000005 |
                    | statistics           | 0.000013 | 0.000001 |   0.000012 |
                    | preparing            | 0.000012 | 0.000001 |   0.000011 |
                    | executing            | 0.000252 | 0.000014 |   0.000238 |
                    | Sending data         | 0.000019 | 0.000001 |   0.000018 |
                    | end                  | 0.000005 | 0.000000 |   0.000004 |
                    | query end            | 0.000003 | 0.000000 |   0.000003 |
                    | closing tables       | 0.000003 | 0.000000 |   0.000002 |
                    | removing tmp table   | 0.000006 | 0.000001 |   0.000006 |
                    | closing tables       | 0.000004 | 0.000000 |   0.000004 |
                    | freeing items        | 0.000013 | 0.000001 |   0.000013 |
                    | cleaning up          | 0.000013 | 0.000000 |   0.000011 |
                    +----------------------+----------+----------+------------+
                    17 rows in set, 1 warning (0.00 sec)
                    
            F. 查看警告信息 (mysql> show warnings;)
                结果:
                    | Warning | 1287 | 'SHOW PROFILE' is deprecated and will be removed in a future release. 
                    Please use Performance Schema instead |
                    
    2.使用Performance Schema
        A.启动监控信息
            mysql> use performance_schema;
            mysql> update setup_instruments set enabled='YES',TIMED='YES' where name like 'stage%';
            mydql> update setup_consumers set enabled='YES' where name like 'events%'; (启动Performance Schema历史记录)
            
            其对数据库全局是有效的，可以分析数据库中每一个线程所执行的sql性能问题，
            
        B 执行SQL语句
            mysql> select count(*) from sakila.film;
            
        C.
            mysql> SELECT a.THREAD_ID,SQL_TEXT,c.EVENT_NAME,(c.TIMER_END - c.TIMER_START)/1000000000 AS'DURATION (ms)'
                   FROM events_statements_history_long a
                   JOIN threads b ON a.`THREAD_ID`=b.`THREAD_ID`
                   JOIN events_stages_history_long c ON c.`THREAD_ID`=b.`THREAD_ID`
                   AND c.`EVENT_ID` BETWEEN a.EVENT_ID AND a.END_EVENT_ID
                   WHERE b.`PROCESSLIST_ID`=CONNECTION_ID()
                   AND a.EVENT_NAME='statement/sql/select'
                   ORDER BY a.THREAD_ID,c.EVENT_ID
                                 
                                 
            实例：
                mysql> SELECT a.THREAD_ID,SQL_TEXT,c.EVENT_NAME,(c.TIMER_END - c.TIMER_START)/1000000000 AS'DURATION (ms)'
                                   FROM events_statements_history_long a
                                   JOIN threads b ON a.`THREAD_ID`=b.`THREAD_ID`
                                   JOIN events_stages_history_long c ON c.`THREAD_ID`=b.`THREAD_ID`
                                   AND c.`EVENT_ID` BETWEEN a.EVENT_ID AND a.END_EVENT_ID
                                   ORDER BY a.THREAD_ID,c.EVENT_ID；
                   只要将PROCESSLIST_ID修改为 对应的 CONNECTION_ID就可以了
```

## 特定SQL的查询优化

```shell
    1.大表的数据修改最好要分批处理(不然会带来长时间的阻塞)
        如果我们要在1000万行记录的表中删除/更新100万行记录，那么我们最好分成多个批次删除，一次只删除/更新5000行记录，
        为了减少主从复制同步带来的压力，我们在每次修改数据后先暂停几秒，已给主从复制集群提供一个同步数据的时间
        
        
        大表的更新和删除
            delimiter $$
            use `imocc`$$
            drop procedure if exists `p_delete_rows`$$
            create definer=`root`@`127.0.0.1` procedure `p_delete_rows`()
            begin
                declare v_rows int;
                set v_rows = 1
                while v_rows > 0
                do
                    只要修改下面一行就可以了
                    delete from sbtet1 where id >= 90000 and id <= 190000 limit 5000;
                    
                    select ROW_COUNT() INTO v_rows;
                    select sleep(5);
                end while;
                
            end$$
            delimiter ;
            
    2.如何修改大表的表结构
        A.直接修改大表的表结构带来的影响
            (1) 对表中的列的字段类型进行修改和改变字段的宽度 会造成锁表。
            (2) 无法解决主从数据库延迟的问题
            
        B 解决方法:
            方法一: 先在从服务器上修改表结构，然后再进行主从切换，最后在老的主服务器上修改表结构。
                       使用这种方法需要进行主从切换，所以还是存在一定风险
                       
            方法二: 在主服务器上建立一个新表，新表结构就是要更改后的表的结构，再把老表的数据导入到新表中，并且在老表中建立一系列的
                   触发器，把老表中数据修改同步更新到新表中，当老表和新表数据同步后，老表加一个排它锁，然后重新命名新表的名字，
                   删除老表。
                   
                   好处：即使尽量减少主从延迟，以及可以在重命名之前不用加任何的锁，只是在重命名的时候加一个短暂的
                        锁，这对应用不会造成太大的影响
                   缺点: 操作比较复杂
                   
                   工具:
                        pt-online-schema-change 
                        --alter="MODIFY c(列名) VARCHAR(150) NOT NULL DEFAULT ‘’"
                        --user=root --password=PassWord D=imooc,t=sbtest4
                        --charset=utf8 --execute
                        参数:
                            D:数据库  t:表名
                            
    3.如何优化not in 和 < >查询(不等于的查询)
        MySQL查询优化器能够自动把一些子查询中优化为关联查询，对于存在not in 和 < > 的查询就无法进行自动优化，这就造成了循环多次
        来查找子表来确认是否满足过滤条件，如果子查询恰好是很大的表的，这样做的效率会非常低
        
        例子:
            mysql> select customer_id,first_name,last_name,email
                   from customer where customer_id NOT IN (select customer_id from payment)
                    
            需要对customer表中每一个customer_id到payment表中去查询看是否有过交费记录，会多次对payment表进行查询
            
            优化后的SQL
            mysql> select a.customer_id,a.first_name,a.last_name,a.email
                   from customer a
                   left join payment b ON a.customer_id = b.customer_id
                   where b.customer_id IS NULL
                   
                   这样对left join 进行关联，这样可以避免对表进行多次查询
                   
    4.使用汇总表优化查询
        在网站上常见对商品进行统计，如果我们在用户访问商品页面时实时去统计商品的评论数，
            通常来说查询的SQL如下:
                mysql> select count(*) from product_comment where product_id = 999
                该SQL的作用是统计出product_id为999的所有评论，如果我们评论表中有上亿记录，那么这个SQL执行起来显然非常慢，
                特别是有大量并发访问情况下则会对数据库造成很大的压力
                
            优化：使用汇总表优化，提前 以要统计的数据进行汇总并记录到表中以备后续的查询使用
                (1) 先建立一张统计表 
                        mysql> create table product_comment_cnt(product_id INT, cnt INT)
                        
                     可以每天凌晨进行维护统计出截止到前一天 每一个商品评论的汇总
                        
                (2) 
                    mysql> select sum(cnt) from (
                           select cnt from product_comment_cnt where product_id = 999
                           union all
                           select count(*) from product_comment where product_id = 999
                           and timestr>DATE(NOW())
                           )
                       
        
            

```