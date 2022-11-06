#pragma once

#include <fstream>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Level.h"
#include "Game.h"
#include "GameModules.h"
#include "ClientServer.h"
#include "Client.h"
#include "Server.h"

class cTitleState : public CGameState {
public:
	cTitleState(CGame& game);
	virtual ~cTitleState();

	virtual void OnEnter();
	virtual void HandleEvent(const CGameEvent& event);
	virtual void Tick(const DeltaTime_t& deltaTime);
	virtual void Draw();
	virtual void OnExit();

	void DoTheJoin();
	void DoTheCreate();

private:
	sf::Font mFont;
	CDrawModule& mDraw;
	CNetworkModule& mNet;
	CInputModule& mInput;

	int mMainSelected = 0;
	int mSubSelected = 0;
	int mSelectedState = 0;
	
	cSkyDrawer mSky;
	CResourceController<sf::Texture> mTextures;
	cLevel mBackgroundLevel;

	CCleanResource<sf::Texture> mJoinGame;
	CSpriteDrawer mJoinGameSprite;
	CCleanResource<sf::Texture> mCreateServer;
	CSpriteDrawer mCreateServerSprite;
	CCleanResource<sf::Texture> mSelector;
	CSpriteDrawer mSelectorSprite;
	CCleanResource<sf::Texture> mTitle;
	CSpriteDrawer mTitleSprite;


	CCleanResource<sf::Texture> mJoinIp;
	CSpriteDrawer mJoinIpSprite;
	CTextDrawer mJoinIpText;
	std::string mJoinIpString;
	CCleanResource<sf::Texture> mJoinName;
	CSpriteDrawer mJoinNameSprite;
	CTextDrawer mJoinNameText;
	std::string mJoinNameString;
	CCleanResource<sf::Texture> mSkinName;
	CSpriteDrawer mSkinNameSprite;
	CTextDrawer mSkinNameText;
	std::string mSkinNameString;

	CCleanResource<sf::Texture> mCreateType0;
	CCleanResource<sf::Texture> mCreateType1;
	CCleanResource<sf::Texture> mCreateType2;
	int mGameType = 0;
	CSpriteDrawer mCreateTypeSprite;
	CCleanResource<sf::Texture> mCreateRate0;
	CCleanResource<sf::Texture> mCreateRate1;
	CCleanResource<sf::Texture> mCreateRate2;
	CSpriteDrawer mCreateRateSprite;
	int mSpawnRate = 0;

	sf::IpAddress mServerIp;

};
