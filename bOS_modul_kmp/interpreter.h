#include <iostream>
#include <string>
#include "bOS_kmp.h"
using namespace std;

struct PCB {
	int PID;
	int pr;
	int io[2];
	int rejA = 0;
	int rejB = 0;
	int rejC = 0;
	int c = 0;
	int kwant = 0;

	PCB(int id, int p, int k) {
		PID = id;
		pr = p;
		kwant = k;
	}
};

class Interpreter
{
public:
	int pid;
	int registerA;
	int registerB;
	int registerC;
	int counter_of_instruction; //licznik rozkazow
	int kwant;


	void load_register(); //pobierz rejestr
	void execute_instruction(PCB &pcb, char *program, Kmp_fifo *fif); //wykonaj rozkazy arytmetyczno-logiczne i specjalne
	void state(); // wyswietl stan rejestrow i licznika rozkazow
	void interpreter(PCB &pcb); //funkcja do wywolania interpretera

	Interpreter::Interpreter() //wywolanie konstruktora
	{
		registerA = 0;
		registerB = 0;
		registerC = 0;
		counter_of_instruction = 0; //poczatkowo wszystkie wartosc ustawione sa na 0
	}
};

void Interpreter::load_register()
{
	//pobieranie rejestrow 
}

inline void Interpreter::execute_instruction(PCB & pcb,char *program,Kmp_fifo *fif)
{
	this->interpreter(pcb);

	int i = 0;
	char k;
	
	while (kwant != 0)
	{
		cout << i << " -- " << program[i] << endl;
		if (program[i] == 'A')
		{
			i++;
			if (program[i] == 'D')
			{
				i++;
				if (program[i] == 'B')
				{
					i++;
					registerA += registerB;
				}
				else if (program[i] == 'C')
				{
					i++;
					registerA += registerC;
				}
				else
				{
					i++;
					int argument = atoi(program + i);
					pcb.rejA += argument;
				}
			}

		}
		else if (program[i] == 'D')
		{
			i++;
			if (program[i] == 'C')
			{
				i++;
				if (program[i] == 'A')
				{
					i++;
					registerA--;
				}
				else if (program[i] == 'B')
				{
					i++;
					registerB--;
				}
				else if (program[i] == 'C')
				{
					i++;
					registerC--;
				}
			}
		}
		else if (program[i] == 'J')
		{
			i++;
			if (program[i] == 'Z')
			{
				i++;
				{
					int argument = atoi(program + i);
					if (registerA == 0)
					{
						counter_of_instruction += argument;
					}
				}
			}
			else if (program[i] == 'P')
			{
				i++;
				int argument = atoi(program + i);
				counter_of_instruction += argument;
			}
		}
		else if (program[i] == 'X')
		{
			i++;
			if (p->Zwroc_znak(i) == 'A')
			{
				i++;
				string temp;
				temp[0] = p->Zwroc_znak(i);
				i++;
				temp[1] = p->Zwroc_znak(i);
				int id = std::stoi(temp);
				i++;

				int y;
				y = p->IO[1] = F0.popen(id, p->Zwroc_znak(i));
				if (y == -1) cout << "Pipe doesn't exist." << endl;
				else if (y == -2) {
					kwant--;
					return 0;
				}
				F0.printinode(p->IO[0]);
			}
			if (program[i] == 'B')
			{
				i++;
				int a = atoi(program + i);
				string temp;

				temp = fif->pread(pcb.io,a);
				if (temp == "null") cout << "Pipe doesn't exist,or amount too large.";
				else if (temp == "empty" || temp == "occ") {
					kwant--;
					return 0;
				}
				fif->printinode(pcb.io[0]);
			}
			if (program[i] == 'E')
			{
				i++;
				string l;
				l.push_back(program[i]);
				int ile = stoi(l);
				string temp;
				for (int j = 0; j < ile; j++)
				{
					i++;
					temp.push_back(program[i]);
				}

				int y;
				y = fif->pwrite(pcb.io, temp);
				if (y == -1) cout << "Cannot write." << endl;
				else if (y == -2) {
					kwant--;
					return 0;
				}
				fif->printinode(pcb.io[0]);
			}
			if (program[i] == 'F')
			{
				i++;
				fif->pclose(pcb.io);
				fif->printinode(pcb.io[0]);
			}
		}
		else {
			break;
			pcb.kwant = -100;
			//return pcb;
		}
		i++;
		kwant--;
		counter_of_instruction++;
		state();
	}

	//return pcb;
}


void Interpreter::interpreter(PCB &pcb) //funkcja do wywolania interpretera (referencja do PCB danego procesu)
{
	pid = pcb.PID;
	registerA = pcb.rejA;
	registerB = pcb.rejB;
	registerC = pcb.rejC;
	counter_of_instruction = pcb.c;
	kwant = pcb.kwant;
}

void Interpreter::state() //stan rejestrow 
{
	cout << "PID: " << pid << endl; //nazwa procesu 
	cout << "Register A: " << registerA << endl;
	cout << "Register B: " << registerB << endl;
	cout << "Register C: " << registerC << endl;
	cout << "Licznik " << counter_of_instruction << endl;
	cout << "Pozostalo " << kwant << endl;
}

