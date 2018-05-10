# MySQL 权限管理

## 简介

```shell
    1.Mysql的权限实现
        (1) 第一阶段：服务器首先会检查你是否允许连接。因为创建用户的时候会加上主机限制，
                    可以限制成本地、某个IP、某个IP段、以及任何地方等，只允许你从配置的指定地方登陆
                    
        (2) 第二阶段：如果你能连接,Mysql会检查你发出的每个请求,看你是否有足够的权限实施它.
                     比如你要更新某个表、或者查询某个表，Mysql会查看你对哪个表或者某个列是否有权限。
                     再比如，你要运行某个存储过程，Mysql会检查你对存储过程是否有执行权限等
                     
    2.权限分布
        (1) 表权限
                可以设置权限:'Select', 'Insert', 'Update', 'Delete', 'Create', 'Drop', 'Grant',
                           'References', 'Index', 'Alter'
                           
        (2) 列权限
                可以设置权限:'Select', 'Insert', 'Update', 'References'
        (3) 过程权限
                可以设置权限:'Execute', 'Alter Routine', 'Grant'
                
    3.MySQL权限经验原则(出于安全因素)
        (1) 只授予能满足需要的最小权限,防止用户干坏事.比如用户只是需要查询,那就只给select权限就可以了,
            不要给用户赋予update、insert或者delete权限.
        (2) 创建用户的时候限制用户的登录主机，一般是限制成指定IP或者内网IP段
        (3) 初始化数据库的时候删除没有密码的用户。安装完数据库的时候会自动创建一些用户，这些用户默认没有密码.
        (4) 为每个用户设置满足密码复杂度的密码
        (5) 定期清理不需要的用户。回收权限或者删除用户.
        
```

## 应用

```shell
    1.grant命令使用说明
        创建一个只允许从本地登录的超级用户jack,并允许将权限赋予别的用户，密码为：jack
            mysql> grant all privileges on *.* to jack@'localhost' identified by "jack" with grant option;
            参数:
                all privileges : 是表示所有权限,你也可以使用select、update等权限。
                on : 用来指定权限针对哪些库和表。
                *.* : 中前面的*号用来指定数据库名，后面的*号用来指定表名。
                to : 表示将权限赋予某个用户。
                jack@'localhost' : 表示jack用户,@后面接限制的主机，可以是IP、IP段、域名以及%, %表示任何地方
                                   注意：这里%有的版本不包括本地，以前碰到过给某个用户设置了%允许任何地方登录，
                                        但是在本地登录不了，这个和版本有关系，遇到这个问题再加一个localhost的用户就可以了
                identified by : 指定用户的登录密码。
                with grant option : 这个选项表示该用户可以将自己拥有的权限授权给别人
                                    注意：经常有人在创建操作用户的时候不指定WITH GRANT OPTION选项导致后来
                                    该用户不能使用GRANT命令创建用户或者给其它用户授权。
                
                备注：可以使用GRANT重复给用户添加权限，权限叠加，比如你先给用户添加一个select权限，
                      然后又给用户添加一个insert权限，那么该用户就同时拥有了select和insert权限。
                      
    2.刷新权限
        在使用grant命令更新权限后,使用flush privileges立马生效
        mysql> flush privileges;
        
    3.查看权限
        (1)查看当前用户的权限：
                mysql> show grants;
                结果:
                    +---------------------------------------------------------------------+
                    | Grants for root@localhost                                           |
                    +---------------------------------------------------------------------+
                    | GRANT ALL PRIVILEGES ON *.* TO 'root'@'localhost' WITH GRANT OPTION |
                    | GRANT PROXY ON ''@'' TO 'root'@'localhost' WITH GRANT OPTION        |
                    +---------------------------------------------------------------------+
                    
        (2) 查看某个用户的权限
                 A.mysql> select * from mysql.user where user='用户名' \G;
                 B.mysql> show grants for 'root'@'localhost';
                    结果:
                   +---------------------------------------------------------------------+
                   | Grants for root@localhost                                           |
                   +---------------------------------------------------------------------+
                   | GRANT ALL PRIVILEGES ON *.* TO 'root'@'localhost' WITH GRANT OPTION |
                   | GRANT PROXY ON ''@'' TO 'root'@'localhost' WITH GRANT OPTION        |
                   +---------------------------------------------------------------------+
                   
    4.回收权限
        mysql> revoke delete on *.* from 'jack'@'localhost';(其中 'jack'@'localhost' 是一个整体)
        
```

