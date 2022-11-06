#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Level.h"
#include "Game.h"
#include "GameModules.h"
#include "ClientServer.h"

extern bool gFastFood;

class cServerState : public CGameState {
public:
	cServerState(CGame& game, int mode, int rate);
	virtual ~cServerState();

	virtual void OnEnter();
	virtual void HandleEvent(const CGameEvent& event);
	void HandleNetwork(CNetworkEvent& event);
	virtual void Tick(const DeltaTime_t& deltaTime);
	virtual void Draw();
	virtual void OnExit();
	void DoWorldProgressCheck();
	void SendAnimalUpdates();
	void SpawnAnimalsTick();
	void DoAnimalDeletes();
	int CreateAnimal(cServerAnimal* animal, int x, int y);
	void ChangeDay();
	void SendResults();

private:
	CResourceController<sf::Texture> mRcTex;
	CDrawModule& mDraw;
	CNetworkModule& mNet;
	cLevel mLevel;
	cSkyDrawer mSky;

	std::stack<int> mAnimalDeletes;
	std::map<int, sServerPlayer> mPlayers;
	std::map<int, std::unique_ptr<cServerAnimal>> mAnimals;
	int mTopAnimalId;
	int mTopId;

	int mGameType = 0;
	int mSpawnRATE = 0;

	float mAllFood = 800.0f;
	float mTimeLimit = 180.0f;

	int mAverageGenHeight = 0;
	bool mDaytime;
	bool mDaylightCycle;
	int mDays = 0;
	int mNighttype = 0;
};
