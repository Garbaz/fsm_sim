#include <iostream>
#include <string>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define _UBR	0
#define _ZFM	1
#define _HFS	2
#define _SSP	3

#define PLAYER_A		0
#define PLAYER_B		1
#define PLAYER_A1		2
#define PLAYER_B1		3
#define UPPER			0
#define LOWER			1

#define SCHERE	1
#define STEIN		-1
#define PAPIER	0

#define A_WON		1
#define B_WON		0
#define TIE			-1

using namespace std;


struct sim_result
{
	char winner;
	int a_money;
	char raw_result[2];
};


sim_result sim(unsigned int type,unsigned int probs[4][2]);
void printNEA(char* arg0);
bool isNumber(string s);
int toInt(string s);
void splitString(string s, char delim,string* retString);
void print_result(sim_result r);

int main(int argc,char* argv[])
{
	srand(time(NULL));
	if(argc < 3 || !(strcmp(argv[1], "SSP") || argc >= 6))
	{
		printNEA(argv[0]);
		return -1;
	}
	else
	{
		unsigned int delay;
		if(argc == 7) delay = toInt(argv[6]);
		else delay = 1000;
		bool quick_update = false;
		if(argc == 8 && !strcmp(argv[7],"true")) quick_update = true;
		
		unsigned int probs[4][2];
		for( int i = 2; i < 6;i++)
		{
			if(i < argc)
			{
				string split[2];
				splitString(string(argv[i]), '/', split);
				if(!(isNumber(split[0]) && isNumber(split[1])))
				{
					printNEA(argv[0]);
					cout << "Incorrect probability parameters!" << endl;
					return -1;
				}
				probs[i-2][0] = toInt(split[0]);
				probs[i-2][1] = toInt(split[1]);
			}
			else
			{
				probs[i-2][0] = 0;
				probs[i-2][1] = 0;
			}
		}
		sim_result result;
		unsigned long round = 0;
		unsigned long playerAwins = 0,playerBwins = 0;
		long playerAmoney = 0, playerBmoney = 0;
		unsigned int type = 255;
		unsigned int last_disp_time = 0;
		if(!strcmp(argv[1], "UBR"))
		{
			type = _UBR;
		}
		else if(!strcmp(argv[1], "ZFM"))
		{
			type = _ZFM;
		}
		else if(!strcmp(argv[1], "HFS"))
		{
			type = _HFS;
		}
		else if(!strcmp(argv[1], "SSP"))
		{
			type = _SSP;
		}
		else
		{
			printNEA(argv[0]);
			return -1;
		}
		while(1)
		{
			result = sim(type,probs);
			
			if(result.winner == A_WON)
			{
				playerAwins++;
			}
			else if(result.winner == B_WON)
			{
				playerBwins++;
			}
			if(quick_update || last_disp_time < time(NULL))
			{
				playerAmoney += result.a_money;
				playerBmoney -= result.a_money;
				cout << "Runde " << round << ':' << endl;
				print_result(result);
				cout << "-----------------------------------" << endl;
				cout << "Player A Siege: " << playerAwins << endl;
				cout << "         Geld: €" << playerAmoney << endl;
				cout << "Player B Siege: " << playerBwins << endl;
				cout << "         Geld: €" << playerBmoney << endl;
				cout << "Geld Differenz = " << playerAmoney - playerBmoney << endl;
				cout << "Sieg Quotient  = " << ((double)playerAwins) / ((double)playerBwins) << endl;
				cout << endl;
				last_disp_time = time(NULL);
			}
			round++;
			for(unsigned int i = 0; i < delay; i++);
		}
	}
	return 0;
}

sim_result sim(unsigned int type,unsigned int probs[4][2])
{
	sim_result ret;
	int res[] = {rand(), rand()};
	ret.raw_result[PLAYER_A] = (res[PLAYER_A] % probs[PLAYER_A][LOWER] < probs[PLAYER_A][UPPER]);
	ret.raw_result[PLAYER_B] = (res[PLAYER_B] % probs[PLAYER_B][LOWER] < probs[PLAYER_B][UPPER]);
	switch(type)
	{
		case _UBR:
			ret.winner = (ret.raw_result[PLAYER_A] == ret.raw_result[PLAYER_B]);
			ret.a_money = -1 + 2*ret.winner;
			break;
		case _ZFM:
			ret.winner = (ret.raw_result[PLAYER_A] == ret.raw_result[PLAYER_B]);
			ret.a_money = (-1 + 2*ret.winner) * (2 + ret.raw_result[PLAYER_A] + ret.raw_result[PLAYER_B]);
			break;
		case _HFS:
			ret.winner = (ret.raw_result[PLAYER_A] == ret.raw_result[PLAYER_B]);
			if(ret.winner) ret.a_money = 20;
			else if(ret.raw_result[PLAYER_A]) ret.a_money = -10;
			else ret.a_money = -30;
			break;
		case _SSP:
			if(!ret.raw_result[PLAYER_A]) ret.raw_result[PLAYER_A] = -(res[PLAYER_A] % probs[PLAYER_A][LOWER] >= probs[PLAYER_A][UPPER]);
			if(!ret.raw_result[PLAYER_B]) ret.raw_result[PLAYER_B] = -(res[PLAYER_B] % probs[PLAYER_B][LOWER] >= probs[PLAYER_B][UPPER]);
			
			if(ret.raw_result[PLAYER_A] == ret.raw_result[PLAYER_B])
				ret.winner = -1;
			else
				ret.winner = ((ret.raw_result[PLAYER_A] == SCHERE && ret.raw_result[PLAYER_B] == PAPIER)
				||            (ret.raw_result[PLAYER_A] == STEIN  && ret.raw_result[PLAYER_B] == SCHERE)
				||            (ret.raw_result[PLAYER_A] == PAPIER && ret.raw_result[PLAYER_B] == STEIN));
				if(ret.winner == -1) ret.a_money = 0;
				else ret.a_money = -1 + 2*ret.winner;
			break;
		default:
			cerr << "Unknown simulation type!" << endl;
	}
	return ret;
}

void printNEA(char* arg0)
{
		cout << "Not enough arguments or wrong arguments:" << endl
		<< arg0 << " <GAME> <POSI_A> <POSI_B> [<POSI_A_2> <POSI_B_2>]" << endl
		<< "GAME = \"UBR\", \"ZFM\", \"HFS\", \"SSP\"" << endl
		<< "UBR ~ Uebereinstimmungsspiel" << endl
		<< "ZFM ~ Zwei-Finger-Morra" << endl
		<< "HFS ~ Hand-Faust-Spiel" << endl
		<< "SSP ~ Schere-Stein-Papier" << endl
		<< "POSI_A   = \"n/m\"" << endl
		<< "POSI_B   = \"n/m\"" << endl
		<< "POSI_A_2 = \"n/m\"" << endl
		<< "POSI_B_2 = \"n/m\"" << endl
		<< "(POSI_X_2 is only for SSP)" << endl;
}

bool isNumber(string s)
{
	for(unsigned int i = 0; i < s.size(); i++)
	{
		if('0'>s[i] || '9'<s[i]) return false;
	}
	return true;
}

int toInt(string s)
{
	int retVal = 0;
	unsigned int pow = 1;
	bool negative = s[0] == '-';
	for(int i = s.size(); i > negative; i--)
	{
		retVal += pow * (s[i-1] -48);
		pow *= 10;
	}
	return retVal * (-1 + 2*(!negative));
}

void splitString(string s, char delim,string* retString)
{
	unsigned int p = 0;
	for(unsigned int i = 0; i < s.size(); i++)
	{
		if(s[i] == delim)
		{
			p++;
		}
		else
		{
			retString[p] += s[i];
		}
	}
}

void print_result(sim_result r)
{
	if(r.winner == TIE) cout << "Unentschieden:" << endl;
	else if(r.winner == A_WON) cout << "Spieler A hat gewonnen:" << endl;
	else cout << "Spieler B hat gewonnen:" << endl;
	if(r.a_money > 0 && r.winner != TIE) cout << "\t  hat " << r.a_money << " Geld gewonnen" << endl;
	else if(r.a_money < 0 && r.winner != TIE) cout << "\t  hat " << -r.a_money << " Geld gewonnen" << endl;
	else cout << "\tUnentschieden" << endl;
	cout << "Raw_Result:  A=" << (int)r.raw_result[PLAYER_A] << ", B=" << (int)r.raw_result[PLAYER_B] << endl;
}
