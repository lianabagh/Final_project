#pragma once
#include<vector>
enum Func_Name
{
	Read, Write
};
class Database
{

private:
	std::vector<int> vec;
public:
	Database()
	{

	}
	~Database()
	{

	}
	void write(Func_Name type, int value)
	{
		if (type == Read)
			throw std::runtime_error("Write during a Read transaction");
		vec.push_back(value);
	}
	int read()
	{
		return  vec[0];
	}
};