# MySQL分库分表

## 概要
```shell
    1.单一的写负载已经对数据库造成影响，这时候需要对单一的主数据库进行拆分
    2.MySQL的分区表是在同一个数据库建立的
```

## 分库分表的几种方式

```shell
    1.把一个实例中的多个数据库拆分到不同的实例
        把单一的主服务器的多个数据库拆分到不同的物理服务器中的数据库实例，在一个MySQL实例中可能包含多个逻辑数据库，
        
        例如，一个MySQL节点一包含了订单库，用户库，促销库，这里的MySQL节点并不是只代表一台MySQL服务器数据库，
        这里的MySQL节点代表是这个数据库的集群，是一主多从的数据库环境， 所以可以拆分3个节点，MySQL节点1(一主多从)则是只包含
        订单数据库，MySQL节点2(一主多从)则是只包含用户数据库， MySQL节点3(一主多从)则是只包含促销数据库。这样把单一节点的写压力
        分摊到不同的节点上。
        
        好处:
            实现数据库的拆分比较简单，特别是如果我们的业务应用本身不存在或则不允许跨库查询的情况下，实现这种拆分只要重新对不同的数据库
            直接进行配置就可以了
            
        缺点:
            如果我们本身的写压力就是集中在一个数据库中，例如原先写压力主要集中在订单数据库中，那么拆分后并不能减少 MySQL节点1
            的负载
            
    2.把一个库中的表分离到不同数据库中
            一个数据库有很多的表，例如订单数据库，可能存在订单表，商品表，购物表。这时候发现数据库压力集中在 订单表和 购物表，
            那么我们可以将2张表拆分到不同的数据库中去
        
```

## 数据库分片

```shell
    1.对一个库中的单表进行水平拆分到不同实例的数据库中，如在数据库1中的订单表会拆分到不同节点数据库，例如数据库2订单表2，数据库3订单表3
    2.如果不到万不得已，最好进行数据库的分片，是否能够进行性能调优来缓解数据库的压力
    3.如何选择分区键
        在对数据库分片前，如何选择分区键，分区键选择是否合适决定了数据库查询的性能
        
        (1) 分区键要尽量避免跨分片查询的发生
                如果完全的不做跨分区的查询数据几乎不可能的，因为有很多的统计分析类的查询都需要跨分片处理和汇总数据，在跨分片
                查询时，应用不得不对多个分片进行查询，然后再把查询的结果合并在一起，这通常来说效率会很低，比如对于博客类的应用来说
                如果我们选择按博客文章id的hash值来分片就可能不太合适，因为我们在显示每个人的博客时，都需要跨所有的分区来进行查找
                才能得到所有博客的文件。如果我们选择用户的user_id来分片则可以保证同一个人的博客全在一个分片中，如果想要显示一个人
                的博客，则只要在一个分片中进行查询就可以了。在这种情况下使用user_id 要比 使用文件id的hash值 作为 分区键更为合适
                
        (2) 分区键要尽量是各个分片中的数据平均
                我们对数据库进行分片的目的是为了减少主库的写负载，如果分片后大部分的写还是集中在某一个分片中，这样就失去了分片的意义
                
                
    4.如何存储无需分片的表
        (1) 每个分片中存储一份相同的数据
                这种方法通常用于表本身数据量不大，而且也不会经常被更新的表，同时这些表可能经常需要和分区表在一起关联查询，所以在
                每一个分片中都存储一份冗余的数据可以更好的提高数据库的查询效率，如果使用这种方式对于 维护分片中每个相同的表的一致性
                就显得非常重要，一般对某个分片的表进行更新时同时对其他分片的对应的表进行更新。
                
        (2) 使用额外的节点 同一存储
                把所有无需分片的表的数据存储在公共的区域，这种方法的好处是在整个集群中不存在冗余的问题，应用也不用维护相同的数据
                如果分片的表要和 无需分片的表的数据 进行关联查询的话，只能有程序分别查询后进行合并操作，所以这种方法在查询效率上
                比方法一要差一些。
                
    5.如何在节点上部署分片
        (1) 每个分片使用单一数据库，并且数据库名也相同
        (2) 将多个分片表存储在一个数据库中，并在表名上加入分片号后缀。
                这个就可能造成单一数据库有写压力
                
        (3) 在一个节点中部署多个数据库，每个数据库包含一个分片
                而一个节点中的数据库名不能重名。
                
    6.如何分配分片中的数据
        数据在各个分片中的分布对于数据库的性能有着很大的影响，我们希望尽量平均的在各个分片中来分配数据，这个平均不单单是数据量的平均
        还包括访问上的平均，在一些情况下热点数据很可能集中到某一个或则某几个分片中，这时这几个分片的负载要比其他分片的负载高的多。
        当数据库查询这几个分片时性能就会大幅下降，所以选择如何分配分片中的数据 得 根据具体业务进行分析。
        
        (1) 按分区键的Hash值与分片数量取模来分配 分片的数据
                用分区键的Hash值是因为 分区键并不总是数值型的数据，所以要利用合适的hash函数
        (2) 按分区键的范围来分配分片数据
                如果我们有10个分片，第一个分片中包含了1到100的数据，第二个分片中包含了101到200数据，以此类推，这种方式常用与
                分区键为日期类型和数值类型的情况
                
                优点:
                    很清楚的知道数据被分配到哪个分区片中
                    
                缺点:
                    很容易造成数据分配不平均和数据的访问量不平均的情况，例如对学生做题题库进行分片，选择课程的id范围来进行分片的，
                    选择某一课的学生远远多于其他课程，这门课程所在分片压力远高于其他分片
                    
        (3) 利用分区键和分片的映射表来分配分片数据
                建立一张分区键和分片的对应关系表，在这张表中可以对什么样的数据存储在哪些分片中来进行配置，在进行数据存储之前先通过
                这张表来获取具体要存储的分片中的信息，然后再把数据存储在对应的分片中，分区键和分片的对应关系会造成很大的压力，
                所以可以通过缓存的方式进行缓存
                
    7.如何生成全局唯一ID
        (1) 使用auto_increment_incremnet和auto_increment_offset参数(2个结合起来)
                使用auto_increment_incremnet在表中生成唯一ID，auto_increment_incremnet的值要和分片节点的数量相同，
                auto_increment_increment = 2 (增加步长，一次增加多少值，在使用主主复制时，一台按1,3,5.
                                                                              另外一台2,4,6， 不会ID冲突)
                auto_increment_offset = 1|2 (这个决定了自增ID从哪个开始，默认从1开始，在使用主主复制时，需要将
                                             一台设置为1，另外一台设置为2)
                                             
            这种方式只适用于一个节点中只保存一套分区表情况，如果一个节点中保存多套分区表，在同一个节点中相同的分区表所生成的ID还是会
            出现冲突。
            
        (2) 使用全局节点来生成ID
            配置一个全局节点ID，并在这个节点中建立相应的表来使用auto_increment的属性来生成自增ID，应用程序先通过这个全局表取
            唯一ID后，再通过分区函数把数据插入到不同的分片中，缺点是 全局节点会成为数据库的瓶颈。
            
        (3) 在Redis等缓存服务器中创建全局ID
                使用Redis等缓存服务器来代替全局节点ID，Redis读写效率要远高于MySQL数据库，
```

## 数据库分片演示

```shell
    1.一个数据库中有订单表，订单商品表，分类表，需要对订单表和订单商品表进行分区。
    2.分片工具
        > wget http://www.onexsoft.cn/software/oneproxy-rhel6-linux64-v5.8.1-ga.tar.gz
        
    3.节点1 第一个分片节点 192.168.3.100
      节点2 第二个分片节点 192.168.3.101
      节点3 oneproxy节点(进行分片的工具) 192.168.3.102
      
    4.配置节点3 oneproxy
        (1) 修改 /home/soft/oneproxy/demo.sh 内容
        
                 export ONEPROXY_HOME=/home/soft/oneproxy/
                 
        (2) 在节点1创建用户:
                I.创建用户
                    mysql> create user ‘用户名’ @ ‘允许使用的ip网段' identified by '密码'
                    例如
                     mysql> create user ‘jame’ @ ‘192.168.3.%' identified by '123456';
                     
                II.创建数据库
                       mysql> create database orders;
                III.授权
                    mysql> grant all privileges on orders.* to 用户名’ @ ‘允许使用的ip网段'
                    其中 all privileges 是所有的权限
                    例如
                       mysql> grant all privileges on orders.* to 'jame’ @ ‘192.168.3.%';
                   
        (3) 在节点2建立相同的用户:
                 I.创建用户
                    mysql> create user ‘用户名’ @ ‘允许使用的ip网段' identified by '密码'
                    例如
                     mysql> create user ‘jame’ @ ‘192.168.3.%' identified by '123456';
                     
                II.创建数据库
                       mysql> create database orders;
                III.授权
                    mysql> grant all privileges on orders.* to 用户名’ @ ‘允许使用的ip网段'
                    其中 all privileges 是所有的权限
                    例如
                       mysql> grant all privileges on orders.* to 'jame’ @ ‘192.168.3.%';
                       
                
                 
        (4) 修改 /home/soft/oneproxy/conf/proxy.conf
                [oneproxy]
                    keepalive = 1 保证oneproxy在后台运行的
                    event-threads = 4 指定线程的数量
                    proxy-address = :3306  (监控的端口)
                    
                    proxy-master-addresses.1   = 192.168.3.100:3306@order01  监控的第一个节点
                    proxy-master-addresses.2   = 192.168.3.101:3306@order02  监控的第二个节点
                    
                    proxy-user-list            = test/1378F6CC3A8E8A43CA388193FBED5405982FBBD3@jame
                                                 用户名/加密后的密码/数据库
                                                 
                    proxy-part-template        = conf/order_part.txt  自定义的配置
                    proxy-group-policy    = order01:master-only  节点1名
                    proxy-group-policy    = order02:master-only  节点2名
                                                 
                A. 先运行一下/home/soft/oneproxy/demo.sh来获取加密后的密码
                B. 
                    (1) > mysql -P4041 -uadmin -pOneProxy -h127.0.0.1
                    (2) mysql> passwd "123456";
                    结果获得了123456加密字符串，写入到/home/soft/oneproxy/conf/proxy.conf中
                
        (5) 修改分区表的分区方式
                    
                [
                	#完成订单表的分区
                	{
                		"table":"order_detail",   #分区表表名
                		"pkey":"order_id",   #分区键
                		"type":"int"  #分区键类型
                		"method":"hash"  #分区的方式(hash方式) 
                		"partitions" :        #分区的存储 方式
                		[
                			#建立2个分片，在分片1中使用0来当它的后缀
                			 {"suffix":"_0","group":"order01"}  #属于order01组 ，在order01中所有分片表都要加上_0
                			 {"suffix":"_1","group":"order02"}
                		]
                	},
                		#完成订单商品表的分区
                	{
                		"table":"order_product",   #分区表表名
                		"pkey":"order_id",   #分区键
                		"type":"int"  #分区键类型
                		"method":"hash"  #分区的方式(hash方式) 
                		"partitions" :        #分区的存储 方式
                		[
                			#建立2个分片，在分片1中使用0来当它的后缀
                			 {"suffix":"_0","group":"order01"}  #属于order01组 ，在order01中所有分片表都要加上_0
                			 {"suffix":"_1","group":"order02"}
                		]
                	},
                	##分类目录表，不需要分区
                	 {
                		"table":"category",   #分区表表名
                		"pkey":"id",   #分区键
                		"type":"int"  #分区键类型
                		"method":"global"  #分区的方式(hash方式) 
                		"partitions" :        #分区的存储 方式
                		[ 
                			 {"group":"order01"}  #不需要分区
                			 {"group":"order02"}
                		]
                	} 
                ]
                
        (6) 在节点1中
                mysql> use orders;
                mysql> create table order_detail_0(order_id int not null,
                								  add_time datetime not null, 
                								  order_amount decimal(6,2),
                								  primary key (order_id));
                								  
                mysql> create table order_product_0(order_id int not null,
                								    order_product_id int not null,
                								    primary key(order_id));
                								    
                创建全局的非分片表
                mysql> create table category(id int not null,
                							category_name varchar(10),
                							primary key(id)); 
                							
        (7) 在节点2
                mysql> use orders;
                mysql> create table order_detail_1(order_id int not null,
                                                  add_time datetime not null, 
                                                  order_amount decimal(6,2),
                                                  primary key (order_id));
                                                  
                mysql> create table order_product_1(order_id int not null,
                                                    order_product_id int not null,
                                                    primary key(order_id));
                                                    
                创建全局的非分片表
                mysql> create table category(id int not null,
                                            category_name varchar(10),
                                            primary key(id)); 
                                            
        (8) 重启oneproxy
                > ./demo.sh
        (9) 进入oneproxy配置页面
            > mysql -P4041 -h127.0.0.1 -uadmin -pOnePrxoy
                A. 查看后台服务
                    mysql> list backend;
                B. 查看后台的配置是否正确
                    mysql> list tables;
        (10) 编写测试脚本
             #通过oneproxy进行代理，只需要逻辑表
                #!/bin/bash
                order_id=2
                while :
                do
                   order_id=`echo $order_id+1| bc`
                   sql="insert into order_detail(order_id,add_time,order_amount,user_name,user_tel) values(${order_id},now(),100,'wyjs','123456')";
                   echo $sql | mysql -utest -p123456 -P3307 -h10.103.9.101
                
                   sql2="insert into order_product(order_id,product_id) values(${order_id},${order_id})"
                   echo $sql2 | mysql -utest -p123456 -P3307 -h10.103.9.101
                
                   sql3="insert into category(id,name) values(${order_id},'wyjs')"
                   echo $sql3 | mysql -utest -p123456 -P3307 -h10.103.9.101
                   
                
                done
                
        (11) 进入oneproxy配置页面
                 > mysql -P4041 -h127.0.0.1 -uadmin -pOnePrxoy
                     A. 查看逻辑表
                         mysql> select count(*) from order_detail;
                         mysql> select count(*) from order_product;
                     B. 查看物理表
                         mysql> select count(*) from order_detail_1;
                         mysql> select count(*) from order_product_1; 
                         


```
