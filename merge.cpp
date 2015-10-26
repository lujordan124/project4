#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <pthread.h>
#include <cstring>
#include <math.h>
#include <vector>
#include <algorithm>

using namespace std;

#define tokenDelim " "

pthread_mutex_t barrier_mutex;
int barrierCount = 0;
int barrierBool = 0;
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

void barrier_signal() {
  	barrierBool = 1;
}

void barrier_wait() {
	pthread_mutex_lock(&barrier_mutex);
	barrierCount--;
	pthread_mutex_unlock(&barrier_mutex);
	if (barrierCount == 0) {
		barrier_signal();
	}
}

void *sortLocal(void* args) {  
	struct mergeData *mergeD = (struct mergeData*)args;
	sort(data.begin() + mergeD->start, data.begin() + mergeD->end, comparator);
	barrier_wait();
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
	int threadsMade = 0;

	pthread_t threads[NUM_THREADS];

	int i;
	int j;
	for (i = 0; i < numCycles; i++) {
		threadsMade = 0;
		while (barrierBool == 0) {
		    if (threadsMade == 0) {
		      for (j = 0; j < numGroups; j++) {
			      struct mergeData *params = new mergeData;
			      params->start = 2 * j * pow(2, pow2);
			      params->end = 2 * j * pow(2, pow2) + groupSize - 1;
			      //cout << params->start << " " << params->end << endl	;
			      pthread_create(&threads[threadCount], NULL, &sortLocal, params);
			      threadCount++;
		      }
		      threadsMade = 1;
		    }
		}

		barrierBool = 0;

		//SIGNAL BARRIER
		// barrier_signal(c);
		
		if (i < numCycles - 1) {
			pow2++;
			numGroups = numGroups / 2;
			groupSize = count / numGroups;
			barrierCount = numGroups;
		}
	}

	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	for (i = 0; i < count; i++) {
		cout << data[i] << " ";
	}
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