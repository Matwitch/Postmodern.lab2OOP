#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

// ITERATOR ТА ITERABLE КЛАСИ
template<typename T>
class Iterator
{
public:
	virtual Iterator* duplicate() = 0;
	virtual T& get() = 0;
	virtual Iterator& next() = 0;
	virtual bool valid() = 0;
};


template<typename T>
class Iterable
{
public:
	virtual Iterator<T>* begin() = 0;
	bool is_equal(Iterable<T>* rhs)
	{
		Iterator<T>* t1 = begin();
		Iterator<T>* t2 = rhs->begin();

		for (; t1->valid() && t2->valid(); t1->next(), t2->next())
		{
			if (t1->get() != t2->get())
			{
				delete t1;
				delete t2;
				return false;
			}
		}

		delete t1;
		delete t2;

		return true;
	}

	void bubble_sort()
	{
		Iterator<T>* i = begin();
		for (; i->valid(); i->next())
		{
			Iterator<T>* j = begin();
			Iterator<T>* k = j->duplicate();
			k->next();
			for (; k->valid(); j->next(), k->next())
			{
				if (j->get() > k->get())
					std::swap(j->get(), k->get());
			}
			delete j;
			delete k;
		}
		delete i;
	}
};



// Далі тут фейкова страктура даних з ітератором чисто для тесту, ніякого сенсу там нема :)
class int_iterator : public Iterator<int>
{
private:
	std::vector<int>* owner;
	int counter = 0;

public:
	Iterator* duplicate() override
	{
		int_iterator* temp = new int_iterator(owner);
		temp->counter = counter;
		return temp;
	}
	int_iterator(std::vector<int>* ownr)
	{
		owner = ownr;
	}

	int& get() override
	{
		return owner->at(counter);
	}
	int_iterator& next() override
	{
		++counter;
		return *this;
	}
	bool valid() override
	{
		return counter < owner->size();
	}

};

class TestSeq : public Iterable<int>
{
private:
	std::vector<int> v;

public:
	TestSeq(std::vector<int> _v)
	{
		v = _v;
	}

	std::vector<int>& get_vec()
	{
		return v;
	}

	Iterator<int>* begin() override
	{
		return new int_iterator(&v);
	}

	void display()
	{
		for (int a : v)
			std::cout << a << ' ';

		std::cout << std::endl;
	}
};


// Набір абстрактних property-based тестів для перевірки Iterable-складової довільної реалізації Iterable.
template<typename T>
bool Iterator_Correct_Size(Iterable<T>* seq, size_t seq_size)
{
	Iterator<T>* t = seq->begin();

	for (size_t i = 0; i < seq_size; i++)
		if (t->valid())
			t->next();
		else
		{
			delete t;
			return false;
		}

	return !(t->valid());
}

template<typename T>
bool Iterator_Correct(Iterable<T>* seq, const std::vector<T>& ref_seq)
{
	Iterator<T>* t = seq->begin();
	std::vector<T> v;

	for (; t->valid(); t->next())
	{
		v.push_back(t->get());
	}

	delete t;
	
	return v == ref_seq;
}

template<typename T>
bool Is_Equal_Correct(Iterable<T>* seq)
{
	return seq->is_equal(seq);
}

template<typename T>
bool Duplicate_Iterator_Correct(Iterable<T>* seq)
{
	Iterator<T>* t1 = seq->begin();
	Iterator<T>* t2 = t1->duplicate();

	for (; t1->valid(); t1->next(), t2->next())
	{
		if (t1->get() != t2->get())
		{
			delete t1;
			delete t2;
			return false;
		}
	}

	Iterator<T>* t3 = seq->begin();
	Iterator<T>* t4 = t3->duplicate();

	for (; t4->valid(); t3->next(), t4->next())
	{
		if (t3->get() != t4->get())
		{
			delete t3;
			delete t4;
			return false;
		}
	}
	delete t1;
	delete t2;
	delete t3;
	delete t4;

	return true;
}

template<typename T>
bool Sort_Correct1(Iterable<T>* seq, Iterable<T>* ref_seq)
{
	seq->bubble_sort();
	return seq->is_equal(ref_seq);
}

template<typename T>
bool Sort_Correct2(Iterable<T>* seq, Iterable<T>* copy_seq)
{
	seq->bubble_sort();
	copy_seq->bubble_sort();
	return seq->is_equal(copy_seq);
}


// Загальний тест, який генерує випадкові послідовності і ганяє їх по тестам, визначеним зверху.
bool Test(size_t tests_num, size_t max_seq_size)
{
	std::vector<std::vector<int>> test_sequences;

	test_sequences.push_back(std::vector<int>());

	for (size_t i = 0; i < tests_num; ++i)
	{
		std::vector<int> temp(rand() % max_seq_size);
		for (int& a : temp)
			a = rand() % 10000;

		test_sequences.push_back(temp);
	}

	for (std::vector<int>& v : test_sequences)
	{
		TestSeq* temp = new TestSeq(v);

		TestSeq* temp3 = new TestSeq(v);

		bool flag1 = Iterator_Correct_Size<int>(temp, v.size());
		bool flag2 = Iterator_Correct<int>(temp, v);
		bool flag3 = Duplicate_Iterator_Correct<int>(temp);
		bool flag4 = Is_Equal_Correct(temp);

		std::sort(v.begin(), v.end());
		TestSeq* temp2 = new TestSeq(v);
		bool flag5 = Sort_Correct1<int>(temp, temp2);

		
		bool flag6 = Sort_Correct2<int>(temp, temp3);

		if (!(flag1 && flag2 && flag3 && flag4 && flag5 && flag6))
			return false;
	}

	return true;
}


// Мейн :)
int main()
{
	std::cout << Test(25, 200);
	
	return 0;
}