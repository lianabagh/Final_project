#pragma once
#include"Database.h"
#include<iostream>

class Transaction {
public:
	Transaction(Database& db, Func_Name type = Write)
		:db(db)
		, type(type)
	{

	}

	Transaction()
	{

	}
	~Transaction()
	{

	}


	void start()
	{
		std::cout << "Transaction started" << std::endl;
	}


	void commit()
	{
		std::cout << "Transaction commited" << std::endl;
	}
	Func_Name GetType()
	{
		return type;
	}
	void abort()
	{
		std::cout << "Transaction aborted " << std::endl;
	}
private: Database db;
		 Func_Name type;
};