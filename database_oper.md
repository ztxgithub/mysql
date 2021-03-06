# 启动mysql数据库

```shell
    启动MySQL
        > sudo /etc/init.d/mysql restart
        
    ubuntu 重启mysqld
        > service mysql restart

    查看服务是否启动
        > ps aux | grep "mysql"
        > systemctl status mysqld.service 
        
    查看配置文件 my.cnf　位置
        > whereis my.cnf
```

- 设置初始化密码

```shell
    > mysql_secure_installation
    
    Enter current password for root (enter for none):
    # 解释：输入当前 root 用户密码，默认为空，直接回车。
    
    Set root password? [Y/n]  y
    # 解释：要设置 root 密码吗？输入 y 表示愿意。
    
    Remove anonymous users? [Y/n]  y
    # 解释：要移除掉匿名用户吗？输入 y 表示愿意。
    
    Disallow root login remotely? [Y/n]  y
    # 解释：不想让 root 远程登陆吗？
    
    Remove test database and access to it? [Y/n]  y
    # 解释：要去掉 test 数据库吗？
    
    Reload privilege tables now? [Y/n]  y
    # 解释：想要重新加载权限吗？输入 y 表示愿意。
```

- 修改密码

```shell
    方法一:
        用set password命令
            mysql> set password for 'root'@'localhost' = password('123456');
        
    方法二:
        用mysqladmin(mysqladmin -u用户名 -p旧密码 password 新密码)
        > mysqladmin -uroot -p123456 password 1234abcd
        
    方法三:
        用update直接编辑user表
            (1) mysql> use mysql
            (2) mysql> update user set PASSWORD = PASSWORD('1234abcd') where user = 'root';
            (3) mysql> flush privileges;
            
    方法四:
        不知道密码情况下
            (1) > mysqld_safe --skip-grant-tables &
            (2) > mysql -u root (这个时候不需要输入root密码)
            (3) mysql> use mysql
            (4) mysql> update user set PASSWORD = PASSWORD('1234abcd') where user = 'root';
            (5) mysql> flush privileges;
            
```

- 安装mysql命令行工具　mycli(https://github.com/dbcli/mycli)

```shell
    1.前提条件是先安装pip
        > yum -y install epel-release (安装扩展源EPEL,是由 Fedora 社区打造，
                                        为 RHEL 及衍生发行版如 CentOS、Scientific Linux 等提供高质量软件包的项目)
                                        
        > yum -y install python-pip
        
    2. > pip install mycli
```

# 连接数据库

```shell
   > mysql –u用户名 [–h主机名或者IP地址,-P端口号] –p密码  (–u、-h、-p后无空格)
   例如：
   >  mysql -h127.0.0.1 -P3306 -uroot -p123456
   
   数据库连接上后,你只需要在mysql>命令中输入 SQL 语句，同时并以分号“;”结束.最后按Enter键即可操作 MySQL软件.
   
```

- 开启MySQL的远程帐号

```shell

    第一步：    
        mysql> grant all PRIVILEGES on wabg.* to  root@'192.168.188.106'  identified by '123456' WITH GRANT OPTION;
                将 wabg 数据库的所有权限授权给 root 这个用户,允许 root 用户在 192.168.1.106 这个 IP 进行远程登陆,
                并设置 root 用户的密码为 123456 
                
                wabg.* 表示上面的权限是针对于哪个表的,wabg指的是数据库，后面的 * 表示对于所有的表，由此可以推理出：
                        对于全部数据库的全部表授权为“ . ”
                        对于某一数据库的全部表授权为“ 数据库名.* ”
                        对于某一数据库的某一表授权为“数据库名.表名”
                root 表示你要给哪个用户授权，这个用户可以是存在的用户，也可以是不存在的用户.
                192.168.188.106 表示允许远程连接的 IP 地址，如果想不限制链接的 IP 则设置为“%”即可。
                123456 为用户的密码．
                
        mysql>  grant all PRIVILEGES on *.* to  'myuser'@'%'  identified by 'mypassword' WITH GRANT OPTION;
                    允许myuser用户在任何ip下的主机访问任何数据库
                    
    第二步:
        mysql> flush privileges;
```

- 查看数据库

```shell
   mysql> show databases;
    
   结果：
        +--------------------+
        | Database           |
        +--------------------+
        | information_schema |
        | mysql              |
        | performance_schema |
        +--------------------+
        
        mysql是必须的，它主要描述用户访问权限
   
```

- 选择数据库

```shell
   mysql> use test;

    选择test数据库
     
```

- 创建数据库

```shell

    1.创建一个数据库,这个数据库一定不存在
        mysql> create database test;

    2.更加全面的方法
        mysql> create database if not exists test;
        
    3.mysql > create {database|SCHEMA} [if not exists] 数据库名 charcter set = 编码字符集(utf8);
     
```

- 删除数据库

```shell

    1.删除一个数据库,这个数据库一定存在
        mysql> drop database test;

    2.更加全面的方法
        mysql>  drop database if exists test;
     
```

- 设置存储引擎

```shell

    在my.cnf 配置文件中设置你需要的存储引擎,这个参数放在 [mysqld] 这个字段下面的 default_storage_engine 参数值,
    例如下面配置的片段

        [mysqld]
            default_storage_engine=CSV
            
    在创建表的时候，对表设置存储引擎，例如：
        
        CREATE TABLE `user` (
          `id`     int(100) unsigned NOT NULL AUTO_INCREMENT,
          `name`   varchar(32) NOT NULL DEFAULT '' COMMENT '姓名',
          `mobile` varchar(20) NOT NULL DEFAULT '' COMMENT '手机',
          PRIMARY KEY (`id`)
        )ENGINE=InnoDB;
        
        AUTO_INCREMENT:代表自增列属性，这种自增属性列都要求是索引列
        KEY：索引是 id 列
      
     
```

- 设置支持Federated存储引擎

```shell

    1.在my.cnf 配置文件中新加
        federatred=1
    2.重启mysql服务器
        
      
     
```

- 常见的命令

```shell

    *.查看某个表的信息
        mysql> desc 表名;
        
    *.查看查询计划
        mysql> explain select 列名 from 表名 where 条件；
        
        实例：
            mysql> explain select language_id from film where language_id=1\G
        结果:
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
            1 row in set (0.00 sec)
            
        用explain extended语句后可以通过show warnings查看一条SQL语句的反编译的结果,
        让我们知道我们输入的一条SQL语句真正是怎么执行的.
        参数:
        
            select_type：表示select类型,常见的取值有SIMPLE（不使用表连接或子查询）,PRIMARY（主查询，即外层的查询）,
                         UNION（UNION中的或者后面的查询语句）,SUBQUERY（子查询中的第一个select）等。
        
            table：输出结果集的表   
            type：表示表的连接类型，性能由好到差的连接类型为system（表中仅有一行，即常量表）,
                                 const(单表中最多有一个匹配行,例如PRIMARY KEY或者UNIQUE INDEX）、
                                 eq_ref（对于前面的每一行，在此表中只查询一条记录，简单来说，
                                         就是多表连接中使用PRIMARYKEY或者UNIQUE INDEX）、
                                 ref（与eq_ref类似，区别在于不使用PRIMARYKEY或者UNIQUE INDEX，而是使用普通的索引）
                                 ref_of_null（与ref类似，区别在于条件中包含对NULL的查询）、
                                 index_merge（索引合并化）、
                                 unique_subquery（in的后面是一个查询主键字段的子查询）、
                                 index_subquery（与unique_subquery类似，区别在于in的后面是查询非唯一索引字段的子查询）
                                 range（单表中的范围查询）
                                 index（对于前面的每一行都通过查询索引来得到数据）
                                 all（对于前面的每一行的都通过全表扫描来获得数据）
        
            possible_keys：表示查询时，可能使用到的索引。
            key：表示实际使用的索引
            key_len：索引字段的长度
            rows：扫描行的数量
            extra：执行情况的说明和描述。
                
    *.查看表的定义
        mysql> show create table 表名;
        
    *.查看innodb状态检查
        mysql> show engine innodb status
        
    *.创建表
        (1)创建一张以存储引擎为csv的表
            mysql> create table my_csv(id int not null,c1 varchar(10) not null,c2 char(10) not null) engine=csv;
            
        (2)创建一张以存储引擎为federated的表
            mysql> create table local_fed(id int, c1 varchar(10)) engine=federated connnection=
                    'mysql://用户名:密码@远程服务器ip:远程服务器端口号/远程服务器数据库名/远程服务器表名'；
                    
        (3) 创建数据表的同时将查询结果写入到新创建数据表
                mysql> create table if not exists 表名(create_definetion) select_statement
                例如:
                mysql> create table tall_tb( id SMALLINT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
                                             hight INT) select 身高 AS hight FROM persion GROUP BY 身高;
                                             
        (4) 表名不能为"std"
                                             
    *.删除表
        mysql> drop table 表名 ;
    *.插入数据
        (1) mysql> insert into 表名 values(xxx,xxx),(yyy,yyy);  //支持插入多条记录
        (2) mysql> insert into 表名(列名1，列名2) values(xxxx,xxx);  
        (3) insert如果为自动编号的字段(比如主键id AUTO_INCREMENT)赋值的话,可对该列写为NULL
            如果字段设置了DEFAULT(例如age TINYINT UNSIGNED NOT NULL DEFAULT 10,),那么值直接写DEFAULT就行,
            注意此时，不能为NULL。例如：
            mysql> insert into users values(NULL,'TOM','123',DEFAULT,1);
            
    *.更新数据
        (1) mysql> update 表名 set 列名=值 where 筛选条件;

    *.删除数据
        mysql> delete from 表名 where 条件
        例如:
            mysql> delete from t1 where t1id = 1 and t1str = 4;
                    
    *.修改表结构
            1. 修改列的属性(数据类型)
                mysql> alter table 表名 modify 列名 数据类型
                例如：
                mysql> alter table t_time modify data_tm datetime(6),modify timestamp_tm timestamp(6); (修改时间精确到毫秒)
                
            2. 增加列
                mysql> alter table 表名 add 列名 数据类型 [first|after col_name]
                    (1) mysql> alter table actor add age int ; 默认会增加到表的最后一列
                    (2) mysql> alter table actor add age int first ; 增加到表的第一列
                    
                    (3) mysql> alter table actor add age int after first_name ; 
                           将新增列(age)插入到first_name列名的后面
                    
            3.删除列
                mysql> alter table 表名 drop 列名1,drop 列名2;
                
            4.修改列名
                mysql> alter table 表名 change 原来的列名 新的列名 数据类型 [first|after col_name];
                例如:
                    mysql> alter table actor change first_name fst_name varchar(45) after last_name;
                    将原来first_name列名改为fst_name,并在last_name列后面
            
    *.对数据库表进行重命名
            1.mysql> alter table 原来的表名 rename {TO|AS} 新的表名;  to和as都可以
            2.mysql> rename table 原来的表名 to 新的表名; 
        
    *.flush 命令 (mysql> flush flush_option ;)
        (1) mysql> flush HOSTS;
               主要是用来清空主机缓存表。如果你的某些主机改变IP地址，或如果你得到错误消息Host ... isblocked，你应该清空主机表。
               当在连接MySQL服务器时，对一台mysql客户端主机有 max_connect_errors 个错误连续不断地发生，
               MySQL为了安全的需要将会阻止该主机进一步的连接请求。清空主机表允许主机再尝试连接
               
        (2) mysql> flush PRIVILEGES;
                每当重新赋权后，为了以防万一，让新权限立即生效，从数据库授权表中重新装载权限到缓存中
                
        (3) mysql> flush tables;
                关闭所有打开的表，同时该操作将会清空查询缓存中的内容,刷新缓存重新将表中的数据加载到缓存中。
                
    *.索引
        mysql> create index 索引名 on 表名(列名); 使用了默认索引类型
        mysql> create index 索引名 using btree on 表名(列名); 使用了BTree索引类型
        mysql> show index from 表名；
        mysql> drop index 索引名 on 表名；
        
        如果该列的数据类型为char或则varchar,length可以小于字段实际长度；
        如果是BLOB和TEXT类型，必须指定 length
        mysql> create index 索引名 on 表名(列名(length));
        
    *.查看mysql系统变量
        mysql> show variables like 'innodb_file_per_table';
        mysql> show variables where variable_name='wait_timeout' or variable_name='interactive_timeout';
        
    *.查看数据库中连接的状态情况
            mysql> show processlist;
            
    *.数据库中所有全局变量的值
        mysql> show global status;
        
    *.设置时区
        mysql> set time_zone='+10:00'    
        
    *.查看表的状态信息
        mysql> show table status like '表名'；
        
    *.设置binlog的存储格式为 段格式(statement)
        mysql> set session binlog_format=statement;
    *.查看binlog日志大小
        mysql> show binary logs;
    *.刷新日志大小
        mysql> flush logs;
        
    *.设置MySQL系统变量
        mysql> set global wait_timeout=3600;set global interactive_timeout=3600;
        
    *.查看表的记录数
        mysql> select count(*) from 表名;
       
    *.建立数据库用户
        实际上可以省略用户名和主机名处的引号，但如果使用了通配符则引号不能省略
        mysql> create user ‘用户名’ @ ‘允许使用的ip网段' identified by '密码'
        例如
           mysql> create user ‘jame’ @ ‘192.168.3.%' identified by '123456';
           
    *.查看数据库用户
        (1) 切换到mysql数据库
                mysql> use mysql;
        (2)
            mysql> select host,user,password from user;
            结果:
                mysql> select host,user,password from user;
                +-----------+------+----------+
                | host      | user | password |
                +-----------+------+----------+
                | localhost | root |          |
                | yytdtest  | root |          |
                | 127.0.0.1 | root |          |
                | ::1       | root |          |
                | localhost |      |          |
                | yytdtest  |      |          |
                +-----------+------+----------+
                
    *.删除数据库用户      
        (1) mysql> drop user 'root'@'localhost';
        (2) mysql> drop user;
        
    *.对用户账号进行重命名
        mysql> rename user 'jack'@'%' to 'jim'@'%';
        
    *.查看数据库中用户信息
        I. mysql> use mysql;
        II. mysql> select user,host from user;
        
    *.查看授权信息
        mysql> show grants for '用户名'@'host'
        
        例如：
            mysql> show grants for 'jame'@'192.168.0.%';
            
    *.查看主数据库的状态
        mysql> show master status \G;
      
    *.在从数据库上查看主数据库的信息
        mysql> show slave status \G;
        
    *.在主数据库上查看从数据库的信息
        mysql> show slave hosts;
        
    *.事务操作
        mysql> begin;
        mysql>commit;
        mysql>rollback;
     
```

- 在shell终端的命令

```shell

    1.显示数据库global变量的值
        > mysql -e "show global status" -uroot -p52346898;
        
    2.查看binlog日志
        > mysqlbinlog 日志文件
        
        使查看的信息利于看懂
        >  mysqlbinlog -vv 日志文件  
        
    3.进行数据库备份
         > mysqldump --master-data -single-transaction --triggers --routines --all-databases -uroot
                                       -p >> all.mysql
            参数:
                -single-transaction : 保证事务的一致性
                --triggers ：备份触发器
                --all-databases：备份所有数据库(包括系统数据库)，这就要求还原的MySQL版本与备份版本要一致
     
```

## 信息函数

```shell
    1.VERSION():返回数据库的版本号
        mysql> select version();
        结果:
            +-----------+
            | version() |
            +-----------+
            | 5.6.40    |
            +-----------+
            
    2.connection_id() 连接id :当前连接的ID
          mysql> select connection_id();
          结果:
              +-----------------+
              | connection_id() |
              +-----------------+
              |               6 |
              +-----------------+
              
    3.database() 当前数据库
        mysql> select database();
        结果:
            +------------+
            | database() |
            +------------+
            | sakila     |
            +------------+

    4.user() 当前用户
        mysql> select user();
        结果:用户名@登录主机
            +----------------+
            | user()         |
            +----------------+
            | root@localhost |
            +----------------+
            
    5.last_insert_id() 返回最近生成的AUTO_INCREMENT值
       注意: 插入多条记录时，只会显示插入第一条的id而不是最后一条ID
       mysql> select last_insert_id();
       结果:
           +------------------+
           | last_insert_id() |
           +------------------+
           |              202 |
           +------------------+

```

## 加密函数

```shell
    1.Password(str)加密(加密函数是不可逆)
            经常用于对用户注册的密码进行加密处理,mysql用户需要设置密码,用户不能将未加密的密码直接存储在mysql数据库的user表中去.
            因为登陆mysql数据库时，数据库系统会将你输入的密码先通过Password（str）函数进行加密，然后再与数据库中的密码进行比较,
            匹配成功才可以登录
        mysql> select password("abcd");
        结果:
              +-------------------------------------------+
              | password("abcd")                          |
              +-------------------------------------------+
              | *A154C52565E9E7F94BFC08A1FE702624ED8EFFDA |
              +-------------------------------------------+
              
    2.MD5(str)加密函数(加密函数是不可逆)
            MD5函数是将一个任意长度的字符串变换成一个一定长的十六进制数字串
            
        mysql> select md5("abcd");
        结果
        +----------------------------------+
        | md5("abcd")                      |
        +----------------------------------+
        | e2fc714c4727ee9395f324cd2e7f331f |
        +----------------------------------+
        
        例如:
            mysql> insert int tb_user(u_name,u_pass) values('test', md5('123456'));
        
    3.encode("被加密的字符串","秘钥")函数(可逆的)
        加密:
            mysql> select encode("test","key");
            结果:
            +----------------------+
            | encode("test","key") |
            +----------------------+
            |                   |
            +----------------------+
            
        解密:
            mysql> select decode(encode("test","key"),"key");
            结果:
            +------------------------------------+
            | decode(encode("test","key"),"key") |
            +------------------------------------+
            | test                               |
            +------------------------------------+
        例如:
           mysql> insert into pass_info(id,pass_info) values(2,ENCODE("wujinfan","king"))

```

## 字符串函数

```shell
    1.lower(column|str)：将字符串参数值 全部转换为 小写字母后返回
      upper(column|str)：将字符串参数值 全部转换为 大写字母后返回
      
    2.字符串连接
        (A)concat(column|str1, column|str2,...)：将多个字符串参数首尾相连后返回
            (1) 如果有任何参数为null,则函数返回null
            (2) 如果参数是数字，则自动转换为字符串
                    mysql> select concat(14.3,'mysql');
                    结果:
                        +----------------------+
                        | concat(14.3,'mysql') |
                        +----------------------+
                        | 14.3mysql            |
                        +----------------------+
        (B) concat_ws(separator,str1,str2,...)：将多个字符串参数以给定的分隔符separator首尾相连后返回
                mysql> select concat_ws(';','First name','Second name','Last name');
                结果:
                    +-------------------------------------------------------+
                    | concat_ws(';','First name','Second name','Last name') |
                    +-------------------------------------------------------+
                    | First name;Second name;Last name                      |
                    +-------------------------------------------------------+
                    
                (1) 如果有任何参数为null，则函数不返回null，而是直接忽略它
                        mysql> select concat_ws(',','id',null,'name');
                        结果:
                            +---------------------------------+
                            | concat_ws(',','id',null,'name') |
                            +---------------------------------+
                            | id,name                         |
                            +---------------------------------+
                            
    3.substr(str,pos[,len])：从源字符串str中的指定位置pos开始取一个字串并返回
        mysql> select substring('hello world',5);
        +----------------------------+
        | substring('hello world',5) |
        +----------------------------+
        | o world                    |
        +----------------------------+
        
        mysql> select substr('hello world',5,3);
        +---------------------------+
        | substr('hello world',5,3) |
        +---------------------------+
        | o w                       |
        +---------------------------+
        
        mysql> select substr('hello world',-5);
        +--------------------------+
        | substr('hello world',-5) |
        +--------------------------+
        | world                    |
        +--------------------------+
        
    4.length(str)：返回字符串的存储长度(字节数)
        mysql> select length('text'),length('你好');
        +----------------+------------------+
        | length('text') | length('你好')   |
        +----------------+------------------+
        |              4 |                6 |
        +----------------+------------------+
        
    5.char_length(str)：返回字符串中的字符个数
        mysql> select char_length('text'),char_length('你好');
        结果:
            +---------------------+-----------------------+
            | char_length('text') | char_length('你好')   |
            +---------------------+-----------------------+
            |                   4 |                     2 |
            +---------------------+-----------------------+
            
    6.lpad(str, len, padstr)：将padstr+str进行拼接
                              如果 padstr+str 长度 超过 len,则还是 padstr+str
                              如果 padstr+str 长度 小于 len,则一直用padstr填充,直到padstr+padstr+str等于len
                              
          mysql> select lpad('h',7,'?!!!');
          +--------------------+
          | lpad('h',7,'?!!!') |
          +--------------------+
          | ?!!!?!h            |
          +--------------------+
          
      rpad(str, len, padstr)：将str+padstr进行拼接
                              如果 str+padstr 长度 超过 len,则还是 str+padstr
                              如果 str+padstr 长度 小于 len,则一直用padstr填充,直到str+padstr+padstr等于len
                              
    7.trim([{BOTH | LEADING | TRAILING} [remstr] FROM] str)
        (1)从源字符串str中去掉两端、前缀或后缀字符remstr并返回；
        (2)如果不指定remstr，则去掉str两端的空格；
        (3)不指定BOTH,LEADING,TRAILING ，则默认为 BOTH。
        
        mysql> select trim(leading 'x' from 'xxxbarxxx');
        +------------------------------------+
        | trim(leading 'x' from 'xxxbarxxx') |
        +------------------------------------+
        | barxxx                             |
        +------------------------------------+
        
    8.replace(str, from_str, to_str)：在源字符串str中查找所有的子串form_str（大小写敏感）,
                                      找到后使用替代字符串to_str替换它
                                      
       mysql> select replace('www.mysql.com','w','Ww');
       结果:
           +-----------------------------------+
           | replace('www.mysql.com','w','Ww') |
           +-----------------------------------+
           | WwWwWw.mysql.com                  |
           +-----------------------------------+
           
      (1) ltrim(str),rtrim(str)：去掉字符串的左边或右边的空格(左对齐、右对齐)
           
    9.repeat(str, count)：将字符串str重复count次后返回
        mysql> select repeat('MySQL',3);
        +-------------------+
        | repeat('MySQL',3) |
        +-------------------+
        | MySQLMySQLMySQL   |
        +-------------------+
        
    10.reverse(str)：将字符串str反转后返回
        mysql> select reverse('abcdef');
        +-------------------+
        | reverse('abcdef') |
        +-------------------+
        | fedcba            |
        +-------------------+
        
    11.
        (1) left(str, len)：返回最左边的len长度的子串
        (2) right(str, len)：返回最右边的len长度的子串
        
```

## 数值运算符与函数

```shell
    1.取整函数
        A.
            (1) ROUND(X) : 将值 X 四舍五入为整数，无小数位
                    mysql> select ROUND('123.456')
                    结果:
                        123
            (2)  ROUND(X,D) :保留D位小数点,采用四舍五入方法
                    mysql> select ROUND('123.654',2)
                    结果:
                        123.65
                        
        B.FLOOR(X)表示向下取整,只返回值X的整数部分,小数部分舍弃
            mysql>select FLOOR('123.654')
            结果:
                123
                
        C.CEILING(X) 表示向上取整,只返回值X的整数部分(整数部分加一),小数部分舍弃
        
    2.truncate(x,y)返回数值x保留到小数点后y位的值（与ROUND最大的区别是不会进行四舍五入）
        mysql> select truncate(3.14,1);
        结果:
            +------------------+
            | truncate(3.14,1) |
            +------------------+
            |              3.1 |
            +------------------+
```

## 比较运算符与函数

```shell
    1.[NOT] BETWEEN ... AND ...  :Check whether a value is within a range of values 
      expr between min and max:
        如果expr在min~max之间,返回1,否则返回0.
        mysql> select 1 between 1 and 3; 
        结果:
            +-------------------+
            | 1 between 1 and 3 |
            +-------------------+
            |                 1 |
            +-------------------+
            
    2.coalesce(value1,...) ：返回第一个非NULL的参数
         说明：返回列表中第一个非空值，如果没有非NULL值，则返回NULL
         
         mysql> select coalesce(NULL,1);
         结果:
             +------------------+
             | coalesce(NULL,1) |
             +------------------+
             |                1 |
             +------------------+
             
    3.greatest(value1,value2,...)
        mysql> select greatest(1,2,3,'a');
        结果:
            +---------------------+
            | greatest(1,2,3,'a') |
            +---------------------+
            | 3                   |
            +---------------------+
            
    4.
       (1) [NOT] IN()	[不]在列出的值范围内
            mysql> select 2 in (1,2,'a');
            结果:
                +----------------+
                | 2 in (1,2,'a') |
                +----------------+
                |              1 |
                +----------------+
                
      (2) IS [NOT] NULL	[不]为空
            mysql> select 1 IS NULL, 0 IS NULL, NULL IS NULL;
            结果:
                 0, 0, 1            

```

## 日期时间函数

```shell
    1.
        (1)获得当前(日期date+时间time) : now()
            mysql> select now();
            结果:
                +---------------------+
                | now()               |
                +---------------------+
                | 2018-05-08 09:38:52 |
                +---------------------+
                
        (2)now()在执行开始时值就得到了(运行sql那一刻), sysdate() 在函数执行时动态得到值
            mysql> select now(),sleep(3),now();
            结果:
                +---------------------+----------+---------------------+
                | now()               | sleep(3) | now()               |
                +---------------------+----------+---------------------+
                | 2018-05-08 09:41:45 |        0 | 2018-05-08 09:41:45 |
                +---------------------+----------+---------------------+
                
        (3) Unix时间戳(timestamp)和日期(date)之间的转化
                A.日期(date)转为时间戳(timestamp) : unix_timestamp(date)
                    mysql> select unix_timestamp('2008-08-08 12:30:00'); 
                    结果:
                        +---------------------------------------+
                        | unix_timestamp('2008-08-08 12:30:00') |
                        +---------------------------------------+
                        |                            1218169800 |
                        +---------------------------------------+
                        
                B.时间戳(timestamp)转为日期(date): from_unixtime(unix_timestamp,format)
                    mysql> select from_unixtime(1218169800, '%Y_%m_%d_%H:%i:%s');
                    结果:
                        +------------------------------------------------+
                        | from_unixtime(1218169800, '%Y_%m_%d_%H:%i:%s') |
                        +------------------------------------------------+
                        | 2008_08_08_12:30:00                            |
                        +------------------------------------------------+
                        
         (4) 时间戳(timestamp)转换、增、减函数
                A.时间戳(timestamp)增加
                    timestamp(dt,time) -- dt + time
                    mysql> select timestamp('2008-08-08 08:00:00', '10 01:01:01'); 
                    结果:
                        +-------------------------------------------------+
                        | timestamp('2008-08-08 08:00:00', '10 01:01:01') |
                        +-------------------------------------------------+
                        | 2008-08-18 09:01:01                             |
                        +-------------------------------------------------+
                        
                B.时间戳(timestamp)增加
                  timestampadd(unit,interval,datetime_expr)
                  mysql> select timestampadd(day, 1, '2008-08-08 08:00:00');
                  结果:
                      +---------------------------------------------+
                      | timestampadd(day, 1, '2008-08-08 08:00:00') |
                      +---------------------------------------------+
                      | 2008-08-09 08:00:00                         |
                      +---------------------------------------------+
                      
                C.时间戳(timestamp)相减函数
                    timestampdiff(unit,datetime_expr1,datetime_expr2) : datetime_expr2 - datetime_expr1
                    
                    mysql> select timestampdiff(year,'2002-05-01','2008-08-01'); 
                    结果:
                        +-----------------------------------------------+
                        | timestampdiff(year,'2002-05-01','2008-08-01') |
                        +-----------------------------------------------+
                        |                                             6 |
                        +-----------------------------------------------+
                        
                    mysql> select timestampdiff(year,'2002-05-01','2008-01-01'); 
                    结果:
                        +-----------------------------------------------+
                        | timestampdiff(year,'2002-05-01','2008-01-01') |
                        +-----------------------------------------------+
                        |                                             5 |
                        +-----------------------------------------------+
                    
            
    2.获得当前时间戳函数 current_timestamp()
        mysql> select current_timestamp();
        结果:
            +---------------------+
            | current_timestamp() |
            +---------------------+
            | 2018-05-08 09:43:39 |    只是显示
            +---------------------+
            
    3.获得当前日期(date)函数：curdate()
        mysql> select curdate();
        结果:
            +------------+
            | curdate()  |
            +------------+
            | 2018-05-08 |
            +------------+
            
    4.获得当前时间（time）函数：curtime()
        mysql> select curtime();
        结果:
            +-----------+
            | curtime() |
            +-----------+
            | 09:50:53  |
            +-----------+
            
    5.获得当前UTC日期时间函数：utc_date(), utc_time(), utc_timestamp()
        mysql> select utc_date(),utc_time(),utc_timestamp();
        结果:
            +------------+------------+---------------------+
            | utc_date() | utc_time() | utc_timestamp()     |
            +------------+------------+---------------------+
            | 2018-05-08 | 02:49:06   | 2018-05-08 02:49:06 |
            +------------+------------+---------------------+
            
    6.返回月份中的最后一天:last_day() 函数
            (1) mysql> select last_day(now());
                结果:
                    +-----------------+
                    | last_day(now()) |
                    +-----------------+
                    | 2018-05-31      |
                    +-----------------+
                    
            (2) 当前月份中有多少天
                    mysql> select day(last_day(now()));
                    结果:
                        +----------------------+
                        | day(last_day(now())) |
                        +----------------------+
                        |                   31 |
                        +----------------------+
                        
    7. 为日期增加一个时间间隔：date_add()
            set @dt = now();
            
            mysql> select date_add(@dt, interval 1 day); -- add 1 day
            mysql> select date_add(@dt, interval 1 hour); -- add 1 hour
            mysql> select date_add(@dt, interval 1 minute); -- ...
            mysql> select date_add(@dt, interval 1 second);
            mysql> select date_add(@dt, interval 1 microsecond);
            mysql> select date_add(@dt, interval 1 week);
            mysql> select date_add(@dt, interval 1 month);
            mysql> select date_add(@dt, interval 1 quarter);
            mysql> select date_add(@dt, interval 1 year);
            
            mysql> select date_add(@dt, interval '1 01:15:30' day_second);
            结果:
                +-------------------------------------------------+
                | date_add(@dt, interval '1 01:15:30' day_second) |
                +-------------------------------------------------+
                | 2008-08-10 13:28:03 |
                +-------------------------------------------------+
                
    8. 为日期减去一个时间间隔：date_sub()
            mysql> select date_sub('1998-01-01 00:00:00', interval '1 1:1:1' day_second);
            结果:
                +----------------------------------------------------------------+
                | date_sub('1998-01-01 00:00:00', interval '1 1:1:1' day_second) |
                +----------------------------------------------------------------+
                | 1997-12-30 22:58:59 |
                +----------------------------------------------------------------+
                
    9.日期、时间相减函数：datediff(date1,date2), timediff(time1,time2)
            (1) datediff(date1,date2):两个日期相减 date1 - date2,返回天数
                    mysql> select datediff('2008-08-08', '2008-08-01');
                    结果:
                        +--------------------------------------+
                        | datediff('2008-08-08', '2008-08-01') |
                        +--------------------------------------+
                        |                                    7 |
                        +--------------------------------------+
                        
            (2)  timediff(time1,time2):两个日期相减 time1 - time2,返回time差值,两个参数类型必须相同
            
    10.时间time和秒second之间的转换:time_to_sec(time), sec_to_time(seconds)
            (1)mysql> select time_to_sec('01:00:05');
               结果:
                    +-------------------------+
                    | time_to_sec('01:00:05') |
                    +-------------------------+
                    |                    3605 |
                    +-------------------------+
            (2) 
                mysql> select sec_to_time(3605); 
                结果:
                    +-------------------+
                    | sec_to_time(3605) |
                    +-------------------+
                    | 01:00:05          |
                    +-------------------+
                    
    11.日期date和天数day之间的转换：to_days(date), from_days(days)
            (1) mysql> select to_days('2008-08-08');
                结果:
                    +-----------------------+
                    | to_days('2008-08-08') |
                    +-----------------------+
                    |                733627 |
                    +-----------------------+
                    
            (2) mysql> select from_days(733627); 
                结果:
                    +-------------------+
                    | from_days(733627) |
                    +-------------------+
                    | 2008-08-08        |
                    +-------------------+
                    
    12.将字符串转换为 日期：str_to_date(str, format)
            (1) mysql> select str_to_date('08/09/2008', '%m/%d/%Y');
                结果:
                    +---------------------------------------+
                    | str_to_date('08/09/2008', '%m/%d/%Y') |
                    +---------------------------------------+
                    | 2008-08-09                            |
                    +---------------------------------------+
                
            (2) mysql> select str_to_date('08.09.2008', '%m.%d.%Y'); 
                结果:
                    +---------------------------------------+
                    | str_to_date('08.09.2008', '%m.%d.%Y') |
                    +---------------------------------------+
                    | 2008-08-09                            |
                    +---------------------------------------+
                    
            (3) mysql> select str_to_date('08:09:30', '%h:%i:%s'); 
                结果:
                    +-------------------------------------+
                    | str_to_date('08:09:30', '%h:%i:%s') |
                    +-------------------------------------+
                    | 08:09:30                            |
                    +-------------------------------------+
                    
    13. 将日期date/时间time转换为字符串 date_format(date,format), time_format(time,format)
            (1) mysql> select date_format('2008-08-08 22:23:01', '%Y_%m_%d_%H:%i:%s');
                结果:
                    +---------------------------------------------------------+
                    | date_format('2008-08-08 22:23:01', '%Y_%m_%d_%H:%i:%s') |
                    +---------------------------------------------------------+
                    | 2008_08_08_22:23:01                                     |
                    +---------------------------------------------------------+
                    
            (2) mysql>  select time_format('22:23:01', '%H.%i.%s');   
                结果:
                    +-------------------------------------+
                    | time_format('22:23:01', '%H.%i.%s') |
                    +-------------------------------------+
                    | 22.23.01                            |
                    +-------------------------------------+
        
```

## 聚合函数

```shell

    1.概要:
        (1).聚合函数(aggregation function)---也就是组函数
        (2).默认情况下，组函数忽略列值为null的行，不参与计算
        (3).使用关键字distinct剔除字段值重复的条数
        (4).组函数不允许嵌套,例如：count(max(…))；
        (5).组函数的参数可以是列或是函数表达式；
        
    2.相关函数
        (1) count函数
                A.count(*)：返回表中满足where条件的行的数量(没有条件，默认统计表数据行数)
                    mysql> select count(*) from salary_tab;
                    
                B.count(列名)：返回 列值非空 的行的数量
                    mysql> select count(salary) from salary_tab;
                    结果:
                        +---------------+
                        | count(salary) |
                        +---------------+
                        |             4 |
                        +---------------+
                C.count(distinct 列名)：返回列值非空的、并且列值不重复的行的数量
                
        (2) max和min函数---统计列中的最大最小值
        (3) sum和avg函数---求和 与 求平均 (表中列值为null的行不参与计算)
```

## SELECT

```shell
    1.分组SELECT的基本格式：
      
      　　select [聚合函数] 字段名 from 表名
            　　　　[where 查询条件]
            　　　　[group by 字段名]
            　　　　[having 过滤条件]

         例如:
            SELECT select_expr [, select_expr ...]
            
                [FROM table_references]
            
                [PARTITION partition_list]
            
                [WHERE where_condition]
            
                [GROUP BY {col_name | expr | position} [ASC | DESC], ... [WITH ROLLUP]]  (ASC为升序,默认)
            
                [HAVING where_condition]
            
                [ORDER BY {col_name | expr | position} [ASC | DESC], ...]
            
                [LIMIT {[offset,] row_count | row_count OFFSET offset}]
                
    2.group by子句
        (1).group by按照xxx进行分组，它必须有“聚合函数”来配合才能使用，使用时至少需要一个分组标识字段
        (2) 使用group by目的就是要将数据分组进行汇总操作
        
        (3) 以“dept_id(部门编号)”为分类标志统计各单位的职工人数和工资总额
            mysql> select dept_id(部门编号),count(emp_id雇员编号),sum(salary工资) form employee  
                   group by dept_id(部门编号);  
                   
        (4) 
            A. mysql> select * from t1;
                结果:
                    +------+-------+
                    | t1id | t1str |
                    +------+-------+
                    |    1 | 1     |
                    |    2 | 2     |
                    |    3 | 3     |
                    |    4 | 4     |
                    |    5 | 1     |
                    |    6 | 1     |
                    +------+-------+
                    
            B. mysql> select * from t1 group by t1str;
                结果:
                   +------+-------+
                   | t1id | t1str |
                   +------+-------+
                   |    1 | 1     |
                   |    2 | 2     |
                   |    3 | 3     |
                   |    4 | 4     |
                   +------+-------+
                   
    3.having 子句
        (1) 为每一个组指定条件,像where指定条件一样,也就是说,可以根据你指定的条件来选择行.
            如果你要使用HAVING子句的话,它必须处在GROUP BY子句之后
            
        (2) 以“dept_id”为分类标志统计各单位的职工人数和工资平均数且工资平均数大于4000
            mysql> select dept_id(部门编号), avg(salary) FROM employee GROUP BY dept_id 
                                                                      HAVING avg(salary) >= 4000;     
                                                                      
   
    4.select distinct 列名 from 表名; 获取不重复的列的数据
            mysql> select CEILING('123.456')
            结果:
                124
                
    5..order by : 默认是从小到大(升序)
            mysql > select * from 表名 order by 列名 desc limit 1; 从大到小(降序)限制一条记录
            
            A.可以排序多个字段，如果第一个字段能排出结果，就不会用到第2个字段。比如：
                mysql> select * from users order by age ,id DESC;
                因为年龄age有相同的,继续排序就要用到id的降序。
                
    6.limit
        起始地址是从0开始,没写默认为0
        (1) mysql> select * from users limit 起始地址,输出数据量;
               SELECT * FROM users LIMIT 3,2;
               
        (2) mysql> select * from users limit 输出数据量 offset 起始地址;
```