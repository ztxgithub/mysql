# MySQL 编程接口

## 连接数据库
```shell
    1.头文件
        #include <mysql_driver.h>     // 驱动类
        #include <mysql_connection.h> // 连接类
        
    2.
        Connection类
            主要成员函数	返回值	说明
            setSchema()	void	切换到指定数据库（参数为数据库名）
            isValid()	bool	连接是否有效
            reconnect()	bool	如果连接中断则重连
        
    
    例子:    
        #include <iostream>
        
        #include <mysql_driver.h>
        #include <mysql_connection.h>
        using namespace std;
        using namespace sql;
        int main()
        {
            mysql::MySQL_Driver *driver;
            Connection *con;
        
            driver = mysql::get_mysql_driver_instance();
            if (driver)
            {
                cout<<"dirver ok"<<endl;
            }
        
            con = driver->connect("tcp://127.0.0.1:3306", "root", "123456");
            if (con)
            {
                cout<<"conn ok"<<endl;
            }
        
            delete con;
            // driver不需要显式delete，Connection对象会小心释放它
        }
        
```

## 执行SQL语句

```shell
    1.头文件
      #include <cppconn/statement.h>
    2.类
      是一个接口类。只包含纯虚函数。
      
      sql::Statement *stmt;
      // 创建对象
      stmt = con->createStatement();
      成员函数
      	virtual bool excute(const sql::SQLString& sql) = 0;
      	
    3.实例代码
        #include <iostream>
        
        #include <mysql_driver.h>
        #include <mysql_connection.h>
        #include <cppconn/statement.h>
        using namespace std;
        using namespace sql;
        int main()
        {
            mysql::MySQL_Driver *driver;
            Connection *con;
        
            driver = mysql::get_mysql_driver_instance();
            if (!driver)
            {
                cout<<"dirver error"<<endl;
                return -1;
            }
        
            con = driver->connect("tcp://127.0.0.1:3306", "root", "123456");
            if (!con)
            {
                cout<<"conn error"<<endl;
                return -1;
            }
        
            Statement *stmt;
            stmt = con->createStatement();
            if (!stmt)
            {
                cout<<"stmt error"<<endl;
                return -1;
            }
        	// 执行各种SQL语句
            stmt->execute("USE test");                                   // 选择数据库
            stmt->execute("DROP TABLE IF EXISTS test");                  // 如果表已存在，则删除
            stmt->execute("CREATE TABLE test(id INT, label CHAR(1))");   // 创建表
            stmt->execute("INSERT INTO test(id, label) VALUES(1, 'a')"); //插入数据
        
            delete stmt;
            delete con;
        }
```

## PreparedStatement(预处理)

```shell
    1.是一种特殊的“语句”类。
        Statement类执行的语句只能是静态的字符串
        PreparedStatement类执行的语句可以含有变量(采用占位符思想实现)
    2.头文件
      #include <cppconn/prepared_statement.h>
      
    3.类
      PreparedStatement *pstmt;
      // 创建对象
      pstmt = con->prepareStatement("INSERT INTO test(id, label) VALUES(?, ?)");
      通过Connector对象的prepareStatement函数来创建该类对象。其参数是一个含有占位符（?）的SQL语句。
      
      填充占位符
      pstmt->setInt(1, 2);       // 给第一个占位符填充数字2
      pstmt->setString(2, "b");  // 给第二个占位符填充字符串"b"
      
      执行SQL
      pstmt->execute();
      
```

## ResultSet(SQL语句返回的结果集)

```shell
    1.头文件
      #include <cppconn/resultset.h>
    2.创建对象
      由sql::Statement::executeQuery()和sql::PreparedStatement::executeQuery()返回。
      比如：
      
      sql::Connection *con;
      sql::Statement *stmt;
      sql::ResultSet *res;
      // ...
      con->setSchema("test");
      // ...
      stmt = con->createStatement();
      res = stmt->executeQuery("SELECT id, label FROM test");
      
    3.成员函数
        A. next
            返回bool类型，用于逐行遍历结果集。
            结果集内部有游标,标记着当前行.游标起始位置为第一行，每次执行完next函数，游标都会移动到下一行。
      
        B. getXXX
          函数	        返回值	        说明
          getInt	    int32_t	        返回当前行某一列的值作为整型
          getString	    SQLString	    返回当前行某一列值作为字符串
          getUInt	    uint32_t	    32位无符号整型
          getInt64	    int64_t	        64位有符号整型
          getUInt64 	uint64_t	    64位无符号整型
          getBoolean	bool	        bool类型
          getBlob	    std::istream	二进制类型
          
          getXXX是一系列函数，用于取出当前行某一列值。XXX表示数据类型。
      
            该系类函数有两个重载：
                (1) 第一个函数: 可以用整型作为参数,表示取第几列的值。
                (2) 第二个函数: 可以使用字符串作为参数,取出对应列名的值。
      比如
      
          while (res->next())
          {
            cout << "id = "<<res->getInt(1)<<";label = "<<res->getString("label")<<endl;
          }
```

## 事务

```shell
    1.关闭自动提交
      con->setAutoCommit(false);
      
    2.提交事务
      con->commit();
      
    3.事务回滚
      con->rollback();
```

