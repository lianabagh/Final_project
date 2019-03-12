
#include"stdafx.h"
#include<iostream>
#include"Database.h"
#include"TxRunner.h"
#include<future>
#include<vector>

int main() {
	//std::vector<int> vec(2);
	Database db;
	TxRunner tx_runner(db);
	//std::cout << "First test.\n In lambda function we calculate a + b function on every recursive call until 'a' is not 0.\n";
	std::function<void(TxRunner* tx, int a, double b, Func_Name type)> func1 = [&func1](TxRunner* tx, int a, double b, Func_Name type) {
		if (a == 0)
			return;
		std::cout << "In lambda func: a+b=" << a + b << std::endl;
		// Call some write method on transaction database

		tx->get_db().write(type, a + b);

		tx->runTransactional(func1, a - 1, b);
	};
	std::function<void(TxRunner* tx, int a, double b, Func_Name type)> func2 = [&func2](TxRunner* tx, int a, double b, Func_Name type) {
		if (a == 0)
			return;
		std::cout << "In lambda func: a+b=" << a + b << std::endl;

		// Call some write method on transaction database
		tx->get_db().write(type, a + b);
		tx->runTransactional(func2, a - 1, b);
	};

	auto f1 = std::async([&]()
	{
		tx_runner.runTransactional<double>(func1, 5, 8.5);
	});

	std::cout << "\nSecond test.\n Here we submit a function that throws an exception\n";
	std::function<void(TxRunner* tx, int a, int b, Func_Name type)> func_error = [](TxRunner* tx, int a, int b, Func_Name type) {
		if (b == 0)
			throw std::runtime_error("Division to zero");

		// Call some write method on transaction database
		tx->get_db().write(type, a / b);
	};
	
	auto f2 = std::async([&]()
	{
	tx_runner.runTransactional<int>(func_error, 10, 0);
	});
		f1.get();
	//f2.get();
	//system("pause");
	return 0;
}