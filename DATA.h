#pragma once
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string.h>
#include<stack>

class DATA;
class COLUMN;
class TABLE;
class DATABASE;
class ALLBASES;

typedef DATA *pDATA;

enum ConstraintType
{             //列的约束类型枚举
    NOT_NULL, //非空
    UNIQUE,
    PRIMARY_KEY, //主键
    FOREIGN_KEY,
    CHECK,
    DEFAULT
};

enum DataType
{ //数据类型枚举
    _INT,
    _DOUBLE,
    _CHAR,
    // _TIME,
    // _DATE,
};

class DATA
{ //数据基类，支持NULL功能
protected:
    bool NullFlag = false; //true -> NULL
public:
    virtual ~DATA() = 0;
    bool isNull();    //获取NullFlag
    void SetNull();   //设置为NULL
    void UnsetNull(); //设置为不是NULL
};

//进行重要修改！！！NULL 值与任何其它值的比较（即使是 NULL）永远返回 false，即 NULL = NULL 返回false

class INT : public DATA
{ //INT类（DATA派生类）
private:
    int Value; //值
public:
    INT(int value) : Value(value)
    { //构造函数
    }
    INT(){};
    int GetValue();            //取值
    void SetValue(int d);      //设置值
    INT &operator=(INT &data); //运算符重载
    bool operator==(INT &b);
    bool operator<(INT &b);
    bool operator>(INT &b);
    bool operator<=(INT &b);
    bool operator>=(INT &b);
    bool operator!=(INT &b);
    friend std::ostream &operator<<(std::ostream &out, INT &d);
};

class DOUBLE : public DATA
{ //DOUBLE类（DATA派生类）
private:
    double Value;

public:
    DOUBLE(double value) : Value(value)
    { //构造函数
    }
    DOUBLE(){};
    double GetValue();
    void SetValue(double d);
    DOUBLE &operator=(DOUBLE &data);
    bool operator==(DOUBLE &b);
    bool operator<(DOUBLE &b);
    bool operator>(DOUBLE &b);
    bool operator<=(DOUBLE &b);
    bool operator>=(DOUBLE &b);
    bool operator!=(DOUBLE &b);
    friend std::ostream &operator<<(std::ostream &out, DOUBLE &d);
};

class CHAR : public DATA
{ //CHAR类（DATA派生类）
private:
    char Value;

public:
    CHAR(char value) : Value(value)
    { //构造函数
    }
    CHAR(){};
    void SetValue(char d);
    char GetValue();
    CHAR &operator=(CHAR &data);
    bool operator==(CHAR &b);
    bool operator<(CHAR &b);
    bool operator>(CHAR &b);
    bool operator<=(CHAR &b);
    bool operator>=(CHAR &b);
    bool operator!=(CHAR &b);
    friend std::ostream &operator<<(std::ostream &out, CHAR &d);
};
