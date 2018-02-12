# Ubuntu 安装mysql

```shell
    1.sudo apt-get install mysql-server mysql-client
    
    MySQL安装后的目录结构分析(此结构只针对于使用apt-get install 在线安装情况)：
    
          数据库存放目录：               /var/lib/mysql/
    
          相关配置文件存放目录：          /usr/share/mysql
    
          相关命令存放目录：             /usr/bin(mysqladmin mysqldump等命令)
    
          启动脚步存放目录：            /etc/rc.d/init.d/
```

# Centos 安装mysql

```shell
     检查 MySQL 是否已安装
        > yum list installed | grep mysql
        
     查看当前 mysql 版本，看是否已安装
        > mysql --version
        > rpm -qa | grep mysql 
        
        
     1.安装
        > wget http://dev.mysql.com/get/mysql-community-release-el7-5.noarch.rpm
        > yum localinstall mysql-community-release-el7-5.noarch.rpm
        > yum install mysql-server
        
     2. 卸载方法
        > rpm -e mysql-community-common-5.7.16-1.el6.x86_64
       
     3.查看安装目录
        > whereis mysql
```

[参考资料](https://github.com/jaywcjlove/mysql-tutorial/blob/master/chapter2/2.2.md)

