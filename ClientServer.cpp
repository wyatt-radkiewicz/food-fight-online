#include "ClientServer.h"
#include "Server.h"

bool gBURN = false;
bool gNoBurn = false;
bool gCoolCows = false;
bool gTBuild = false;

std::unique_ptr<cClientAnimal> MakeNewClientAnimal(int type, CResourceController<sf::Texture>& textures,
	sf::Font& fonts) {
	switch (type) {
	case 0:
		return std::make_unique<cClientAnimal>(textures, fonts, type);
	case 1:
		return std::make_unique<cClientPig>(textures, fonts, type);
	case 2:
		return std::make_unique<cClientCow>(textures, fonts, type);
	case 3:
		return std::make_unique<cClientChicken>(textures, fonts, type);
	case 4:
		return std::make_unique<cClientZombie>(textures, fonts, type);
	case 5:
		return std::make_unique<cClientCropduster>(textures, fonts, type);
	case 6:
		return std::make_unique<cClientAcid>(textures, fonts, type);
	default:
		return std::make_unique<cClientAnimal>(textures, fonts, type);
	}
}

cClientAnimal::cClientAnimal(CResourceController<sf::Texture>& textures,
	sf::Font& fonts, int type)
	: mXSpeed(0.0f), mYSpeed(0.0f), mType(type) {
	
}
cClientAnimal::~cClientAnimal() {

}
void cClientAnimal::OnUpdate() {
	pos.x += mXSpeed;
	pos.y += mYSpeed;
}
void cClientAnimal::OnDraw(CDrawModule& drawer) {
	mSprite.SetPos(Vector2i(pos.x, pos.y));
	drawer.DrawToLayer(mSprite);
	mHpText.SetPos(Vector2i(pos.x + 16, pos.y - 30));
	mHpText.SetString(std::to_string(mHp));
	drawer.DrawToLayer(mHpText);
}

cServerAnimal::cServerAnimal() {

}
cServerAnimal::~cServerAnimal() {

}
void cServerAnimal::OnUpdate(int id, cLevel& level, std::stack<int>& deletes, std::map<int, std::unique_ptr<cServerAnimal>>& animals,
	std::map<int, sServerPlayer>& players, bool daytime, cServerState& state) {

}

// CUSTOM ANIMALLS
// ANIMALS ANIMALS ANIMALS ANIMALS ANIMALS ANIMALS
cClientPig::cClientPig(CResourceController<sf::Texture>& textures,
	sf::Font& fonts, int type)
	: cClientAnimal(textures, fonts, type){
	mFriendly = true;
	mDamage = 0;
	mHp = 2;
	mFood = 50;
	mHpText.SetFont(fonts);
	if (mFriendly) {
		mHpText.SetColor(Color(0.0f, 1.0f, 0.0f));
	}
	else {
		mHpText.SetColor(Color(0.7f, 0.0f, 0.0f));
	}
	mHpText.SetAlign(ETextAlign::Middle);
	mHpText.SetLayer(10);
	mTexture.Load(textures, "Assets/FlyingPig.png");
	mSprite.SetTexture(mTexture.Get());
	mSprite.SetLayer(15);
}
cClientPig::~cClientPig() {

}
void cClientPig::OnDraw(CDrawModule& drawer) {
	mSprite.SetPos(Vector2i(pos.x, pos.y));
	drawer.DrawToLayer(mSprite);
	mHpText.SetPos(Vector2i(pos.x + 16, pos.y - 30));
	mHpText.SetString(std::to_string(mHp));
	drawer.DrawToLayer(mHpText);
}
cServerPig::cServerPig() {
	mFriendly = true;
	mType = 1;
	mDamage = 0;
	mFood = 50;
	mHp = 2;
	mTargetHeight = 1024;
	mFacing = getRandom(0, 2) * 2 - 1;
	mBlocks = 8;
}
cServerPig::~cServerPig() {

}
void cServerPig::OnUpdate(int id, cLevel& level, std::stack<int>& deletes, std::map<int, std::unique_ptr<cServerAnimal>>& animals,
	std::map<int, sServerPlayer>& players, bool daytime, cServerState& state) {
	pos.x += 0.5f * mFacing;
	mXSpeed = 0.5f * mFacing;

	if (pos.x < 0 || pos.x > 512 * 16) {
		deletes.push(id);
	}

	if (pos.y > mTargetHeight) {
		pos.y -= 4.0f;
		mYSpeed = -4.0f;
	}
	else {
		mYSpeed = 0.0f;
		mTargetHeight = 1024;
	}

	if (IsBlockSolid((level.GetBlockFree(pos.x, pos.y)) && mTargetHeight == 1024)) {
		mTargetHeight = pos.y - getRandom(300, 475);
	}
}

cClientCow::cClientCow(CResourceController<sf::Texture>& textures,
	sf::Font& fonts, int type)
	: cClientAnimal(textures, fonts, type){
	mFriendly = true;
	mDamage = 0;
	mHp = 5;
	mFood = 300;
	mHpText.SetFont(fonts);
	if (mFriendly) {
		mHpText.SetColor(Color(0.0f, 1.0f, 0.0f));
	}
	else {
		mHpText.SetColor(Color(0.7f, 0.0f, 0.0f));
	}
	mHpText.SetAlign(ETextAlign::Middle);
	mHpText.SetLayer(10);
	mTexture.Load(textures, "Assets/Cow.png");
	mSprite.SetTexture(mTexture.Get());
	mSprite.SetLayer(15);
}
cClientCow::~cClientCow() {

}

void cClientCow::OnDraw(CDrawModule& drawer) {
	mSprite.SetPos(Vector2i(pos.x, pos.y));
	drawer.DrawToLayer(mSprite);
	mHpText.SetPos(Vector2i(pos.x + 16, pos.y - 30));
	mHpText.SetString(std::to_string(mHp));
	drawer.DrawToLayer(mHpText);
}

cServerCow::cServerCow() {
	mFriendly = true;
	mType = 2;
	mDamage = 0;
	mFood = 300;
	mHp = 5;
	mFacing = getRandom(0, 2) * 2 - 1;
	mXsp = 0.0f;
	mYsp = 0.0f;
}
cServerCow::~cServerCow() {

}

void cServerCow::OnUpdate(int id, cLevel& level, std::stack<int>& deletes, std::map<int, std::unique_ptr<cServerAnimal>>& animals,
	std::map<int, sServerPlayer>& players, bool daytime, cServerState& state) {
	mXsp = 2.8f * mFacing;
	mXSpeed = mXsp;
	mYsp += 0.2f;

	if (pos.x < 0) {
		mFacing = 1.f;
		pos.x = 16.0f;
		mYsp = -10.0f;
	}
	if (pos.x > 511 * 16) {
		mFacing = -1.f;
		pos.x = (510 * 16);
		mYsp = -10.0f;
	}

	if (pos.y > 64 * 16) {
		deletes.push(id);
	}

	// Collision
	if (gCoolCows) {
		if (level.LevelCollide(pos.x + mXsp, pos.y, 32, 32)) {
			int tries = 0;
			while (!level.LevelCollide((int)(pos.x + sign(mXsp)), (int)(pos.y), 32, 32)) {
				tries++;
				if (sign(mXsp) != 0) {
					pos.x += sign(mXsp);
				}
				else {
					break;
				}
				if (tries > 16 * 5) {
					deletes.push(id);
					break;
				}
			}
			mXsp = 0;
			mXSpeed = 0;
		}
	}
	if (level.LevelCollide(pos.x, pos.y + mYsp, 32, 32)) {
		int tries = 0;
		while (!level.LevelCollide((int)pos.x, (int)(pos.y + sign(mYsp)), 32, 32)) {
			tries++;
			if (sign(mYsp) != 0) {
				pos.y += sign(mYsp);
			}
			else {
				break;
			}
			if (tries > 16 * 5) {
				deletes.push(id);
				break;
			}
		}
		mYsp = 0;
	}
	if (level.LevelCollide(pos.x, pos.y + 2, 32, 32)) {
		mYsp = 0.0f;
		if (level.LevelCollide(pos.x + (mFacing * 4), pos.y, 32, 32)) {
			mYsp = -5.0f;
		}
	}
	pos.y += mYsp;
	mYSpeed = mYsp;
	pos.x += mXsp;
}

cClientChicken::cClientChicken(CResourceController<sf::Texture>& textures,
	sf::Font& fonts, int type) 
	: cClientAnimal(textures, fonts, type) {
	mFriendly = true;
	mDamage = 100;
	mHp = 3;
	mFood = 100;
	mHpText.SetFont(fonts);
	if (mFriendly) {
		mHpText.SetColor(Color(0.0f, 1.0f, 0.0f));
	}
	else {
		mHpText.SetColor(Color(0.7f, 0.0f, 0.0f));
	}
	mHpText.SetAlign(ETextAlign::Middle);
	mHpText.SetLayer(10);
	mTexture.Load(textures, "Assets/Chicken.png");
	mSprite.SetTexture(mTexture.Get());
	mSprite.SetLayer(15);
}
cClientChicken::~cClientChicken() {

}

void cClientChicken::OnDraw(CDrawModule& drawer) {
	if (mFriendly) {
		mHpText.SetColor(Color(0.0f, 1.0f, 0.0f));
	}
	else {
		mHpText.SetColor(Color(0.7f, 0.0f, 0.0f));
	}
	mSprite.SetPos(Vector2i(pos.x, pos.y));
	drawer.DrawToLayer(mSprite);
	mHpText.SetPos(Vector2i(pos.x + 16, pos.y - 30));
	mHpText.SetString(std::to_string(mHp));
	drawer.DrawToLayer(mHpText);
}
cServerChicken::cServerChicken() {
	mFriendly = true;
	mType = 3;
	mDamage = 100;
	mFood = 100;
	mHp = 1;
	mFacing = getRandom(0, 2) * 2 - 1;
	mState = 0;
	mSpeed = getRandom(8, 32);
}
cServerChicken::~cServerChicken() {

}

void cServerChicken::OnUpdate(int id, cLevel& level, std::stack<int>& deletes, std::map<int, std::unique_ptr<cServerAnimal>>& animals,
	std::map<int, sServerPlayer>& players, bool daytime, cServerState& state) {
	if (pos.y > 64 * 16) {
		deletes.push(id);
	}

	if (pos.x < 0 || pos.x > 512 * 16) {
		deletes.push(id);
	}
	
	if (mState == 0) {
		mFriendly = true;
		pos.x += 1.2f * mFacing;
		mXSpeed = 1.2f * mFacing;
		mYSpeed = 0.0f;

		for (auto& p : players) {
			if (pos.x > p.second.x - 6 &&
				pos.x < p.second.x + 6 &&
				pos.y < p.second.y) {
				mState = 1;
				break;
			}
		}
	}
	else {
		mFriendly = false;
		pos.y += mSpeed;
		mXSpeed = 0.0f;
		mYSpeed = mSpeed;

		if (gTBuild)
			return;

		if (IsBlockSolid(level.GetBlock(pos.x / 16, pos.y / 16))) {
			level.SetBlock(pos.x / 16, pos.y / 16, 0);
			level.SetBlock((pos.x / 16) + 1, (pos.y / 16) + 0, 0);
			level.SetBlock((pos.x / 16) - 1, (pos.y / 16) + 0, 0);
			level.SetBlock((pos.x / 16) + 0, (pos.y / 16) + 1, 0);
			level.SetBlock((pos.x / 16) + 1, (pos.y / 16) + 1, 0);
			level.SetBlock((pos.x / 16) - 1, (pos.y / 16) + 1, 0);
			// -10 to -26
			deletes.push(-10 - (pos.x / 16 / 32));
			deletes.push(id);
		}
	}
}

cClientZombie::cClientZombie(CResourceController<sf::Texture>& textures,
	sf::Font& fonts, int type) 
	: cClientAnimal(textures, fonts, type) {
	mFriendly = false;
	mDamage = 50;
	mHp = 3;
	mFood = 750;
	mHpText.SetFont(fonts);
	if (mFriendly) {
		mHpText.SetColor(Color(0.0f, 1.0f, 0.0f));
	}
	else {
		mHpText.SetColor(Color(0.7f, 0.0f, 0.0f));
	}
	mHpText.SetAlign(ETextAlign::Middle);
	mHpText.SetLayer(10);
	mTexture.Load(textures, "Assets/Zombie.png");
	mSprite.SetTexture(mTexture.Get());
	mSprite.SetLayer(15);
}
cClientZombie::~cClientZombie() {

}

void cClientZombie::OnDraw(CDrawModule& drawer) {
	mSprite.SetPos(Vector2i(pos.x, pos.y));
	drawer.DrawToLayer(mSprite);
	mHpText.SetPos(Vector2i(pos.x + 16, pos.y - 30));
	mHpText.SetString(std::to_string(mHp));
	drawer.DrawToLayer(mHpText);
}

cServerZombie::cServerZombie() {
	mFindtick = 0;
	mXsp = 0;
	mYsp = 0;
	mFriendly = false;
	mType = 4;
	mDamage = 50;
	mFood = 750;
	mHp = 3;
	mFacing = getRandom(0, 2) * 2 - 1;
	mSpeed = getRandom(1, 3);
	int sm = getRandom(1, 21);
	if (sm >= 0 && sm <= 8) {
		mSpeed *= 1;
	}
	if (sm >= 9 && sm <= 15) {
		mSpeed *= 2;
	}
	if (sm >= 16 && sm <= 18) {
		mSpeed *= 3;
	}
	if (sm >= 19 && sm <= 21) {
		mSpeed *= 4;
	}
}
cServerZombie::~cServerZombie() {

}

void cServerZombie::OnUpdate(int id, cLevel& level, std::stack<int>& deletes, std::map<int, std::unique_ptr<cServerAnimal>>& animals,
	std::map<int, sServerPlayer>& players, bool daytime, cServerState& state) {
	mXsp = mSpeed * mFacing;
	mXSpeed = mXsp;
	mYsp += 0.2f;
	mFindtick++;

	if (mFindtick % 120 == 0) {
		// See if there is a player close to us
		for (auto& p : players) {
			if (pos.x > p.second.x - 640 && 
				pos.x < p.second.x + 640) {
				if (pos.x > p.second.x) {
					mFacing = -1;
				}
				else {
					mFacing = 1;
				}
			}
		}
	}

	if (pos.x < 0) {
		mFacing = 1.f;
		pos.x = 16.0f;
		mYsp = -8.0f;
	}
	if (pos.x > 511 * 16) {
		mFacing = -1.f;
		pos.x = (510 * 16);
		mYsp = -8.0f;
	}

	if (pos.y > 64 * 16) {
		deletes.push(id);
	}

	if (level.LevelCollide(pos.x, pos.y + 1, 16, 32)) {
		mYsp = 0.0f;
	}
	if (level.LevelCollide(pos.x + (mFacing * mSpeed), pos.y - 1, 16, 32)) {
		mXsp = 0.0f;
		mXSpeed = 0.0f;
		mYsp = -1.5f;
	}

	// Collision
	if (level.LevelCollide(pos.x, pos.y + mYsp, 16, 32)) {
		int tries = 0;
		while (!level.LevelCollide((int)pos.x, (int)(pos.y + sign(mYsp)), 16, 32)) {
			tries++;
			if (sign(mYsp) != 0) {
				pos.y += sign(mYsp);
			}
			else {
				break;
			}
			if (tries > 16 * 5) {
				deletes.push(id);
				break;
			}
		}
		mYsp = 0;
	}
	pos.y += mYsp;
	mYSpeed = mYsp;
	pos.x += mXsp;

	if (level.LevelCollide(pos.x, pos.y, 16, 32)) {
		pos.y -= 10;
	}

	if (gTBuild)
		return;

	if (gBURN) {
		if (level.LevelCollide(pos.x + (mFacing * mSpeed) + 2, pos.y + 2, 10, 26) && mSpeed > 3
			&& getRandom(0, 3) == 1) {
			level.SetBlock((pos.x + (mFacing * mSpeed)) / 16, pos.y / 16, 0);
			level.SetBlock((pos.x + (mFacing * mSpeed)) / 16, (pos.y / 16) + 1, 0);
			level.SetBlock((pos.x + (mFacing * mSpeed)) / 16, (pos.y / 16) + 2, 0);
			level.SetBlock((pos.x + (mFacing * mSpeed)) / 16, (pos.y / 16) - 1, 0);
			deletes.push(-10 - (pos.x / 16 / 32));
		}
	}
	else  if (!gNoBurn) {
		if (level.LevelCollide(pos.x + 2, pos.y + 2, 10, 26) && mSpeed > 1) {
			level.SetBlock(pos.x / 16, pos.y / 16, 0);
			level.SetBlock(pos.x / 16, (pos.y / 16) + 1, 0);
			if (getRandom(0, 5) == 3) {
				level.SetBlock(pos.x + (mFacing * mSpeed) / 16, pos.y / 16, 0);
				level.SetBlock(pos.x + (mFacing * mSpeed) / 16, (pos.y / 16) + 1, 0);
			}
			deletes.push(-10 - (pos.x / 16 / 32));
		}

		if (getRandom(0, 16) == 8) {
			level.SetBlock(pos.x / 16, (pos.y / 16) + 2, 0);
			level.SetBlock(pos.x / 16, (pos.y / 16) + 3, 0);
			level.SetBlock((pos.x / 16) + 1, (pos.y / 16) + 2, 0);
			level.SetBlock((pos.x / 16) + 1, (pos.y / 16) + 3, 0);
			level.SetBlock((pos.x / 16) - 1, (pos.y / 16) + 2, 0);
			level.SetBlock((pos.x / 16) - 1, (pos.y / 16) + 3, 0);
		}

		if (level.LevelCollide(pos.x + (mFacing * mSpeed) + 2, pos.y + 2, 10, 26) && mSpeed > 3
			&& getRandom(0, 8) == 1) {
			level.SetBlock((pos.x + (mFacing * mSpeed)) / 16, pos.y / 16, 0);
			level.SetBlock((pos.x + (mFacing * mSpeed)) / 16, (pos.y / 16) + 1, 0);
			level.SetBlock((pos.x + (mFacing * mSpeed)) / 16, (pos.y / 16) + 2, 0);
			level.SetBlock((pos.x + (mFacing * mSpeed)) / 16, (pos.y / 16) - 1, 0);
			deletes.push(-10 - (pos.x / 16 / 32));
		}
	}
	else {
		if (level.LevelCollide(pos.x + 2, pos.y + 2, 10, 26) && mSpeed > 1) {
			level.SetBlock(pos.x / 16, pos.y / 16, 0);
			level.SetBlock(pos.x / 16, (pos.y / 16) + 1, 0);
		}
	}
}

cClientCropduster::cClientCropduster(CResourceController<sf::Texture>& textures,
	sf::Font& fonts, int type)
	: cClientAnimal(textures, fonts, type) {
	mFriendly = false;
	mDamage = 200;
	mHp = 99;
	mFood = 5000;
	mHpText.SetFont(fonts);
	if (mFriendly) {
		mHpText.SetColor(Color(0.0f, 1.0f, 0.0f));
	}
	else {
		mHpText.SetColor(Color(0.7f, 0.0f, 0.0f));
	}
	mHpText.SetAlign(ETextAlign::Middle);
	mHpText.SetLayer(10);
	mTexture.Load(textures, "Assets/Cropduster.png");
	mSprite.SetTexture(mTexture.Get());
	mSprite.SetLayer(15);
}
cClientCropduster::~cClientCropduster() {

}

void cClientCropduster::OnDraw(CDrawModule& drawer) {
	mSprite.SetPos(Vector2i(pos.x, pos.y));
	drawer.DrawToLayer(mSprite);
	mHpText.SetPos(Vector2i(pos.x + 16, pos.y - 30));
	mHpText.SetString(std::to_string(mHp));
	drawer.DrawToLayer(mHpText);
}

cServerCropduster::cServerCropduster() {
	mFriendly = false;
	mDamage = 200;
	mHp = 99;
	mFood = 5000;
	mType = 5;
	mFacing = 0;
	mSpawnTimer = 10;
}
cServerCropduster::~cServerCropduster() {

}

void cServerCropduster::OnUpdate(int id, cLevel& level, std::stack<int>& deletes, std::map<int, std::unique_ptr<cServerAnimal>>& animals,
	std::map<int, sServerPlayer>& players, bool daytime, cServerState& state) {
	if (mFacing == 0) {
		if (pos.x > 256 * 16) {
			mFacing = -1;
		}
		if (pos.x < 256 * 16) {
			mFacing = 1;
		}
	}
	
	pos.x += 32.0f * mFacing;
	mXSpeed = 32.0f * mFacing;
	mYSpeed = 0.0f;

	if (pos.x < -16 || pos.x > 513 * 16) {
		deletes.push(id);
	}

	mSpawnTimer--;

	if (mSpawnTimer < 0) {
		mSpawnTimer = 2;
		int times = getRandom(1, 5);
		for (int i = 0; i < times; i++) {
			cServerAcid* acid = new cServerAcid();
			acid->mXsp = getRandom(-2, 2);
			acid->mYsp = getRandom(6, 16);
			int acid_id = state.CreateAnimal(acid, pos.x+getRandom(0, 200), pos.y+32);
		}
	}
}

cClientAcid::cClientAcid(CResourceController<sf::Texture>& textures,
	sf::Font& fonts, int type)
	: cClientAnimal(textures, fonts, type) {
	mFriendly = false;
	mDamage = 75;
	mHp = 1;
	mFood = 10;
	mHpText.SetAlign(ETextAlign::Middle);
	mHpText.SetLayer(10);
	mTexture.Load(textures, "Assets/Acid.png");
	mSprite.SetTexture(mTexture.Get());
	mSprite.SetLayer(15);
}
cClientAcid::~cClientAcid() {

}

void cClientAcid::OnDraw(CDrawModule& drawer) {
	mSprite.SetPos(Vector2i(pos.x, pos.y));
	drawer.DrawToLayer(mSprite);
	mHpText.SetPos(Vector2i(pos.x + 16, pos.y - 30));
	mHpText.SetString(std::to_string(mHp));
	drawer.DrawToLayer(mHpText);
}

cServerAcid::cServerAcid() {
	mFriendly = false;
	mDamage = 75;
	mHp = 1;
	mFood = 10;
	mType = 6;
	mXsp = 0;
	mYsp = 0;
}
cServerAcid::~cServerAcid() {

}

void cServerAcid::OnUpdate(int id, cLevel& level, std::stack<int>& deletes, std::map<int, std::unique_ptr<cServerAnimal>>& animals,
	std::map<int, sServerPlayer>& players, bool daytime, cServerState& state) {
	pos.x += mXsp;
	pos.y += mYsp;
	mXSpeed = mXsp;
	mYSpeed = mYsp;

	if (gTBuild)
		return;

	if (pos.y > 64 * 16 || IsBlockSolid(level.GetBlockFree(pos.x, pos.y))) {
		if (gBURN) {
			level.SetBlock(pos.x / 16, pos.y / 16, 0);
			deletes.push(-10 - (pos.x / 16 / 32));
		}
		else if (getRandom(0, 10) == 5 && !gNoBurn) {
			level.SetBlock(pos.x / 16, pos.y / 16, 0);
			deletes.push(-10 - (pos.x / 16 / 32));
		}
		else if (getRandom(0, 13) == 5) {
			level.SetBlock(pos.x / 16, pos.y / 16, 0);
				deletes.push(-10 - (pos.x / 16 / 32));
		}
		deletes.push(id);
	}
}
