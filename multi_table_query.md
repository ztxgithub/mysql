# mysql 多表查询

## 概要

```shell
    1.多表查询:数据同时从多张表中获取
```

## cross join (交叉连接)

```shell
    1.交叉连接: 没有任何限制条件的连接方式,相当与2个表的全排列,在实际运用中要尽量避免
    2.
        (1) mysql> select * from t1;
            结果:
                +------+-------+
                | t1id | t1str |
                +------+-------+
                |    1 | 1     |
                |    2 | 2     |
                |    3 | 3     |
                +------+-------+
                
        (2) mysql> select * from t2;
            结果:
                +------+-------+
                | t2id | t2str |
                +------+-------+
                |    2 | a     |
                |    3 | b     |
                +------+-------+
                
        (3) 标准的交叉连接
                mysql> select * from t1 cross join t2;
                结果:
                    +------+-------+------+-------+
                    | t1id | t1str | t2id | t2str |
                    +------+-------+------+-------+
                    |    1 | 1     |    2 | a     |
                    |    1 | 1     |    3 | b     |
                    |    2 | 2     |    2 | a     |
                    |    2 | 2     |    3 | b     |
                    |    3 | 3     |    2 | a     |
                    |    3 | 3     |    3 | b     |
                    +------+-------+------+-------+
                
        (4) 另外一个交叉连接
                mysql> select * from t1,t2;
                结果:
                    +------+-------+------+-------+
                    | t1id | t1str | t2id | t2str |
                    +------+-------+------+-------+
                    |    1 | 1     |    2 | a     |
                    |    1 | 1     |    3 | b     |
                    |    2 | 2     |    2 | a     |
                    |    2 | 2     |    3 | b     |
                    |    3 | 3     |    2 | a     |
                    |    3 | 3     |    3 | b     |
                    +------+-------+------+-------+
                    
        (5) 多表交叉连接
                mysql> select * from t1 cross join t2 cross join t3;
```

## inner join (内连接) 

```shell
    1.内连接:两张表同时符合某种条件的数据记录的组合
    2.mysql> select * from 表1 inner join 表2 on 条件;
    3.
        (1) 标准的内连接(尽量使用标准的)
                mysql> select * from t1 inner join t2 on t1.t1id=t2.t2id;
                结果:
                +------+-------+------+-------+
                | t1id | t1str | t2id | t2str |
                +------+-------+------+-------+
                |    2 | 2     |    2 | a     |
                |    3 | 3     |    3 | b     |
                +------+-------+------+-------+
                
        (2) 另外一种内连接
                mysql> select * from t1,t2 where t1.t1id = t2.t2id;
                结果:
                    +------+-------+------+-------+
                    | t1id | t1str | t2id | t2str |
                    +------+-------+------+-------+
                    |    2 | 2     |    2 | a     |
                    |    3 | 3     |    3 | b     |
                    +------+-------+------+-------+
                    
    4.在mysql中,"inner join" 等价与 "join"

```

## 外连接 (left join, right join)

```shell
    1.左外连接(left outer join)
        左外连接"左侧的表"中的所有记录都会被展示出来，左侧表中符合条件的记录将会与右侧表中符合条件的记录相互连接组合，
        左侧不符合条件的记录将会与右侧表中的"空记录"进行连接
        
    2.
        (1) 左外连接
                mysql> select * from t1 left outer join t2 on t1.t1id=t2.t2id;
                结果:
                    +------+-------+------+-------+
                    | t1id | t1str | t2id | t2str |
                    +------+-------+------+-------+
                    |    2 | 2     |    2 | a     |
                    |    3 | 3     |    3 | b     |
                    |    1 | 1     | NULL | NULL  |
                    +------+-------+------+-------+
                    
    3.左外连接可以简称为左连接,"left outer join" 也可以简写为 "left join"
      同理,右外连接可以简称为右连接,"right outer join" 也可以简写为 "right join"
      
    4.更过条件的外连接
            mysql> select * from t1 left join t2 on t1.t1id=t2.t2id where t2id is not NULL;  
            结果:
                +------+-------+------+-------+
                | t1id | t1str | t2id | t2str |
                +------+-------+------+-------+
                |    2 | 2     |    2 | a     |
                |    3 | 3     |    3 | b     |
                +------+-------+------+-------+
               
```

## 联合查询(union 与 union all)

```shelll
    1.联合查询: 把多个表的查询结果放在一起进行显示(可以通过as进行列名重命名)
    2.mysql> 左边查询结果(select) union 右边查询结果(select)
    3.
        (1) 
            mysql> select t1id as id, t1str as str from t1 union select * from t2; 
            +----+------+
            | id | str  |
            +----+------+
            |  1 | 1    |
            |  2 | 2    |
            |  3 | 3    |
            |  2 | a    |
            |  3 | b    |
            +----+------+
            
    4.当时使用union连接两个查询语句,两个语句查询出的字段数量要相同,否则无法使用union进行联合查询
    5.使用union将两个查询结果集中显示时，重复的数据会被合并为一条
        (1) mysql> select * from t1;
            结果:
                +------+-------+
                | t1id | t1str |
                +------+-------+
                |    1 | 1     |
                |    2 | 2     |
                |    3 | 3     |
                |    4 | 4     |
                +------+-------+
                
        (2) mysql> select * from t2;
            结果:
               +------+-------+
               | t2id | t2str |
               +------+-------+
               |    2 | a     |
               |    3 | b     |
               |    4 | 4     |
               +------+-------+
                
        (3) mysql> select * from t2 union select * from t1;
            结果:
                +------+-------+
                | t1id | t1str |
                +------+-------+
                |    1 | 1     |
                |    2 | 2     |
                |    3 | 3     |
                |    4 | 4     |
                |    2 | a     |
                |    3 | b     |
                +------+-------+
                
    6. 使用union all 进行联合查询时，两个查询结果集中显示重复的数据会被同时显示
            (1) mysql> select * from t1 union all select * from t2;
                结果:
                    +------+-------+
                    | t1id | t1str |
                    +------+-------+
                    |    1 | 1     |
                    |    2 | 2     |
                    |    3 | 3     |
                    |    4 | 4     |
                    |    2 | a     |
                    |    3 | b     |
                    |    4 | 4     |
                    +------+-------+
                    
```

## 外连接

```shell
    1.完全连接(在内连接的基础上,还包含两个表中所有不符合条件的数据行,并在其中的左表、和右表列填写NULL)
```