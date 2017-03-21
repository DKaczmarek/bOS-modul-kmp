#pragma once
#include<vector>
#include<string>

const int bsize = 16; //wielkoœæ maksymalna buffora
const int bmess = 4; //wielkoœæ maksymalna komunikatu

//Struktura informacyjna, zawieraj¹ca zmienne wykorzystywane przez metody
struct inode_info {
	int nr; //unikalny numer identyfikacyjny potoku
	std::vector<char> bdata; //miejsce na bufor pamiêci
	int writers; //liczba procesów pisz¹cych do ³¹cza
	int readers; //liczba procesów czytaj¹cych z ³¹cz¹
	bool f_readers; //flaga "czy czytelnik otwar³ ³¹cze?", false - nie, true - tak
	bool occupy; //flaga "czy buffor jest zajêty?", false - nie, true - tak
	int bg_buf; //pozycja od której zaczyn¹j¹ siê dane zapisane w bufforze
	int len_buf; //pozycja w której koñcz¹ siê zapisane dane
	bool w_acc;
};

//Klasa udostêpniaj¹ca metody obs³uguj¹ce potok
class Kmp_fifo {
	std::vector<inode_info> ftab;
	int size;
	bool first = true;

	//Inicjacja potoku w systemie, konfiguracja inode_info.
	//int - identyfikator potoku
	//Zwraca: 1 - operacja przebieg³a pomyœlnie, 0 - wyst¹pi³ b³¹d.
public:

	int pinit(int);

	//Otwarcie ³¹cz¹ pomiêdzy potokiem, a procesem.
	//int - id potoku, char: "r" - czytelnik, "w" - pisarz
	//Zwraca: bg_buf dla "r", len_buf dla "w".
	int popen(int, char);

	//Umo¿liwia czytanie danych, ustawia flagê occupy = true.
	//PCB - blok kontrolny procesu, int - bg_buf, int - rozmiar
	//Zwraca: string z danymi
	std::string pread(int *, int, int);

	//Zapisywanie danych do buffora, flaga occupu = true.
	//PCB - blok kontrolny procesu, int - leb_buf, string - dane
	//Przesuwa znacznik len_buf
	//Zwraca: 1 - operacja przebieg³a pomyœlnie, 0 - wyst¹pi³ b³¹d
	int pwrite(int *, std::string);

	//Zamyka ³¹czê pomiêdzy procesem, a potokiem
	//PCB - blok kontrolny procesu
	void pclose(int[]);

	//Usuwa ³¹czê, zeruje inode_info, uwalnia pamiêæ
	//int - id ³¹czê które chcemy od³¹czyæ
	int punlink(int);

	////////////////////////////////////////////////////////////////////
	void printinode(int id);

	int find_id(int id);

	void inLine(int, int);

	int wwrite(int, std::string);

	std::string rread(int, int);

};