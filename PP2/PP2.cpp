#include "pch.h"
#include <iostream>
#include <C:\Users\Геральт из Ривии\source\repos\PP2\pthread.h>
#include <stack>
#include <queue>
#include <math.h>
#include <fstream>
#include <memory>
#include <string>

using namespace std;

struct ThreadPair
{
	pthread_t calcThread;
	pthread_t writeThread;
};

struct CalcThreadInput
{
	shared_ptr<pthread_mutex_t> inputStackMutex;
	shared_ptr<pthread_mutex_t> writeQueueMutex;
	shared_ptr<pthread_cond_t> condition;
	shared_ptr<stack<int>> inputStack;
	shared_ptr<queue<int>> outputQueue;
};

struct WriteThreadInput
{
	shared_ptr<pthread_mutex_t> inputStackMutex;
	shared_ptr<pthread_mutex_t> writeQueueMutex;
	shared_ptr<pthread_cond_t> condition;
	shared_ptr<stack<int>> inputStack;
	shared_ptr<queue<int>> writeQueue;
	string outputFileName;
};

stack<int> SeedData(size_t count);

void Quest(size_t countThread);

void* CalcualtingThread(void* input);

void* WriteingThread(void* input);

bool Prime(int n);

/////////////////Реализация

int main()
{
	const size_t threadCount = 2;
	Quest(threadCount);
	cout << "Hello World!" << endl;
	return 0;
}

void Quest(size_t countThread)
{
	vector<ThreadPair> threadPairs;
	shared_ptr<stack<int>> inputStack(new stack<int>(SeedData(1000000)));
	shared_ptr<pthread_mutex_t> inputStackMutex(new pthread_mutex_t);

	if (0 != pthread_mutex_init(inputStackMutex.get(), nullptr))
	{
		cout << "Wrong input stack mutex" << endl;
	}

	for (size_t i = 0; i < countThread; i++)
	{
		ThreadPair pair;
		CalcThreadInput *calcInput = new CalcThreadInput();
		WriteThreadInput *writeInput = new WriteThreadInput();
		shared_ptr<pthread_cond_t> condition(new pthread_cond_t);
		shared_ptr<queue<int>> writeQueue(new queue<int>());
		shared_ptr<pthread_mutex_t> writeQueueMutex(new pthread_mutex_t);

		calcInput->condition = writeInput->condition = condition;
		calcInput->inputStackMutex = writeInput->inputStackMutex = inputStackMutex;
		calcInput->inputStack = writeInput->inputStack = inputStack;
		calcInput->writeQueueMutex = writeInput->writeQueueMutex = writeQueueMutex;
		calcInput->outputQueue = writeInput->writeQueue = writeQueue;

		if (0 != pthread_cond_init(condition.get(), nullptr))
		{
			cout << "Wrong cond init" << endl;
		};
		if (0 != pthread_mutex_init(writeQueueMutex.get(), nullptr))
		{
			cout << "Wrong writeQueueMutex";
		}

		writeInput->outputFileName.append(to_string(i) + ".txt");

		pthread_create(&pair.calcThread, nullptr, CalcualtingThread, calcInput);
		pthread_create(&pair.writeThread, nullptr, WriteingThread, writeInput);

		auto wecoj = writeInput->inputStack.use_count();

		threadPairs.push_back(pair);
	}

	for (ThreadPair pair : threadPairs)
	{
		void ** result = nullptr;
		pthread_join(pair.calcThread, result);
		pthread_join(pair.writeThread, result);
	}
}

void* CalcualtingThread(void* input)
{
	unique_ptr<CalcThreadInput> castInput(static_cast<CalcThreadInput*>(input));

	while (true)
	{
		if (0 == pthread_mutex_lock(castInput->inputStackMutex.get()))
		{
			//cout<<"InputStackMutex"<< castInput->inputStackMutex << "unlocked"<<endl;
		};

		if (castInput->inputStack->empty())
		{
			pthread_mutex_unlock(castInput->inputStackMutex.get());
			pthread_cond_signal(castInput->condition.get());
			pthread_exit(nullptr);
		}
		auto value = castInput->inputStack->top();
		castInput->inputStack->pop();

		if (0 == pthread_mutex_unlock(castInput->inputStackMutex.get()))
		{
			//cout<<"InputStackMutex"<< castInput->inputStackMutex << "unlocked"<<endl;
		}

		if (Prime(value))
		{
			pthread_mutex_lock(castInput->writeQueueMutex.get());

			castInput->outputQueue->push(value);

			pthread_cond_signal(castInput->condition.get());

			pthread_mutex_unlock(castInput->writeQueueMutex.get());
		}
	}
}

void* WriteingThread(void* input)
{
	unique_ptr<WriteThreadInput> castedInput(static_cast<WriteThreadInput*>(input));

	ofstream fout(castedInput->outputFileName);

	while (true)
	{
		pthread_mutex_lock(castedInput->writeQueueMutex.get());


		while (castedInput->writeQueue->empty())
		{
			pthread_mutex_lock(castedInput->inputStackMutex.get());
			auto isInputEmpty = castedInput->inputStack->empty();
			pthread_mutex_unlock(castedInput->inputStackMutex.get());

			if (isInputEmpty)
			{
				pthread_mutex_unlock(castedInput->writeQueueMutex.get());
				fout.close();
				pthread_exit(nullptr);
			}

			pthread_cond_wait(castedInput->condition.get(), castedInput->writeQueueMutex.get());
		}

		fout << castedInput->writeQueue->front() << endl;

		castedInput->writeQueue->pop();

		pthread_mutex_unlock(castedInput->writeQueueMutex.get());
	}
}

bool Prime(int n)
{
	for (int i = 2; i <= sqrt(n); i++)
		if (n%i == 0)
			return false;
	return true;
}

stack<int> SeedData(size_t count)
{
	stack<int> enter;

	for (size_t i = 0; i < count; i++)
	{
		enter.push(rand());
	}
	return enter;
}