#pragma once
#include<vector>
#include<string>

const int bsize = 16; //wielko�� maksymalna buffora
const int bmess = 4; //wielko�� maksymalna komunikatu

//Struktura informacyjna, zawieraj�ca zmienne wykorzystywane przez metody
struct inode_info {
	int nr; //unikalny numer identyfikacyjny potoku
	std::vector<char> bdata; //miejsce na bufor pami�ci
	int writers; //liczba proces�w pisz�cych do ��cza
	int readers; //liczba proces�w czytaj�cych z ��cz�
	bool f_readers; //flaga "czy czytelnik otwar� ��cze?", false - nie, true - tak
	bool occupy; //flaga "czy buffor jest zaj�ty?", false - nie, true - tak
	int bg_buf; //pozycja od kt�rej zaczyn�j� si� dane zapisane w bufforze
	int len_buf; //pozycja w kt�rej ko�cz� si� zapisane dane
	bool w_acc;
};

//Klasa udost�pniaj�ca metody obs�uguj�ce potok
class Kmp_fifo {
	std::vector<inode_info> ftab;
	int size;
	bool first = true;

	//Inicjacja potoku w systemie, konfiguracja inode_info.
	//int - identyfikator potoku
	//Zwraca: 1 - operacja przebieg�a pomy�lnie, 0 - wyst�pi� b��d.
public:

	int pinit(int);

	//Otwarcie ��cz� pomi�dzy potokiem, a procesem.
	//int - id potoku, char: "r" - czytelnik, "w" - pisarz
	//Zwraca: bg_buf dla "r", len_buf dla "w".
	int popen(int, char);

	//Umo�liwia czytanie danych, ustawia flag� occupy = true.
	//PCB - blok kontrolny procesu, int - bg_buf, int - rozmiar
	//Zwraca: string z danymi
	std::string pread(int *, int, int);

	//Zapisywanie danych do buffora, flaga occupu = true.
	//PCB - blok kontrolny procesu, int - leb_buf, string - dane
	//Przesuwa znacznik len_buf
	//Zwraca: 1 - operacja przebieg�a pomy�lnie, 0 - wyst�pi� b��d
	int pwrite(int *, std::string);

	//Zamyka ��cz� pomi�dzy procesem, a potokiem
	//PCB - blok kontrolny procesu
	void pclose(int[]);

	//Usuwa ��cz�, zeruje inode_info, uwalnia pami��
	//int - id ��cz� kt�re chcemy od��czy�
	int punlink(int);

	////////////////////////////////////////////////////////////////////
	void printinode(int id);

	int find_id(int id);

	void inLine(int, int);

	int wwrite(int, std::string);

	std::string rread(int, int);

};