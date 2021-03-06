# mysql 存储过程

```shell
    1.简介
        (1) 存储过程的优点主要还是执行效率和SQL 代码封装, 特别是 SQL 代码封装功能,如果没有存储过程,
            在外部程序访问数据库时（例如 PHP,python）,要组织很多 SQL 语句。特别是业务逻辑复杂的时候，
            一大堆的 SQL 和条件夹杂在 PHP 代码中，让人不寒而栗。现在有了 MySQL 存储过程，业务逻辑可以封装存储过程中，
            这样不仅容易维护，而且执行效率也高.
            
        (2) 操作数据库语言SQL语句在执行的时候需要要先编译,然后执行,
            而存储过程(Stored Procedure)是一组为了完成特定功能的SQL语句集，经编译后存储在数据库中，
            用户通过指定存储过程的名字并给定参数(如果该存储过程带有参数)来调用执行它.
            
        (3) 一个存储过程是一个可编程的函数,它在数据库中创建并保存.它可以有SQL语句和一些特殊的控制结构组成。
           当希望在不同的应用程序或平台上执行相同的函数,或者封装特定功能时,存储过程是非常有用的.
           数据库中的存储过程可以看做是对编程中面向对象方法的模拟。它允许控制数据的访问方式。
           
    优点:
        1.存储过程增强了SQL语言的功能和灵活性。存储过程可以用流控制语句编写，有很强的灵活性，
           可以完成复杂的判断和较复杂的运算。
        2.存储过程允许标准组件是编程。存储过程被创建后，可以在程序中被多次调用，而不必重新编写该存储过程的SQL语句。
          而且数据库专业人员可以随时对存储过程进行修改，对应用程序源代码毫无影响。
        3.存储过程能实现较快的执行速度。如果某一操作包含大量的Transaction-SQL代码或分别被多次执行，
          那么存储过程要比批处理的执行速度快很多。因为存储过程是预编译的。在首次运行一个存储过程时查询，
          优化器对其进行分析优化，并且给出最终被存储在系统表中的执行计划。
          而批处理的Transaction-SQL语句在每次运行时都要进行编译和优化，速度相对要慢一些。
        4.存储过程能过减少网络流量。针对同一个数据库对象的操作（如查询、修改），
          如果这一操作所涉及的Transaction-SQL语句被组织程存储过程，那么当在客户计算机上调用该存储过程时，
          网络中传送的只是该调用语句，从而大大增加了网络流量并降低了网络负载。
        5.存储过程可被作为一种安全机制来充分利用。系统管理员通过执行某一存储过程的权限进行限制，
         能够实现对相应的数据的访问权限的限制，避免了非授权用户对数据的访问，保证了数据的安全。
         
    缺点:
        1.不易维护，一旦逻辑变了修改起来麻烦
        2.太耗性能
```

