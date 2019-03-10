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
	template <typename T>
	void runTransactional(std::function<void(TxRunner* tx, int a, T b)>&  func, int a, T b)
	{
		int First_Commit = 0;



		auto iter = map_.find(std::this_thread::get_id());
		if (iter == map_.end())
		{
			++First_Commit;
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
			map_.erase(std::this_thread::get_id());
			std::terminate();
		}
		if (First_Commit == 1)
		{
			iter2->second.commit();
			map_.erase(std::this_thread::get_id());
		}

	}


private:
	Database db;
	std::map<std::thread::id, Transaction> map_;
	std::mutex m;

};