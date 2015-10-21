#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <pthread.h>
#include <cstring>
#include <mutex>
#include <math.h>
#include <vector>
#include <condition_variable>

using namespace std;

#define tokenDelim " "

mutex barrier_mutex;
int barrierCount = 0;
vector<int> data;

struct mergeData {
	vector<int> a;
	int start;
	int end;
};

struct sorter {
	bool operator() (int i, int j) {
		return (i < j);
	}
} comparator;

void barrier_signal(condition_variable c) {
	c.notify_all();
}

int barrier_wait() {
	barrier_mutex.lock();
	barrierCount--;
	barrier_mutex.unlock();
	return barrierCount;
}

void *sortLocal(void* args) {
	struct mergeData *mergeD = (struct mergeData*)args;
	sort(mergeD->a.begin(), mergeD->a.end(), comparator);
	int i = 0;
	int j = 0;
	for (i = mergeD->start; i <= mergeD->end; i++) {
		data[i] = mergeD->a[j];
		j++;
	}
	return NULL;
}

void parseLine(char *line) {
	char *token = strtok(line, tokenDelim);
	int count = 0;

	while (token) {
		int num = atoi(token);
		data.push_back(num);
		token = strtok(NULL, tokenDelim);
		count++;
	}

	int threadCount = 0;
	int NUM_THREADS = count - 1;	
	int numCycles = log(count)/log(2);

	int numGroups = count / 2;
	barrierCount = numGroups;
	int groupSize = count/numGroups;
	int pow2 = 0;

	pthread_t threads[NUM_THREADS];


	int i;
	int j;
	for (i = 0; i < numCycles; i++) {
		for (j = 0; j < numGroups; j++) {
			struct mergeData *params = new mergeData;
			vector<int> myVals(data.begin() + 2 * j * pow(2, pow2), data.begin() + 2 * j * pow(2, pow2) + groupSize - 1);
			params->a = myVals;
			params->start = 2 * j * pow(2, pow2);
			params->end = 2 * j * pow(2, pow2) + groupSize - 1;

			cout << params->start << " " << params->end << endl	;
			pthread_create(&threads[threadCount], NULL, &sortLocal, params);
			threadCount++;
		}
		if (i < numCycles - 1) {
			pow2++;
			numGroups = numGroups / 2;
			groupSize = count / numGroups;
			barrierCount = numGroups;
		}
	}

	cout << data[0] << data[1] << data[2] << data[3] << data[4] << data[5] << data[6] << data[7];
}

int main() {
	string line;

	ifstream myfile("indata.txt");
	if (myfile.is_open()) {
		while (getline(myfile, line))
		{
			char *data = new char[line.length() + 1];
			strcpy(data, line.c_str());
			parseLine(data);
		}

		myfile.close();
	} else {
		cout << "Unable to open file";
	}

	return 0;
}