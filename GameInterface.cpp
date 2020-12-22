//
// Created by Rodrigo on 30/11/2020.
//

#include "GameInterface.h"
#include "Factory.h"

#include <ostream>
#include <fstream>

using namespace std;

GameInterface::GameInterface() {
	currentGame = new Game();

	phaseCommand =
			{PhaseCommand("conquista", 1), PhaseCommand("passa", 1),
			 PhaseCommand("maisouro", 2), PhaseCommand("maisprod", 2),
			 PhaseCommand("maismilitar", 3), PhaseCommand("adquire", 3),
			 PhaseCommand("lista", 0), PhaseCommand("avanca", 0),
			 PhaseCommand("grava", 0), PhaseCommand("ativa", 0),
			 PhaseCommand("apaga", 0), PhaseCommand("toma", 0),
			 PhaseCommand("modifica", 1), PhaseCommand("fevento", 0)};

}

GameInterface::~GameInterface() {
	//delete [] phaseCommand; // já não é necessário porque passou a ser um vector com objetos que não sao criados no heap
	delete currentGame;
}

void GameInterface::printMenu(ostream &out) {
	//TODO make this proper
	out << "\nIntroduza o commando que deseja correr\n ->";
}

void GameInterface::handleCommand(ostream &out, vector<std::string> &inputParts) {

	if (inputParts.empty()) {
		out << "Comando vazio. Introduza novamente\n";
		return;
	}
	const string &action = inputParts[0];

	if (action == "lista") {
		listGame(inputParts, out);
		return;
	}

	if (!currentGame->isInProgress()) {
		handleCreationCommand(out, inputParts);
	} else {
		handleGameCommand(out, inputParts);
	}
}

void GameInterface::handleCreationCommand(ostream &out, vector<std::string> &inputParts) {
	// Pre Game ----------------------------------------------------------------------------------------------------
	//Criação do jogo
	const string &action = inputParts[0];
	if (action == "avanca") {
		if (currentGame->start()) {
			out << "Jogo iniciado\n";
		} else {
			out << "Mundo vazio, adicione territorios primeiro\n";
		}
	} else if (action == "cria") {
		//cria <tipo> <n> - Acrescenta ao mundo n territórios de um determinado tipo (exemplo:
		//mina, refugio, para respetivamente um território do tipo Mina ou do tipo Refugio).
		if (inputParts.size() == 3) {
			int amount = stoi(inputParts[2]);
			string &type = inputParts[1];

			for (int i = 0; i < amount; ++i) {
				Territorio *newTerritory = Factory::createTerritoryFromType(type);
				if (newTerritory != nullptr) currentGame->addTerritoryToWorld(newTerritory);
				else {
					out << "tipo de territorio invalido : " << type << endl;
					break;
				}
			}
			out << type << " criado(a) na fase de criacao\n";
		} else {
			out << "sintaxe valida -> cria <tipo> <quantidade>\n";
		}
	} else if (action == "carrega") {
		if (inputParts.size() == 2) {
			string filePath = inputParts[1];
			ifstream file(filePath);
			if (!file.is_open()) {
				out << "Nao foi possivel abrir o ficheiro : " << filePath << " \n";
			} else {
				Game *newGame = new Game(file, cout);
				delete currentGame;
				currentGame = newGame;
				currentGame->printGame(cout);
				out << "Carregado o ficheiro com sucesso\n";
			}
		} else {
			out << "Sintaxe valida -> carrega <nome_ficheiro>\n";
		}
	} else {
		out << "Comando invalido para a criacao do jogo\n";
	}
}

void GameInterface::handleGameCommand(ostream &out, vector<std::string> &inputParts) {
	//In Game ----------------------------------------------------------------------------------------------------------
	const string &action = inputParts[0];

	int commandPhase = getCommandPhase(action);

	if (commandPhase != currentGame->getPhase() && commandPhase != 0) { // = 0 ,quer dizer que é para todas as fazes
		out << "Commando invalido para a fase atual\n";
		return;
	}

	if (commandPhase == 0) { //
		handleCommandAnyPhase(out, inputParts);
	} else if (commandPhase == 1) {
		handleCommandPhase1(out, inputParts);
	} else if (commandPhase == 2) {
		handleCommandPhase2(out, inputParts);
	} else if (commandPhase == 3) {
		handleCommandPhase3(out, inputParts);
	} // na fase 4 não existem comandos, apenas podem acontecer coisas que o jogador não pode afetar
}

void GameInterface::handleCommandPhase1(ostream &out, vector<std::string> &inputParts) {
	const string &action = inputParts[0];
	if (action == "conquista") {
		//conquista <nome> - Dá a ordem ao império para adquirir um determinado território
		//neste turno desde que este esteja disponível no World. O parâmetro nome indica
		//qual o nome do território a conquistar (planicie1, duna3, etc.).

		if (inputParts.size() == 2) {
			int whatHappened = currentGame->conquer(inputParts[1]);
			if (whatHappened == -1) {
				out << "Nome de territorio invalido\n";
			} else if (whatHappened == false) {
				out << "Nao consegui conquistar\n";
			} else {
				// Conquistou
				currentGame->nextPhase();
				out << "Conquistado com sucesso\n";
			}
		} else {
			out << "Sintaxe valida -> conquista <nome_do_territorio>\n";
		}

	} else if (action == "passa") {
		currentGame->nextPhase();
		out << "Passou o turno a frente\n";
	}
}

void GameInterface::handleCommandPhase2(ostream &out, vector<std::string> &inputParts) {
	const string &action = inputParts[0];

	if (action == "maisouro") {
		if (currentGame->moreGold()) {
			out << "Obteve mais 1 de ouro e perdeu 2 de produtos\n";
		} else {
			out << "Nao tens produtos suficientes para trocar, necessario 2\n";
		}
	} else if (action == "maisprod") {
		if (currentGame->moreProducts()) {
			out << "Obteve mais 1 de produtos e perdeu 2 de ouro\n";
		} else {
			out << "Nao tens ouro suficiente para trocar, necessario 2\n";
		}
	}
}

void GameInterface::handleCommandPhase3(ostream &out, vector<std::string> &inputParts) {
	const string &action = inputParts[0];

	if (action == "maismilitar") {
		if (currentGame->moreMilitary()) {
			out << "Obteve mais 1 de forca militar á custa de 1 de ouro e 1 de produutos\n";
		} else {
			out << "Nao tens produtos e ouro suficientes para trocar, necessario 1 de cada\n";
		}
	} else if (action == "adquire") {
		if (inputParts.size() == 2) {
			if (currentGame->acquire(inputParts[1])) {
				//TODO finish this part
			}
		} else {
			out << "sintaxe valida -> adquire <tipo>\n";
		}
	}
}

void GameInterface::handleCommandAnyPhase(ostream &out, vector<std::string> &inputParts) {
	const string &action = inputParts[0];
	//TODO acabar de fazer estes comandos

	if (action == "lista") {
		listGame(inputParts, out);
	} else if (action == "avanca") {
		// Avança para a próxima fase ----------------------------------------------------------------------------------
		if (inputParts.size() == 2) {
			const string &name = inputParts[1];
			if (!gameSaver.usedSaveGameName(name)) {
				gameSaver.saveGame(name, currentGame);
			} else {
				out << "Ja existe um jogo guardado com esse nome\n";
			}
		} else {
			out << "sintaxe valida -> grava <nome_do_jogo>\n";
		}
	} else if (action == "grava") {
		// Grava o jogo em memória -------------------------------------------------------------------------------------
		if (inputParts.size() == 2) {
			const string &name = inputParts[1];
			if (!gameSaver.usedSaveGameName(name)) {
				gameSaver.saveGame(name, currentGame);
			} else {
				out << "Ja existe um jogo guardado com esse nome\n";
			}
		} else {
			out << "sintaxe valida -> grava <nome_do_jogo>\n";
		}
	} else if (action == "ativa") { // TODO ativa
		// Recupera um jogo que estava em memória ----------------------------------------------------------------------
	} else if (action == "apaga") { // TODO apaga
		// Apaga um jogo que estava em memória -------------------------------------------------------------------------
	} else if (action == "toma") { // TODO toma
		// Obtem tecnologia ou territorio sem seguir as regras do jogo -------------------------------------------------
	} else if (action == "modifica") { // TODO modifica
		// Modifica a quantidade de recursos sem seguir as regras do jogo ----------------------------------------------
	} else if (action == "fevento") { // TODO fevento
		// Força o acontecimento de um evento --------------------------------------------------------------------------
	}
}

int GameInterface::getCommandPhase(const string &command) {
	for (const auto &item : phaseCommand) {
		if (item.getCommand() == command)
			return item.getPhase();
	}
	return -1;
}

void GameInterface::listGame(vector<std::string> &inputParts, ostream &out) {
// Lista o jogo ou territorio ----------------------------------------------------------------------------------
	if (inputParts.size() == 1) {
		currentGame->printGame(cout);
	} else if (inputParts.size() == 2) {
		string &name = inputParts[1];
		const Territorio *territory = currentGame->getTerritoryByName(name);
		if (territory != nullptr) {
			out << *territory << endl;
		} else {
			out << "Territory com esse nome nao encontrado\n";
		}
	} else {
		out << "sintaxe valida -> lista OU lista <nome_do_territorio>\n";
	}
}
