# 索引优化

## 概念

```shell
    1.索引的主要作用是告诉存储引擎 如何快速的找到我们所需要的数据，当表中的数据比较少时，查询的数据频率比较低的情况下，索引的作用不是
      太明显，这时表中的数据都可以缓存到内存中，就算进行全表扫描也不会太慢。随着表中数据越来越多，查询的频率越来越高，内存已经不能
      完全缓存表中的数据，这时索引的作用就很重要。
    2.MySQL的索引是在存储引擎层实现的，不同的存储引擎其索引的实现方式不同。
    3.聚集索引（主键索引）
        innodb存储引擎是索引组织表，即表中的数据按照主键顺序存放。而聚集索引就是按照每张表的主键构造一颗B+树，
        同时叶子节点中存放的即为整张表的记录数据，聚集索引的叶子节点称为数据页,聚集索引的这个特性决定了索引组织表中的数据
        也是索引的一部分
```

## MySQL支持的索引类型

```shell
    1.B-tree索引
        (1) 特点
                通常我们所说的索引是B-tree索引，它使用的是B+树中存储数据，在B+树中每一个节点都包含指向下一个节点的指针，这样可以方便
                节点之间的遍历
                A. B-tree索引以B+树中存储数据
                B. B-tree索引能够加快数据的查询速度,使用B-tree索引就不需要进行全表扫描来获取需要的数据，是从索引的根节点进行搜索，
                   逐一的向下进行查找，在innodb中叶子节点指向的主键，而在MyISAM中叶子节点指向的是物理地址
                C. B-tree索引更适合进行范围查找。 其是顺序存储
            
        (2) 适合场景
                A.全值匹配的查询 
                B.匹配最左前缀的查询
                    如果定义索引为联合索引(order_id,order_data)2列联合形成索引，需要查询order_id=1&&order_data=2018-1-1其用到
                    了索引，如果只查询order_id =2,则也会用到索引。
                    如果只查询order_data=2018-1-1则不会用到联合索引(order_id,order_data)。这就是最左前缀的查询
                C.匹配列前缀查询
                    可以匹配某一列的前缀部分，order_id like '9856%',查找所有9856开头的订单号
                    
                D.匹配范围值的查询(更适合的操作)
                    例如 order_id > '1000' and order_id < '9000' 
                E.精确匹配左前列并范围匹配另外一列
                    如果定义索引为联合索引(order_id,order_data)2列联合形成索引，则可以精确order_id=‘123’，并且order_data
                    处于某个时间段的范围
                    
        (3) Btree 索引的使用限制
                使用索引所命中数据如果占用表中大部分数据时，MySQL的查询优化器会认为使用全表扫描的方式性能会更好，就不会使用索引的方式
                进行查询。
                
                A.如果不是按照索引最左列开始查找，则无法使用索引
                    这是针对联合索引而言的，例如联合索引(order_id,order_data)，查询的顺序一定是先order_id，例如
                    order_id ='123' and order_data='2018-01-01' 这是有用到索引
                    order_data='2018-01-01' and order_id ='123'   这是没有用到索引
                    
                B.使用索引时不能跳过索引中的列
                    有联合索引 (order_date,order_name,order_phone_number),如果我们在查找中只包括order_date和
                    order_phone_number，那么对于这个查询来说，只能使用order_date这一列进行查询过滤，
                    而无法使用到order_phone_number这一列，因为我们的查询条件中跳过了 order_name 这一列
                    
                C.Not in 和 < > 操作无法使用索引
                
                D.如果查询中有某个列的范围查询，则其右边所有列都无法使用索引
                    这是针对联合索引而言的
                    
    2.Hash索引
    
        (1) 特点
            A. Hash索引是基于Hash表实现的，只有查询条件精确匹配Hash索引中的所有列时，才能够使用到Hash索引。
               Hash索引只能用于等值查询中，如果我们要进行范围查询或则模糊查询就不能使用Hash索引
            B. 对于Hash索引中的所有列，存储引擎都会为每一行计算一个Hash码，Hash索引中存储的就是Hash码
            
        (2) 使用限制
            A.hash索引必须进行二次查找
                使用Hash索引需要2次查找，hash索引中包括只是键值，hash码以及对应的指针，索引中并没有保存字段值，
                所以使用hash索引时必须先通过hash索引找到对应的行，再对行的记录进行读取
                
            B.Hash索引无法用于排序
            C.Hash索引不支持部分索引查找也不支持范围查找
            D.Hash索引中Hash码的计算可能存在Hash冲突
                Hash索引在计算hash码时是有可能冲突的，不同的索引列计算出的hash码是相同的，特别是对应hash的查找效率，
                hash码通常是比较短就算md5的hash值也是有可能存在冲突的，在hash索引中一旦产生大量的hash冲突就会影响hash索引的查找
                和维护的性能，所以说hash索引不适合用在选择性很差（键值列的重复值很多，比如性别）的列上，
                比如身份证列是唯一的，比较合适用于hash索引
                    
```

## 索引的基本原理

```shell

    数据记录本身被存于索引（一颗B+Tree）的叶子节点上，这种索引叫聚集索引，InnoDB使用聚集索引。
    反之则为非聚集索引，MyISAM使用非聚集索引，它的叶子节点的data域指向数据记录的地址，
    而不是存储数据记录本身


    1.MyISAM存储引擎 的 索引
        (1) MyISAM 的索引不论是 Primary Key 还是普通 Index，它的值都是指向 数据物理地址的指针。不管是主键索引还是普通索引，
            都是根据要查找的列值找到对应的节点，然后取出节点中的数据物理地址的指针，找到对应存储数据，再取出同一行其他列的值。
        (2) 在MyISAM中，主索引和辅助索引（Secondarykey）在结构上没有任何区别，只是主索引要求key是唯一的，
            而辅助索引的key可以重复
            
        (3) MyISAM的索引方式也叫做 非聚集 的
        
    2.InnoDB存储引擎 的索引
        (1) InnoDB主索引其节点的内容包含了完整的数据记录(即表中一行记录，该记录是以主键划分的)（这些数据是在内存中）
        (2) InnoDB要求表必须有主键（MyISAM可以没有）,如果声声明了主键(primary key)，则这个列会被做为聚集索引。
            如果没有声明主键，则会用一个唯一且不为空的索引列做为主键，成为此表的聚集索引。
            上面二个条件都不满足，InnoDB会自己产生一个虚拟的字段作为聚集索引，这个字段长度为6个字节，类型为长整形。
        (3) InnoDB普通索引其节点的值是对应与满足筛选条件该记录的主键列的值，所以辅助索引（普通索引）搜索需要 
            检索两遍索引：首先检索辅助索引获得主键，然后用主键到主索引中检索获得记录
        
```

## 索引的作用

```shell
    1.使用索引可以快速定位到我们需要的数据
    2.索引大大减少了存储引擎需要扫描的数据量
        索引的大小通常远小于表的大小，innodb存储引擎发生一次IO，最小的单位是以页为单位的，所以页内可以存储的信息越多那么他的读取效率
        越快，默认情况下innodb页的大小为16KB。由于索引的大小通常远小于表的大小，所以一页内可以存储更多的索引数据，因此通过索引来进行
        查找所需读取页的数量越少，这样就减少存储引擎进行IO的次数，加快数据查找的速度
    3.索引可以帮助我们进行排序以避免使用临时表
        由于Btree存储是按照键值的顺序来存储，我们可以利用Btree索引排序，这样避免使用临时表进行排序减少IO消耗，
        同时提高了MySQL的处理能力
        
    4.索引可以把随机I/O变为顺序I/O
        由于Btree存储是按照键值的顺序来存储，而数据行物理地址通常随机分布的，使用索引进行查找可以把随机I/O变为顺序I/O，这样
        可以充分发挥磁盘的IO性能
```

## 索引的损耗

```shell
    1.索引会增加写操作的成本
        由于我们在进行数据更新，插入，删除操作时必须要同时对相关的索引进行维护，所以索引越多修改数据所需要的时间也就越长，为了解决
        这个问题，innodb引入了插入缓存，把多次插入操作合并到一次
        
    2.太多的索引会增加查询优化器的选择时间
        MySQL的查询优化器根据索引的统一信息和查询的条件来为查询选择合适的索引，如果对同一个查询有很多个索引可以使用，则会增加MySQL
        查询优化器对于查询进行分析的时间，从而也会影响数据库的查询效率
```

## 索引的优化策略

```shell
    1.索引列上不能使用表达式或则函数
        例如 select 。。。 from product where to_days(out_date) - to_days(current_date) <= 30
        其中 to_days是函数，而out_data是索引列(代表过期日期)，这是不符合索引优化策略的
        
        经过索引优化
            mysql> select ... from product where out_data <= date_add(current_date,interval 30 day);
        其 out_data列上不再使用函数，可以用到索引优化查询
        
    2.前缀索引和索引列的选择性
        MySQL中的B-tree索引对键值的大小是有限制的，其根据存储引擎不同而不同，对应innodb来说，索引键的最大大小不能超过767个字节，
        而MyISAM中限制则是1000个字节，限制对应整型和浮点型以及日期来说已经是足够了，但是对应字符串来说可能有些不足，一方面能够
        对大字符串（txt）这列上建立索引，如果索引很长的字符串列其对索引使用会很慢，就达不到索引查询的目的，所以MySQL支持对字符串的
        前缀建立索引，这样可以大大节约索引的空间，从而提高索引的查询效率
        
        例如
            mysql> create index index_name(索引名字) on table(col_name(n));
            其中index_name为实际的索引名字， col_name为实际的列名， n: 为列的宽度(以字符来算，对应innodb最大是767字节)
            如果超过最大限制是无法建立前缀索引的
            
        索引的选择性：不重复的索引值和表的记录数的比值
        索引键值的唯一性越高，其选择性也越高，由此可见 唯一索引以及主键的选择性是最高的。索引的选择性越高其查找的效率就越快。
        前缀索引减低了索引的选择性。
        前缀索引只是取了字符串数据的一部分来作为索引的键值，所以其选择性必然会减低。
        选择前缀索引长度要注意既要做到 索引尽可能得小， 也要保证索引的选择性不能太差
        
    3.联合索引
        
        A. 如何选择索引列的顺序
                (1) 经常会被使用到的列优先
                    如果该列经常被使用到，则将该列优先放到联合索引的左边,
                (2) 选择性高的优先(优先考虑该原则)
                        如果其索引的选择性很差，则不适合将该列放在联合索引的最左边，因为查询处理的记录结果很多，
                        其查询优化器则会认为使用全表扫描比使用该索引的成本更低
                (3) 宽度小的列优先
                        这时在选择性高的优先的原则下进行的，宽度越小意味着一页中所能存储的索引越多，也就代表了使用索引过滤时候的
                        IO就越小，同时加快了索引查找的效率
                        
    4.覆盖索引
                  
         在进行索引优化时，关注的是sql中where语句过滤条件，B-tree索引除了帮助我们加快过滤数据找到我们所需要的内容外，还可以排序，
         分组等等，还可以使用B-tree索引直接使用我们所需要的数据，B-tree索引和hash索引是不同的，在B-tree的聚集索引的节点上包含了
         所有记录的内容
         
         优点：
            (1) 可以优化缓存，减少磁盘IO操作
                    只需要读取索引就可以完成我们业务查询，MySQL极大减少对磁盘数据访问量
            (2) 可以减少随机IO，将随机IO操作变为顺序IO操作
                     由于B-tree索引是按键顺序存储，对于IO密集型的查找来说，会比随机从磁盘读取每一行数据IO要少的多，而且
                     B-tree索引在访问时可以把磁盘随机访问IO转变为索引的顺序IO
            (3) 可以避免对innodb主键索引的二次查询
            (4) 可以避免MyISAM表进行系统调用
                    innodb存储引擎表可以通过覆盖索引获取查询的好处，对MyISAM表来说，MySQL只会缓存索引的信息，数据则要依赖与
                    操作系统缓存，因此如果我们要访问数据，需要进行系统调用，而系统调用的性能通常会比较差，所以很有可以会产生
                    性能问题，所以如果能够通过索引来获取全部所需要的数据就可以避免系统调用的产生
                        
         无法使用覆盖索引的情况
            (1) 存储引擎不支持覆盖索引
                    比如memory存储引擎就不支持，只有在索引的节点上包括键值的索引，才能建立覆盖索引，所以hash索引就不能够作为
                    覆盖索引使用
                    
            (2) 查询中使用太多列
                    覆盖索引之所以能够提高查询的性能，主要是因为索引的大小要比行的大小要小得多，如果索引本身很大，那么就没有使用
                    覆盖索引的必要了，特别是对于一些select查询，由于查询表中的所有列，所以不可能有索引覆盖到所有的列，因此对于
                    这种类型的查询也无法使用B-tree索引来优化。
            (3) 使用双%号的like查询
                    如果查询中存在双%的过滤条件，就无法使用覆盖索引，这时MySQL存储引擎的api所限制的，对这样的查询，MySQL只能
                    提供数据行的值然后在内存中再进行过滤
                    
         实例：
            (1) mysql> explain select language_id from film where language_id=1\G;
                结果：
                *************************** 1. row ***************************
                           id: 1
                  select_type: SIMPLE
                        table: film
                         type: ref
                possible_keys: idx_fk_language_id
                          key: idx_fk_language_id
                      key_len: 1
                          ref: const
                         rows: 1000
                        Extra: Using index
                        
            (2) mysql> explain select * from film where language_id=1\G;
                这个查询不能使用覆盖索引，而必须把数据放到内存中，再进行where条件的过滤，对与select * 的查询是无法使用覆盖索引覆盖的
                结果：
                    *************************** 1. row ***************************
                               id: 1
                      select_type: SIMPLE
                            table: film
                             type: ALL
                    possible_keys: idx_fk_language_id
                              key: NULL
                          key_len: NULL
                              ref: NULL
                             rows: 1000
                            Extra: Using where  (不能从索引中获取数据的，不能使用覆盖索引)
                    1 row in set (0.00 sec)
                    
            (3) mysql> explain select actor_id,last_name from actor where last_name='Joe'\G;
                使用到了覆盖索引，在innodb索引上，会自动将主键加入到聚合索引上
                *************************** 1. row ***************************
                           id: 1
                  select_type: SIMPLE
                        table: actor
                         type: ref
                possible_keys: idx_actor_last_name
                          key: idx_actor_last_name
                      key_len: 137
                          ref: const
                         rows: 1
                        Extra: Using where; Using index
                1 row in set (0.00 sec)
                       
```

## 使用索引来优化查询

```shell
    1.使用索引扫描来优化排序
        如果explain查询计划中的type为index，则说明MySQL使用索引扫描来进行排序，使用索引扫描排序时，只需要将一条索引记录移动到下一条
        索引记录，通常来说是很快的，如果想利用索引扫描方式优化查询并不太容易
        
        需要的条件
            (1) 索引的列顺序和Order By子句的顺序完全一致
                    UNIQUE KEY `rental_index` (`rental_date`,`inventory_id`,`customer_id`),
                     mysql> select * from rental where rental_date='2005-05-09' order by inventory_id,customer_id\G ;
            (2) 索引中所有列方向(升序，降序)和Order By 子句方向完全一致
            (3) Order By中的字段全部在关联表中第一张表中
            
        实例:
            
            A. 在innodb中数据的逻辑顺序和主键顺序是一致的，可以利用主键进行排序
                mysql> explain select * from rental where rental_date>'2005-01-01' order by rental_id\G ;
                *************************** 1. row ***************************
                           id: 1
                  select_type: SIMPLE
                        table: rental
                         type: index (索引进行排序)
                possible_keys: rental_date
                          key: PRIMARY
                      key_len: 4
                          ref: NULL
                         rows: 16005
                        Extra: Using where(使用主键进行排序)
                1 row in set (0.00 sec)
                
            B. 在MyISAM存储引擎中
               *************************** 1. row ***************************
                          id: 1
                 select_type: SIMPLE
                       table: rental_myisam
                        type: ALL
               possible_keys: rental_date
                         key: NULL
                     key_len: NULL
                         ref: NULL
                        rows: 16044
                       Extra: Using where;Using filesort(使用文件进行排序)
                       
            C. mysql> explain select * from rental where rental_date='2005-05-09' order by inventory_id,customer_id\G ;
              使用到二级索引进行排序 
               *************************** 1. row ***************************
                          id: 1
                 select_type: SIMPLE
                       table: rental
                        type: ref
               possible_keys: rental_date
                         key: rental_date
                     key_len: 5
                         ref: const
                        rows: 1
                       Extra: Using where
               1 row in set (0.00 sec)
               
            D.mysql> explain select * from rental where rental_date='2005-05-09' order by inventory_id desc,customer_id\G ; 
              索引中所有列方向(升序，降序)和Order By 子句方向完全一致
              *************************** 1. row ***************************
                         id: 1
                select_type: SIMPLE
                      table: rental
                       type: ref
              possible_keys: rental_date
                        key: rental_date
                    key_len: 5
                        ref: const
                       rows: 1
                      Extra: Using where; Using filesort(使用文件进行排序而不是索引排序)
                      
            E.mysql> explain select * from rental where rental_date>'2005-05-09' order by inventory_id,customer_id\G ; 
              如果查询中有某个列的范围查询，则其右边所有列都无法使用索引,这是针对联合索引而言的
              
              *************************** 1. row ***************************
                         id: 1
                select_type: SIMPLE
                      table: rental
                       type: ALL
              possible_keys: rental_date
                        key: NULL
                    key_len: NULL
                        ref: NULL
                       rows: 16005
                      Extra: Using where; Using filesort
                      
              因为rental_date是联合索引最左边的列(第一列)，一旦使用这列的范围查找，其联合索引右边的2列会失效，
              这样也是无法使用索引排序
              
              
    2.模拟Hash索引优化查询
    
            符合条件:
                (1) 只能处理键值的全值匹配查找
                (2) 所使用的Hash函数决定这索引键的大小
                        如果我们所使用的Hash函数对应值太大，就会造成索引比较大的情况
                        既不能生成太大的hash值，也不能造成太多的hash冲突
            如果我们想要在很长的字符串进行查找，只能使用前缀索引，这样使得索引的可选择性非常差，可以通过B-tree索引来模拟Hash索引
            
            实例：在title列是255字符的字符串，如果正常使用索引的化可以得使用前缀索引，导致可选择性差，可以新建一列title_md5
                 进行模拟Hash索引
            
            (1) mysql> alter table film add title_md5 varchar(32);
            (2) mysql> update film set title_md5=md5(title);
            (3) mysql> create index idx_md5 on film(title_md5);
            (4) mysql> explain select * from film where title_md5=md5('EGG IGBY') and title='EGG IGBY'\G ;
                where中2个条件是为了避免hash冲突
                *************************** 1. row ***************************
                           id: 1
                  select_type: SIMPLE
                        table: film
                         type: ref
                possible_keys: idx_title,idx_md5
                          key: idx_title
                      key_len: 767
                          ref: const
                         rows: 1
                        Extra: Using index condition; Using where
                        
                        
    3.利用索引优化锁
           (1) 索引可以减少锁定的行数
                通过索引我们可以在存储引擎层过滤掉我们所不需要的行，就能够更好的利用索引，减少锁带来的开销
                
           (2) 索引可以加快处理速度，同时也加快锁的释放
                    我们只需要对很少的行进行处理，而不是要把所有数据都加载到内存中
                    
           实例:
                在connection1中
                
               A. mysql> explain select * from actor where  last_name='WOOD'\G ;
                    没有使用到任何的索引，Extra: Using where 是要把数据加载到内存中，再进行过滤
                    *************************** 1. row ***************************
                               id: 1
                      select_type: SIMPLE
                            table: actor
                             type: ALL
                    possible_keys: NULL
                              key: NULL
                          key_len: NULL
                              ref: NULL
                             rows: 200
                            Extra: Using where
                            
               B. 开始事务
                   mysql> begin ;
               C. 进行查询事务，占排它锁
                   其中for update 代表显式调用排他锁,并且在 last_name 列上没有索引
                   mysql> select * from actor where last_name='WOOD' for update;
                   +----------+------------+-----------+---------------------+
                   | actor_id | first_name | last_name | last_update         |
                   +----------+------------+-----------+---------------------+
                   |       13 | UMA        | WOOD      | 2006-02-15 04:34:33 |
                   |      156 | FAY        | WOOD      | 2006-02-15 04:34:33 |
                   +----------+------------+-----------+---------------------+
                   
               在connection2中进行查询事务
               D.开始事务
                    mysql> begin ;
                    
               E. 进行查询事务，占排它锁（其中for update 代表显式调用排他锁），并且在 last_name 列上没有索引
                    mysql> select * from actor where last_name='Willis' for update; 
                    
                    这个时候程序阻塞了，在connection2中虽然查询的不同的last_name，但是connection2还是被connection1
                    阻塞了，说明connection1只需要2行数据，但是把整个表锁住了
                    
               F. 将connection1和connection2都进行事务回滚
                    mysql>rollback;
               G. 在last_name列上新加索引
                    mysql> create index idx_lastname on actor(last_name);
               H. 再进行查询计划
                    mysql> explain select * from actor where  last_name='WOOD'\G ;
                    *************************** 1. row ***************************
                               id: 1
                      select_type: SIMPLE
                            table: actor
                             type: ref
                    possible_keys: idx_lastname
                              key: idx_lastname
                          key_len: 137
                              ref: const
                             rows: 2
                            Extra: Using index condition (使用索引进行查询)
                    1 row in set (0.01 sec)
                    
               再进行排它锁测试
               I. 在connection1中
                    (1) mysql> begin ;
                    (2) mysql> select * from actor where last_name='Willis' for update; (进行排它锁查询)
                    
               J. 在connection2中
                      (1) mysql> begin ;
                      (2) mysql> select * from actor where last_name='WOOD' for update; (进行排它锁查询)
                       这个时候有数据产生，不会堵塞。
                       +----------+------------+-----------+---------------------+
                       | actor_id | first_name | last_name | last_update         |
                       +----------+------------+-----------+---------------------+
                       |       83 | BEN        | WILLIS    | 2006-02-15 04:34:33 |
                       |       96 | GENE       | WILLIS    | 2006-02-15 04:34:33 |
                       |      164 | HUMPHREY   | WILLIS    | 2006-02-15 04:34:33 |
                       +----------+------------+-----------+---------------------+
                       但是如果where中last_name为Willis，则也会阻塞，行级锁       
```

## 索引的维护和优化

```shell
    在相同的列上建立多个索引，MySQL都需要单独的维护这些索引，并且在优化器优化查询的过程中还要逐个对这些索引进行选择，这无疑会
    影响数据库的性能。
    
    维护和优化方法:
    
        1.删除重复和冗余的索引
            如果我们在同一个表中建立，3个索引主键索引(primary key(id)), 唯一索引(unique key(id)), 单列索引(index(id)),
            那么这些则叫做重复索引，对应MySQL来说，实际上主键就是为非空的索引，所以在id列上建立主键就没有必要建立唯一索引，更
            没有必要在id列上建立二级索引。
            
            冗余索引：
               (1) 第一种情况
                    index(a),index(a,b) 建立了联合索引(a列和b列)，又在a列上建立索引。
                    但是有时候如果a列经常被查询，则会有意得进行冗余索引，
                    
               (2) 第二种情况
                        primary key(id)， index(a,id)
                        主要是不了解innodb存储引擎B-tree存储结构，每一个二级索引后面都会增加主键信息，所以为了优化查询，把
                        主键也加入到二级索引中，这不太合适
                        
        2.查找未被使用过的索引
            mysql> select object_schema,object_name,index_name,
                   b.`TABLE_ROWS` FROM performance_schema.table_io_waits_summary_by_index_usage a 
                   JOIN information_schema.tables b ON a.`OBJECT_SCHEMA`=`TABLE_SCHEMA` AND
                    a.`OBJECT_NAME`=b.`TABLE_NAME` where index_name IS NOT NULL AND count_star=0 
                    ORDER BY object_schema,object_name;
            
        3.更新索引的统计信息及减少索引的碎片
            MySQL的查询优化器根据索引的统计信息来决定使用哪个索引优化查询，如果表中的统计信息不准确，优化器可能做出错误的判断，
                        
             更新索引的统计信息 ： mysql> analyze table 表名
             更新索引的碎片： mysql> optimize table 表名  使用不当会导致锁表，
```