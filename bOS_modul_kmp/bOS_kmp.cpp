#include "bOS_kmp.h"
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
using namespace std;
//PCB
//int io[4] - [id][mode][pozycja]
//io[1] - 1 "pisanie" 0 "czytanie"

int Kmp_fifo::pinit(int id)
{
	vector<inode_info>::iterator it;
	for (it = ftab.begin(); it != ftab.end(); it++) {
		if (it->nr == id) return -1;
	}

	inode_info temp;
	temp.nr = id;
	temp.writers = 0;
	temp.readers = 0;
	temp.f_readers = false;
	temp.occupy = false;
	temp.bg_buf = 0;
	temp.len_buf = 0;
	temp.w_acc = true;
	size = 0;

	ftab.push_back(temp);

	cout << "======================================================" << endl;
	cout << "Pipe: " << id << " created." << endl;
	return id;
}
 
int Kmp_fifo::popen(int id, char mode)
{
	int fd = -1;
	int y = find_id(id);
	if (y == -1) return fd;
	fd = -1;

	cout << "======================================================" << endl;
	cout << "Open pipe " << ftab[y].nr << " as " << mode << endl;

	switch (mode) {
	case 'W':
		if (!ftab[y].f_readers) return -2;
		ftab[y].writers++;
		ftab[y].w_acc = true;
		fd = 1;
		break;
	case 'R':
		ftab[y].f_readers = true;
		ftab[y].readers++;

		fd = 0;
		break;
	default:
		return fd;
	}

	if (first) {
		first = false;
		for (int i = 0;i < bsize;i++) ftab[y].bdata.push_back('0');
	}

	return fd;
}

std::string Kmp_fifo::pread(int *io, int amount, int first)
{
	int y = find_id(io[0]);
	if (io[1] != 0) return "null";
	if (ftab[y].occupy) return "occ";
	if (amount > bmess) return "null";
	else if (ftab[y].w_acc) {
		if (size - amount < 0) return "empty";
	}


	ftab[y].occupy = true;
	string temp;
	if (size + amount > bsize) amount = (size + amount) - bsize;
	int j = ftab[y].bg_buf;
	for (int i = 0;i < amount;i++) {
		temp.push_back(ftab[y].bdata[j + i]);
		ftab[y].bdata[j + i] = '0';
	}

	if (ftab[y].w_acc) {
		if (ftab[y].bg_buf + amount == bsize) ftab[y].bg_buf = 0;
		else if (ftab[y].bg_buf + amount == ftab[y].len_buf) ftab[y].len_buf -= amount;
		else {
			ftab[y].len_buf -= amount;
			ftab[y].bg_buf = 0;
		}
	}
	else ftab[y].len_buf = 0;

	inLine(amount, y);
	if (size - amount < 0) size = 0;
	else size -= amount;

	ftab[y].occupy = false;
	cout << "======================================================" << endl;
	cout << "Read from pipe" << ftab[y].nr << ":	" << temp << endl;
	return temp;
}

int Kmp_fifo::pwrite(int *io, string data)
{
	int y = find_id(io[0]);
	if (io[1] != 1) return -1;
	else if (ftab[y].f_readers == false) return -1;
	else if (size + data.size() > bsize || ftab[y].occupy) return -2;
	else if (data.size() > bmess) return -1;

	ftab[y].occupy = true;
	int j = ftab[y].len_buf;
	for (int i = 0;i < data.size();i++) {
		ftab[y].bdata[j + i] = data[i];
	}

	ftab[y].len_buf += data.size();
	size += data.size();

	ftab[y].occupy = false;

	cout << "======================================================" << endl;
	cout << "Write to pipe" << ftab[y].nr << ":	" << data << endl;
	return data.size();
}
 
void Kmp_fifo::pclose(int io[])
{
	int y = find_id(io[0]);

	cout << "======================================================" << endl;
	cout << "Close pipe" << ftab[y].nr << endl;
	switch (io[1]) {
	case 1:
		ftab[y].writers--;
		if(ftab[y].writers == 0) ftab[y].w_acc = false;
		break;
	case 0:
		ftab[y].readers--;
		if (ftab[y].readers == 0) ftab[y].f_readers = false;
		break;
	default:
		cout << "======================================================" << endl;
		cout << "Failed to close." << endl;
	}
}

int Kmp_fifo::punlink(int id)
{
	int y = find_id(id);
	if (y == -1) {
		cout << "======================================================" << endl;
		cout << "Pipe doesn't exist." << endl; return -1;;
	}
	else if (ftab[y].writers != 0 || ftab[y].readers != 0) {
		cout << "======================================================" << endl;
		cout << "Cannot unlink pipe." << endl; return -1;
	}

	ftab[y].bdata = vector<char>();
	ftab.erase(ftab.begin() + y);
	return 0;
}

void Kmp_fifo::printinode(int id)
{
	cout << "======================================================" << endl;
	int y = find_id(id);
	if (y == -1) {
		cout << "Pipe doesn't exist." << endl; return;
	}

	cout << "Pipe - id: " << ftab[y].nr << endl;
	cout << "bufor: ";
	for (int i = 0;i < ftab[y].bdata.size();i++) printf("%3c|", ftab[y].bdata[i]);
	cout << endl << "index: ";
	for (int i = 0;i < ftab[y].bdata.size();i++) printf("%3i|", i);
	cout << endl << "Pointers bg_buf: " << ftab[y].bg_buf << "	len_buf: " << ftab[y].len_buf << endl;
	cout << "Counters writers: " << ftab[y].writers << "	readers: " << ftab[y].readers << endl;
	cout << "Flags	f_readers: " << ftab[y].f_readers << "	occupy: " << ftab[y].occupy << endl;
	cout << "======================================================" << endl;


}

int Kmp_fifo::find_id(int id)
{
	for (int i = 0; i < ftab.size(); i++) {
		if (ftab[i].nr == id) return i;
	}

	return -1;
}

void Kmp_fifo::inLine(int amount, int y) {
	if (size - amount > 0) {
		vector<char>::iterator it;
		int j = ftab[y].len_buf;
		for (int i = 0; i < amount;i++) {
			for (it = ftab[y].bdata.begin(); it != ftab[y].bdata.end() - 1; it++) swap(*it, *(it + 1));
		}
	}
}

int Kmp_fifo::wwrite(int id, string data)
{
	int y = find_id(id);
	if (y == -1) return -1;
	if (size + data.size() > bsize || ftab[y].occupy) return -1;
	if (first) {
		first = false;
		for (int i = 0;i < bsize;i++) ftab[y].bdata.push_back('0');
	}

	int j = ftab[y].len_buf;
	for (int i = 0;i < data.size();i++) {
		ftab[y].bdata[j + i] = data[i];
	}
	
	ftab[y].len_buf += data.size();
	size += data.size();

	cout << "======================================================" << endl;
	cout << "Write to pipe" << ftab[y].nr << ":	" << data << endl;

	return 0;
}

string Kmp_fifo::rread(int id, int amount)
{
	int y = find_id(id);
	if (y == -1) return "null";
	if (size - amount < 0 || size + amount > bsize) return "null";

	string temp;
	if (size + amount > bsize) amount = (size + amount) - bsize;
	int j = ftab[y].bg_buf;
	for (int i = 0;i < amount;i++) {
		temp.push_back(ftab[y].bdata[j + i]);
		ftab[y].bdata[j + i] = '0';
	}

	if (ftab[y].bg_buf + amount == bsize) ftab[y].bg_buf = 0;
	else if (ftab[y].bg_buf + amount == ftab[y].len_buf) ftab[y].len_buf -= amount;
	else {
		ftab[y].len_buf -= amount;
		ftab[y].bg_buf = 0;
	}
	inLine(amount, y);
	size -= amount;

	cout << "======================================================" << endl;
	cout << "Read from pipe" << ftab[y].nr << ":	" << temp << endl;
	return temp;
}
