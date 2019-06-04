## 总体得分
7/10
## 评价一

#### <label style="color:MediumTurquoise">优点</label>

* 各个类封装合理，基本数据用`std::string`统一存储有一定的合理性与可拓展性。

#### <label style="color:MediumTurquoise">缺点</label>

* `main`函数与`read_WhereClause`等函数体太过于庞大繁杂且缺乏注释，导致可读性较差，在二次开发中缺乏优势。
* 数据库的各个功能实现分离度不足，可能会导致后续的开发影响已实现的功能。
* 健壮性有待加强，在测试第一阶段功能时，发现当输入样例4，输入回车，再次输入样例4时，`showtables`功能在表头未显示数据库名，`showcolumns`功能也有bug。(测试输入，程序输出如下)

```sql
CREATE DATABASE OOP;
USE OOP;
CREATE TABLE qaz(wsx DOUBLE NOT NULL, PRIMARY KEY(wsx), edc INT NOT NULL, rfv CHAR);
CREATE TABLE maz(wsx DOUBLE NOT NULL, PRIMARY KEY(wsx), edc INT NOT NULL, rfv CHAR);
CREATE TABLE oaz(wsx DOUBLE NOT NULL, PRIMARY KEY(wsx), edc INT NOT NULL, rfv CHAR);
SHOW columns from qaz;
SHOW TABLES;
DROP TABLE oaz;
CREATE TABLE aaz(wsx DOUBLE, PRIMARY KEY(wsx), edc INT NOT NULL, rfv CHAR);
SHOW TABLES;
DROP DATABASE OOP;

CREATE DATABASE OOP;
USE OOP;
CREATE TABLE qaz(wsx DOUBLE NOT NULL, PRIMARY KEY(wsx), edc INT NOT NULL, rfv CHAR);
CREATE TABLE maz(wsx DOUBLE NOT NULL, PRIMARY KEY(wsx), edc INT NOT NULL, rfv CHAR);
CREATE TABLE oaz(wsx DOUBLE NOT NULL, PRIMARY KEY(wsx), edc INT NOT NULL, rfv CHAR);
SHOW columns from qaz;
SHOW TABLES;
DROP TABLE oaz;
CREATE TABLE aaz(wsx DOUBLE, PRIMARY KEY(wsx), edc INT NOT NULL, rfv CHAR);
SHOW TABLES;
DROP DATABASE OOP;
```

```sql
Field   Type    Null    Key Default Extra
wsx double  NO  PRI NULL    
edc int(11) NO      NULL    
rfv char(1) YES     NULL    
Tables_in_OOP
maz
oaz
qaz
Tables_in_OOP
aaz
maz
qaz
Field   Type    Null    Key Default Extra
Tables_in_
aaz
maz
maz
oaz
qaz
qaz
Tables_in_
aaz
aaz
maz
maz
qaz
qaz
```
#### <label style="color:MediumTurquoise">总体评价</label>

* 第一阶段任务具有较好的实现，也预留了较多的接口，但整体逻辑尤其是主函数入口处略显繁复，导致二次开发上的困难，在逻辑运算方面，由于原有代码的单进程性，很难在原有基础上添加功能。

## 评价二

* 底层：底层实现太朴素，直接用string存储然后需要转换时再转的做法，遇到需要频繁转换时有不必要的效率上的损失。System概念比较出彩，然而实际上也只是一个简单的进一步封装。总体基本符合OOP思想。
* 字符串解析：在字符串中频繁取某个常数长度的子串，效率比较差，而且无法应对多余空格的情况，辅以大量if-else语句，代码也显得冗长。
* 说明文档：说明文档对接口的说明不太足，而且有些关键实现没有提及（比如直接用string存储所有信息）。

## 评价三

1. 第一阶段功能完善程度 /2.0
    * B 关键字大小写敏感问题(primary ...) 1.9/2.0
2. 对于一些特殊输入数据的处理 /0.5
    * B 对于空格处理不细致 0.4/0.5
3. 代码结构 /1.5
    * B 1.5/1.5
4. 可读性 /1.0
    * B 1.0/1.0
5. BONUS
    * B 解释文档清晰 0.1
    

#### 附1：评分总结
评分主要是通过数据测试和代码上来给分，二者各占一半。
第一阶段完善程度上主要出现的问题是对关键字大小写的敏感程度处理，在mysql中是严格的不敏感而有些组只判断了全部大写或者全部小写，进行酌情的扣分。而特殊的数据则是空格和引号的变化。
具体代码上可读性作为较次要的标准，扣分点主要是有些代码一个函数过长或者一个文件过长，没有对代码进行合理的拆分。主要的部分是代码结构上，因为结构上各有各的合理性，而个人因素考察上过于主观，鉴于四个组都较好地完成了内容而结构也不至于混乱，因而全部给了满分。

#### 附2：测试数据
```
Create database oop;
Use oop;
Create table my(zyx          char,     cls iNt, lyh DouBle not null, PRImARY key(zyx));
Show        columns from my;
Insert into          my(lyh, zyx) values (2.33, "b");
Insert into my(cls, zyx, lyh) values (2, 'a', 2.34);
seLect * from my;
seLect *             from my where cls < 3;
seleCt * from my              where cls < 3 or zyx       > "b" and lyh =       2.33;
```

## 评价四

* 说明文档有条理，较为明晰；命名符号的规定使得代码容易辨识，而且设置了统计数据库里元素的计数器，比较方便；代码的结构一般，有的处理复杂化了。