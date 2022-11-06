#include "Level.h"

bool IsBlockSolid(int type) {
	if (type == 0 ||
		type == 4 ||
		type == 5 ||
		type == 10 ||
		type == 11) {
		return false;
	}
	return true;
}

cLevelDrawer::cLevelDrawer(cLevel* level) {
	mLevel = level;
}
cLevelDrawer::~cLevelDrawer() {

}
void cLevelDrawer::Draw(sf::RenderTarget& target) {
	int viewx = (target.getView().getCenter().x - 320) / 16;
	int viewy = (target.getView().getCenter().y - 240) / 16;

	sf::Sprite spr;
	spr.setTexture(mLevel->mTexture.Get());
	sf::IntRect rect;

	for (int x = viewx-1; x < viewx+43; x++) {
		for (int y = viewy-1; y < viewy+33; y++) {
			spr.setPosition(x * 16, y * 16);
			rect.left = mLevel->GetBlock(x, y) % 16 * 16;
			rect.top = mLevel->GetBlock(x, y) / 16 * 16;
			rect.height = 16;
			rect.width = 16;
			spr.setTextureRect(rect);
			target.draw(spr);
		}
	}
}

cLevel::cLevel()
	: mDrawer(this) {
	mWidth = 512;
	mHeight = 64;
	mData = new char[mWidth * mHeight];
}
cLevel::~cLevel() {
	delete mData;
}
int cLevel::Generate() {
	// Generate level
	randomSeed();
	int startHeight = getRandom(30, 39);
	int height = startHeight;
	int biome = getRandom(0, 5);
	int newBiome = getRandom(25, 50);
	for (int x = 0; x < mWidth; x++) {
		for (int y = 0; y < mHeight; y++) {
			SetBlock(x, y, 0);
		}
	}
	for (int x = 0; x < mWidth; x++) {
		newBiome--;
		if (newBiome <= 0) {
			newBiome = getRandom(25, 50);
			biome = getRandom(0, 5);
		}
		GenerateBiomeColumn(x, biome, height);
	}

	return startHeight;
}
void cLevel::GenerateStructure(int x, int y, int type) {
	switch (type) {
	case 0:
		SetBlock(x, y, 4);
		SetBlock(x, y-1, 4);
		SetBlock(x, y-2, 4);
		SetBlock(x, y-3, 4);
		SetBlock(x, y-4, 4);

		SetBlock(x-2, y - 5, 5);
		SetBlock(x-1, y - 5, 5);
		SetBlock(x+0, y - 5, 5);
		SetBlock(x+1, y - 5, 5);
		SetBlock(x+2, y - 5, 5);
		SetBlock(x - 2, y - 6, 5);
		SetBlock(x - 1, y - 6, 5);
		SetBlock(x + 0, y - 6, 5);
		SetBlock(x + 1, y - 6, 5);
		SetBlock(x + 2, y - 6, 5);
		SetBlock(x - 1, y - 7, 5);
		SetBlock(x + 0, y - 7, 5);
		SetBlock(x + 1, y - 7, 5);
		break;
	case 1:
		SetBlock(x, y - 0, 10);
		SetBlock(x, y - 1, 10);
		SetBlock(x, y - 2, 10);
		SetBlock(x, y - 3, 10);
		SetBlock(x, y - 4, 10);
		SetBlock(x, y - 5, 10);
		SetBlock(x, y - 6, 10);

		SetBlock(x - 2, y - 1, 11);
		SetBlock(x - 1, y - 1, 11);
		SetBlock(x + 1, y - 1, 11);
		SetBlock(x + 2, y - 1, 11);

		SetBlock(x - 1, y - 3, 11);
		SetBlock(x + 1, y - 3, 11);
		SetBlock(x - 1, y - 5, 11);
		SetBlock(x + 1, y - 5, 11);
		SetBlock(x + 0, y - 7, 11);
		break;
	default:
		break;
	}
}
void cLevel::GenerateBiomeColumn(int x, int biome, int& height) {
	for (int y = 0; y < mHeight; y++) {
		if (y == height) {
			switch (biome) {
			case 0:
				SetBlock(x, y, 1);
				break;
			case 2:
				SetBlock(x, y, 6);
				break;
			case 3:
				SetBlock(x, y, 8);
				break;
			case 4:
				SetBlock(x, y, 12);
				break;
			case 1:
				SetBlock(x, y, 3);
				break;
			}
		}
		else if (y > height && y < height + 15) {
			switch (biome) {
			case 0:
				SetBlock(x, y, 2);
				break;
			case 2:
				SetBlock(x, y, 7);
				break;
			case 3:
				SetBlock(x, y, 9);
				break;
			case 4:
				SetBlock(x, y, 12);
				break;
			case 1:
				SetBlock(x, y, 3);
				break;
			}
		}
		else if (y > height + 14) {
			switch (biome) {
			case 0:
				SetBlock(x, y, 3);
				break;
			case 2:
				SetBlock(x, y, 7);
				break;
			case 3:
				SetBlock(x, y, 9);
				break;
			case 4:
				SetBlock(x, y, 12);
				break;
			case 1:
				SetBlock(x, y, 3);
				break;
			}
		}
	}
	if (biome == 0 || biome == 3 || biome == 4) {
		height += getRandom(-2, 2);
	}
	else if (biome == 1) {
		height += getRandom(-2, 2) * 2;
	}
	else if (biome == 2) {
		height += getRandom(0, 2) * 2 - 1;
	}

	if (getRandom(0, 6) == 4 && biome == 0) {
		GenerateStructure(x, height-1, 0);
	}
	if (getRandom(0, 7) == 4 && biome == 3) {
		GenerateStructure(x, height-1, 1);
	}
}
void cLevel::LoadContent(CResourceController<sf::Texture>& c) {
	mTexture.Load(c, "Assets/Level.png");
}
sf::Packet cLevel::ContructPacket(int xOffset) {
	sf::Packet p;
	p << (int32_t)EGameEvent::NetClient_WorldUpdate;

	int xOffsetEnd = xOffset + 32;
	if (xOffsetEnd > mWidth) {
		xOffsetEnd = mWidth;
	}

	p << xOffset;
	p << xOffsetEnd;
	for (int x = xOffset; x < xOffsetEnd; x++) {
		for (int y = 0; y < mHeight; y++) {
			p << (sf::Uint8)mData[x + y * mWidth];
		}
	}
	return p;
}
char cLevel::GetBlock(int x, int y) {
	if (x >= 0 && x < mWidth &&
		y >= 0 && y < mHeight) {
		return mData[x + y * mWidth];
	}
	return 0;
}
char cLevel::GetBlockFree(int x, int y) {
	x /= 16;
	y /= 16;
	if (x >= 0 && x < mWidth &&
		y >= 0 && y < mHeight) {
		return mData[x + y * mWidth];
	}
	return 0;
}
void cLevel::SetBlock(int x, int y, char block) {
	if (x >= 0 && x < mWidth &&
		y >= 0 && y < mHeight) {
		mData[x + y * mWidth] = block;
	}
}
void cLevel::LoadFromPacket(sf::Packet p) {
	int xOffsetEnd = 32;
	int xOffset = 0;

	p >> xOffset;
	p >> xOffsetEnd;
	for (int x = xOffset; x < xOffsetEnd; x++) {
		for (int y = 0; y < mHeight; y++) {
			sf::Uint8 d;
			p >> d;
			mData[x + y * mWidth] = d;
		}
	}
}
void cLevel::Render(CDrawModule& draw) {
	mDrawer.SetLayer(10);
	draw.DrawToLayer(mDrawer);
}
int cLevel::Width() {
	return mWidth;
}
bool cLevel::LevelCollide(int x, int y, int w, int h) {
	int left = x;
	int top = y;
	int right = x + w;
	int bottom = y + h;

	if (IsBlockSolid(GetBlockFree(left, top)))
		return true;
	if (IsBlockSolid(GetBlockFree(right, top)))
		return true;
	if (IsBlockSolid(GetBlockFree(right, bottom)))
		return true;
	if (IsBlockSolid(GetBlockFree(left, bottom)))
		return true;

	return false;
}
