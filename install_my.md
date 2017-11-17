# Ubuntu 安装mysql

```shell
    1.sudo apt-get install mysql-server mysql-client
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

# 启动mysql数据库

```shell
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
    3.
```