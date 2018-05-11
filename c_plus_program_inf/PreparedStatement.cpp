//
// Created by jame on 18-5-11.
//

#include <iostream>

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
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

    con->setSchema("test");  //切换到 test 数据库
    PreparedStatement *pstmt;
    pstmt = con->prepareStatement("INSERT INTO test(id, label) VALUES(?, ?)");
    if (!pstmt)
    {
        cout<<"prepared stmt error"<<endl;
        return -1;
    }
    pstmt->setInt(1, 2);
    pstmt->setString(2, "b");
    pstmt->execute();

    delete pstmt;
    delete con;
}

