#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <omp.h>

using namespace std;
typedef unordered_map<string, size_t> dictionary;
typedef pair<string, size_t> couple;

bool compare(couple i, couple j) {
	return (i.second > j.second); 
}

string toLowerStr(string & s) {
	for (size_t i = 0; i < s.size(); ++i)
		s[i] = tolower(s[i]);
	return s;
}

void countLine(string & line, dictionary & dic) {
	string s = line;
	smatch m;
	regex e("\\w+'\\w+|\\w+");

	while (regex_search(s, m, e)) {
		for (string x : m)
#pragma omp critical
			dic[toLowerStr(x)]++;
		s = m.suffix().str();
	}
}

int main(int argc, char * argv[]) {
	dictionary dic;
	string finn, foutn;
	fstream fins, fouts;
	vector<string> lines;
	double start, end;

	if (argc != 3) {
		cout << "Wrong params." << endl;
		cout << "Usage: WordCounter input_file_name output_file_name" << endl;
		return -1;
	} 

	start = omp_get_wtime();
	finn = argv[1];
	fins.open(finn, fstream::in);
	if (!fins.is_open()) {
		cout << "Cannot open input file " + finn << endl;
		return -1;
	}
	else {
		string line;
		while (getline(fins, line)) {
			//cout << line << endl;
			lines.push_back(line);
		}

		int len = lines.size();
#pragma omp parallel for schedule(static) 
		for (int i = 0; i < len; ++i)
			countLine(lines[i], dic);
	}

	foutn = argv[2];
	fouts.open(foutn, fstream::out | fstream::trunc);
	if (!fouts.is_open()) {
		cout << "Cannot open output file " + foutn << endl;
	}
	else {
		vector<couple> pairs;
		for (auto pair : dic) pairs.push_back(pair);

		sort(pairs.begin(), pairs.end(), compare);

		for (size_t i = 0; i < pairs.size(); ++i)
			fouts << pairs[i].first << " " << pairs[i].second << endl;
	}
	

	fins.close();
	fouts.close();

	end = omp_get_wtime();
	cout << "Time used: " << end - start << "s" << endl;

	return 0;
}