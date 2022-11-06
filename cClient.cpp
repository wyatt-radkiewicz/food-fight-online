#include "Client.h"
#include <iostream>

cClientState::cClientState(CGame& game, sf::IpAddress server, std::string playername, std::string skinName)
	: CGameState(game), mSky(sf::Color(40, 160, 255)),
	mDraw(*((CDrawModule*)game.GetModulePtr("CDrawModule"))),
	mNet(*((CNetworkModule*)game.GetModulePtr("CNetworkModule"))),
	mInput(*((CInputModule*)game.GetModulePtr("CInputModule"))) {
	mLevel.LoadContent(mRcTex);
	mFont.loadFromFile("Assets/GameFont.ttf");

	mCanCollide = true;
	mClientId = -1;
	mSayingSomething = false;
	mLastMessage = 0;
	mFreezeControls = false;


	mSayingText.SetColor(Color(0.0f, 1.0f, 0.0f));
	mSayingText.SetFont(mFont);
	mSayingText.SetLayer(100);
	mSayingText.SetString(mSaying);
	mSayingText.SetAlign(ETextAlign::Left);
	mMessagesText.SetColor(Color(0.0f, 0.8f, 0.0f));
	mMessagesText.SetFont(mFont);
	mMessagesText.SetLayer(100);
	mMessagesText.SetString(mMessages);
	mMessagesText.SetAlign(ETextAlign::Left);

	LoadSkin(skinName);

	mNet.SetTickRate(64);
	//unsigned short port = 3333;
	//while (!mNet.BindSocket(port)) {
	//	port++;
	//}

	//std::string ipstr;
	//std::cout << "Enter server IP: " << std::endl;
	//std::cin >> ipstr;
	//mServer = sf::IpAddress(ipstr);
	mServer = server;
	std::string pname = playername;
	//std::cout << "Enter a name: " << std::endl;
	//std::cin >> pname;

	// NETWORKING YAY
	auto event = std::make_unique<CNetworkEvent>();
	event->AddTypeToPacket(EGameEvent::NetServer_PlayerConnected);
	event->GetPacket() << pname;
	mNet.SendEvent(std::move(event), mServer, gServerPort);
}
cClientState::~cClientState() {

}

void cClientState::LoadSkin(std::string skinname) {
	std::ifstream file("Skins/SKIN_" + skinname + ".txt");
	if (file.is_open()) {
		mHasSkin = true;
		std::string marker;
		file >> marker;
		file >> mSkin.speed;
		file >> marker;
		file >> mSkin.jump;
		file >> marker;
		file >> mSkin.damage;
		file >> marker;
		file >> mSkin.special;
		file >> marker;
		file >> mColW;
		file >> marker;
		file >> mColH;
		file.close();
	}
	else {
		mHasSkin = false;
	}
	mSkinLoadPath = "Skins/PIC_" + skinname + ".png";
	if (mHasSkin) {
		mSkinPicture.loadFromFile(mSkinLoadPath);
	}
}
void cClientState::OnEnter() {

}
void cClientState::HandleEvent(const CGameEvent& event) {
	if (event.GetEvent() >= EGameEvent::Net_SentPacket) {
		HandleNetwork(*((CNetworkEvent*)&event));
	}

	if (event.GetEvent() == EGameEvent::Game_End) {
		auto evnt = std::make_unique<CNetworkEvent>();
		evnt->AddTypeToPacket(EGameEvent::NetServer_PlayerDisconnected);
		evnt->GetPacket() << mClientId;
		mNet.SendEventImmidient(std::move(evnt), mServer, gServerPort);
	}

	if (mClientId == -1)
		return;

	if (event.GetEvent() == EGameEvent::TextEntered) {
		if (mSayingSomething) {
			CTextEnteredEvent& evnt = *((CTextEnteredEvent*)&event);
			
			if (evnt.GetKey() != 8) {
				mSaying += evnt.GetKey();
			}
			else if (evnt.GetKey() == 8) {
				if (mSaying.size() > mPlayers[mClientId].name.size() + 2) {
					mSaying.pop_back();
				}
			}
		}

		if (((CTextEnteredEvent*)&event)->GetKey() == 'y' && !mSayingSomething) {
			mSayingSomething = true;
			mFreezeControls = true;
			mSaying = mPlayers[mClientId].name + ": ";
		}

		if (((CTextEnteredEvent*) & event)->GetKey() == 'p' && !mSayingSomething) {
			mPlayers[mClientId].x = 128 * 16;
			mPlayers[mClientId].y = 0;
		}
	}

	if (event.GetEvent() == EGameEvent::Key_Pressed && mSayingSomething) {
		if (((CKeyEvent*) & event)->GetName() == "Select") {
			mSayingSomething = false;
			mFreezeControls = false;

			if (mSaying == mPlayers[mClientId].name + ": " + "IMMORTAL") {
				mIMMORTAL = true;
			}
			if (mSaying == mPlayers[mClientId].name + ": " + "BIGBOY") {
				mPlayers[mClientId].blocks = 100;
			}
			if (mSaying == mPlayers[mClientId].name + ": " + "MORTAL") {
				mIMMORTAL = false;
			}

			if (mSaying == mPlayers[mClientId].name + ": " + "BURN!") {
				mSaying = "BURN!";
			}

			if (mSaying == mPlayers[mClientId].name + ": " + "PLZNOBURN") {
				mSaying = "PLZNOBURN";
			}

			if (mSaying == mPlayers[mClientId].name + ": " + "COOLCOWS") {
				mSaying = "COOLCOWS";
			}

			if (mSaying == mPlayers[mClientId].name + ": " + "TRUEBUILD") {
				mSaying = "TRUEBUILD";
			}

			auto evnt2 = std::make_unique<CNetworkEvent>();
			evnt2->AddTypeToPacket(EGameEvent::NetServer_PlayerSaid);
			evnt2->GetPacket() << mSaying;
			mNet.SendEvent(std::move(evnt2), mServer, gServerPort);
		}
	}
}
void cClientState::HandleNetwork(CNetworkEvent& event) {
	if (event.GetEvent() == EGameEvent::NetClient_ConnectionSuccess) {
		event.GetPacket() >> mClientId;

		if (mHasSkin) {
			mPlayers[mClientId].tex.Load(mRcTex, mSkinLoadPath);
		}
		else {
			mPlayers[mClientId].tex.Load(mRcTex, "Assets/PlayerTex.png");
		}
		mPlayers[mClientId].spr.SetTexture(mPlayers[mClientId].tex.Get());
		mPlayers[mClientId].spr.SetLayer(20);
		event.GetPacket() >> mPlayers[mClientId].name;
		mPlayers[mClientId].nametext.SetColor(Color(1.0f, 0.0f, 0.0f));
		mPlayers[mClientId].nametext.SetFont(mFont);
		mPlayers[mClientId].nametext.SetLayer(20);
		mPlayers[mClientId].nametext.SetString(mPlayers[mClientId].name);
		mPlayers[mClientId].nametext.SetAlign(ETextAlign::Middle);

		mPlayers[mClientId].foodtext.SetColor(Color(1.0f, 0.0f, 0.0f));
		mPlayers[mClientId].foodtext.SetFont(mFont);
		mPlayers[mClientId].foodtext.SetLayer(19);
		mPlayers[mClientId].foodtext.SetString(std::to_string(mPlayers[mClientId].food));
		mPlayers[mClientId].foodtext.SetAlign(ETextAlign::Middle);
		mPlayers[mClientId].food = 800.0f;
		mJumping = false;
		mPlayers[mClientId].x = 0;
		mPlayers[mClientId].y = 0;
		mPlayers[mClientId].xsp = 0;
		mPlayers[mClientId].ysp = 0;
		if (mHasSkin) {
			mPlayers[mClientId].grv = 8.8f * 76.0f;
			mPlayers[mClientId].jmp = mSkin.jump;
		}
		else {
			mPlayers[mClientId].grv = 8.8f * 76.0f;
			mPlayers[mClientId].jmp = -24.0f * 20.5f;
		}
		mPlayers[mClientId].jumping = false;

		event.GetPacket() >> mGameType;

		mPlayers[mClientId].blocks = 0;

		mHammerTex.Load(mRcTex, "Assets/Hammer.png");
		mHammerSprite.SetTexture(mHammerTex.Get());
		mHammerSprite.SetLayer(30);
		mHammerText.SetColor(Color(1.0f, 0.0f, 0.0f));
		mHammerText.SetFont(mFont);
		mHammerText.SetLayer(19);
		mHammerText.SetString(std::to_string(mPlayers[mClientId].blocks));
		mHammerText.SetAlign(ETextAlign::Left);

		mCurrentBlockTex.Load(mRcTex, "Assets/Level.png");
		mCurrentBlockSprite.SetTexture(mCurrentBlockTex.Get());

		// GAMETYPES
		if (mGameType == 0) {
			mIMMORTAL = false;
		}
		else if (mGameType == 1) {
			mIMMORTAL = false;
			mFoodDec = 0;
		}
		else if (mGameType == 2) {
			mIMMORTAL = true;
			mSecondsLeft = 180.0f;
			mSecondsLeftText.SetColor(Color(0.0f, 1.0f, 0.0f));
			mSecondsLeftText.SetFont(mFont);
			mSecondsLeftText.SetLayer(30);
			mSecondsLeftText.SetString(std::to_string(mSecondsLeft));
			mSecondsLeftText.SetAlign(ETextAlign::Middle);
		}

		// SKINS
		if (mHasSkin) {
			int pxlSizeX = mSkinPicture.getSize().x;
			int pxlSizeY = mSkinPicture.getSize().y;
			if (pxlSizeX <= 64 &&
				pxlSizeY <= 64) {
				const sf::Uint8* pxls = mSkinPicture.getPixelsPtr();
				auto evnt2 = std::make_unique<CNetworkEvent>();
				evnt2->AddTypeToPacket(EGameEvent::NetServer_PlayerSkinPic);
				evnt2->GetPacket() << mClientId;
				evnt2->GetPacket() << mSkinLoadPath;
				evnt2->GetPacket() << pxlSizeX;
				evnt2->GetPacket() << pxlSizeY;

				// The pixels
				for (int i = 0; i < pxlSizeX * pxlSizeY * 4; i++) {
					evnt2->GetPacket() << pxls[i];
				}

				mNet.SendEvent(std::move(evnt2), mServer, gServerPort);
			}
		}
	}

	if (mClientId == -1)
		return;

	if (event.GetEvent() == EGameEvent::NetClient_PlayerConnected) {
		int id;
		event.GetPacket() >> id;
		event.GetPacket() >> mPlayers[id].name;
		mPlayers[id].nametext.SetColor(Color(0.0f, 0.0f, 0.7f));
		mPlayers[id].nametext.SetFont(mFont);
		mPlayers[id].nametext.SetLayer(19);
		mPlayers[id].nametext.SetString(mPlayers[id].name);
		mPlayers[id].nametext.SetAlign(ETextAlign::Middle);

		mPlayers[id].foodtext.SetColor(Color(0.0f, 0.0f, 0.7f));
		mPlayers[id].foodtext.SetFont(mFont);
		mPlayers[id].foodtext.SetLayer(19);
		mPlayers[id].foodtext.SetString(std::to_string(mPlayers[id].food));
		mPlayers[id].foodtext.SetAlign(ETextAlign::Middle);
		event.GetPacket() >> mPlayers[id].x;
		event.GetPacket() >> mPlayers[id].y;
		mPlayers[id].tex.Load(mRcTex, "Assets/Player2Tex.png");
		mPlayers[id].spr.SetTexture(mPlayers[id].tex.Get());
		mPlayers[id].spr.SetLayer(15);
	}

	if (event.GetEvent() == EGameEvent::NetClient_PlayerDisconnected) {
		int id;
		event.GetPacket() >> id;
		mPlayers.erase(id);
	}

	if (event.GetEvent() == EGameEvent::NetClient_PlayerPos) {
		int id;
		event.GetPacket() >> id;

		event.GetPacket() >> mPlayers[id].x;
		event.GetPacket() >> mPlayers[id].y;
	}

	if (event.GetEvent() == EGameEvent::NetClient_PlayerSkinPic) {
		int id;
		event.GetPacket() >> id;

		int x;
		int y;
		std::string path;
		event.GetPacket() >> path;
		event.GetPacket() >> x;
		event.GetPacket() >> y;

		sf::Uint8* imgpxls = new sf::Uint8[x * y * 4];

		for (int i = 0; i < x * y * 4; i++) {
			event.GetPacket() >> imgpxls[i];
		}

		sf::Image img;
		img.create(x, y, imgpxls);
		img.saveToFile(path);

		std::cout << path << std::endl;

		// Load the FILE
		mPlayers[id].tex.Load(mRcTex, path);
		mPlayers[id].spr.SetTexture(mPlayers[id].tex.Get());
		sf::IntRect rect;
		rect.top = 0;
		rect.left = 0;
		rect.width = x;
		rect.height = y;
		mPlayers[id].spr.GetSprite().setTextureRect(rect);
	}

	if (event.GetEvent() == EGameEvent::NetClient_PlayerFoodUpdate) {
		int id;
		event.GetPacket() >> id;

		event.GetPacket() >> mPlayers[id].food;
	}


	if (event.GetEvent() == EGameEvent::NetClient_PlayerDied) {
		int id;
		event.GetPacket() >> id;

		mPlayers[id].dead = true;
		//CCleanResource<sf::Texture> tex;
		//tex.Load(mRcTex, "Assets/PlayerDead.png");
		mPlayers[id].tex.Load(mRcTex, "Assets/PlayerDead.png");
		mPlayers[id].spr.SetTexture(mPlayers[id].tex.Get());
	}

	if (event.GetEvent() == EGameEvent::NetClient_ResultsScreen) {
		int players;
		event.GetPacket() >> players;

		std::cout << "==== RESULTS ====" << std::endl;
		if (mGameType == 0) {
			std::cout << "RANK\tPLAYERNAME\tTIMEALIVE" << std::endl;
		}
		if (mGameType == 1) {
			std::cout << "RANK\tPLAYERNAME\tFOOD EARNED" << std::endl;
		}
		if (mGameType == 2) {
			std::cout << "RANK\tPLAYERNAME\tFOOD" << std::endl;
		}

		for (int i = 0; i < players; i++) {
			std::string playerName = "NULL";
			int playerScore = 0;
			event.GetPacket() >> playerScore;
			event.GetPacket() >> playerName;

			if (i > -1) {
				std::cout << i << "\t|";
				std::cout << playerName;
				std::cout << "\t|" << playerScore << std::endl;
			}
		}

		auto evnt = std::make_unique<CNetworkEvent>();
		evnt->AddTypeToPacket(EGameEvent::NetServer_PlayerDisconnected);
		evnt->GetPacket() << mClientId;
		mNet.SendEventImmidient(std::move(evnt), mServer, gServerPort);
		mClientId = -1;
	}

	if (event.GetEvent() == EGameEvent::NetClient_PlayerSaid) {
		std::string message;
		event.GetPacket() >> message;

		mMessages = message;
		mMessagesText.SetString(message);
		mLastMessage = 60 * 3;
	}

	if (event.GetEvent() == EGameEvent::NetClient_GiveFood) {
		float food;
		event.GetPacket() >> food;
		mPlayers[mClientId].food += food;
	}

	if (event.GetEvent() == EGameEvent::NetClient_GiveBlocks) {
		int blocks;
		event.GetPacket() >> blocks;
		mPlayers[mClientId].blocks += blocks;
		mHammerText.SetString(std::to_string(mPlayers[mClientId].blocks));
	}

	if (event.GetEvent() == EGameEvent::NetClient_SetFood) {
		float food;
		event.GetPacket() >> food;
		mPlayers[mClientId].food = food;
	}

	if (event.GetEvent() == EGameEvent::NetClient_UpdateTimer) {
		float time;
		event.GetPacket() >> time;
		mSecondsLeft = time;
		mSecondsLeftText.SetString(std::to_string(mSecondsLeft));
	}

	if (event.GetEvent() == EGameEvent::NetClient_WorldUpdate) {
		mLevel.LoadFromPacket(event.GetPacket());
	}

	if (event.GetEvent() == EGameEvent::NetClient_UpdateAnimals) {
		SetAnimalsFromPacket(event);
	}

	if (event.GetEvent() == EGameEvent::NetClient_Daytime) {
		mSky.SetColor(sf::Color(40, 160, 255));
	}

	if (event.GetEvent() == EGameEvent::NetClient_Nighttime) {
		mSky.SetColor(sf::Color(40, 100, 150));
	}
}
void cClientState::Tick(const DeltaTime_t& deltaTime) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && m_game.GetFrameRate() < 60) {
		m_game.SetFrameRate(m_game.GetFrameRate() + 1);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && m_game.GetFrameRate() > 45) {
		m_game.SetFrameRate(m_game.GetFrameRate() - 1);
	}

	if (mClientId == -1)
		return;

	mDeltaTime = deltaTime;

	mLastMessage--;

	if (!mPlayers[mClientId].dead) {
		if (PlayerMove()) {
			auto evnt2 = std::make_unique<CNetworkEvent>();
			evnt2->AddTypeToPacket(EGameEvent::NetServer_PlayerPos);
			evnt2->GetPacket() << mClientId;
			evnt2->GetPacket() << mPlayers[mClientId].x;
			evnt2->GetPacket() << mPlayers[mClientId].y;
			mNet.SendEvent(std::move(evnt2), mServer, gServerPort);
		}
	}
	else if (mPlayers[mClientId].dead) {
		if (PlayerMoveDead()) {
			auto evnt2 = std::make_unique<CNetworkEvent>();
			evnt2->AddTypeToPacket(EGameEvent::NetServer_PlayerPos);
			evnt2->GetPacket() << mClientId;
			evnt2->GetPacket() << mPlayers[mClientId].x;
			evnt2->GetPacket() << mPlayers[mClientId].y;
			mNet.SendEvent(std::move(evnt2), mServer, gServerPort);
		}
	}

	mPlayers[mClientId].food -= (mFoodDec * mDeltaTime.count());

	if (m_game.GetTick() % 10 == 0) {
		auto evnt2 = std::make_unique<CNetworkEvent>();
		evnt2->AddTypeToPacket(EGameEvent::NetServer_PlayerFoodUpdate);
		evnt2->GetPacket() << mClientId;
		evnt2->GetPacket() << mPlayers[mClientId].food;
		mNet.SendEvent(std::move(evnt2), mServer, gServerPort);
	}

	if (!mIMMORTAL && mPlayers[mClientId].food < 0.0f && !mPlayers[mClientId].dead) {
		mPlayers[mClientId].dead = true;
		auto evnt2 = std::make_unique<CNetworkEvent>();
		evnt2->AddTypeToPacket(EGameEvent::NetServer_PlayerDied);
		evnt2->GetPacket() << mClientId;
		mNet.SendEvent(std::move(evnt2), mServer, gServerPort);
	}

	if (!mIMMORTAL && mPlayers[mClientId].food < 0.0f) {
		mPlayers[mClientId].food = 0.0f;
		mPlayers[mClientId].dead = true;
	}

	// Adding blocks
	//sf::Mouse::isButtonPressed(sf::Mouse::Left)
	if (mInput.GetBind("Build").IsPressed() && mPlayers[mClientId].blocks > 0 && !mFreezeControls) {
		sf::Vector2f p = mDraw.GetWindow().mapPixelToCoords(sf::Mouse::getPosition(mDraw.GetWindow()));
		//sf::Vector2i p = sf::Mouse::getPosition(mDraw.GetWindow());
		//p.x += mDraw.GetPos().x - 320;
		//p.y += mDraw.GetPos().y - 240;
		int x = p.x / 16;
		int y = p.y / 16;

		if (mLevel.GetBlock(x, y) == mCurrentBlock) {
			return;
		}

		mPlayers[mClientId].blocks--;
		auto evnt2 = std::make_unique<CNetworkEvent>();
		evnt2->AddTypeToPacket(EGameEvent::NetServer_BuildBlock);
		evnt2->GetPacket() << x;
		evnt2->GetPacket() << y;
		evnt2->GetPacket() << mCurrentBlock;
		mNet.SendEvent(std::move(evnt2), mServer, gServerPort);
	}

	if (mInput.GetBind("Break").IsPressed()) {
		mCycling = true;
		mFreezeControls = true;
	}

	if (mCycling) {
		if (mInput.GetBind("Forward").IsPressed()) {
			mCurrentBlock++;
		}
		else if (mInput.GetBind("Backward").IsPressed()) {
			mCurrentBlock--;
		}
		if (mCurrentBlock < 0) {
			mCurrentBlock = 0;
		}
		if (mCurrentBlock > 13) {
			mCurrentBlock = 13;
		}
		if (mInput.GetBind("Build").IsPressed()) {
			mCycling = false;
			mFreezeControls = false;
		}
	}
	
	PlayerCamera();

	for (auto& p : mAnimals) {
		p.second->OnUpdate();
	}
}
void cClientState::PlayerHammer() {

}
void cClientState::PlayerCamera() {
	mDraw.SetCamPos(mPlayers[mClientId].x, mPlayers[mClientId].y);
}
bool cClientState::PlayerMoveDead() {
	// Movement
	mPlayerSpeed = 3.0f * 76.0f;
	bool moved = false;
	if (mInput.GetBind("Right").IsDown() && !mFreezeControls) {
		mPlayers[mClientId].x += mPlayerSpeed * mDeltaTime.count();
		moved = true;
	}
	else if (mInput.GetBind("Left").IsDown() && !mFreezeControls) {
		mPlayers[mClientId].x -= mPlayerSpeed * mDeltaTime.count();
		moved = true;
	}
	if (mInput.GetBind("Forward").IsDown() && !mFreezeControls) {
		mPlayers[mClientId].y -= mPlayerSpeed * mDeltaTime.count();
		moved = true;
	}
	else if (mInput.GetBind("Backward").IsDown() && !mFreezeControls) {
		mPlayers[mClientId].y += mPlayerSpeed * mDeltaTime.count();
		moved = true;
	}
	return moved;
}
bool cClientState::PlayerMove() {
	// Movement
	if (mHasSkin) {
		mPlayerSpeed = mSkin.speed;
	}
	else {
		mPlayerSpeed = 3.0f * 76.0f;
	}
	bool moved = false;
	if (mInput.GetBind("Right").IsDown() && !mFreezeControls) {
		mPlayers[mClientId].xsp = mPlayerSpeed * mDeltaTime.count();
		mFacing = 1;
	}
	else if (mInput.GetBind("Left").IsDown() && !mFreezeControls) {
		mPlayers[mClientId].xsp = -mPlayerSpeed * mDeltaTime.count();
		mFacing = -1;
	}
	else {
		mPlayers[mClientId].xsp = 0.f;
	}

	if (mHasSkin) {
		if (mSkin.special == 1) {
			if (mInput.GetBind("Special").IsDown() && mPowerupCooldown == 0 && !mPlayers[mClientId].grd) {
				mPowerupCooldown = 31;
			}

			if (mPowerupCooldown > 1) {
				mPowerupCooldown--;
				mPlayers[mClientId].xsp = mFacing * (mPlayerSpeed * 3) * mDeltaTime.count();
				mPlayers[mClientId].ysp = 0.0f;
			}

			if (mPlayers[mClientId].grd) {
				mPowerupCooldown = 0;
			}
		}

		if (mSkin.special == 2) {
			if (mInput.GetBind("Special").IsDown() && mPowerupCooldown == 0 && !mPlayers[mClientId].grd) {
				mPowerupCooldown = 1;
				mPlayers[mClientId].ysp = mPlayers[mClientId].jmp;
			}

			if (mPlayers[mClientId].grd) {
				mPowerupCooldown = 0;
			}
		}

		if (mSkin.special == 4) {
			// BUILDING
			if (mInput.GetBind("Special").IsDown() && mPowerupCooldown <= 0 && mPlayers[mClientId].blocks >= 8) {
				mPowerupCooldown = 15;
				
				mPlayers[mClientId].blocks -= 8;
				int x = mPlayers[mClientId].x / 16;
				int y = mPlayers[mClientId].y / 16;
				x += (int)(mFacing);
				if (mFacing == 1) {
					x += mColW / 16;
				}
				y += 1;

				for (int i = 0; i < 10; i++) {
					auto evnt2 = std::make_unique<CNetworkEvent>();
					evnt2->AddTypeToPacket(EGameEvent::NetServer_BuildBlock);
					evnt2->GetPacket() << x;
					evnt2->GetPacket() << y;
					evnt2->GetPacket() << mCurrentBlock;
					mNet.SendEvent(std::move(evnt2), mServer, gServerPort);
					x += mFacing;
				}
			}

			mPowerupCooldown--;
		}

		if (mSkin.special == 3) {
			if (mInput.GetBind("Special").IsDown() && mPowerupCooldown == -10 && !mPlayers[mClientId].grd) {
				mPowerupCooldown = 120;
			}

			if (mInput.GetBind("Special").IsDown() && mPowerupCooldown > 0 && !mPlayers[mClientId].grd) {
				mPowerupCooldown--;
				mPlayers[mClientId].ysp = mPlayers[mClientId].jmp / 4.0f;
			}

			if (mPlayers[mClientId].grd) {
				mPowerupCooldown = -10;
			}
		}
	}

	mPlayers[mClientId].ysp += mPlayers[mClientId].grv * mDeltaTime.count();

	if (mPlayers[mClientId].grd && mPlayers[mClientId].ysp >= 0.0f && !mJumping) {
		mPlayers[mClientId].ysp = 0.0f;
	}

	if (mJumping && mPlayers[mClientId].grd) {
		mJumping = false;
	}

	if (mPlayers[mClientId].grd && mInput.GetBind("Jump").IsDown()
		&& !mFreezeControls &&      !IsPlayerLevelCollide(mPlayers[mClientId].x, mPlayers[mClientId].y - 5)) {
		mPlayers[mClientId].ysp = mPlayers[mClientId].jmp;
		mJumping = true;
	}

	// animals
	int collider = CollidingWith(-1);
	if (collider == -1) {
		mCanCollide = true;
	}
	if (collider != -1 && mCanCollide) {
		mCanCollide = false;
		
		if (mAnimals[collider]->mFriendly) {
			mPlayers[mClientId].y -= 40;
			if (!mInput.GetBind("Jump").IsDown() && !mFreezeControls) {
				mPlayers[mClientId].ysp = mPlayers[mClientId].jmp * 0.5f;
			}
			else {
				mPlayers[mClientId].ysp = mPlayers[mClientId].jmp;
			}
		}

		auto evnt2 = std::make_unique<CNetworkEvent>();
		evnt2->AddTypeToPacket(EGameEvent::NetServer_AnimalHurt);
		evnt2->GetPacket() << collider;
		evnt2->GetPacket() << mClientId;
		if (mHasSkin) {
			evnt2->GetPacket() << mSkin.damage;
		}
		else {
			evnt2->GetPacket() << 1;
		}
		mNet.SendEvent(std::move(evnt2), mServer, gServerPort);
	}

	sClientPlayer& mP = mPlayers[mClientId];

	float xsp = mP.xsp;// *mDeltaTime.count();
	float ysp = mP.ysp * mDeltaTime.count();

	//std::cout << "x: " << mPlayers[mClientId].x << std::endl;

	// Collision
	if (IsPlayerLevelCollide(mP.x + xsp, mP.y)) {
		while (!IsPlayerLevelCollide(mP.x + sign(xsp), mP.y)) {
			mP.x += sign(xsp);
		}
		mP.xsp = 0;
		xsp = 0;
	}
	mP.x += xsp;
	mP.grd = false;
	if (IsPlayerLevelCollide(mP.x, mP.y + ysp)) {
		while (!IsPlayerLevelCollide(mP.x, mP.y + sign(ysp))) {
			mP.y += sign(ysp);
		}
		mP.ysp = 0;
		ysp = 0;
		mP.grd = true;
	}
	//if (IsPlayerLevelCollide(mP.x, mP.y - 2)) {
	//	ysp = 0;
	//}
	mP.y += ysp;

	// Bounds detection
	if (mP.x < 8)
		mP.x = 8;
	if (mP.x > 511.5f * 16.f)
		mP.x = 511.5f * 16.f;
	if (mP.y > 64.f * 16.f) {
		mPlayers[mClientId].x = 128 * 16;
		mPlayers[mClientId].y = 0;
		mPlayers[mClientId].food -= 250;
	}

	// Networking
	if (mPlayers[mClientId].xsp != 0 ||
		mPlayers[mClientId].ysp != 0) {
		moved = true;
	}

	return moved;
}
bool cClientState::IsPlayerLevelCollide(int x, int y) {
	int left = x;
	int top = y;
	int right = x + mColW;
	int bottom = y + mColH;

	if (IsBlockSolid(mLevel.GetBlockFree(left, top)))
		return true;
	if (IsBlockSolid(mLevel.GetBlockFree(right, top)))
		return true;
	if (IsBlockSolid(mLevel.GetBlockFree(right, bottom)))
		return true;
	if (IsBlockSolid(mLevel.GetBlockFree(left, bottom)))
		return true;

	return false;
}
int cClientState::CollidingWith(int type) {
	for (auto& p : mAnimals) {
		if (p.second->mType == type ||
			type == -1) {
			if (p.second->pos.x < mPlayers[mClientId].x + 16 &&
				p.second->pos.x + p.second->mTexture.Get().getSize().x > mPlayers[mClientId].x &&
				p.second->pos.y < mPlayers[mClientId].y + 32 &&
				p.second->pos.y + p.second->mTexture.Get().getSize().y > mPlayers[mClientId].y) {
				return p.first;
			}
		}
	}

	return -1;
}
void cClientState::Draw() {
	if (mClientId == -1)
		return;

	mDraw.DrawToLayer(mSky);
	mLevel.Render(mDraw);

	for (auto& p : mPlayers) {
		p.second.spr.SetPos(p.second.x, p.second.y);
		mDraw.DrawToLayer(p.second.spr);
		p.second.nametext.SetPos(p.second.x+8, p.second.y-40);
		if (p.second.nametext.GetPos().x > mDraw.GetPos().x+290) {
			p.second.nametext.SetPos(mDraw.GetPos().x + 290, p.second.nametext.GetPos().y);
		}
		if (p.second.nametext.GetPos().x < mDraw.GetPos().x - 290) {
			p.second.nametext.SetPos(mDraw.GetPos().x - 290, p.second.nametext.GetPos().y);
		}
		if (p.second.nametext.GetPos().y > mDraw.GetPos().y + 210) {
			p.second.nametext.SetPos(p.second.nametext.GetPos().x, mDraw.GetPos().y + 210);
		}
		if (p.second.nametext.GetPos().y < mDraw.GetPos().y - 210) {
			p.second.nametext.SetPos(p.second.nametext.GetPos().x, mDraw.GetPos().y - 210);
		}
		mDraw.DrawToLayer(p.second.nametext);
		p.second.foodtext.SetPos(p.second.x + 8, p.second.y - 80);
		p.second.foodtext.SetString(std::to_string((int)p.second.food));
		mDraw.DrawToLayer(p.second.foodtext);
	}

	for (auto& p : mAnimals) {
		p.second->OnDraw(mDraw);
	}

	if (mSayingSomething) {
		mSayingText.SetString(mSaying);
		Vector2i pos;
		pos.x = mDraw.GetPos().x - 320;
		pos.y = mDraw.GetPos().y - 240;
		mSayingText.SetPos(pos);
		mDraw.DrawToLayer(mSayingText);
	}

	if (mLastMessage > 0) {
		Vector2i pos;
		pos.x = mDraw.GetPos().x - 320;
		pos.y = mDraw.GetPos().y - 240 + 40;
		mMessagesText.SetPos(pos);
		mDraw.DrawToLayer(mMessagesText);
	}

	if (mGameType == 2) {
		//mSecondsLeftText.SetString(std::to_string(mSecondsLeft));
		Vector2i pos;
		pos.x = mDraw.GetPos().x;
		pos.y = mDraw.GetPos().y - 230;
		mSecondsLeftText.SetPos(pos);
		mDraw.DrawToLayer(mSecondsLeftText);
	}

	Vector2i pos2;
	pos2.x = mDraw.GetPos().x - 64;
	pos2.y = mDraw.GetPos().y + 240 - 64;
	mHammerSprite.SetPos(pos2);
	mDraw.DrawToLayer(mHammerSprite);
	pos2.x = mDraw.GetPos().x + 10;
	pos2.y = mDraw.GetPos().y + 240 - 40;
	mHammerText.SetPos(pos2);
	mHammerText.SetString(std::to_string(mPlayers[mClientId].blocks));
	mDraw.DrawToLayer(mHammerText);

	pos2.x = mDraw.GetPos().x + 25;
	pos2.y = mDraw.GetPos().y + 240 - 60;
	mCurrentBlockSprite.SetPos(pos2);
	sf::IntRect rect;
	rect.left = mCurrentBlock % 16 * 16;
	rect.top = mCurrentBlock / 16 * 16;
	rect.height = 16;
	rect.width = 16;
	mCurrentBlockSprite.GetSprite().setTextureRect(rect);
	mDraw.DrawToLayer(mCurrentBlockSprite);
}
void cClientState::OnExit() {

}
void cClientState::SetAnimalsFromPacket(CNetworkEvent& packet) {
	mAnimals.clear();

	for (int i = 0; i < gAnimalsMax; i++) {
		int id;
		packet.GetPacket() >> id;
		if (id == -1) {
			break;
		}
		int type;
		packet.GetPacket() >> type;
		mAnimals[id] = std::move(MakeNewClientAnimal(type, mRcTex, mFont));
		packet.GetPacket() >> mAnimals[id]->mHp;
		packet.GetPacket() >> mAnimals[id]->mFriendly;
		packet.GetPacket() >> mAnimals[id]->pos.x;
		packet.GetPacket() >> mAnimals[id]->pos.y;
		packet.GetPacket() >> mAnimals[id]->mXSpeed;
		packet.GetPacket() >> mAnimals[id]->mYSpeed;
	}
}
