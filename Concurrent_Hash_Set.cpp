#include"stdafx.h"
#include "test_runner.h"

#include <forward_list>
#include <iterator>
#include <vector>
#include <mutex>

using namespace std;

template <typename T>
class Synchronized {
public:
	explicit Synchronized(T initial = T())
		: value(move(initial))
	{
	}

	struct Access {
		Access(T& ref_to_value, mutex& mutex)
			: ref_to_value(ref_to_value)
			, guard(mutex)
		{

		}

		T& ref_to_value;
		lock_guard<mutex> guard;
	};

	Access GetAccess() {
		return { value, m };
	}

	const T& GetRefAccess() const {
		return value;
	}

private:
	T value;
	mutex m;
};

template <typename Type, typename Hasher = hash<Type>>
class HashSet {
public:
	using BucketList = forward_list<Type>;

public:
	explicit HashSet(size_t num_buckets, const Hasher& hasher = {})
		: data_(num_buckets),
		num_buckets_(num_buckets),
		hasher_(hasher)
	{}

	void Add(const Type& value) {
		size_t index = hasher_(value) % num_buckets_;
		for (const Type& x : data_[index].GetAccess().ref_to_value) {
			if (x == value)
				return;
		}
		data_[index].GetAccess().ref_to_value.push_front(std::move(value));
	}
	bool Has(const Type& value) const {
		size_t index = hasher_(value) % num_buckets_;
		for (const Type& x : data_[index].GetRefAccess()) {
			if (x == value)
				return true;
		}
		return false;
	}
	void Erase(const Type& value) {
		size_t index = hasher_(value) % num_buckets_;
		data_[index].GetAccess().ref_to_value.remove_if([&value](const Type& x) { return x == value; });
	}

	const BucketList& GetBucket(const Type& value) const {
		return data_[hasher_(value) % num_buckets_].GetRefAccess();
	}

private:
	size_t num_buckets_;
	Hasher hasher_;
	vector<Synchronized<BucketList>> data_;
};

struct IntHasher {
	size_t operator()(int value) const {
		return value;
	}
};

struct TestValue {
	int value;

	bool operator==(TestValue other) const {
		return value / 2 == other.value / 2;
	}
};

struct TestValueHasher {
	size_t operator()(TestValue value) const {
		return value.value / 2;
	}
};

void TestSmoke() {
	HashSet<int, IntHasher> hash_set(2);
	hash_set.Add(3);
	hash_set.Add(4);

	auto val = hash_set.Has(3);
	ASSERT(val);
	ASSERT(hash_set.Has(4));
	ASSERT(!hash_set.Has(5));

	hash_set.Erase(3);

	ASSERT(!hash_set.Has(3));
	ASSERT(hash_set.Has(4));
	ASSERT(!hash_set.Has(5));

	hash_set.Add(3);
	hash_set.Add(5);

	ASSERT(hash_set.Has(3));
	ASSERT(hash_set.Has(4));
	ASSERT(hash_set.Has(5));
}

void TestEmpty() {
	HashSet<int, IntHasher> hash_set(10);
	for (int value = 0; value < 10000; ++value) {
		ASSERT(!hash_set.Has(value));
	}
}

void TestIdempotency() {
	HashSet<int, IntHasher> hash_set(10);
	hash_set.Add(5);
	ASSERT(hash_set.Has(5));
	hash_set.Add(5);
	ASSERT(hash_set.Has(5));
	hash_set.Erase(5);
	ASSERT(!hash_set.Has(5));
	hash_set.Erase(5);
	ASSERT(!hash_set.Has(5));
}

void TestEquivalence() {
	HashSet<TestValue, TestValueHasher> hash_set(10);
	hash_set.Add(TestValue{ 2 });
	hash_set.Add(TestValue{ 3 });

	ASSERT(hash_set.Has(TestValue{ 2 }));
	ASSERT(hash_set.Has(TestValue{ 3 }));

	const auto& bucket = hash_set.GetBucket(TestValue{ 2 });
	const auto& three_bucket = hash_set.GetBucket(TestValue{ 3 });
	ASSERT_EQUAL(&bucket, &three_bucket);

	ASSERT_EQUAL(1, distance(begin(bucket), end(bucket)));
	ASSERT_EQUAL(2, bucket.front().value);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestSmoke);
	RUN_TEST(tr, TestEmpty);
	RUN_TEST(tr, TestIdempotency);
	RUN_TEST(tr, TestEquivalence);
	return 0;
}