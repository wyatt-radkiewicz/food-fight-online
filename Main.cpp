#include <memory>
#include <iostream>
#include <map>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Game.h"
#include "GameModules.h"
#include "ResourceController.h"
#include "Client.h"
#include "Server.h"
#include "cTitleState.h"

int main()
{
	std::unique_ptr<CGame> game = std::make_unique<CGame>();
	game->SetFrameRate(60);
	/*char c;
	std::cout << "enter (s) or (c):" << std::endl;
	std::cin >> c;
	if (c == 's') {
		game->PushState(std::make_unique<cServerState>(*game.get()));
	}
	else {
		game->PushState(std::make_unique<cClientState>(*game.get()));
	}*/
	game->PushState(std::make_unique<cTitleState>(*game.get()));
	game->StartGame();

	return 0;
}
