#### 4.3、sql数字函数
#####设计思路：
sql数字函数的本质是用```select```关键词后加数字函数，对数据进行处理，在其中，我们实现了取绝对值```ABS```函数、取反余弦值```ACOS```函数、取反正弦值```ASIN```函数、取反正切值```ATAN```函数，还有向上取整```CEIL```函数、```CEILING```函数，一共6个函数。

#####功能示例：
取绝对值函数的输入：

	SELECT ABS(-1);
输出：

	1
取反余弦值的输入：

	SELECT ACOS(0.25);
输出：

	75.5225 degree
取反正弦值的输入：

	SELECT ASIN(0.25);
输出：

	14.4775 degree
取反正切值的输入：

	SELECT ATAN(2.5);
输出：

	68.1986 degree
向上取整的输入：

	SELECT CEIL(1.5);
	SELECT CEIL(1.0);
	SELECT CEILING(1.5);
	SELECT CEILING(1.00); 
输出：

	2
	1
	2
	1
##### 优点：
直接在```Parse.EXEC```函数体内```else if (ele1 == "SELECT")```的分支内加分支进行扩展，无需改动其它代码。

通过在判断时，使用大写字母化后的表达式进行比对的方法，实现对关键词和运算符的大小写不敏感。

实现了6个函数，大于5个，要想再加数字函数可以直接继续在分支里扩展。
##### 局限性与展望：
时间比较紧迫，没有与```whereclauses```有机结合，最理想的方式是像excel那样，对数值列中的数据进行数字函数操作，比如对数据表中的某个或某列数值数据取绝对值等，使数据库更加灵活。
#### 4.4、sql字符串函数
#####设计思路：
sql字符串函数是用select关键词加字符串函数，输出处理后的字符串，在其中，我们实现了获取字符串长度的```CHAR_LENGTH```函数、```CHARACTER_LENGTH```函数，按顺序连接字符串的```CONCAT```函数（默认连接符为" "，即空格）、按顺序连接字符串并给定连接符的```CONCAT_WS```函数一共4个函数。
#####功能示例：
获取字符串长度的输入：

	SELECT CHAR_LENGTH("RUNOOB") AS LengthOfString;
	SELECT CHAR_LENGTH("RUNOOB")AS LengthOfString;
	SELECT CHARACTER_LENGTH("RUNOOB") AS LengthOfString;
	SELECT CHARACTER_LENGTH("RUNOOB")AS LengthOfString;
输出：

	The length of the entered string "RUNOOB" is 6.
	The length of the entered string "RUNOOB" is 6.
	The length of the entered string "RUNOOB" is 6.
	The length of the entered string "RUNOOB" is 6.
按顺序连接字符串的输入：

	SELECT CONCAT("SQL ", "Runoob ", "Gooogle ", "Facebook") AS ConcatenatedString;
	SELECT CONCAT("SQL ", "Runoob ", "Gooogle ", "Facebook")AS ConcatenatedString;

输出：

	The merged string is "SQL Runoob Gooogle Facebook".
	The merged string is "SQL Runoob Gooogle Facebook".	
按顺序连接字符串并给定间隔符（比如```-```)的输入：

	SELECT CONCAT_WS("-", "SQL", "Tutorial", "is", "fun!") AS ConcatenatedString;
	SELECT CONCAT_WS("-", "SQL", "Tutorial", "is", "fun!")AS ConcatenatedString;
输出：

	The merged string with entered seperation is "SQL-Tutorial-is-fun!".
	The merged string with entered seperation is "SQL-Tutorial-is-fun!".
#####优点：
直接在```Parse.EXEC```函数体内```else if (ele1 == "SELECT")```的分支内加分支进行扩展，无需改动其它代码。

通过在判断时，使用大写字母化后的表达式进行比对的方法，实现对关键词和运算符的大小写不敏感。

因为```AS```关键词与函数参数表后的右括号之间有没有空格在mysql中都是合法的，我们对这个corner case作了处理，使程序更稳健。
#####局限性与展望：
其实字符串函数可以不仅仅处理独立的字符串，也可以把一个表中的某些“单元格”（类比excel表）的字符串元素进行操作（比如把某行的某几列字符串数据连接起来）然后再赋值给某一“单元格”。
#### 4.5、sql逻辑运算
#####设计思路：
sql逻辑运算可以说就是用```select```关键字的不带括号的逻辑计算器，通过为各运算符设定优先级，再运用数据栈和运算符栈，来得到最终的```bool```值答案。

因为此处的特殊符号```!```是前后有无空格都是合法的，而我们知道取非是唯一的单目逻辑运算符，```!```是从属于它后面的数据的，```NOT```也是，所以通过函数```string PARSE::whole_expression_standardize```进行逻辑表达式的标准化，将出现```!```或```NOT```的表达式转化为符号前有空格而后面直接连数据的新表达式，以便后续通过字符串流进行分割。若分割的子段中有```!```或```NOT```，直接取逻辑非加入数据栈中。
#####功能示例：
输入：

	Select 2 aNd 0;
	seLEct 2 oR 1;
	SElect noT 1;
	sEleCt 1 Xor 1;
	sEleCt 0 Xor 1 AnD 9 and !10 Or 3;
输出：

	false
	true
	false
	true
	true
#####优点：
直接在```Parse.EXEC```函数体内```else if (ele1 == "SELECT")```的分支内加分支进行扩展，无需改动其它代码。

通过在判断时，使用大写字母化后的表达式进行比对的方法，实现对关键词和运算符的大小写不敏感。

对特殊符号```!```前后有无空格的corner cases进行处理，使程序更加稳健。

可以支持任何仅有与、或、非、异或的逻辑表达式，且运算符数量与组合形式不限。
#####局限性与展望：
逻辑运算可以不仅仅处理独立的逻辑表达式，它可以与```whereclauses```的逻辑运算兼并，去计算筛选输出的条件，或者可以把一个表中的某些“单元格”（类比excel表）的字符串元素进行操作（比如连接起来）然后再赋值给某一“单元格”。
#### 4.6、sql算术运算
#####设计思路：
sql算术运算本质使```SELECT```关键字的无括号的算术计算器，运用数据栈和运算符栈以及给各运算符设定优先级，来得到最终的数值答案，最终答案小数点后保留6位。
#####功能示例：
输入加法表达式：

	SeLeCt 2+1;
输出：

	2 + 1
	3.000000
输入减法表达式：

	SELect 2-1;
输出：

	2 - 1
	1.000000
输入除法表达式：

	SeLect 3/2;
	select 3 DIV 2;
输出：

	3 / 2
	1.500000
	3 DIV 2
	1.500000
输入取模表达式：

	select 100%3;
	selecT 100 MOD 3;
输出：

	100 % 3
	1.000000
	100 MOD 3
	1.000000	
输入数学符号前后没有空格的混合运算：

	SELECT 2 MOd 3+4/2-1+8/6;
输出：

	2 MOD 3 + 4 / 2 - 1 + 8 / 6
	4.333333
输入数学符号前后没有空格或有空格的混合运算：

	SelECT 1/3-5 DIV 3+8 Mod 7-3/5;
	SelECT 1 / 3 -5 DIV   3 +   8   Mod 7  - 3 /5;
输出：

	1 / 3 - 5 DIV 3 + 8 MOD 7 - 3 / 5
	-0.933333
	1  /  3  - 5 DIV   3  +    8   MOD 7   -  3  / 5
	-0.933333
#####优点：
直接在```Parse.EXEC```函数体内```else if (ele1 == "SELECT")```的分支内加分支进行扩展，无需改动其它代码。

通过在判断时，使用大写字母化后的表达式进行比对的方法，实现对关键词和运算符的大小写不敏感。

支持```int```和```double```数据。

因为数学符号在mysql里前后是不一定有空格的，而我们使用的方法是自动以空格作为间隔的字符串流，所以程序会在数学运算符前后加入空格，对输入的corner cases作处理，以保证运算结果的准确性与程序稳健性。

可以支持任何仅有加、减、乘、除、取模的算术表达式，无论多少个算术运算符或者任何组合都可以。
#####局限性与展望：
其实算术运算可以不仅仅处理独立的字符串，可以用到```whereclases```里，进行表达式的值与数据表中数据的比较，或者可以把一个表中的某些“单元格”（类比excel表）的数值元素进行操作（比如求某一列的和）然后再赋值给某一“单元格”。
#### 4.8、LIKE子句模糊匹配
#####设计思路：
通过对数据表中字符串子串的查找，来进行类似正则的模糊匹配，

####功能展示：
输入，新建数据库、数据表，并将数据插入:

    CREATE DATABASE CAFE;
    USE CAFE;
    CREATE TABLE menu(price INT, cost INT, kind CHAR, PRIMARY KEY(kind));
    INSERT INTO menu(price, cost, kind) VALUES (15, 5, "milk_coffee");
    INSERT INTO menu(price, cost, kind) VALUES (20, 8, "milk_cake");
    INSERT INTO menu(price, cost, kind) VALUES (30, 12, "abc");
    INSERT INTO menu(price, cost, kind) VALUES (15, 5, "red_rose_cake");
    INSERT INTO menu(price, cost, kind) VALUES (22, 10, "pink_rose_cake");
    INSERT INTO menu(price, cost, kind) VALUES (20, 8, "ki");
显示模糊匹配结果：
输入：

    SELECT * from menu WHERE kind LIKE '%cake';
输出：

    price   cost    kind
    20      8       milk_cake
    22      10      pink_rose_cake
    15      5       red_rose_cake
输入：

    SELECT price from menu WHERE kind LIKE '%cake';
输出：

    price
    20
    22
    15
输入：

    SELECT price, kind from menu WHERE kind LIKE '%cake';
输出：
    price   kind
    20      milk_cake
    22      pink_rose_cake
    15      red_rose_cake
输入：

    SELECT * from menu WHERE kind LIKE "milk%";
输出：
    price   cost    kind
    20      8       milk_cake
    15      5       milk_coffee
输入：

    SELECT price from menu WHERE kind LIKE "milk%";
输出：

    price
    20
    15
输入：

    SELECT price, cost from menu WHERE kind LIKE "milk%";
输出：

    price   cost
    20      8
    15      5
输入：

    SELECT * from menu WHERE kind LIKE '%rose%';
输出：

    price   cost    kind
    22      10      pink_rose_cake
    15      5       red_rose_cake
输入：

    SELECT price from menu WHERE kind LIKE '%rose%';
输出：

    price
    22
    15
输入：

    SELECT price, cost from menu WHERE kind LIKE '%rose%';
输出：

    price   cost
    22      10
    15      5  
输入：

    SELECT * from menu WHERE kind LIKE '_i';
输出：

    price   cost    kind
    20      8       ki  
输入：

    SELECT price, kind from menu WHERE kind LIKE '_i';
输出：

    price   kind
    20      ki
输入：

    SELECT price from menu WHERE kind LIKE 'k_';
输出：

    price
    20
输入：

    SELECT price, kind from menu WHERE kind LIKE 'k_';
输出：

    price   kind
    20      ki
#####优点：
直接在```Parse.EXEC```函数体内```else if (ele1 == "SELECT")```的```where```分支调用的判断函数```TABLE:: Judge```函数内进行扩展，无需改动其它代码。

实现了mysql中所有的模糊匹配形式：

    '%a'     //以a结尾的数据
	'a%'     //以a开头的数据
	'%a%'    //含有a的数据
	'_a_'    //三位且中间字母是a的
	'_a'     //两位且结尾字母是a的
	'a_'     //两位且开头字母是a的
#####局限性：
从功能上来看是比较完整了，但是实现过程是通过```string```字符串的```find```和```substr```函数，大概会分支比较多，形式上会比较散，可能通过正则表达式进行模糊匹配会更加精妙。