# MySQL 约束

## 简介
```shell
    1.作用:
        给表添加约束值,从而约束用户操作数据库的行为
```

## 约束类型

```shell
    1.默认值约束(default)
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
               
    2.非空约束(not null)
        当某个字段必须要值(不能插入null,也不能不插)
            A.mysql> create table student(id int,name varchar(20) not null, hight int default 1);
            B.mysql> insert into student(id,name) values(1,null);
              ERROR 1048 (23000): Column 'name' cannot be null
            C.mysql> insert into student(id,hight) values(1,3);
              ERROR 1364 (HY000): Field 'name' doesn't have a default value
              
    3.唯一值约束(unique)
        列的值不能重复
            A.mysql> create table student(id int unique,name varchar(20) not null, hight int default 1);
            B.mysql> insert into student(id,name,hight) values(1,'john',3); 
              ERROR 1062 (23000): Duplicate entry '1' for key 'id'
              
    4.主键值约束(primary key,自带唯一非空)
        通常情况下,我们会给每张表都会设置一个主键字段,用来标记记录的唯一性
        但是不建议把业务含义字段作为主键，因为随着业务的变化，业务字段可能会出现重复。
        建议给每张张独立添加一个叫id的字段，把这个id字段设置成主键，用来作为记录的唯一性
            A.mysql> create table student(id int primary key,name varchar(20) not null, hight int default 1);
            
    5.自增长约束(auto_increment)
        一个表中的id值通常都是一次+1的,这样的需求其实不需要我们每次手动去插入数值,
        数据库会自动帮我们插入数据,需要配合主键使用
            A.mysql> create table student(id int primary key auto_increment,
                                          name varchar(20) not null,
                                          hight int default 1);
                                          
    6.外键约束(foreign)
        外键约束： 约束两种表的情况
        当有了外键约束之后，操作数据的顺序如下：
                    插入数据： 先插入主表的数据，再插入副表数据
                    修改数据： 先修改主表数据，再修改副表数据
                    删除数据： 先删除副表数据，再删除主表数据
                    
        A.先创建部门表
            mysql> create table dept(id int primary key auto_increment,name varchar(20));
        B.创建人员表
            mysql> create table person(id int primary key auto_increment,
                                       name varchar(20), deptid int,
                                       constraint person_dept_fk foreign key(deptid) references dept(id));
                                       
            mysql> create table person(id int primary key auto_increment,
                                                   name varchar(20), deptid int,
                                                   constraint 外键名 foreign key(列名) references 关联表名(关联列名));
        C.向dept表插入数据
            mysql> insert into dept(name) values('sale'),('office'),('development');
        D.
            mysql> insert into person(name,deptid) values('johm',1);  在对应dept表中deptid有的数据可以插入
            mysql> insert into person(name,deptid) values('joe',9);   在对应dept表中没有deptid有的数据禁止插入
            ERROR 1452 (23000): Cannot add or update a child row:
            a foreign key constraint fails (`test`.`person`, CONSTRAINT `person_dept_fk` FOREIGN KEY (`deptid`)
             REFERENCES `dept` (`id`))
```