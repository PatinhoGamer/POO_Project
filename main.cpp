#include <iostream>
#include <string>
#include <ctime>

#include "GameInterface.h"

using namespace std;

int main() {
	srand((unsigned) time(nullptr));

	//Game::testMemoryLeak();
	//string str;
	//cin >> str;

	{ // Para destruir a GameInterface
		GameInterface gameInterface;

		bool exit = false;

		while (!exit) {

			gameInterface.printMenu(cout);

			string inputString;
			getline(cin, inputString);
			vector<string> inputParts = Utils::stringSplit(inputString, " ");
			if (inputParts.empty()) {
				cout << "Comando vazio. Introduza novamente\n";
			}
			string &action = inputParts[0];

			if (action == "sair") {
				exit = true;
				continue;
			}

			gameInterface.handleCommand(cout, inputParts);
		}
	}
	return 0;
}

