# 基准测试

## 基本概念

```shell
    1.基准测试：测量系统的性能，是一种测量和评估软件性能指标的活动，用于建立某个时刻的性能基准，以便当系统软硬件发生变化时重新进行
               基准测试来评估变化对性能的影响。是针对系统设置的一种压力测试。
    2.基准测试与压力测试的区别
        基准测试：直接，简单，易于比较，用于评估服务器的处理能力，用现成的测试工具，它是简化了的压力测试，不关心业务逻辑
        压力测试：对真实的业务数据进行测试，获取真实系统所承受的压力
        例如对一个网站进行压力测试，需要针对不同的主题，所使用的数据和查询也是真实用到的。
```

## 基准测试的目的

```shell
    1.建立MySQL服务器的性能基准线：确定当前MySQL服务器当前的运行情况
    2.模拟比当前系统更高的负载，以找出系统的扩张瓶颈： 主要的实现方式增加对数据库并发，观察QPS，TPS变化，确定并发量和性能最优的关系(
                                                通过不断的调整对数据库的并发得到性能最优的情况，超过这个并发量性能可能会下降)
    3.基准测试还测试不同的硬件，软件和操作系统配置：比如在硬件方面RAID0和RAID5哪种更适合MySQL，如果系统从机械硬盘升级到SSD对随机写
                                              会有什么帮助，linux下不同磁盘分区格式(文件系统)对数据库性能是否有影响，升级MySQL的
                                              版本是否能改善性能。
    4.证明新的硬件设备是否配置正确： 在新的硬件正式上线到生产环境之前，一定要对新的系统进行基准测试，这不仅可以帮助我们了解新的设备的
                                 基准线，还可以帮助我们看看其配置是否正确
```

## 如何进行基准测试

```shell
    1.对整个系统进行基准测试：
           从系统的入口进行测试(如网站Web前端，手机App前端),
                    优点 I:能够测试整个系统的性能，包括Web服务器，缓存，数据库等
                        II:能反映出系统中各个组件接口间的性能问题，体现真实性能的状况。
                        
                    缺点 测试设计复杂，不够简单，消耗时间长
                    
    2.单独对MySQL进行基准测试：
            不需要对整个系统进行基准测试，只需要对单独组件进行测试，适合场景是只对MySQL进行了优化(包括表结构和索引)，其他组件保持不变，
                    优点: 试设计简单，所需耗费的时间短
                    缺点: 无法全面了解整个系统的性能基线，也无法体现不同组件接口间的性能问题
```

## MySQL基准测试常用的指标

```shell
    1.单位时间内所处理的事务数(TPS)
    2.单位时间内所处理的查询数(QPS)
    3.响应时间：用于衡量完成一个测试所花费的整体时间，有百分比响应时间：一个SQL90%的响应时间是10ms,那么认为该SQL在正常情况下为10ms
    4.并发量:同时处理的查询请求的数量 ！= 连接数，并发量是正在工作中的并发的操作数或者同时工作的数量
```

## 基准测试的步骤

```shell
    1.计划和设计基准测试
        (1) 对整个系统进行基准测试还是某一组件
        (2) 使用什么样的数据进行基准测试，比如我们希望基准测试能够反映实际情况，最好能够采用生产环境的实际数据，提交准备好数据库的备份
            以及在真实环境中SQL的记录。如果我们的测试没有必要使用真实数据，可以采用一般的基准测试工具
        (3) 准备基准测试及数据收集脚本：需要记录CPU使用率，IO，网络流量，状态和计数器信息等
                脚本如下：
                    #!/bin/bash
                    INTERVAL=5
                    PREFIX=/home/imooc/benchmarks/$INTERVAL-sec-status
                    RUNFILE=/home/imooc/benchmarks/running
                    echo "1" > $RUNFILE
                    MYSQL=/usr/local/mysql/bin/mysql
                    $MYSQL -e "show global variables" >> mysql-variables
                    while test -e $RUNFILE; do
                    	file=$(date +%F_%I)
                    	sleep=$(date +%s.%N | awk '{print 5 - ($1 % 5)}')
                    	sleep $sleep
                    	ts="$(date +"TS %s.%N %F %T")"
                    	loadavg="$(uptime)"
                    	echo "$ts $loadavg" >> $PREFIX-${file}-status
                    	$MYSQL -e "show global status" >> $PREFIX-${file}-status &
                    	echo "$ts $loadavg" >> $PREFIX-${file}-innodbstatus
                    	$MYSQL -e "show engine innodb status" >> $PREFIX-${file}-innodbstatus &
                    	echo "$ts $loadavg" >> $PREFIX-${file}-processlist
                    	$MYSQL -e "show full processlist\G" >> $PREFIX-${file}-processlist &
                    	echo $ts
                    done
                    echo Exiting because $RUNFILE does not exists
        (4) 运行基准测试
        (5) 保存及分析基准测试结果:最好能够用脚本进行分析，也可以有图形化呈现
                
```

## 基准测试需要主要问题

```shell
    1.使用生产环境数据时只使用部分数据，如果要使用生产环境的数据则最好使用数据库完全备份来测试，而不是人为的在生产环境的全部数据中只取
      一小部分进行测试
    2.在多用户场景中，只做单用户的测试。比如Web中，通常存在大量的并发的场景，所以对Web应用的数据库测试就一定要考虑并发情况，使用多个连接
      线程对数据库进行测试。在并发测试和单个连接测试可能所表现的性能不太一样(并发测试可能会出现阻塞等现象)
    3.反复得执行同一个查询(容易缓存命中，无法反应真实查询性能)，在真实生产环境所使用SQL是不同的，
                
```

## 常见的基准测试工具

```shell
    1.mysqlslap
        特点：
            (1) 可以模拟服务器负载，并输出相关统计信息
            (2) 测试时可以指定执行的并发连接数以及我们所要指定的SQL语句。如果我们没有指定SQL语句，mysqlslap可以自动生成完成基准测试
                所需要的数据及相关的查询语句
                
        常用的参数:
             -a, --auto-generate-sql:由系统自动生成SQL脚本进行测试
             --auto-generate-sql-add-autoincrement ：在生成的表中增加自增ID列，如果是innodb存储引擎，则这个参数很重要，innodb的
                                                     主键是聚集索引，所以最好使用自增ID作为innodb的主键
             --auto-generate-sql-load-type=name ：指定测试中使用的查询类型
                                                  name 值有 mixed, update, write, key,read，
                                                  默认是mixed(包含有update, write, key,read)
                                                  
             --auto-generate-sql-write-number=#  : 指定初始化数据时生成的数据量(每个线程插入的记录数)
             --concurrency=# ：指定并发线程的数量    
             --engine=name ：指定要测试表的存储引擎，可以用逗号分割多个存储引擎，如果是多个存储引擎进行测试，mysqlslap会按照指定的
                             顺序，先按照MyISAM存储引擎，然后会清理MyISAM表，然后在对innodb进行测试，这个参数不能和
                             --no-drop( Do not drop the schema after the test)一起使用。如果没有使用这个参数，则MySQL会使用
                             默认存储引擎进行测试。
             --no-drop ：在测试完后不进行清理数据，如果没有显式使用该参数，MySQL是在测试完后自动清理数据。如果我们想确认MySQL测试
                         完后是否是我们想象的结果就可以使用该参数。
             --iterations=# ：指定测试运行的次数，如果想要得出正确的结果，则要进行多次测试，得出百分比结果
             --number-of-queries=# ：指定每一个线程执行的查询数量
             --debug-info ： 指定输出额外的内存及CPU统计信息
             --number-int-cols=# ：指定测试表中包含的INT类型列的数量,可以通过设置INT类型列的数量来得出性能最优的情况
             --number-char-cols=# ：指定测试表中包含的varchar类型列的数量
             --create-schema=name :指定用于执行测试的数据库的名字
             --query=name :指定自定义SQL脚本文件，如果不想使用MySQL生成的自动SQL脚本进行测试
             --only-print :并不运行测试脚本(连接数据库等操作)，而是把脚本的数据库操作打印出来
             
        运行使用：
            > mysqlslap --concurrency=1,50,100,200 --iterations=3 --number-int-cols=5 --number-char-cols=5 
              --auto-generate-sql -auto-generate-sql-add-autoincrement --engine=myisam,innodb --number-of-queries=10 
              --create-schema=slap_test -uroot -p52346898
              
        不足：只能较为简单的测试，不能进行索引创建等复杂的测试，也不能收集相关的cpu利用率和内存等参数
              
    2.sysbench
         可以对系统各个参数(I/O,cpu,内存)进行测试
         安装说明：
            https://github.com/akopytov/sysbench#linux
            进行源代码安装
            
         常用参数:
            --mysql-db:指定用于基准测试的数据库名(该数据库名是已经存在的)
            --mysql-table-engine:指定所使用的存储引擎，有myisam, innodb, heap,默认是innodb
            --oltp-table-size :指定每个表的数据行数
            --max-time : 指定最大的整个测试时间
            --report-interval:指定间隔多长时间输出一次统计信息
            --mysql-db=imooc : 指定的数据库名
            --mysql-user:指定执行测试的MySQL的用户
            --mysql-password:指定执行测试的MySQL用户对应的密码
            --num-threads :有多少个线程进行并发工作
            --oltp-tables-count=num : 指定测试数据库中表的个数
            --init-rnd: on(代表在测试开始时，将数据初始化为随机的)
            prepare : 用于准备测试数据
            run:用于实际进行测试
            
         实际演示:
            (1) 对CPU进行测试(进行素数的加法运算)
                    > sysbench --test=cpu --cpu-max-prime=20000 run    (最大的素数为20000以内)
            (2) I/O测试
                    准备的数据文件一定要比当前的内存要大，这样才会忽略缓冲区的影响。
                    > sysbench --test=fileio --file-total-size=1G prepare
                      (为I/O测试进行准备，准备1G大小文件(其中生产很多小文件，这些小文件大小总和为1G)
                      
                    > sysbench --test=fileio help  (帮助信息)
                    
                    > sysbench --test=fileio --num-threads=8 --init-rnd=on --file-total-size=1G  --file-test-mode=rndrw
                      --report-interval=1 run
                      
                      (工作线程为8个，测试文件大小为1G,IO操作模式为随机读写)
                      
            当拿到新的硬件时，通过以上测试方法掌握 其新的硬件的基准线
            
            (3) 对数据库进行基准测试：
                    1.先在建立一个新的数据库
                        mysql> create database imooc;
                    2.创建一个用户，指定对数据库的权限
                        mysql> grant all PRIVILEGES on *.* to  'jame'@'%'  identified by '123456' WITH GRANT OPTION;
                        
                    3.对innodb存储引擎进行测试
                            准备测试数据：
                                > sysbench --test=oltp --mysql-table-engine=innodb --oltp-table-size=10000 
                                  --mysql-db=imooc --mysql-user=jame --mysql-password=123456  prepare
                            启动系统收集脚本:
                                Get_Test_info.sh
        
```