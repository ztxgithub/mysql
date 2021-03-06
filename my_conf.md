# my_conf

```shell
            # 以下选项会被MySQL客户端应用读取。
            # 注意只有MySQL附带的客户端应用程序保证可以读取这段内容。
            # 如果你想你自己的MySQL应用程序获取这些值。
            # 需要在MySQL客户端库初始化的时候指定这些选项。
            
            # For advice on how to change settings please see
            # http://dev.mysql.com/doc/refman/5.7/en/server-configuration-defaults.html
            # *** DO NOT EDIT THIS FILE. It's a template which will be copied to the
            # *** default location during install, and will be replaced if you
            # *** upgrade to a newer version of MySQL.
            
            # mysqld程序
            
            [mysqld]
            
            # Remove leading # and set to the amount of RAM for the most important data
            # cache in MySQL. Start at 70% of total RAM for dedicated server, else 10%.
            # innodb_buffer_pool_size = 128M
            
            # ★★★这里很重要️能让MySQL登陆链接变快速
            skip-name-resolve
            
            # Remove leading # to turn on a very important data integrity option: logging
            # changes to the binary log between backups.
              log_bin    ##不指定binlog日志的保存路径，则默认是/var/lib/mysql
              log_bin = /var/log/mysql-bin    ##也可以指定路径
              expire-logs-days=7   ##只保持7天，之后覆盖写
              
            
            # These are commonly set, remove the # and set as required.
            # 使用给定目录作为根目录(安装目录)。
            # basedir = .....
            # 从给定目录读取数据库文件。
            # datadir = .....
            # 为mysqld程序指定一个存放进程ID的文件(仅适用于UNIX/Linux系统);
            # pid-file = .....
            # 指定MsSQL侦听的端口
            # port = .....
            # server_id = .....
            # 为MySQL客户程序与服务器之间的本地通信指定一个套接字文件(Linux下默认是/var/lib/mysql/mysql.sock文件)
            # socket = .....
            
            
            sql_mode=NO_ENGINE_SUBSTITUTION,STRICT_TRANS_TABLES
            
            # 一般配置选项
            basedir = /data/apps/mysql
            datadir = /data/appData/mysql
            port = 3306
            socket = /var/run/mysqld/mysqld.sock
            # 设置
            character-set-server=utf8
            
            
            # 指定MySQL可能的连接数量。
            # 当MySQL主线程在很短时间内接收到非常多的连接请求，该参数生效，主线程花费很短时间检查连接并且启动一个新线程。
            # back_log参数的值指出在MySQL暂时停止响应新请求之前的短时间内多少个请求可以被存在堆栈中。
            # 如果系统在一个短时间内有很多连接，则需要增大该参数的值，该参数值指定到来的TCP/IP连接的侦听队列的大小。
            # 试图设定back_log高于你的操作系统的限制将是无效的。默认值为50。对于Linux系统推荐设置为小于512的整数。
            
            # back_log 是操作系统在监听队列中所能保持的连接数,
            # 队列保存了在 MySQL 连接管理器线程处理之前的连接.
            # 如果你有非常高的连接率并且出现 “connection refused” 报错,
            # 你就应该增加此处的值.
            # 检查你的操作系统文档来获取这个变量的最大值.
            # 如果将back_log设定到比你操作系统限制更高的值，将会没有效果
            back_log = 300
            
            # 不在 TCP/IP 端口上进行监听.
            # 如果所有的进程都是在同一台服务器连接到本地的 mysqld,
            # 这样设置将是增强安全的方法
            # 所有 mysqld 的连接都是通过 Unix Sockets 或者命名管道进行的.
            # 注意在 Windows下如果没有打开命名管道选项而只是用此项
            # (通过 “enable-named-pipe” 选项) 将会导致 MySQL 服务没有任何作用!
            # skip-networking
            
            # MySQL 服务所允许的同时会话数的上限
            # 其中一个连接将被 SUPER 权限保留作为管理员登录.
            # 即便已经达到了连接数的上限.
            max_connections = 3000
            
            # 每个客户端连接最大的错误允许数量,如果达到了此限制.
            # 这个客户端将会被 MySQL 服务阻止直到执行了 “FLUSH HOSTS” 或者服务重启
            # 非法的密码以及其他在链接时的错误会增加此值.
            # 查看 “Aborted_connects” 状态来获取全局计数器.
            max_connect_errors = 50
            
            # 所有线程所打开表的数量.
            # 增加此值就增加了 mysqld 所需要的文件描述符的数量
            # 这样你需要确认在 [mysqld_safe] 中 “open-files-limit” 变量设置打开文件数量允许至少等于 table_cache 的值
            table_open_cache = 4096
            
            # 允许外部文件级别的锁. 打开文件锁会对性能造成负面影响
            # 所以只有在你在同样的文件上运行多个数据库实例时才使用此选项(注意仍会有其他约束!)
            # 或者你在文件层面上使用了其他一些软件依赖来锁定 MyISAM 表
            #external-locking
            
            # 服务所能处理的请求包的最大大小以及服务所能处理的最大的请求大小(当与大的 BLOB 字段一起工作时相当必要)
            # 每个连接独立的大小，大小动态增加
            max_allowed_packet = 32M
            
            # 在一个事务中 binlog 为了记录 SQL 状态所持有的 cache 大小
            # 如果你经常使用大的,多声明的事务,你可以增加此值来获取更大的性能.
            # 所有从事务来的状态都将被缓冲在 binlog 缓冲中然后在提交后一次性写入到 binlog 中
            # 如果事务比此值大, 会使用磁盘上的临时文件来替代.
            # 此缓冲在每个连接的事务第一次更新状态时被创建
            binlog_cache_size = 4M
            
            # 独立的内存表所允许的最大容量。
            # 此选项为了防止意外创建一个超大的内存表导致永尽所有的内存资源。
            max_heap_table_size = 128M
            
            # 随机读取数据缓冲区使用内存(read_rnd_buffer_size)：和顺序读取相对应，
            # 当 MySQL 进行非顺序读取（随机读取）数据块的时候，会利用>这个缓冲区暂存读取的数据
            # 如根据索引信息读取表数据，根据排序后的结果集与表进行 Join 等等
            # 总的来说，就是当数据块的读取需要满足>一定的顺序的情况下，MySQL 就需要产生随机读取，进而使用到 read_rnd_buffer_size 参数所设置的内存缓冲区
            read_rnd_buffer_size = 16M
            
            # 排序缓冲被用来处理类似 ORDER BY 以及 GROUP BY 队列所引起的排序
            # 如果排序后的数据无法放入排序缓冲,一个用来替代的基于磁盘的合并分类会被使用
            # 查看 “Sort_merge_passes” 状态变量。
            # 在排序发生时由每个线程分配
            # 每个需要进行排序的线程分配该大小的一个缓冲区。增加这值加速ORDER BY或GROUP BY操作。
            # 注意：该参数对应的分配内存是每连接独占！如果有100个连接，那么实际分配的总共排序缓冲区大小为100×6=600MB
            sort_buffer_size = 16M
            
            # 此缓冲被使用来优化全联合(FULL JOINS 不带索引的联合)。
            # 类似的联合在极大多数情况下有非常糟糕的性能表现,但是将此值设大能够减轻性能影响。
            # 通过 “Select_full_join” 状态变量查看全联合的数量
            # 当全联合发生时,在每个线程中分配
            join_buffer_size = 16M
            
            # 缓存可重用的线程数
            # thread_cache = 8
            
            # 避免MySQL的外部锁定，减少出错几率增强稳定性。
            # skip-locking                 
            
            # 我们在 cache 中保留多少线程用于重用
            # 当一个客户端断开连接后,如果 cache 中的线程还少于 thread_cache_size,则客户端线程被放入cache 中。
            # 这可以在你需要大量新连接的时候极大的减少线程创建的开销
            # (一般来说如果你有好的线程模型的话,这不会有明显的性能提升。)
            thread_cache_size = 16
            
            # 此允许应用程序给予线程系统一个提示在同一时间给予渴望被运行的线程的数量。
            # 此值只对于支持 thread_concurrency() 函数的系统有意义( 例如Sun Solaris)。
            # 你可可以尝试使用 [CPU数量]*(2..4) 来作为 thread_concurrency 的值
            #****(此属性对当前环境无效)****
            # thread_concurrency = 8
            
            # 查询缓冲常被用来缓冲 SELECT 的结果并且在下一次同样查询的时候不再执行直接返回结果。
            # 打开查询缓冲可以极大的提高服务器速度, 如果你有大量的相同的查询并且很少修改表。
            # 查看 “Qcache_lowmem_prunes” 状态变量来检查是否当前值对于你的负载来说是否足够高。
            # 注意: 在你表经常变化的情况下或者如果你的查询原文每次都不同,
            # 查询缓冲也许引起性能下降而不是性能提升。
            query_cache_size = 128M
            
            # 只有小于此设定值的结果才会被缓冲
            # 此设置用来保护查询缓冲,防止一个极大的结果集将其他所有的查询结果都覆盖。
            query_cache_limit = 4M
            
            # 被全文检索索引的最小的字长。
            # 你也许希望减少它，如果你需要搜索更短字的时候。
            # 注意在你修改此值之后，你需要重建你的 FULLTEXT 索引
            ft_min_word_len = 8
            
            # 如果你的系统支持 memlock() 函数，你也许希望打开此选项用以让运行中的 mysql 在在内存高度紧张的时候，数据在内存中保持锁定并且防止可能被 swapping out
            # 此选项对于性能有益
            #memlock
            
            # 当创建新表时作为默认使用的表类型，
            # 如果在创建表示没有特别执行表类型，将会使用此值
            #****(此属性对当前环境无效)****
            #default_table_type = InnoDB
            
            # 线程使用的堆大小. 此容量的内存在每次连接时被预留.
            # MySQL 本身常不会需要超过 64K 的内存
            # 如果你使用你自己的需要大量堆的 UDF 函数或者你的操作系统对于某些操作需要更多的堆，你也许需要将其设置的更高一点.
            thread_stack = 512K
            
            # 设定默认的事务隔离级别.可用的级别如下:
            # READ-UNCOMMITTED， READ-COMMITTED， REPEATABLE-READ， SERIALIZABLE
            transaction_isolation = REPEATABLE-READ
            
            # 内部(内存中)临时表的最大大小
            # 如果一个表增长到比此值更大，将会自动转换为基于磁盘的表。
            # 此限制是针对单个表的，而不是总和。
            tmp_table_size = 128M
            
            # 打开二进制日志功能。
            # 在复制(replication)配置中，作为 MASTER 主服务器必须打开此项
            # 如果你需要从你最后的备份中做基于时间点的恢复，你也同样需要二进制日志。
            log-bin = mysql-bin
            
            # 如果你在使用链式从服务器结构的复制模式 (A->B->C)，
            # 你需要在服务器B上打开此项。
            # 此选项打开在从线程上重做过的更新的日志， 并将其写入从服务器的二进制日志。
            # log_slave_updates
            
            # 打开全查询日志。 所有的由服务器接收到的查询 (甚至对于一个错误语法的查询)
            # 都会被记录下来。 这对于调试非常有用， 在生产环境中常常关闭此项。
            # log
            
            # 将警告打印输出到错误 log 文件。 如果你对于 MySQL 有任何问题
            # 你应该打开警告 log 并且仔细审查错误日志，查出可能的原因。
            # log_warnings
            
            # 记录慢速查询。 慢速查询是指消耗了比 “long_query_time” 定义的更多时间的查询。
            # 如果 log_long_format 被打开，那些没有使用索引的查询也会被记录。
            # 如果你经常增加新查询到已有的系统内的话。 一般来说这是一个好主意，
            # log_slow_queries
            
            # 有的使用了比这个时间(以秒为单位)更多的查询会被认为是慢速查询。
            # 不要在这里使用“1″, 否则会导致所有的查询,甚至非常快的查询页被记录下来(由于 MySQL 目前时间的精确度只能达到秒的级别)。
            long_query_time = 6
            
            # 在慢速日志中记录更多的信息。
            # 一般此项最好打开。
            # 打开此项会记录使得那些没有使用索引的查询也被作为到慢速查询附加到慢速日志里
            # log_long_format
            
            # 此目录被MySQL用来保存临时文件。例如,
            # 它被用来处理基于磁盘的大型排序,和内部排序一样。
            # 以及简单的临时表。
            # 如果你不创建非常大的临时文件,将其放置到 swapfs/tmpfs 文件系统上也许比较好
            # 另一种选择是你也可以将其放置在独立的磁盘上。
            # 你可以使用”;”来放置多个路径
            # 他们会按照 roud-robin 方法被轮询使用。
            # tmpdir = /tmp
            
            # *** 主从复制相关的设置
            
            # 唯一的服务辨识号,数值位于 1 到 2^32-1之间。
            # 此值在master和slave上都需要设置。
            # 如果 “master-host” 没有被设置,则默认为1, 但是如果忽略此选项,MySQL不会作为master生效。
            server-id = 1
            
            # 复制的Slave (去掉master段的注释来使其生效)
            #
            # 为了配置此主机作为复制的slave服务器,你可以选择两种方法:
            #
            # 1) 使用 CHANGE MASTER TO 命令 (在我们的手册中有完整描述) -
            # 语法如下:
            #
            # CHANGE MASTER TO MASTER_HOST=, MASTER_PORT=,
            # MASTER_USER=, MASTER_PASSWORD= ;
            #
            # 你需要替换掉，等被尖括号包围的字段以及使用master的端口号替换 (默认3306)。
            #
            # 例子:
            #
            # CHANGE MASTER TO MASTER_HOST=’125.564.12.1′, MASTER_PORT=3306,
            # MASTER_USER=’joe’, MASTER_PASSWORD=’secret’;
            #
            # 或者
            #
            # 2) 设置以下的变量. 不论如何, 在你选择这种方法的情况下， 然后第一次启动复制(甚至不成功的情况下，
            # 例如如果你输入错密码在master-password字段并且slave无法连接)，
            # slave会创建一个 master.info 文件，并且之后任何对于包含在此文件内的参数的变化都会被忽略
            # 并且由 master.info 文件内的内容覆盖， 除非你关闭slave服务， 删除 master.info 并且重启slave 服务。
            # 由于这个原因，你也许不想碰一下的配置(注释掉的) 并且使用 CHANGE MASTER TO (查看上面) 来代替
            #
            # 所需要的唯一id号位于 2 和 2^32 – 1之间
            # (并且和master不同)
            # 如果master-host被设置了.则默认值是2
            # 但是如果省略,则不会生效
            #server-id = 2
            #
            # 复制结构中的master – 必须
            #master-host =
            #
            # 当连接到master上时slave所用来认证的用户名 – 必须
            #master-user =
            #
            # 当连接到master上时slave所用来认证的密码 – 必须
            #master-password =
            #
            # master监听的端口.
            # 可选 – 默认是3306
            #master-port =
            
            # 使得slave只读。只有用户拥有SUPER权限和在上面的slave线程能够修改数据。
            # 你可以使用此项去保证没有应用程序会意外的修改slave而不是master上的数据
            #read_only
            
            #*** MyISAM 相关选项
            
            # 关键词缓冲的大小， 一般用来缓冲 MyISAM 表的索引块。
            # 不要将其设置大于你可用内存的30%，
            # 因为一部分内存同样被OS用来缓冲行数据
            # 甚至在你并不使用 MyISAM 表的情况下， 你也需要仍旧设置起 8-64M 内存由于它同样会被内部临时磁盘表使用。
            # key_buffer_size = 128M
            
            # 用来做 MyISAM 表全表扫描的缓冲大小.
            # 当全表扫描需要时,在对应线程中分配.
            # read_buffer_size = 8M
            
            # 当在排序之后,从一个已经排序好的序列中读取行时,行数据将从这个缓冲中读取来防止磁盘寻道.
            # 如果你增高此值,可以提高很多 ORDER BY 的性能.
            # 当需要时由每个线程分配
            # read_rnd_buffer_size = 64M
            
            # MyISAM 使用特殊的类似树的 cache 来使得突发插入
            # (这些插入是,INSERT … SELECT, INSERT … VALUES (…), (…), …, 以及 LOAD DATA INFILE) 更快.
            # 此变量限制每个进程中缓冲树的字节数.
            # 设置为 0 会关闭此优化.
            # 为了最优化不要将此值设置大于 “key_buffer_size”.
            # 当突发插入被检测到时此缓冲将被分配.
            # bulk_insert_buffer_size = 256M
            
            # 此缓冲当 MySQL 需要在 REPAIR, OPTIMIZE, ALTER 以及 LOAD DATA INFILE 到一个空表中引起重建索引时被分配.
            # 这在每个线程中被分配.所以在设置大值时需要小心.
            # myisam_sort_buffer_size = 256M
            
            # MySQL 重建索引时所允许的最大临时文件的大小 (当 REPAIR, ALTER TABLE 或者 LOAD DATA INFILE).
            # 如果文件大小比此值更大,索引会通过键值缓冲创建(更慢)
            # myisam_max_sort_file_size = 10G
            
            # 如果被用来更快的索引创建索引所使用临时文件大于制定的值,那就使用键值缓冲方法.
            # 这主要用来强制在大表中长字串键去使用慢速的键值缓冲方法来创建索引.
            # myisam_max_extra_sort_file_size = 10G
            
            # 如果一个表拥有超过一个索引, MyISAM 可以通过并行排序使用超过一个线程去修复他们.
            # 这对于拥有多个 CPU 以及大量内存情况的用户,是一个很好的选择.
            # myisam_repair_threads = 1
            
            # 自动检查和修复没有适当关闭的 MyISAM 表.
            # myisam_recover
            
            # 默认关闭 Federated
            # skip-federated
            
            # *** BDB 相关选项 ***
            
            # 如果你运行的MySQL服务有BDB支持但是你不准备使用的时候使用此选项. 这会节省内存并且可能加速一些事.
            #****(此属性对当前环境无效)****
            #skip-bdb
            
            # *** INNODB 相关选项 ***
            
            # 如果你的 MySQL 服务包含 InnoDB 支持但是并不打算使用的话,
            # 使用此选项会节省内存以及磁盘空间,并且加速某些部分
            #skip-innodb
            
            # 附加的内存池被 InnoDB 用来保存 metadata 信息(5.6中不再推荐使用)
            # 如果 InnoDB 为此目的需要更多的内存,它会开始从 OS 这里申请内存.
            # 由于这个操作在大多数现代操作系统上已经足够快, 你一般不需要修改此值.
            # SHOW INNODB STATUS 命令会显示当先使用的数量.
            #****(此属性对当前环境无效)****
            #innodb_additional_mem_pool_size = 64M
            
            # InnoDB使用一个缓冲池来保存索引和原始数据, 不像 MyISAM.
            # 这里你设置越大,这能保证你在大多数的读取操作时使用的是内存而不是硬盘,在存取表里面数据时所需要的磁盘 I/O 越少.
            # 在一个独立使用的数据库服务器上,你可以设置这个变量到服务器物理内存大小的80%
            # 不要设置过大,否则,由于物理内存的竞争可能导致操作系统的换页颠簸.
            # 注意在32位系统上你每个进程可能被限制在 2-3.5G 用户层面内存限制,
            # 所以不要设置的太高.
            innodb_buffer_pool_size = 6G
            
            # InnoDB 将数据保存在一个或者多个数据文件中成为表空间.
            # 如果你只有单个逻辑驱动保存你的数据,一个单个的自增文件就足够好了.
            # 其他情况下.每个设备一个文件一般都是个好的选择.
            # 你也可以配置 InnoDB 来使用裸盘分区 – 请参考手册来获取更多相关内容
            innodb_data_file_path = ibdata1:10M:autoextend
            
            # 设置此选项如果你希望InnoDB表空间文件被保存在其他分区.
            # 默认保存在MySQL的datadir中.
            #innodb_data_home_dir =
            
            # 用来同步IO操作的IO线程的数量.
            # 此值在Unix下被硬编码为8,但是在Windows磁盘I/O可能在一个大数值下表现的更好.
            #innodb_file_io_threads = 8
            
            # 如果你发现 InnoDB 表空间损坏, 设置此值为一个非零值可能帮助你导出你的表.
            # 从1开始并且增加此值知道你能够成功的导出表.
            #innodb_force_recovery=1
            
            # 在 InnoDb 核心内的允许线程数量.
            # 最优值依赖于应用程序,硬件以及操作系统的调度方式.
            # 过高的值可能导致线程的互斥颠簸.
            innodb_thread_concurrency = 16
            
            innodb_flush_log_at_trx_commit = 2
            #（说明：如果是游戏服务器，建议此值设置为2；如果是对数据安全要求极高的应用，建议设置为1；
            
            #设置为0性能最高，但如果发生故障，数据可能会有丢失的危险！设成0会更快一点，但安全方面比较差，
            #MySQL挂了会丢失事务的数据,每秒写入日志文件并且日志文件刷新到磁盘(而不是由事务提交驱动的，可能一秒中
            有大量的事务提交)
            
            #默认值1的意思是每一次事务提交或事务外的指令都需要把日志写入（flush）硬盘，这是很费时的。
            #特别是使用电池供电缓存（Battery backed up cache）时。,InnoDB 会在每次提交后刷新(fsync)事务日志到磁盘上,
            #这提供了完整的 ACID 行为.
            
            #设成2对于很多运用，特别是从MyISAM表转过来的是可以的，它的意思是不写入硬盘而是写入系统缓存。
            #日志仍然会每秒flush到硬盘，所以你一般不会丢失超过1-2秒的更新。
            #值2只会在整个操作系统挂了时才可能丢数据。推荐使用2
            
            # 加速 InnoDB 的关闭. 这会阻止 InnoDB 在关闭时做全清除以及插入缓冲合并.
            # 这可能极大增加关机时间, 但是取而代之的是 InnoDB 可能在下次启动时做这些操作.
            #innodb_fast_shutdown
            
            # 用来缓冲日志数据的缓冲区的大小.
            # 当此值快满时, InnoDB 将必须刷新数据到磁盘上.
            # 由于基本上每秒都会刷新一次,所以没有必要将此值设置的太大(甚至对于长事务而言)
            innodb_log_buffer_size = 16M
            
            # 在日志组中每个日志文件的大小.
            # 你应该设置日志文件总合大小到你缓冲池大小的25%~100%
            # 来避免在日志文件覆写上不必要的缓冲池刷新行为.
            # 不论如何, 请注意一个大的日志文件大小会增加恢复进程所需要的时间.
            innodb_log_file_size = 512M
            
            # 在日志组中的文件总数.
            # 通常来说2~3是比较好的.
            innodb_log_files_in_group = 3
            
            # InnoDB 的日志文件所在位置. 默认是 MySQL 的 datadir.
            # 你可以将其指定到一个独立的硬盘上或者一个RAID1卷上来提高其性能
            #innodb_log_group_home_dir
            
            # 在 InnoDB 缓冲池中最大允许的脏页面的比例.
            # 如果达到限额, InnoDB 会开始刷新他们防止他们妨碍到干净数据页面.
            # 这是一个软限制,不被保证绝对执行.
            innodb_max_dirty_pages_pct = 90
            
            # InnoDB 用来刷新日志的方法.
            # 表空间总是使用双重写入刷新方法
            # 默认值是 “fdatasync”, 另一个是 “O_DSYNC”.
            # 一般来说，如果你有硬件 RAID 控制器，并且其独立缓存采用 write-back 机制，并有着电池断电保护，那么应该设置配置为 O_DIRECT
            # 否则，大多数情况下应将其设为 fdatasync
            #innodb_flush_method=fdatasync
            
            # 在被回滚前,一个 InnoDB 的事务应该等待一个锁被批准多久.
            # InnoDB 在其拥有的锁表中自动检测事务死锁并且回滚事务.
            # 如果你使用 LOCK TABLES 指令, 或者在同样事务中使用除了 InnoDB 以外的其他事务安全的存储引擎
            # 那么一个死锁可能发生而 InnoDB 无法注意到.
            # 这种情况下这个 timeout 值对于解决这种问题就非常有帮助.
            innodb_lock_wait_timeout = 120
            
            # 这项设置告知InnoDB是否需要将所有表的数据和索引存放在共享表空间里（innodb_file_per_table = OFF） 或者为
            # 每张表的数据单独放在一个.ibd文件（innodb_file_per_table = ON）
            # 每张表一个文件允许你在drop、truncate或者rebuild表时回收磁盘空间
            # 这对于一些高级特性也是有必要的，比如数据压缩,但是它不会带来任何性能收益
            innodb_file_per_table = on
            
            [mysqldump]
            # 不要在将内存中的整个结果写入磁盘之前缓存. 在导出非常巨大的表时需要此项
            quick
            
            max_allowed_packet = 32M
            
            [mysql]
            no-auto-rehash
            default-character-set=utf8
            # 仅仅允许使用键值的 UPDATEs 和 DELETEs .
            safe-updates
            
            [myisamchk]
            key_buffer = 16M
            sort_buffer_size = 16M
            read_buffer = 8M
            write_buffer = 8M
            
            [mysqlhotcopy]
            interactive-timeout
            
            [mysqld_safe]
            # 增加每个进程的可打开文件数量.
            # 警告: 确认你已经将全系统限制设定的足够高!
            # 打开大量表需要将此值设大
            open-files-limit = 8192
            
            #
            # MySQL 服务端
            #
            [client]
            default-character-set=utf8

```

## MySQL服务器配置参数

```shell
    1.ubuntu系统下/etc/mysql/mysql.conf.d/mysqld.cnf
            
```

## MySQL服务器参数概要

```shell
    设置MySQL服务器参数有很多种方式，往往要注意设置的先后顺序
    (1) 命令行参数
            mysqld_safe --datadir=/data/sql_data
            
    (2) 配置文件(永久的修改配置参数)
            不同的linux读取的MySQL配置文件的顺序不一样，通过命令行进行确认
            > mysqld --help --verbose | grep -A 1 'Default options'
            结果：
                /etc/my.cnf /etc/mysql/my.cnf ~/.my.cnf 
                
    MySQL配置参数的作用域
        MySQL配置参数有些参数是全局的，有些是会话内的，有些是MySQL服务启动时只加载一次的，有些则是可以时刻进行动态修改的
        
        (1) 全局参数
                mysql> set global 参数名=参数值;
                mysql> set @@global.参数名 := 参数值;
                
                有些全局参数设置对所有的连接有效(对所有的MySQL客户端有效)，有些全局参数只能对之后新的连接有效.
                例如 
                    mysql> set global wait_timeout=3600;set global interactive_timeout=3600;(只有同时修改2个系统变量值才有效)
                    同时修改这2个参数对老的连接是没有用的,只有重新连接到MySQL服务器才起作用。
                
        (2) 会话参数
                mysql> set [session] 参数名=参数值;
                mysql> set @@session.参数名 := 参数值;
            
```

### 内存配置相关参数

```shell
    1.确定可以使用的内存的上限(针对MySQL整个服务而言)
    2.确定MySQL的每个连接使用的内存
            (1) sort_buffer_size:对于每一个连接中有使用排序查询时，都要分配固定大小sort_buffer_size内存，
                                要考虑到连接数*sort_buffer_size的大小不能超过系统内存,防止内存溢出
            (2) join_buffer_size:一个查询中关联了多张表，为每一个连接关联分配一个连接缓存，每个查询会有多个连接缓存，不能设置太大
            (3) read_buffer_size:对MyISAM表进行全表扫描时，分配读缓存读缓存池的大小，该大小一定要是4KB的倍数.
            (4) read_rnd_buffer_size:索引缓冲池的大小，MySQL在有查询需要时才分配该内存，并且只会分配实际需要的大小，而不是参数的大小。
            以上4个参数都是以线程为单位的,每一个线程在查询时都会分配对应大小的内存。
    在生产环境，一台物理机上最好只有一个MySQL服务实例，因为多个实例会造成内存竞争，对性能有影响。
    
    3.如何为缓冲池分配内存        
            innodb_buffer_pool_size：用于innodb存储引擎，保存数据和索引，对写也有帮助，先在缓冲池中保存，
                                    达到一定数量时再一起写入，减少IO磁盘。
            key_buffer_size:用于MyISAM存储引擎，用于缓存索引
                                   
```

### I/O相关的配置参数

```shell
    如何将缓存区中的数据同步到磁盘上，要在性能(数据的修改暂时保持在缓冲区中)和安全之间做出权衡。
        1.Innodb存储引擎 I/O相关的配置参数
                (1) innodb_log_file_size:控制了单个事务的大小，如果业务相对繁忙，则设置innodb_log_file_size要大一些。
                (2) innodb_log_files_in_group：控制事务文件的个数
                事务日志的总大小=innodb_log_file_size*innodb_log_files_in_group
                事务日志是循环使用的，写满一个innodb_log_file_size后，才会使用下一个。
                
                这2个参数决定了innodB事务日志，为了减少提交事务时的I/O开销，在事务提交时，先写入事务日志中，而不是每次修改数据刷新到
                数据文件中，这样做是为了提高I/O的性能，因为事务的修改使数据和索引文件都会映射到表空间的随机位置，所以刷新数据变更到文件
                会产生大量的随机I/O，而记录日志文件所需要的是顺序I/O，一旦事务日志安全写到磁盘中(即被提交的事务成功写到事务日志文件中)，
                事务就算是持久化，因为即使变更没有写入数据文件，即使这时候MySQL服务器发生宕机，
                我们还是可以使用事务日志来恢复已经提交的事务。
                
                (3) innodb_log_buffer_size : 事务日志缓存区，一般1秒钟就会将事务日志缓存区刷新到事务日志文件中，
                                             所以没有必要设置这么大。
                (4) innodb_flush_log_at_trx_commit 
                
                事务日志文件(磁盘中)真正有数据要经历事务->事务日志缓存区->系统Cache->事务日志文件(磁盘)
                (5) innodb_flush_method :如果是linux,一般设置为O_DIRECT
                (6) innodb_file_per_table = on
                (7) innodb_doublewrite = 1 控制innodb是否启用双写缓存，双写缓存主要是避免页没有写完导致数据损坏，innodb的一个页
                    默认大小是16KB，当程序崩溃导致一个磁盘的写操作不能完整得完成，不完整的页写入可能会发生
                    
        2. MyISAM 存储引擎 I/O相关的配置参数
                (1) delay_key_write :  
                                     off:每次写操作后刷新 键缓冲中的脏块到磁盘中,I/O性能最差
                                     on:只对在新建表时(指定delay_key_write)的表使用延时刷新
                                     all:对所有MyISAM表都使用延时 键写入
                                     
                     如果使用delay_key_write为on时系统崩溃，导致缓存中有的块没有写入到磁盘文件，这个会造成MyISAM表索引的损坏，这时
                     就得进行索引表的修复
                                  
```

### 安全相关的配置文件

```shell
        1.expire_logs_days : 指定自动清理binlog的天数
        2.max_allowed_packet:控制MySQL可以接收的包的大小，如果配置了主从复制,那么它们之间的参数最好保持一致。
        3.skip_name_resolve:禁用DNS查找，需要注意的是在进行授权时也只能指定特定的ip,不能是域名。
        4.read_only:禁止非super权限的用户写权限，这个主要是用在从库中，只接收来之主库的写操作。
        5.skip_slave_start : 禁用Slave自动恢复，也是用于主从复制，在服务器重启时禁止自动复制(可能重启一种情况是服务器崩溃导致数据异常)
        6. sql_mode:设置MySQL所使用的SQL模式，在默认情况下MySQL对SQL语句是比较宽松的，如果使用了最严的mode会导致MySQL查询失败
                     最好不要轻易改动默认的sql_mode
                                  
```

### 其他常用的配置参数

```shell
        1.sync_binlog 控制MySQL如何向磁盘刷新binlog(从系统cache到磁盘) 
                      0：MySQL不会主动去刷新到磁盘而是由操作系统自己决定的。
                      1：每次事务操作都会一次binlog写到磁盘(这样是最安全的，但对于I/O会有压力)，如果是主从复制，还是建议主数据库
                         sync_binlog设置为1,这样较为安全
                         
        2.tmp_table_size 和 max_heap_table_size:控制内存临时表大小，这2个变量大小要设置为一致也不能太大，
        3.max_connections: 控制允许的最大的连接数，因为有些内存分配是以 连接 为单位的,所以各个连接内存总和不能超过系统的内存。
                      
```

### 配置参数常见问题

```shell
        1.在 /etc/my.cnf 中设置了 max_connections = 3000,重启mysqld.service,但MySQL系统变量中还是没有该表
            解决方法:
                mysql有默认打开文件数限制。可以通过配置/usr/lib/systemd/system/mysqld.service来调大打开文件数目
                
                配置/usr/lib/systemd/system/mysqld.service
                
                [Service]新添加两行如下参数：
                        LimitNOFILE=10000
                        LimitNPROC=10000
                        
                > systemctl daemon-reload 
                > systemctl restart mysqld.service 
                      
```

### ubuntu配置文件(外部的mysql客户端(其他ip而不是本机ip)连接到mysql服务器)
```shell
    1.> vim /etc/mysql/mysql.conf.d/mysqld.cnf
             bind-address       = 0.0.0.0
    2.授权
        mysql> grant all PRIVILEGES on *.* to  root@'%'  identified by '密码' WITH GRANT OPTION;
        mysql>flush privileges ;
        
```