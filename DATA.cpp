#include "DATA.h"

using namespace std;

bool DATA::isNull()
{ //获取NullFlag
	return NullFlag;
}

void DATA::SetNull()
{ //设置为NULL
	NullFlag = true;
}

void DATA::UnsetNull()
{ //设置为不是NULL
	NullFlag = false;
}

DATA::~DATA() {}

int INT::GetValue()
{ //取值
	return Value;
}

void INT::SetValue(int d)
{
	Value = d;
}

INT &INT::operator=(INT &data)
{
	Value = data.GetValue();
	NullFlag = data.isNull();
	return *this;
}

bool INT::operator==(INT &b)
{
	if (this->isNull() == false && b.isNull() == false)
	{
		if (this->GetValue() == b.GetValue())
			return true;
		else
			return false;
	}
	else if (this->isNull() == true || b.isNull() == true)
	{
		return false;
	}
	return false;
}

bool INT::operator<(INT &b)
{
	return this->GetValue() < b.GetValue();
}
bool INT::operator>(INT &b)
{
	return this->GetValue() > b.GetValue();
}
bool INT::operator<=(INT &b)
{
	return this->GetValue() <= b.GetValue();
}
bool INT::operator>=(INT &b)
{
	return this->GetValue() >= b.GetValue();
}
bool INT::operator!=(INT &b)
{
	return !(*this == b);
}

ostream &operator<<(ostream &out, INT &d)
{
	if (!d.isNull())
	{
		out << d.GetValue();
	}
	return out;
}

double DOUBLE::GetValue()
{
	return Value;
}

void DOUBLE::SetValue(double d)
{
	Value = d;
}

DOUBLE &DOUBLE::operator=(DOUBLE &data)
{
	Value = data.GetValue();
	NullFlag = data.isNull();
	return *this;
}

bool DOUBLE::operator==(DOUBLE &b)
{
	if (this->isNull() == false && b.isNull() == false)
	{
		if (this->GetValue() == b.GetValue())
			return true;
		else
			return false;
	}
	else if (this->isNull() == true || b.isNull() == true)
	{
		return false;
	}
	return false;
}

bool DOUBLE::operator<(DOUBLE &b)
{
	return this->GetValue() < b.GetValue();
}
bool DOUBLE::operator>(DOUBLE &b)
{
	return this->GetValue() > b.GetValue();
}
bool DOUBLE::operator<=(DOUBLE &b)
{
	return this->GetValue() <= b.GetValue();
}
bool DOUBLE::operator>=(DOUBLE &b)
{
	return this->GetValue() >= b.GetValue();
}
bool DOUBLE::operator!=(DOUBLE &b)
{
	return !(*this == b);
}

ostream &operator<<(ostream &out, DOUBLE &d)
{
	if (!d.isNull())
	{
		out << d.GetValue();
	}
	return out;
}

void CHAR::SetValue(char d)
{
	Value = d;
}

char CHAR::GetValue()
{
	return Value;
}

CHAR &CHAR::operator=(CHAR &data)
{
	Value = data.GetValue();
	NullFlag = data.isNull();
	return *this;
}

bool CHAR::operator==(CHAR &b)
{
	if (this->isNull() == false && b.isNull() == false)
	{
		if (this->GetValue() == b.GetValue())
			return true;
		else
			return false;
	}
	else if (this->isNull() == true || b.isNull() == true)
	{
		return false;
	}
	return false;
}

bool CHAR::operator<(CHAR &b)
{
	return this->GetValue() < b.GetValue();
}
bool CHAR::operator>(CHAR &b)
{
	return this->GetValue() > b.GetValue();
}
bool CHAR::operator<=(CHAR &b)
{
	return this->GetValue() <= b.GetValue();
}
bool CHAR::operator>=(CHAR &b)
{
	return this->GetValue() >= b.GetValue();
}
bool CHAR::operator!=(CHAR &b)
{
	return !(*this == b);
}

ostream &operator<<(ostream &out, CHAR &d)
{
	if (!d.isNull())
	{
		out << d.GetValue();
	}
	return out;
}
