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
            在FROM子句中使用子查询时,从子查询返回的结果集将用作临时表. 该表称为派生表或物化子查询
            查找订单表中的最大，最小和平均数:
            
                mysql> select max(items),min(items),floor(avg(items)) from
                       (select orderNumber , count(orderNumber) as items from orderdetails group by orderNumber) 
                       as lineitems;
                
```


