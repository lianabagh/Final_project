#pragma once
#include"Database.h"
#include<iostream>
class Transaction {
public:
	Transaction(Database& db)
		:db(db)
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

	void abort()
	{
		std::cout << "Transaction aborted " << std::endl;
	}
private: Database db;
};