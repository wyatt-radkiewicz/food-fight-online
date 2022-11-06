#pragma once

#include <fstream>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Level.h"
#include "Game.h"
#include "GameModules.h"
#include "ClientServer.h"

class cClientState : public CGameState {
public:
	cClientState(CGame& game, sf::IpAddress server, std::string playername, std::string skinName);
	virtual ~cClientState();

	virtual void OnEnter();
	virtual void HandleEvent(const CGameEvent& event);
	void HandleNetwork(CNetworkEvent& event);
	virtual void Tick(const DeltaTime_t& deltaTime);
	virtual void Draw();
	virtual void OnExit();

	void LoadSkin(std::string skinname);

	void PlayerHammer();
	bool PlayerMove();
	bool PlayerMoveDead();
	void PlayerCamera();
	bool IsPlayerLevelCollide(int x, int y);
	int CollidingWith(int type);

	void SetAnimalsFromPacket(CNetworkEvent& event);

private:
	CResourceController<sf::Texture> mRcTex;
	sf::Font mFont;
	CDrawModule& mDraw;
	CNetworkModule& mNet;
	CInputModule& mInput;
	cLevel mLevel;
	cSkyDrawer mSky;
	sf::IpAddress mServer;
	int mClientId;
	bool mCanCollide;
	DeltaTime_t mDeltaTime;

	std::map<int, sClientPlayer> mPlayers;
	std::map<int, std::unique_ptr<cClientAnimal>> mAnimals;

	int mGameType = 0;
	float mFoodDec = 10.0f;

	std::string mSaying;
	std::string mMessages;
	CTextDrawer mSayingText;
	CTextDrawer mMessagesText;
	bool mSayingSomething;
	bool mJumping;
	int mLastMessage;
	bool mFreezeControls;
	float mPlayerSpeed;
	float mInternalFood;
	bool mIMMORTAL = false;

	float mSecondsLeft;
	CTextDrawer mSecondsLeftText;

	CCleanResource<sf::Texture> mHammerTex;
	CSpriteDrawer mHammerSprite;
	CCleanResource<sf::Texture> mCurrentBlockTex;
	CSpriteDrawer mCurrentBlockSprite;
	CTextDrawer mHammerText;
	int mCurrentBlock = 1;
	bool mCycling = false;

	bool mHasSkin = false;
	std::string mSkinLoadPath = "";
	sClientPlayerSkin mSkin;
	sf::Image mSkinPicture;

	int mColW = 14;
	int mColH = 30;
	int mPowerupCooldown = 0;
	float mFacing = 1;
};
