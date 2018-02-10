# 数据库基础

## 什么是数据库

```shell
    1.数据库软件应称为DBMS(数据库管理系统)
    2.数据库是通过 DBMS 创建和操纵的容器.数据库可以是保存在硬设备上的文件,但也可以不是.在很大程度上说,
      数据库究竟是文件还是别的什么东西并不重要,因为你并不直接访问数据库；你使用的是DBMS,它替你访问数据库.
      
    3.RDBMS即关系数据库管理系统(Relational Database Management System)的特点：
         (1)数据以表格的形式出现
         (2)每行为各种记录
         (3)每列为记录所对应的数据域
         (4)许多的行和列组成一张表单
         (5)若干的表单组成database
```

- 主键 

```shell
    主键(primary)在一张表中只能有一个,而且是唯一的不能重复,而且不能为NULL
    unique键可空,可以在一个表里的一个或多个字段定义.
    唯一(unique)键和主键一样都是约束的范畴,而且都可以作为外键的参考,不同的是，一张表只能有一个主键
```

- 外键

```shell

  父数据表（Parent Entity）的主键（primary key）会放在另一个数据表,当做属性以创建彼此的关,，而这个属性就是外键
  比如学生跟老师之间是教学的关系,学生表中会有个属性叫指导老师（FK）,而这个值就是对应到老师表的老师代号（PK）在老师表中作为主键或则唯一键,
  学生的指导老师就是外键.
```

- 代理键

```shell

    在关系型数据库设计中,代理键是在当数据表中的候选键都不适合当主键时,例如数据太长,或是意义层面太多,
    就会请一个无意义的但唯一的字段来代为作主键.
    代理键值是数据库生成的,从来不显示给用户或应用程序看.
    在实践中,代理键值通常是个自动递增的数字,在MySQL里用一个标记有AUTO_INCREMENT的字段.
```

- 什么是锁

```shell
    作用:
        (1) 锁的主要作用是管理共享资源的并发访问
        (2) 锁用于实现事务的的隔离性
    锁的类型：
        (1) 共享锁(读锁) :多个线程可以在同一时间对同一个资源进行读操作,不会阻塞.
        (2) 独占锁(写锁) :一个写锁独占临街资源,其他的线程要阻塞等待
        
    锁的粒度：一个锁的粒度有行级锁和表级锁等等
            实际上是锁的策略,即加锁的最小单位，例如行级锁(加锁的最小单位是一行)
            表级锁的开销比较小，但并发量也很小。行级锁的并发量大，但开销也很大(需要频繁操作行级锁)
            表级锁一般是在MySQL服务层实现，而行级锁是在存储引擎层实现

    
```

[参考资料](https://github.com/jaywcjlove/mysql-tutorial/blob/master/README.md)

# 数据库技术构成

```shell
    数据库系统包括数据库,数据库管理系统,数据库应用程序3个部分构成
    数据库: 用于存储数据的地方
    数据库管理系统: 用户管理数据库的软件
    数据库应用程序（Database Application）：为了提高数据库系统的处理能力所使用的管理数据库的软件补充
```

## SQL语言

```shell
    SQL,(Structured Query Language)即结构化查询语言,数据库管理系统专门通过SQL语言来管理数据库中的数据,与数据库通信.
    SQL不是某个特定数据库供应商专有的语言.几乎所有重要的 DBMS都支持SQL
```

# 数据库的存储引擎

    在 MySQL 中,不需要在整个服务器中使用同一种存储引擎，针对具体的要求，可以对每一个表使用不同的存储引擎.
    存储引擎是针对表的而不是针对于库的(一个库中不同表可以使用不用的存储引擎)，但不推荐使用在一个库中使用不同的存储引擎。
    
## 存储引擎介绍

```shell
     存储引擎就是如何存储数据、如何为存储的数据建立索引和如何更新、查询数据等技术的实现方法．
     例如，如果你在研究大量的临时数据,你也许需要使用内存存储引擎。内存存储引擎能够在内存中存储所有的表格数据。
     又或者，你也许需要一个支持事务处理的数据库(以确保事务处理不成功时数据的回退能力).
     
    存储引擎的分类：
        1.InnoDB
            InnoDB是一个健壮的事务型存储引擎,为用户操作非常大的数据存储提供了一个强大的解决方案.
            InnoDB还引入了行级锁定和外键约束，在以下场合下，使用InnoDB是最理想的选择：
            
                (1)更新密集的表,InnoDB存储引擎特别适合处理多重并发的更新请求。
                (2)事务,InnoDB存储引擎是支持事务的标准MySQL存储引擎。
                (3)自动灾难恢复,与其它存储引擎不同，InnoDB表能够自动从灾难中恢复。
                (4)外键约束,MySQL支持外键的存储引擎只有InnoDB。
                (5)支持自动增加列AUTO_INCREMENT属性。
                (6)从5.7开始innodb存储引擎成为默认的存储引擎。
            一般来说，如果需要事务支持，并且有较高的并发读取频率，InnoDB是不错的选择,其索引和数据被缓存在内存中
            
            mysql> show variables like 'innodb_file_per_table';
            innodb_file_per_table:on 为每一个逻辑表建立独立的表空间 def_innodb_use.frm文件和def_innodb_use.ibd(表数据存储文件)
            innodb_file_per_table:off 所有逻辑表的数据保存在ibdataX文件中(其中X代表序号1,2,3....)
            
            设置innodb_file_per_table参数的值
                mysql> set global innodb_file_per_table=off;
            
            创建一张innodb存储引擎的表
                mysql> create table def_innodb_use(id int,des varchar(10)) engine='innodb';
                
            系统表空间和独立表空间的区别
                系统表空间缺点：
                 (1)如果mysql中的参数innodb_file_per_table设置为off,则默认所有的逻辑表的数据会存储在系统表空间中(ibdataX文件),
                     这时由于磁盘空间不足，删除一些逻辑的日志类的逻辑表或者老的数据表，这时系统表空间的大小并不会减小，需要很复杂的操作，
                     无法简单的收缩文件大小。
                 (2) 系统表空间会产生IO瓶颈： 因为所有的逻辑表都是往一个系统表空间文件中写数据。
                    
                使用独立表空间可以通过optimize table 命令收缩系统文件，不需要重启mysql服务
                使用独立表空间可以达到多个逻辑表同时操作对应的存储文件。
                
             所以最好对innodb使用独立表空间(innodb_file_per_table置为on)
             
             表转移步骤(将系统表空间转为独立表空间)
                (1) 使用mysqldump导出所有数据库表数据(要将存储事件触发器一起导出)
                (2) 停止MySQL服务，修改参数，并删除innodb相关的文件
                (3) 重启MySQL服务，重建Inoodb系统表空间
                (4) 重新导入数据
                
             tb_demo.frm，存储表定义这个文件是MySQL服务层产生的文件，它是与存储引擎无关的，
             
             特性：
                (1) Redo Log:用于事务的持久性（存的是已提交的事务），与MySQL系统变量innodb_log_buffer_size=16777216有关，
                             代表存储Redo Log大小，最多每隔1秒会将Redo Log文件更新到磁盘中，
                             所以innodb_log_buffer_size大小不用太大。
                (2) Undo Log:用于未提交事务提供回滚
                
                             在数据目录下有ib_logfileX(序号X：0,1,2.....)的个数跟系统变量innodb_log_files_in_group值有关，
                             如果innodb_log_files_in_group值为2，则在数据目录下有ib_logfile0和ib_logfile1
                             
                (3) Innodb 状态检测
                        进行一些数据的统计，死锁信息，外键的信息
            
        2.MyISAM(表级锁)
            
            MyISAM是MySQL5.5之前的版本默认的存储引擎。
            MyISAM表是独立于操作系统的,这说明可以轻松地将其从Windows服务器移植到Linux服务器；
            每当我们建立一个MyISAM引擎的表时，就会在本地磁盘上建立三个文件,文件名就是表名,3个文件的后缀名不一样
            例如，建立了一个MyISAM引擎的tb_Demo表，那么就会生成以下三个文件：
            
                tb_demo.frm，存储表定义。
                tb_demo.MYD，存储数据。
                tb_demo.MYI，存储索引。
             
             mysql > create table my_isam(id int, vaule varchar(10))engine=myisam;
                
            MyISAM表无法处理事务，这就意味着有事务处理需求的表，不能使用MyISAM存储引擎。MyISAM存储引擎特别适合在以下几种情况下使用：
            
                1.选择密集型的表.MyISAM存储引擎在筛选大量数据时非常迅速,这是它最突出的优点，MyISAM管理非事务表。
                  它提供高速存储和检索，以及全文搜索能力。如果应用中需要执行大量的SELECT查询，那么MyISAM是更好的选择。
                2.插入密集型的表.MyISAM的并发插入特性允许同时选择和插入数据.例如:MyISAM存储引擎很适合管理邮件或Web服务器日志数据.
                3.只读应用（MyISAM支持压缩功能）
                
                
            MyISAM用于系统表，临时表(在排序，分组等操作中，当数量超过一定大小之后，有查询优化器建立的临时表)
            
            特性：
                (1) 并发性和锁级别：表级锁，对于数据库进行写操作会对作用的表进行加锁。
                (2) 表损坏修复：MyISAM可以对任意意外关闭而损坏表进行修复。这里的修复不是进行事务的恢复，MyISAM不支持处理事务
                        mysql > check table table_name (进行表的检查)
                        mysql > repair table table_name (进行表的恢复)
                (3) 支持的索引类型
                        支持全文索引，但不是事务安全的，而且不支持外键
                (4) MyISAM表支持数据压缩
                        > myisampack -b -f my_isam.MYI(文件名)
                        
                        --backup -b  产生tbl_name.OLD备份数据文件
                        --force  -f  ：产生一个压缩的表，即使它比原始表大，
                                       或如果以前调用myisampack的中间文件存在(myisampack压缩表时在数据库目录中创建一个名为
                                       tbl_name.TMD的中间文件。如果杀掉myisampack，.TMD文件会被删除）。通常情况，
                                       如果myisampack发现tbl_name.TMD存在则退出并提示错误。用--force，myisampack则一定压缩表。
                        
                        这个时候my_isam.MYI文件是压缩后的文件,它只具有只读操作,不能对其进行插入和更新
                        
            限制:
                (1) 在MySQL版本小于5.0时,MyISAM存储引擎下的单表默认最大为4GB,如需要建立大表要修改MAX_Rows和AVG_ROW_LENGTH
                    MySQL版本大于5.0时，表默认支持256TB
                    
        3.CSV存储引擎
                CSV存储引擎可以将CSV文件作为表处理，使用该存储引擎存储的数据，我们可以直接查看csv文件。
                
                table_name.CSV :文件存储表内容
                table_name.CSM : 文件存储表的元数据(如表状态和数据量)
                table_name.frm : 文件存储表结构信息
                
                特点:
                    (1) 以CSV格式进行数据存储（建立逻辑表时对应的物理存储是csv文件），可以进行直接编辑
                    (2) 所有的列都不能为NULL
                    (3) 不支持索引(不适合大表)
                    
                适用场景
                    (1):适合做 为数据交换 的中间表 数据->CSV文件->MYSQL数据目录
                    
        4.Archive存储引擎
                Archive存储引擎会缓存所有的写，以zlib对插入的行进行压缩
                
                特点：
                    (1) 以zlib对表数据进行压缩，磁盘I/O更少(更加节约存储空间)
                    (2) 物理存储形式
                            table_name.ARZ 文件存储表内容
                            table_name.frm 文件存储表结构信息
                    (3) 只支持insert和select操作
                    (4) 只允许在 自增ID列 加索引
                    
                适用场景：
                    (1) :比较适用于 日志和数据采集应用，没有修改功能
                    
        5.Memory存储引擎
                也称为heap存储引擎，数据只存在内存中，无法持久化，MySQL服务重启后数据会丢失，但表结构会保持下来
                因为table_name.frm是MySQL服务层实现的，跟存储引擎无关。
                
                特点：
                    (1) 支持HASH索引和BTree索引，Memory存储引擎默认会建立HASH索引，HASH索引适用于等值查询，就通过查询筛选出一条记录
                         ，HASH索引不太适用于范围查询。
                         BTree索引：适用于范围查询。
                         
                    (2) 所有字段只能为固定长度，例如varchar(10)==char(10),在其他的存储引擎中适用varchar(10)代表最大为10bytes，
                        其实际的长度跟实际的值有关，而char(10)则不管实际值多大存储时都占用10bytes.
                        
                    (3) 不支持BLOG和TEXT等大字段。
                    (4) 使用表级锁
                    (5) 表的最大大小由系统变量max_heap_table_size决定的。修改max_heap_table_size大小对已经在内存中的memory表是
                        不生效的，需要对其进行重建。
                        
                物理存储方式：
                    table_name.frm 文件存储表结构信息
                    
                Memory存储引擎表和临时表的区别
                    临时表：(1)查询分析器和查询优化器使用的临时表，跟存储引擎没有关系
                           (2) create temporary table (SQL语句建立临时表)
                           
                适用场景：
                    (1) 进行一些等值查找，用于映射表，例如邮编和地区的对应表
                    
        6.Federated存储引擎
                特点：
                    (1) 提供访问远程MySQL服务器上的表的方法，在本地建立了到远程服务器的连接，所以要访问表的数据都在远程服务器上，
                        并不在Federated存储引擎中存储相关的表的数据，每次通过查询Federated存储引擎时，会发对应的命令到远程的服务器上。
                        
                    (2) 本地不存储数据，数据全部放在远程服务器上。
                    (3) 本地需要保持表结构和远程服务器的连接信息。
                    
                要进行Federated存储引擎步骤：
                    (1) 本地的MySQL服务器要支持Federated存储引擎(默认关闭的)
                    (2) 远程MySQL服务器要开启MySQL的远程帐号
                            mysql> grant all PRIVILEGES on wabg.* to  root@'192.168.188.106'  identified by '
                                   123456' WITH GRANT OPTION;
                            mysql> flush privileges;
                            
                    (3) 在远程服务器要有相关的数据库和表(存储引擎随便)
                    (4) 在本地的服务器中也要有对应的数据库和表(但其用的存储引擎的是federated)
                            mysql> create table local_fed(id int, c1 varchar(10)) engine=federated connnection=
                                   'mysql://用户名:密码@远程服务器ip:远程服务器端口号/远程服务器数据库名/远程服务器表名'；
                                   
                
                    
        如何选择存储引擎
            大部分情况下都应该选择innodb存储引擎，innodb支持热备方案，数据库不要混合使用多种存储引擎。
            
        
                            
                
```
