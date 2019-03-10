#pragma once
#include<vector>
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
	void write(int value)
	{
		vec.push_back(value);
	}
};