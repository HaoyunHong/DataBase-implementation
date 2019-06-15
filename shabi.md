# 大作业第二阶段说明文档

## **第一阶段框架代码分析与修改**
### 1、对第一阶段代码及第二阶段需求的分析：
第一阶段代码的复用性强，接口完备，对实现第二阶段的功能带来一定便利，但对于不同数据类型的处理仍然依赖if-else语句进行处理，可能带来较大的工作量。

第二阶段大部分需求都基于SELECT关键字，因此需要对PARSE::EXEC()函数(命令处理函数)中识别SELECT类型命令的分支代码进行较精细的修改。

数据的导出：
是SELECT语句的一种，需要识别关键字"INTO","OUTFILE"等，使用表相关函数读取表数据，并利用ifstream相关函数将数据导出到文件

数据的导入：
是LOAD语句，需要增加语句处理新分支。需要利用ofstream相关函数读取文件数据，并利用表相关函数插入数据

COUNT函数：

GROUP BY分组语句：

ORDER BY 排序语句：


多表whereclause：
需要修改原代码中默认FROM之后表名只有一个的部分，并且对输入的列名以及whereclause里的列名进行处理(因为多表whereclause中的列名表示法为table_name.column_name)

UNION：



### 2、对第一阶段方法的修改：
#### 2.1、对Select方法的修改

原有select方法包括了查找符合要求的行和输出两种功能,为了实现多表whereclause以及含UNION,JOIN,ORDER,GROUP BY的SELECT语句，将查找、输出功能分离开来

原函数：
```
void TABLE::Select(string name, string condition){......}
```
修改之后的查找函数：
```
const std::vector<int> &TABLE::Select
(const std::vector<std::string> &col_name, string condition){......}
//函数内存在一个static std::vector<int>，在函数体内通过clear()维护稳定性
储存的是符合whereclause子句condition的所有行的下标，即找到所有符合要求的行
这一函数的返回值是这个静态vector变量的常量引用
作用范围：单表
作用其他特点：对符合要求的行按照主键顺序排序，方便了之后直接调用各种输出函数
```

修改之后的输出函数:
```
void TABLE::show_output_from_select
(const std::vector<std::string> &col_name,
 const std::vector<int> &outorder)
 //用于普通select语句，输出一个表中某些行的语句，附带打印表头
 ```


