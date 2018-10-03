#include "pch.h"
//#include "stdafx.h"
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
	pthread_mutex_t inputStackMutex;
	pthread_mutex_t writeQueueMutex;
	pthread_cond_t condition;
	shared_ptr<stack<int>> inputStack;
	shared_ptr<queue<int>> outputQueue;
};

struct WriteThreadInput
{
	pthread_mutex_t inputStackMutex;
	pthread_mutex_t writeQueueMutex;
	pthread_cond_t condition;
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
	size_t threadCount = 2;
	Quest(threadCount);
	cout << "Hello World!" << endl;
	return 0;
}

void Quest(size_t countThread)
{
	vector<ThreadPair> threadPairs;
	shared_ptr<stack<int>> inputStack(new stack<int>(SeedData(10000)));

	pthread_mutex_t inputStackMutex;
	pthread_mutex_init(&inputStackMutex, nullptr);

	for (size_t i = 0; i < countThread; i++)
	{
		ThreadPair pair;
		CalcThreadInput *calcInput = new CalcThreadInput();
		WriteThreadInput *writeInput = new WriteThreadInput();
		pthread_cond_t condition;
		shared_ptr<queue<int>> writeQueue(new queue<int>());

		writeInput->inputStack = calcInput->inputStack = inputStack;

		pthread_cond_init(&condition, nullptr);
		pthread_mutex_init(&calcInput->writeQueueMutex, nullptr);
		writeInput->writeQueueMutex = calcInput->writeQueueMutex;

		writeInput->inputStackMutex = calcInput->inputStackMutex = inputStackMutex;

		writeInput->writeQueue = calcInput->outputQueue = writeQueue;

		writeInput->condition = calcInput->condition = condition;

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
		pthread_mutex_lock(&castInput->inputStackMutex);

		if (castInput->inputStack->empty())
		{
			pthread_mutex_unlock(&castInput->inputStackMutex);
			pthread_cond_signal(&castInput->condition);
			pthread_exit(nullptr);
		}
		auto value = castInput->inputStack->top();
		castInput->inputStack->pop();

		pthread_mutex_unlock(&castInput->inputStackMutex);

		if (Prime(value))
		{
			pthread_mutex_lock(&castInput->writeQueueMutex);

			castInput->outputQueue->push(value);

			pthread_mutex_unlock(&castInput->writeQueueMutex);

			pthread_cond_signal(&castInput->condition);
		}
	}
}

void* WriteingThread(void* input)
{
	unique_ptr<WriteThreadInput> castedInput(static_cast<WriteThreadInput*>(input));

	ofstream fout(castedInput->outputFileName);

	while (true)
	{
		pthread_mutex_lock(&castedInput->inputStackMutex);
		pthread_mutex_lock(&castedInput->writeQueueMutex);


		while (castedInput->writeQueue->empty())
		{
			if (castedInput->inputStack->empty())
			{
				pthread_mutex_unlock(&castedInput->inputStackMutex);
				pthread_mutex_unlock(&castedInput->writeQueueMutex);
				fout.close();
				pthread_exit(nullptr);
			}
			else
			{
				pthread_mutex_unlock(&castedInput->inputStackMutex);
				pthread_cond_wait(&castedInput->condition, &castedInput->writeQueueMutex);
				pthread_mutex_lock(&castedInput->inputStackMutex);
			}
		}

		fout << castedInput->writeQueue->front() << endl;

		castedInput->writeQueue->pop();

		pthread_mutex_unlock(&castedInput->inputStackMutex);
		pthread_mutex_unlock(&castedInput->writeQueueMutex);
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