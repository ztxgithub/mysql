# mysql 子查询

## 概要
```shell
    1.MySQL子查询是 嵌套在另一个查询(如SELECT,INSERT,UPDATE或DELETE)中的查询
    2.MySQL子查询可以嵌套在另一个子查询中
    3.MySQL子查询称为内部查询，而包含子查询的查询称为外部查询。 子查询可以在使用表达式的任何地方使用，并且必须在括号中关闭
    
```

## MySQL子查询 在where子句中

```shell
    1.MySQL子查询与比较运算符
        (1) 查询返回最大付款额的客户
                                                             |-----------子查询--------|
                mysql> select * from payments where amount = (select max(amount) from payments);
                结果:
                    +----------------+-------------+-------------+-----------+
                    | customerNumber | checkNumber | paymentDate | amount    |
                    +----------------+-------------+-------------+-----------+
                    |            141 | JE105477    | 2015-03-18  | 120166.58 |
                    +----------------+-------------+-------------+-----------+
                    
    2.具有IN和NOT IN运算符的MySQL子查询
        如果子查询返回多个值，则可以在WHERE子句中使用IN或NOT IN运算符等其他运算符
        (1) 查找没有下过任何订单的客户
                mysql> select customerName from customers where customerNumber not in 
                           (select distinct customerNumber from orders);
                结果:
                    +--------------------------------+
                    | customerName                   |
                    +--------------------------------+
                    | Havel & Zbyszek Co             |
                    | American Souvenirs Inc         |
                    | Porto Imports Co.              |
                    | Asian Shopping Network, Co     |
                    | Natrlich Autos                 |
                    | ANG Resellers                  |
                    | Messner Shopping Network       |
                    | Franken Gifts, Co              |
                    | BG&E Collectables              |
                    | Schuyler Imports               |
                    | Der Hund Imports               |
                    | Cramer Spezialitten, Ltd       |
                    | Asian Treasures, Inc.          |
                    | SAR Distributors, Co           |
                    | Kommission Auto                |
                    | Lisboa Souveniers, Inc         |
                    | Precious Collectables          |
                    | Stuttgart Collectable Exchange |
                    | Feuer Online Stores, Inc       |
                    | Warburg Exchange               |
                    | Anton Designs, Ltd.            |
                    | Mit Vergngen & Co.             |
                    | Kremlin Collectables, Co.      |
                    | Raanan Stores, Inc             |
                    +--------------------------------+
                    
    3. FROM子句中的MySQL子查询
            (1) 在FROM子句中使用子查询时,从子查询返回的结果集将用作临时表. 该表称为派生表或物化子查询,临时表要使用一个别名
                 查找订单表中的最大，最小和平均数:
            
                mysql> select max(items),min(items),floor(avg(items)) from
                       (select orderNumber , count(orderNumber) as items from orderdetails group by orderNumber) 
                       as lineitems;
                       
            (2) FROM子句部分的子查询只能是非相关子查询,非相关子查询出现在FROM子句中可以上拉到父层,
                在多表连接时统一考虑连接代价然后进行优化。
                如果是相关子查询出现在FROM字句中，数据库可能返回错误提示
                    A.mysql> select * from t1, (select *from t2 where t1.k1 = t2.k2) as t3;
                        ERROR 1054 (42S22): Unknown column 't1.customerNumber' in 'where clause'
                    B.相关条件去掉后可以得出：
                      mysql> select * from t1, (select * from t2) as a_t2;
                      结果:
                          +----+------+----+------+
                          | k1 | c1   | k2 | c2   |
                          +----+------+----+------+
                          |  1 |    1 |  1 |   10 |
                          |  2 |    2 |  1 |   10 |
                          |  3 |    3 |  1 |   10 |
                          |  1 |    1 |  2 |    2 |
                          |  2 |    2 |  2 |    2 |
                          |  3 |    3 |  2 |    2 |
                          |  1 |    1 |  3 |   30 |
                          |  2 |    2 |  3 |   30 |
                          |  3 |    3 |  3 |   30 |
                          +----+------+----+------+
                       
    4. MySQL "相关子查询"
         一个子查询是独立的. 这意味着您可以将子查询作为独立查询执行,
         与独立子查询不同,子查询的执行依赖于外层父查询的一些属性的值.子查询依赖于父查询的一些参数,
         当父查询的参数改变时,子查询需要根据新参数值重新执行
         例如:
            查询 选择购买价格 高于 每个产品线中的产品的平均购买价格的产品
                mysql> select 
                           productname,
                           buyprice
                       FROM
                           products as p1
                       WHERE
                           buyprice > (SELECT 
                                   AVG(buyprice)
                               FROM
                                   products
                               WHERE
                                   productline = p1.productline);
                                   
    5.MySQL子查询与EXISTS和NOT EXISTS
        (1)如果子查询(subquery)有返回任何行，则EXISTS子查询返回TRUE，否则返回FALSE
            mysql> select * from 表名 where exists(子查询语句);
            
        (2) 把外层sql的结果，拿到内层sql去测试，如果内层的sql成立，则该行取出。内层查询是exists后的查询
            例如:
                从类别表中取出其类别下有商品的类别(如果该类别下没有商品，则不取出)
                    mysql> select c.cat_id,c.cat_name from category c where 
                           exists(SELECT 1 FROM goods g WHERE g.cat_id = c.cat_id);
           
                
```

## 子查询优化

```shell
    1.子查询出现在目标列位置
        当子查询出现在目标列位置的时候，这种查询只能是标量子查询.也就是说子查询返回的结果只能是一个元组的一个属性。
        否则，数据库会返回错误信息
            (1) mysql> select t1.c1, (select t2.c2 from t2 where k2=1) from t1, t2;
                结果:
                    +------+-----------------------------------+
                    | c1   | (select t2.c2 from t2 where k2=1) |
                    +------+-----------------------------------+
                    |    1 |                                10 |
                    |    2 |                                10 |
                    |    3 |                                10 |
                    |    1 |                                10 |
                    |    2 |                                10 |
                    |    3 |                                10 |
                    |    1 |                                10 |
                    |    2 |                                10 |
                    |    3 |                                10 |
                    +------+-----------------------------------+
                    
            (2) mysql> select t1.c1, (selectt2.c2 from t2 where c2 > 1) from t1, t2;
                ERROR 1242 (21000): Subqueryreturns more than 1 row
                
            出现在目标列上的子查询只能返回标量(即空值或单个元组的单个属性)
            
    2.子查询合并
        (1) 多个子查询可以合并为一个子查询,可以把多次表扫描，多次表连接转化为单次表扫描和单次表连接
            例如:
                (效率低的SQL语句)
                mysql> select * from t1 where k1 < 10 and 
                       (
                           exists(select k2 from t2 where t2.k2 < 5 and t2.c2 = 1) or
                           exists(select k2 from t2 where t2.k2 < 5 and t2.c2 = 2)
                       );
                       
                查看这条语句的查询执行计划:
                    mysql> explain extended select * from t1 where k1 < 10 and 
                           (
                                 exists(select k2 from t2 where t2.k2 < 5 and t2.c2 = 1) or
                                 exists(select k2 from t2 where t2.k2 < 5 and t2.c2 = 2)
                            );
                            
                    结果:
                        +----+-------------+-------+-------+---------------+---------+---------+------+------+----------+-------------+
                        | id | select_type | table | type  | possible_keys | key     | key_len | ref  | rows | filtered | Extra       |
                        +----+-------------+-------+-------+---------------+---------+---------+------+------+----------+-------------+
                        |  1 | PRIMARY     | t1    | range | PRIMARY       | PRIMARY | 4       | NULL |    3 |   100.00 | Using where |
                        |  3 | SUBQUERY    | t2    | range | PRIMARY       | PRIMARY | 4       | NULL |    3 |   100.00 | Using where |
                        |  2 | SUBQUERY    | t2    | range | PRIMARY       | PRIMARY | 4       | NULL |    3 |   100.00 | Using where |
                        +----+-------------+-------+-------+---------------+---------+---------+------+------+----------+-------------+
                        
                优化后的SQL:
                    mysql> select * from t1 where k1 < 10 and
                             (
                                 exists(select k2 from t2 where t2.k2 < 5 and (t2.c2 = 1 or t2.c2 = 2))
                             );

            
```


