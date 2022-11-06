#include "cTitleState.h"

cTitleState::cTitleState(CGame& game)
	: CGameState(game),
	mDraw(*((CDrawModule*)game.GetModulePtr("CDrawModule"))),
	mNet(*((CNetworkModule*)game.GetModulePtr("CNetworkModule"))),
	mSky(sf::Color(40, 160, 255)),
	mInput(*((CInputModule*)game.GetModulePtr("CInputModule"))) {
	mFont.loadFromFile("Assets/GameFont.ttf");

	mBackgroundLevel.LoadContent(mTextures);
	int level = mBackgroundLevel.Generate();
	mDraw.SetCamPos(340, level * 16);

	mJoinGame.Load(mTextures, "Assets/JoinGameBtn.png");
	mCreateServer.Load(mTextures, "Assets/CreateServerBtn.png");
	mJoinGameSprite.SetTexture(mJoinGame.Get());
	mCreateServerSprite.SetTexture(mCreateServer.Get());
	mTitle.Load(mTextures, "Assets/Title.png");
	mTitleSprite.SetTexture(mTitle.Get());
	mSelector.Load(mTextures, "Assets/Selector.png");
	mSelectorSprite.SetTexture(mSelector.Get());

	mJoinIp.Load(mTextures, "Assets/IpBoxBtn.png");
	mJoinIpSprite.SetTexture(mJoinIp.Get());
	mJoinName.Load(mTextures, "Assets/NameBoxBtn.png");
	mJoinNameSprite.SetTexture(mJoinName.Get());
	mSkinName.Load(mTextures, "Assets/SkinBoxBtn.png");
	mSkinNameSprite.SetTexture(mSkinName.Get());

	mJoinIpText.SetFont(mFont);
	mJoinIpText.SetColor(Color(1.0f, 0.0f, 0.0f));
	mJoinNameText.SetFont(mFont);
	mJoinNameText.SetColor(Color(1.0f, 0.0f, 0.0f));
	mSkinNameText.SetFont(mFont);
	mSkinNameText.SetColor(Color(1.0f, 0.0f, 0.0f));

	mCreateType0.Load(mTextures, "Assets/GameMode0Btn.png");
	mCreateType1.Load(mTextures, "Assets/GameMode1Btn.png");
	mCreateType2.Load(mTextures, "Assets/GameMode2Btn.png");
	mCreateRate0.Load(mTextures, "Assets/SpawnRate0Btn.png");
	mCreateRate1.Load(mTextures, "Assets/SpawnRate1Btn.png");
	mCreateRate2.Load(mTextures, "Assets/SpawnRate2Btn.png");
	mCreateRateSprite.SetTexture(mCreateRate0.Get());
	mCreateTypeSprite.SetTexture(mCreateType0.Get());

	std::ifstream file("Assets/ip.txt");
	std::string ipstr;
	file >> ipstr;
	mJoinIpString = ipstr;
	mJoinIpText.SetString(mJoinIpString);
	file.close();
}
cTitleState::~cTitleState() {

}

void cTitleState::OnEnter() {

}
void cTitleState::HandleEvent(const CGameEvent& event) {
	if (mSelectedState == 1 && mMainSelected == 0) {
		if (event.GetEvent() == EGameEvent::TextEntered) {
			CTextEnteredEvent& evnt = *((CTextEnteredEvent*) & event);
			if (mSubSelected == 0) {
				if (evnt.GetKey() != 8) {
					mJoinIpString += evnt.GetKey();
				}
				else if (evnt.GetKey() == 8) {
					if (mJoinIpString.size() > 0) {
						mJoinIpString.pop_back();
					}
				}

				mJoinIpText.SetString(mJoinIpString);
			}
			
			if (mSubSelected == 1) {
				if (evnt.GetKey() != 8) {
					mJoinNameString += evnt.GetKey();
				}
				else if (evnt.GetKey() == 8) {
					if (mJoinNameString.size() > 0) {
						mJoinNameString.pop_back();
					}
				}

				mJoinNameText.SetString(mJoinNameString);
			}

			if (mSubSelected == 2) {
				if (evnt.GetKey() != 8) {
					mSkinNameString += evnt.GetKey();
				}
				else if (evnt.GetKey() == 8) {
					if (mSkinNameString.size() > 0) {
						mSkinNameString.pop_back();
					}
				}

				mSkinNameText.SetString(mSkinNameString);
			}
		}

		if (event.GetEvent() == EGameEvent::NetClient_CanJoin) {
			CNetworkEvent& evnt = (*((CNetworkEvent*) & event));
			DoTheJoin();
		}
	}
}
void cTitleState::Tick(const DeltaTime_t& deltaTime) {
	if (m_game.GetTick() % 5 == 0) {
		mDraw.SetCamPos(mDraw.GetPos().x + 1, mDraw.GetPos().y);
	}

	// Selector
	if (mSelectedState == 1 && mMainSelected == 0) {
		if (mInput.GetBind("Select").IsPressed() && mSubSelected < 3) {
			mSubSelected++;

			if (mSubSelected == 3) {
				std::ofstream file("Assets/ip.txt");
				file << mJoinIpString;
				file.close();

				mSubSelected = 4;

				mServerIp = sf::IpAddress(mJoinIpString);
				mNet.SetTickRate(64);
				unsigned short port = 3333;
				while (!mNet.BindSocket(port)) {
					port++;
				}
			}
		}

		if (mSubSelected == 4 && m_game.GetTick() % 200 == 0) {
			auto event = std::make_unique<CNetworkEvent>();
			event->AddTypeToPacket(EGameEvent::NetServer_Joinable);
			mNet.SendEvent(std::move(event), mServerIp, gServerPort);
		}
	}

	if (mSelectedState == 1 && mMainSelected == 1) {
		if (mInput.GetBind("Select").IsPressed() && mSubSelected < 2) {
			mSubSelected++;

			if (mSubSelected == 2) {
				DoTheCreate();
			}
		}

		if (mInput.GetBind("Jump").IsPressed() && mSubSelected == 0) {
			mGameType++;
			mGameType = mGameType % 3;
			if (mGameType == 0) {
				mCreateTypeSprite.SetTexture(mCreateType0.Get());
			}
			if (mGameType == 1) {
				mCreateTypeSprite.SetTexture(mCreateType1.Get());
			}
			if (mGameType == 2) {
				mCreateTypeSprite.SetTexture(mCreateType2.Get());
			}
		}

		if (mInput.GetBind("Jump").IsPressed() && mSubSelected == 1) {
			mSpawnRate++;
			mSpawnRate = mSpawnRate % 3;
			if (mSpawnRate == 0) {
				mCreateRateSprite.SetTexture(mCreateRate0.Get());
			}
			if (mSpawnRate == 1) {
				mCreateRateSprite.SetTexture(mCreateRate1.Get());
			}
			if (mSpawnRate == 2) {
				mCreateRateSprite.SetTexture(mCreateRate2.Get());
			}
		}
	}
	if (mSelectedState == 0) {
		if (mInput.GetBind("Left").IsDown() && mMainSelected == 1) {
			mMainSelected = 0;
		}
		if (mInput.GetBind("Right").IsDown() && mMainSelected == 0) {
			mMainSelected = 1;
		}
		if (mInput.GetBind("Select").IsDown()) {
			mSelectedState = 1;
		}
	}
}
void cTitleState::Draw() {
	mDraw.DrawToLayer(mSky);
	mBackgroundLevel.Render(mDraw);

	// Selector
	if (mSelectedState == 0) {
		mJoinGameSprite.SetPos(mDraw.GetPos().x - 150 - 128, mDraw.GetPos().y - 100);
		mDraw.DrawToLayer(mJoinGameSprite);

		mCreateServerSprite.SetPos(mDraw.GetPos().x + 150, mDraw.GetPos().y - 100);
		mDraw.DrawToLayer(mCreateServerSprite);

		if (mMainSelected == 0) {
			mSelectorSprite.SetPos(mDraw.GetPos().x - 150 - 128 - 30, mDraw.GetPos().y - 100 - 30);
			mDraw.DrawToLayer(mSelectorSprite);
		}
		else if (mMainSelected == 1) {
			mSelectorSprite.SetPos(mDraw.GetPos().x + 150 - 30, mDraw.GetPos().y - 100 - 30);
			mDraw.DrawToLayer(mSelectorSprite);
		}
	}
	if (mSelectedState == 1 && mMainSelected == 0) {
		mJoinGameSprite.SetPos(mDraw.GetPos().x - 150 - 128, mDraw.GetPos().y - 100);
		mDraw.DrawToLayer(mJoinGameSprite);

		mJoinIpSprite.SetPos(mDraw.GetPos().x - 150 - 128, mDraw.GetPos().y);
		mDraw.DrawToLayer(mJoinIpSprite);
		mJoinNameSprite.SetPos(mDraw.GetPos().x - 150 - 128, mDraw.GetPos().y + 100);
		mDraw.DrawToLayer(mJoinNameSprite);

		mJoinIpText.SetPos(mDraw.GetPos().x - 150 - 128 + 40, mDraw.GetPos().y + 20);
		mDraw.DrawToLayer(mJoinIpText);
		mJoinNameText.SetPos(mDraw.GetPos().x - 150 - 128 + 40, mDraw.GetPos().y + 120);
		mDraw.DrawToLayer(mJoinNameText);

		mSkinNameSprite.SetPos(mDraw.GetPos().x - 150 - 128, mDraw.GetPos().y + 100 + 70);
		mDraw.DrawToLayer(mSkinNameSprite);
		mSkinNameText.SetPos(mDraw.GetPos().x - 150 - 128 + 40, mDraw.GetPos().y + 100 + 70);
		mDraw.DrawToLayer(mSkinNameText);

		if (mSubSelected == 0) {
			mSelectorSprite.SetPos(mDraw.GetPos().x - 150 - 128 - 30, mDraw.GetPos().y - 30);
			mDraw.DrawToLayer(mSelectorSprite);
		}
		else if (mSubSelected == 1) {
			mSelectorSprite.SetPos(mDraw.GetPos().x - 150 - 30, mDraw.GetPos().y + 120 - 30);
			mDraw.DrawToLayer(mSelectorSprite);
		}
		else if (mSubSelected == 2) {
			mSelectorSprite.SetPos(mDraw.GetPos().x - 150 - 30, mDraw.GetPos().y + 120 - 30 + 70);
			mDraw.DrawToLayer(mSelectorSprite);
		}
	}

	if (mSelectedState == 1 && mMainSelected == 1) {
		mCreateServerSprite.SetPos(mDraw.GetPos().x + 150, mDraw.GetPos().y - 100);
		mDraw.DrawToLayer(mCreateServerSprite);

		mCreateTypeSprite.SetPos(mDraw.GetPos().x + 150, mDraw.GetPos().y);
		mDraw.DrawToLayer(mCreateTypeSprite);
		mCreateRateSprite.SetPos(mDraw.GetPos().x + 150, mDraw.GetPos().y + 100);
		mDraw.DrawToLayer(mCreateRateSprite);

		if (mSubSelected == 0) {
			mSelectorSprite.SetPos(mDraw.GetPos().x + 150 - 30, mDraw.GetPos().y - 30);
			mDraw.DrawToLayer(mSelectorSprite);
		}
		else if (mSubSelected == 1) {
			mSelectorSprite.SetPos(mDraw.GetPos().x + 150 - 30, mDraw.GetPos().y + 120 - 30);
			mDraw.DrawToLayer(mSelectorSprite);
		}
	}

	mTitleSprite.SetPos(mDraw.GetPos().x - 320, mDraw.GetPos().y - 240);
	mDraw.DrawToLayer(mTitleSprite);
}
void cTitleState::DoTheJoin() {
	m_game.SetTick(0);
	mJoinNameString.pop_back();
	mSkinNameString.pop_back();
	m_game.PushState(std::make_unique<cClientState>(m_game, sf::IpAddress(mJoinIpString), mJoinNameString, mSkinNameString));
}
void cTitleState::DoTheCreate() {
	m_game.SetTick(0);
	m_game.PushState(std::make_unique<cServerState>(m_game, mGameType, mSpawnRate));
}
void cTitleState::OnExit() {

}
