/**
 * convert.c
 *
 * Copyright (c) 2012 Rifur Ni, Joe Angel and Seachaos Ye.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */

#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <cstring>

using namespace std;

class wordNode
{
public:
	string spelling;
	string word;
};

class WordTable
{
public:
	WordTable();
	void append(string str);
	void sort();
	void show();
	string search(string str);

private:
	void quicksort(int left, int right);

private:
	wordNode *table[40960];
	int length;
};

WordTable::WordTable()
{
	length = 0;
}


void WordTable::append(string str)
{
	char *s = (char *)str.c_str();
	table[length] = new wordNode;

	table[length]->spelling = strtok(s, " ");
	table[length]->word = strtok(NULL, " ");

	length += 1;
}

void WordTable::show()
{
	for(int i=0; i<length; ++i) {
		cout << table[i]->word << endl;
	}
}

void WordTable::sort()
{
	quicksort(0, length-1);
}

void WordTable::quicksort(int left, int right)
{
	wordNode *tmp;

	if(left < right) {
		int i = left; 
		int j = right+1;

		while(true) {
			while((i+1 < length) && (table[++i]->word.compare(table[left]->word) < 0));
			while((j-1 > -1) && (table[--j]->word.compare(table[left]->word) > 0));

			if(i >= j) { 
				break;	
			}

			tmp = table[i];
			table[i] = table[j];
			table[j] = tmp;
		}

		tmp = table[left];
		table[left] = table[j];
		table[j] = tmp;

		quicksort(left, j-1);
		quicksort(j+1, right);
	}
}

string WordTable::search(string str)
{
	int k = length >> 1;
	int left = 0;
	int right = length - 1;

	while(left <= right) {

		k = (left + right) >> 1;
		if(str.compare(table[k]->word) > 0) { left = k+1; }
		else if (str.compare(table[k]->word) < 0) { right = k-1; }
		else { 
			return table[k]->spelling;
		}
	}

	return "";

}

string key2Zhuin(string str)
{
	char *key = (char *)str.c_str();
	int len = strlen(key);
	string s = "";
	bool split = true;

	static const char *ph_str[] =
	{"\xE3\x84\x85", "\xE3\x84\x86", "\xE3\x84\x87", "\xE3\x84\x88",
		/* ㄅㄆㄇㄈ */
	"\xE3\x84\x89", "\xE3\x84\x8A", "\xE3\x84\x8B", "\xE3\x84\x8C",
		/* ㄉㄊㄋㄌ */
	"\xE3\x84\x8D", "\xE3\x84\x8E", "\xE3\x84\x8F", "\xE3\x84\x90",
		/* ㄍㄎㄏㄐ */
	"\xE3\x84\x91", "\xE3\x84\x92", "\xE3\x84\x93", "\xE3\x84\x94",
		/* ㄑㄒㄓㄔ */
	"\xE3\x84\x95", "\xE3\x84\x96", "\xE3\x84\x97", "\xE3\x84\x98",
		/* ㄕㄖㄗㄘ */
	"\xE3\x84\x99", "\xE3\x84\xA7", "\xE3\x84\xA8", "\xE3\x84\xA9",
		/* ㄙㄧㄨㄩ */
	"\xE3\x84\x9A", "\xE3\x84\x9B", "\xE3\x84\x9C", "\xE3\x84\x9D",
		/* ㄚㄛㄜㄝ */
	"\xE3\x84\x9E", "\xE3\x84\x9F", "\xE3\x84\xA0", "\xE3\x84\xA1",
		/* ㄞㄟㄠㄡ */
	"\xE3\x84\xA2", "\xE3\x84\xA3", "\xE3\x84\xA4", "\xE3\x84\xA5", "\xE3\x84\xA6",
		/* ㄢㄣㄤㄥㄦ */
	"\xCB\x99", "\xCB\x8A", "\xCB\x87", "\xCB\x8B"};
		/* ˙ˊˇˋ */

	static char key_str[] = "1qaz2wsxedcrfv5tgbyhnujm8ik,9ol.0p;/-7634";		/* standard kb */
	static int	key_len = strlen(key_str);

	for(int i=0; i<len; ++i) {
		split = true;
		for(int j=0; j<key_len; ++j) {
			if(key_str[j] == key[i]) {
				s.append(ph_str[j]);
				split = false;
				break;
			}
		}

		if(split) {
			s.append(" ");
		}
	}
	return s;
}


int main(int argc, char *argv[]) 
{
	long time1, time2;
	time1 = time(0);
	char buf[512];

	string	line;
	int		wideCnt;
	bool	allChi;

	WordTable WT;

	ifstream infile;
	ofstream outfile;
	sprintf(buf, "%s_out", argv[1]);

	infile.open(argv[1], ios::binary);
	outfile.open(buf, ios::out);
	
	// sort
	ifstream wordTableFile;
	wordTableFile.open("phone.cin", ios::in);
	while(getline(wordTableFile, line)) {
		WT.append(line);
	}
	WT.sort();
	wordTableFile.close();


	while(getline(infile, line))
	{
		allChi = true;
		wideCnt = strlen(line.c_str());

		if(wideCnt % 3 != 0)
			continue;

 
		string zhuin = "";
		for(int i=0; i<wideCnt; i+=3) {

			string tmp = WT.search(line.substr(i, 3));

			if(tmp == "") {
				allChi = false;
				break;
			}

			zhuin.append(tmp + " ");
		}

		if(allChi) {
			outfile << line << "\t" << key2Zhuin(zhuin) << "\t" << "0\t0\t0\t0" << endl;
		}
	}


	infile.close();
	outfile.close();

	time2 = time(0);
	cout << time2 - time1 << " Seconds." << endl;

	WT.~WordTable();

}
