#pragma once
#include<thread>

#include"Database.h"
#include<functional>
#include"Transaction.h"
#include<map>
#include<mutex>
class TxRunner
{
public:
	TxRunner(Database& db)
		:db(db)
	{

	}

	Database& get_db()
	{
		return db;
	}

	void runTransactional(std::function<void(TxRunner* tx, int a, double b)>&  func,int a,double b)
	{
		
		
			
			
			auto iter = map_.find(std::this_thread::get_id());
			if (iter == map_.end())
			{
				std::lock_guard<std::mutex> ob(m);
				Transaction obj(db);
				obj.start();
				map_.insert({ std::this_thread::get_id(),obj });
			}
	
			auto iter2 = map_.find(std::this_thread::get_id());

			try {
				func(this, a, b);
			}
			catch (const std::runtime_error& error)
			{
				iter2->second.abort();
				std::terminate();
			}
			iter2->second.commit();
	}

	void runTransactional(std::function<void(TxRunner* tx, int a, int b)>&  func, int a, int b)
	{
		auto iter = map_.find(std::this_thread::get_id());
		if (iter == map_.end())
		{
			std::lock_guard<std::mutex> ob(m);
			Transaction obj(db);
			obj.start();
			map_.insert({ std::this_thread::get_id(),obj });
		}

		auto iter2 = map_.find(std::this_thread::get_id());

		try {
			func(this, a, b);
		}
		catch (const std::runtime_error& error)
		{
			iter2->second.abort();
			std::terminate();
		}
		iter2->second.commit();
	}
private:
	Database db;
	std::map<std::thread::id,Transaction> map_;
	//Transaction obj;
	std::mutex m;

};