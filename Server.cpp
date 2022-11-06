#include "Server.h"
#include <iostream>

bool gFastFood = false;

cServerState::cServerState(CGame& game, int mode, int rate)
	: CGameState(game), mSky(sf::Color(40, 160, 255)),
	mDraw(*((CDrawModule*)game.GetModulePtr("CDrawModule"))),
	mNet(*((CNetworkModule*)game.GetModulePtr("CNetworkModule"))) {
	mLevel.LoadContent(mRcTex);
	mAverageGenHeight = mLevel.Generate() * 16;

	//std::cout << "GAMETYPE (0, 1, 2): ";
	//std::cin >> mGameType;
	mGameType = mode;
	mSpawnRATE = rate;

	mNet.SetTickRate(64);
	mNet.BindSocket(gServerPort);
	mTopId = 0;
	mTopAnimalId = 0;
	mDaylightCycle = true;

	mDaytime = true;

	if (!mDaylightCycle) {
		mDaytime = true;
	}
}
cServerState::~cServerState() {

}

void cServerState::OnEnter() {

}
void cServerState::HandleEvent(const CGameEvent& event) {
	if (event.GetEvent() >= EGameEvent::Net_SentPacket) {
		HandleNetwork(*((CNetworkEvent*) & event));
	}
}
void cServerState::HandleNetwork(CNetworkEvent& event) {
	if (event.GetEvent() == EGameEvent::NetServer_PlayerConnected) {
		mTopId++;

		auto evnt = std::make_unique<CNetworkEvent>();
		evnt->AddTypeToPacket(EGameEvent::NetClient_ConnectionSuccess);
		evnt->GetPacket() << mTopId;
		std::string playerName;
		event.GetPacket() >> playerName;
		evnt->GetPacket() << playerName;
		evnt->GetPacket() << mGameType;
		mNet.SendEvent(std::move(evnt), event.GetSenderIp(), event.GetSenderPort());

		std::cout << "Player Name: " << playerName << " connected with ID: " << mTopId << "!\n";

		// Notify player
		for (auto& p : mPlayers) {
			auto evnt2 = std::make_unique<CNetworkEvent>();
			evnt2->AddTypeToPacket(EGameEvent::NetClient_PlayerConnected);
			evnt2->GetPacket() << p.first;
			evnt2->GetPacket() << p.second.name;
			evnt2->GetPacket() << p.second.x;
			evnt2->GetPacket() << p.second.y;
			mNet.SendEvent(std::move(evnt2), event.GetSenderIp(), event.GetSenderPort());
		}

		// Notify all other players
		for (auto& p : mPlayers) {
			auto evnt2 = std::make_unique<CNetworkEvent>();
			evnt2->AddTypeToPacket(EGameEvent::NetClient_PlayerConnected);
			evnt2->GetPacket() << mTopId;
			evnt2->GetPacket() << playerName;
			evnt2->GetPacket() << 0.0f;
			evnt2->GetPacket() << 0.0f;
			mNet.SendEvent(std::move(evnt2), p.second.ip, p.second.port);
		}

		// Add player
		mPlayers[mTopId].ip = event.GetSenderIp();
		mPlayers[mTopId].port = event.GetSenderPort();
		mPlayers[mTopId].name = playerName;

		// extra stuff like the world and time of day
		mPlayers[mTopId].worldProgress = 1;
		auto evnt3 = std::make_unique<CNetworkEvent>();
		evnt3->GetPacket() = mLevel.ContructPacket(0);
		mNet.SendEvent(std::move(evnt3), event.GetSenderIp(), event.GetSenderPort());

		auto evnt2 = std::make_unique<CNetworkEvent>();
		if (mDaytime) {
			evnt2->AddTypeToPacket(EGameEvent::NetClient_Daytime);
		}
		else {
			evnt2->AddTypeToPacket(EGameEvent::NetClient_Nighttime);
		}
		mNet.SendEvent(std::move(evnt2), event.GetSenderIp(), event.GetSenderPort());

		// DEaths 
		// Notify all other players
		for (auto& p : mPlayers) {
			if (p.second.dead) {
				auto evnt2 = std::make_unique<CNetworkEvent>();
				evnt2->AddTypeToPacket(EGameEvent::NetClient_PlayerDied);
				evnt2->GetPacket() << p.first;
				mNet.SendEvent(std::move(evnt2), event.GetSenderIp(), event.GetSenderPort());
			}
		}

		// Skin updates
		for (auto& p : mPlayers) {
			if (p.second.hasImg) {
				auto evnt2 = std::make_unique<CNetworkEvent>();
				evnt2->AddTypeToPacket(EGameEvent::NetClient_PlayerSkinPic);
				evnt2->GetPacket() << p.first;
				evnt2->GetPacket() << p.second.imgPath;
				int x = p.second.img.getSize().x;
				int y = p.second.img.getSize().y;
				evnt2->GetPacket() << x;
				evnt2->GetPacket() << y;
				const sf::Uint8* pxls = p.second.img.getPixelsPtr();
				// The pixels
				for (int i = 0; i < x * y * 4; i++) {
					evnt2->GetPacket() << pxls[i];
				}
				mNet.SendEvent(std::move(evnt2), event.GetSenderIp(), event.GetSenderPort());
			}
		}
	}

	if (event.GetEvent() == EGameEvent::NetServer_PlayerDisconnected) {
		int id;
		event.GetPacket() >> id;

		// Remove player
		mPlayers.erase(id);

		// Notify all other players
		for (auto& p : mPlayers) {
			auto evnt2 = std::make_unique<CNetworkEvent>();
			evnt2->AddTypeToPacket(EGameEvent::NetClient_PlayerDisconnected);
			evnt2->GetPacket() << id;
			mNet.SendEvent(std::move(evnt2), p.second.ip, p.second.port);
		}
	}

	if (event.GetEvent() == EGameEvent::NetServer_PlayerPos) {
		auto evnt = std::make_unique<CNetworkEvent>();

		float x;
		float y;
		int id;
		event.GetPacket() >> id;
		event.GetPacket() >> x;
		event.GetPacket() >> y;

		mPlayers[id].x = x;
		mPlayers[id].y = y;

		// Notify player
		for (auto& p : mPlayers) {
			if (p.first == id)
				continue;

			auto evnt2 = std::make_unique<CNetworkEvent>();
			evnt2->AddTypeToPacket(EGameEvent::NetClient_PlayerPos);
			evnt2->GetPacket() << id;
			evnt2->GetPacket() << x;
			evnt2->GetPacket() << y;
			mNet.SendEvent(std::move(evnt2), p.second.ip, p.second.port);
		}
	}

	if (event.GetEvent() == EGameEvent::NetServer_PlayerSaid) {
		auto evnt = std::make_unique<CNetworkEvent>();

		std::string message;
		event.GetPacket() >> message;

		if (message == "BURN!") {
			gBURN = true;
		}

		if (message == "PLZNOBURN") {
			gNoBurn = true;
		}

		if (message == "COOLCOWS") {
			gCoolCows = true;
		}

		if (message == "TRUEBUILD") {
			gTBuild = true;
		}

		if (message == "STARVE") {
			gFastFood = true;
		}

		// Notify player
		for (auto& p : mPlayers) {
			auto evnt2 = std::make_unique<CNetworkEvent>();
			evnt2->AddTypeToPacket(EGameEvent::NetClient_PlayerSaid);
			evnt2->GetPacket() << message;
			mNet.SendEvent(std::move(evnt2), p.second.ip, p.second.port);
		}
	}

	if (event.GetEvent() == EGameEvent::NetServer_PlayerDied) {
		auto evnt = std::make_unique<CNetworkEvent>();

		int id;
		event.GetPacket() >> id;
		mPlayers[id].dead = true;

		// Notify players
		for (auto& p : mPlayers) {
			auto evnt2 = std::make_unique<CNetworkEvent>();
			evnt2->AddTypeToPacket(EGameEvent::NetClient_PlayerDied);
			evnt2->GetPacket() << id;
			mNet.SendEvent(std::move(evnt2), p.second.ip, p.second.port);
		}

		bool everyoneIsDead = true;
		for (auto& p : mPlayers) {
			if (!p.second.dead) {
				everyoneIsDead = false;
			}
		}

		if (everyoneIsDead) {
			SendResults();
		}
	}

	if (event.GetEvent() == EGameEvent::NetServer_PlayerFoodUpdate) {
		auto evnt = std::make_unique<CNetworkEvent>();

		float x;
		int id;
		event.GetPacket() >> id;
		event.GetPacket() >> x;

		mPlayers[id].food = x;

		// Notify player
		for (auto& p : mPlayers) {
			if (p.first == id)
				continue;

			auto evnt2 = std::make_unique<CNetworkEvent>();
			evnt2->AddTypeToPacket(EGameEvent::NetClient_PlayerFoodUpdate);
			evnt2->GetPacket() << id;
			evnt2->GetPacket() << x;
			mNet.SendEvent(std::move(evnt2), p.second.ip, p.second.port);
		}
	}

	if (event.GetEvent() == EGameEvent::NetServer_PlayerSkinPic) {
		auto evnt = std::make_unique<CNetworkEvent>();

		int playerid = -1;
		int imgX = 0;
		int imgY = 0;
		std::string mSkinLoadPath;
		event.GetPacket() >> playerid;
		event.GetPacket() >> mSkinLoadPath;
		event.GetPacket() >> imgX;
		event.GetPacket() >> imgY;
		sf::Uint8* imgpxls = new sf::Uint8[imgX * imgY * 4];

		mPlayers[playerid].hasImg = true;
		mPlayers[playerid].imgPath = mSkinLoadPath;

		// The pixels
		for (int i = 0; i < imgX * imgY * 4; i++) {
			event.GetPacket() >> imgpxls[i];
		}

		mPlayers[playerid].img.create(imgX, imgY, imgpxls);

		// Notify player
		for (auto& p : mPlayers) {
			if (p.first == playerid)
				continue;

			auto evnt2 = std::make_unique<CNetworkEvent>();
			evnt2->AddTypeToPacket(EGameEvent::NetClient_PlayerSkinPic);
			evnt2->GetPacket() << playerid;
			evnt2->GetPacket() << mSkinLoadPath;
			evnt2->GetPacket() << imgX;
			evnt2->GetPacket() << imgY;

			// The pixels
			for (int i = 0; i < imgX * imgY * 4; i++) {
				evnt2->GetPacket() << imgpxls[i];
			}
			mNet.SendEvent(std::move(evnt2), p.second.ip, p.second.port);
		}
	}

	if (event.GetEvent() == EGameEvent::NetServer_AnimalHurt) {
		int clientId;
		int damage;
		int animalid;
		event.GetPacket() >> animalid;
		event.GetPacket() >> clientId;
		event.GetPacket() >> damage;

		if (mAnimals.find(animalid) != mAnimals.end()) {
			if (mAnimals[animalid]->mFriendly) {
				mAnimals[animalid]->mHp -= damage;
			}

			if (mGameType == 1) {
				if (mAnimals[animalid]->mHp <= 0) {
					mAllFood += mAnimals[animalid]->mFood;

					auto evnt3 = std::make_unique<CNetworkEvent>();
					evnt3->AddTypeToPacket(EGameEvent::NetClient_GiveBlocks);
					evnt3->GetPacket() << mAnimals[animalid]->mBlocks;
					mNet.SendEvent(std::move(evnt3), event.GetSenderIp(), event.GetSenderPort());

					for (auto& p : mPlayers) {
						if (p.second.ip == event.GetSenderIp() &&
							p.second.port == event.GetSenderPort()) {
							p.second.foodEarned += mAnimals[animalid]->mFood;
						}
					}

					mAnimals.erase(animalid);
				}
				else if (!mAnimals[animalid]->mFriendly) {
					for (auto& p : mPlayers) {
						if (p.second.ip == event.GetSenderIp() &&
							p.second.port == event.GetSenderPort()) {
							p.second.foodEarned -= mAnimals[animalid]->mDamage;
						}
					}

					mAllFood -= mAnimals[animalid]->mDamage;
				}
			}
			else {
				if (mAnimals[animalid]->mHp <= 0) {
					auto evnt2 = std::make_unique<CNetworkEvent>();
					evnt2->AddTypeToPacket(EGameEvent::NetClient_GiveFood);
					evnt2->GetPacket() << mAnimals[animalid]->mFood;
					mNet.SendEvent(std::move(evnt2), event.GetSenderIp(), event.GetSenderPort());
					auto evnt3 = std::make_unique<CNetworkEvent>();
					evnt3->AddTypeToPacket(EGameEvent::NetClient_GiveBlocks);
					evnt3->GetPacket() << mAnimals[animalid]->mBlocks;
					mNet.SendEvent(std::move(evnt3), event.GetSenderIp(), event.GetSenderPort());
					mAnimals.erase(animalid);
				}
				else if (!mAnimals[animalid]->mFriendly) {
					auto evnt2 = std::make_unique<CNetworkEvent>();
					evnt2->AddTypeToPacket(EGameEvent::NetClient_GiveFood);
					evnt2->GetPacket() << -mAnimals[animalid]->mDamage;
					mNet.SendEvent(std::move(evnt2), event.GetSenderIp(), event.GetSenderPort());
				}
			}
		}
	}

	if (event.GetEvent() == EGameEvent::NetServer_BuildBlock) {
		int fx;
		int fy;
		int b;
		event.GetPacket() >> fx;
		event.GetPacket() >> fy;
		event.GetPacket() >> b;

		mLevel.SetBlock(fx, fy, b);

		for (auto& p : mPlayers) {
			auto evnt3 = std::make_unique<CNetworkEvent>();
			evnt3->GetPacket() = mLevel.ContructPacket(((float)fx / 32.0f) * 32.0f);
			mNet.SendEvent(std::move(evnt3), p.second.ip, p.second.port);
		}
	}

	if (event.GetEvent() == EGameEvent::NetServer_Joinable) {
		auto evnt2 = std::make_unique<CNetworkEvent>();
		evnt2->AddTypeToPacket(EGameEvent::NetClient_CanJoin);
		mNet.SendEvent(std::move(evnt2), event.GetSenderIp(), event.GetSenderPort());
	}
}
void cServerState::SendResults() {
	std::vector<std::pair<std::string, int>> scores;
	// Put peoples in first
	for (auto& p : mPlayers) {
		if (mGameType == 0) {
			scores.push_back(std::make_pair(p.second.name, (int)p.second.timeAlive));
		}
		else if (mGameType == 2) {
			scores.push_back(std::make_pair(p.second.name, (int)p.second.food));
		}
		else if (mGameType == 1) {
			scores.push_back(std::make_pair(p.second.name, (int)p.second.foodEarned));
		}
	}

	for (int i = 0; i < scores.size() - 1; i++) {
		for (int j = 0; j < scores.size() - i - 1; j++) {
			if (scores[j] < scores[j + 1])
				std::swap(scores[j], scores[j + 1]);
		}
	}
	
	// If everyone is dead commence the results screen
	for (auto& p : mPlayers) {
		auto evnt2 = std::make_unique<CNetworkEvent>();
		evnt2->AddTypeToPacket(EGameEvent::NetClient_ResultsScreen);
		evnt2->GetPacket() << scores.size();
		// Then send over the data
		for (int i = 0; i < scores.size(); i++) {
			evnt2->GetPacket() << scores[i].second;
			evnt2->GetPacket() << scores[i].first;
		}
		mNet.SendEvent(std::move(evnt2), p.second.ip, p.second.port);
	}
}
void cServerState::Tick(const DeltaTime_t& deltaTime) {
	if (mSpawnRATE == 0) {
		if (m_game.GetTick() % 15 == 0) {
			SpawnAnimalsTick();
		}
	}
	else if (mSpawnRATE == 1) {
		if (m_game.GetTick() % 4 == 0) {
			SpawnAnimalsTick();
		}
	}
	else if (mSpawnRATE == 2) {
		if (m_game.GetTick() % 30 == 0) {
			SpawnAnimalsTick();
		}
	}

	for (auto& a : mAnimals) {
		a.second->OnUpdate(a.first, mLevel, mAnimalDeletes, mAnimals, mPlayers, mDaytime, *this);
	}
	DoAnimalDeletes();

	if (m_game.GetTick() % 2400 == 0 && mDaylightCycle) {
		ChangeDay();
	}

	if (m_game.GetTick() % 10 == 0) {
		SendAnimalUpdates();
	}

	if (m_game.GetTick() % 20 == 0) {
		DoWorldProgressCheck();
	}

	for (auto& p : mPlayers) {
		if (!p.second.dead) {
			p.second.timeAlive++;
		}
	}

	if (mGameType == 2) {
		mTimeLimit -= 1.f * deltaTime.count();

		for (auto& p : mPlayers) {
			auto evnt2 = std::make_unique<CNetworkEvent>();
			evnt2->AddTypeToPacket(EGameEvent::NetClient_UpdateTimer);
			evnt2->GetPacket() << mTimeLimit;
			mNet.SendEvent(std::move(evnt2), p.second.ip, p.second.port);
		}

		if (mTimeLimit <= 0.0f) {
			SendResults();
		}
	}

	if (mGameType == 1) {

		if (gFastFood) {
			mAllFood -= (15.0f * ((float)mPlayers.size() / 3.0f)) * deltaTime.count();
		}
		else {
			mAllFood -= (10.0f * ((float)mPlayers.size() / 3.0f)) * deltaTime.count();
		}

		for (auto& p : mPlayers) {
			auto evnt2 = std::make_unique<CNetworkEvent>();
			evnt2->AddTypeToPacket(EGameEvent::NetClient_SetFood);
			evnt2->GetPacket() << mAllFood;
			mNet.SendEvent(std::move(evnt2), p.second.ip, p.second.port);
		}
	}
}
void cServerState::ChangeDay() {
	mDays++;
	mDaytime = !mDaytime;
	if (mDays <= 2) {
		mNighttype = 0;
	}
	else if (mDays > 2 && mDays <= 5) {
		int night = getRandom(0, 10);
		if (night >= 0 && night < 7) {
			mNighttype = 0;
		}
		if (night >= 7 && night < 10) {
			mNighttype = 1;
		}
	}
	else if (mDays > 5) {
		int night = getRandom(0, 15);
		if (night >= 0 && night < 8) {
			mNighttype = 0;
		}
		if (night >= 8 && night < 12) {
			mNighttype = 1;
		}
		if (night >= 12 && night < 15) {
			mNighttype = 2;
		}
	}
	if (mDaytime) {
		for (auto& p : mPlayers) {
			auto evnt2 = std::make_unique<CNetworkEvent>();
			evnt2->AddTypeToPacket(EGameEvent::NetClient_Daytime);
			mNet.SendEvent(std::move(evnt2), p.second.ip, p.second.port);
		}

		for (auto it = mAnimals.begin(); it != mAnimals.end();) {
			if (!it->second->mFriendly) {
				mAnimals.erase(it++);
			}
			else {
				++it;
			}
		}
	}
	else {
		for (auto& p : mPlayers) {
			auto evnt2 = std::make_unique<CNetworkEvent>();
			evnt2->AddTypeToPacket(EGameEvent::NetClient_Nighttime);
			mNet.SendEvent(std::move(evnt2), p.second.ip, p.second.port);
		}

		for (auto it = mAnimals.begin(); it != mAnimals.end();) {
			if (it->second->mFriendly) {
				mAnimals.erase(it++);
			}
			else {
				++it;
			}
		}
	}
}
void cServerState::DoWorldProgressCheck() {
	for (auto& sp : mPlayers) {
		if (sp.second.worldProgress < mLevel.Width() / 32 + 1) {
			auto evnt3 = std::make_unique<CNetworkEvent>();
			evnt3->GetPacket() = mLevel.ContructPacket(sp.second.worldProgress * 32);
			mNet.SendEvent(std::move(evnt3), sp.second.ip, sp.second.port);
			sp.second.worldProgress++;
		}
	}
}
void cServerState::Draw() {
	mLevel.Render(mDraw);
	mDraw.DrawToLayer(mSky);
}
void cServerState::OnExit() {

}
void cServerState::SendAnimalUpdates() {
	for (auto& p : mPlayers) {
		auto packet = std::make_unique<CNetworkEvent>();
		packet->AddTypeToPacket(EGameEvent::NetClient_UpdateAnimals);
		for (auto& a : mAnimals) {
			if (a.second->pos.x > p.second.x - 400 &&
				a.second->pos.x < p.second.x + 320 &&
				a.second->pos.y < p.second.y + 280 &&
				a.second->pos.y > p.second.y - 280) {
				packet->GetPacket() << a.first;
				packet->GetPacket() << a.second->mType;
				packet->GetPacket() << a.second->mHp;
				packet->GetPacket() << a.second->mFriendly;
				packet->GetPacket() << a.second->pos.x;
				packet->GetPacket() << a.second->pos.y;
				packet->GetPacket() << a.second->mXSpeed;
				packet->GetPacket() << a.second->mYSpeed;
			}
		}
		packet->GetPacket() << (int)-1;
		mNet.SendEvent(std::move(packet), p.second.ip, p.second.port);
	}
}
void cServerState::DoAnimalDeletes() {
	while (!mAnimalDeletes.empty()) {
		if (mAnimalDeletes.top() >= 0) {
			if (mAnimals.find(mAnimalDeletes.top()) != mAnimals.end()) {
				mAnimals.erase(mAnimalDeletes.top());
			}
		}
		else {
			if (mAnimalDeletes.top() <= -10 && mAnimalDeletes.top() >= -26) {
				for (auto& p : mPlayers) {
					auto evnt3 = std::make_unique<CNetworkEvent>();
					evnt3->GetPacket() = mLevel.ContructPacket(((-mAnimalDeletes.top()) - 10) * 32);
					mNet.SendEvent(std::move(evnt3), p.second.ip, p.second.port);
				}
			}
		}
		
		mAnimalDeletes.pop();
	}
}
void cServerState::SpawnAnimalsTick() {
	if (mAnimals.size() < gAnimalsMax) {
		if (mDaytime) {
			int spawnType = getRandom(0, 7);

			if (spawnType >= 0 && spawnType <= 2) {
				CreateAnimal(new cServerPig(), getRandom(0, 512 * 16), mAverageGenHeight + getRandom(-32, 256));
			}
			else if (spawnType >= 3 && spawnType <= 4) {
				int sx = getRandom(0, 512);
				for (int i = 0; i < 64; i++) {
					if (mLevel.GetBlock(sx, i)) {
						CreateAnimal(new cServerCow(), sx * 16, (i - 5) * 16);
						break;
					}
				}
			}
			else if (spawnType >= 5 && spawnType <= 6) {
				CreateAnimal(new cServerChicken(), getRandom(0, 512 * 16), mAverageGenHeight + getRandom(-256, -412));
			}
		}
		else {
			if (mNighttype == 0) {
				int spawnType = getRandom(0, 4);

				if (spawnType >= 0 && spawnType <= 1) {
					CreateAnimal(new cServerZombie(), getRandom(0, 512 * 16), -40);
				}
			}
			else if (mNighttype == 1) {
				int spawnType = getRandom(0, 15);
				int worldSize = 512 * 16;

				if (spawnType >= 0 && spawnType <= 1) {
					CreateAnimal(new cServerCropduster(), getRandom(0, 2) * worldSize, -70);
				}
			}
			else if (mNighttype == 2) {
				int spawnType = getRandom(0, 16);

				if (spawnType >= 0 && spawnType <= 5) {
					CreateAnimal(new cServerZombie(), getRandom(0, 512 * 16), -40);
				}

				int worldSize = 512 * 16;

				if (spawnType >= 6 && spawnType <= 8) {
					CreateAnimal(new cServerCropduster(), getRandom(0, 2) * worldSize, -70);
				}
			}
		}
	}
}
int cServerState::CreateAnimal(cServerAnimal* animal, int x, int y) {
	mTopAnimalId++;
	Vector2f pos;
	pos.x = x;
	pos.y = y;
	mAnimals[mTopAnimalId] = std::move(std::unique_ptr<cServerAnimal>(animal));
	mAnimals[mTopAnimalId]->pos = pos;
	return mTopAnimalId;
}
