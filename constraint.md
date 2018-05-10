# MySQL 约束

## 简介
```shell
    1.作用:
        给表添加约束值,从而约束用户操作数据库的行为
```

## 约束类型

```shell
    1.默认值约束
        当没有插入数值的时候默认值才会起作用；
            A. mysql> create table student(id int,name varchar(20), hight int default 1);
            B. mysql> insert into student(id,name) values(1,'john');
            C. mysql> select * from student;
               结果:
               +------+------+-------+
               | id   | name | hight |
               +------+------+-------+
               |    1 | john |     1 |
               +------+------+-------+
```