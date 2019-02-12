# 常见MySQL语句

```shell
    1.当数据库中出现插入冲突(主键相同),使用 MySQL 特有的语句 使得主键不动, 只更新指定的字段
        insert into zhihu_answer(zhihu_id, url, content) 
                               values(%s, %s, %s)
                               ON DUPLICATE KEY UPDATE content=values(content);
                               
    2.随机从数据库表中选取记录
        > select ip, port from proxy_ip order by RAND() LIMIT 1
```

