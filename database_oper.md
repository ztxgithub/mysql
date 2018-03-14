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
                    
    *.插入数据
        mysql> insert into 表名 values(xxx,xxx),(yyy,yyy);  //支持插入多条记录
        mysql> insert into 表名(列名1，列名2) values(xxxx,xxx);    
                    
    *.修改表结构
            1. mysql> alter table 表名 modify 列名 数据类型
                例如：
                mysql> alter table t_time modify data_tm datetime(6),modify timestamp_tm timestamp(6); (修改时间精确到毫秒)
                
            2. mysql> alter table 表名 add 列名 数据类型
        
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
       
    
     
```

- 在shell终端的命令

```shell

    1.显示数据库global变量的值
        > mysql -e "show global status" -uroot -p52346898;
        
      
     
```
