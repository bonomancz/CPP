#include "threads.h"

std::unordered_map<std::thread::id, ThreadInfo>& Threads::getThreadPool() {
	return this->threadPool;
}

void Threads::removeThread(std::thread::id& threadID) {
	{
		std::lock_guard<std::mutex> lock(threadPoolMutex);
		if (this->threadPool[threadID].threadObj.joinable()) {
			this->threadPool[threadID].threadObj.join();
		}
		this->threadPool.erase(threadID);
	}
}

void Threads::removeAllThreads() {
	{
		std::lock_guard<std::mutex> lock(threadPoolMutex);
		for (auto& mapItem : this->threadPool) {
			auto& key = mapItem.first;
			auto& value = mapItem.second;
			if (value.threadObj.joinable()) {
				value.threadObj.join();
			}
		}
		this->threadPool.clear();
	}
}

void Threads::removeFinishedThreads() {
	std::lock_guard<std::mutex> lock(threadPoolMutex);
	for (auto iter = this->threadPool.begin(); iter != this->threadPool.end();) {
		auto& key = iter->first;
		auto& value = iter->second;
		if (value.isFinished == true) {
			if (value.threadObj.joinable()) {
				value.threadObj.join();
			}
			iter = this->threadPool.erase(iter);
		}
		else {
			iter++;
		}
	}
}

string Threads::getThreadPoolStatistic() {
	string statistic = "################ THREADS ###################\n";
	{
		std::lock_guard<std::mutex> lock(threadPoolMutex);
		for (auto& mapItem : this->threadPool) {
			auto& key = mapItem.first;
			auto& value = mapItem.second;
			ostringstream oss;
			oss << key;
			string threadId = oss.str();
			string state = "Working";
			if (value.isFinished == true) {
				state = "Finished";
			}
			statistic += "ThreadID " + threadId + ": " + state + ".\n";
		}
	}
	return(statistic);
}

void Threads::setThreadFinished(std::thread::id& threadID) {
	{
		std::lock_guard<std::mutex> lock(threadPoolMutex);
		this->threadPool[threadID].isFinished = true;
	}
}

int Threads::getThreadPoolSize() {
	{
		std::lock_guard<std::mutex> lock(threadPoolMutex);
		return static_cast<int>(this->threadPool.size());
	}
}
