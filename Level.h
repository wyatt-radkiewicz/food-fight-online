#pragma once

#include "Drawer.h"
#include "Game.h"
#include "GameModules.h"
#include "ResourceController.h"

bool IsBlockSolid(int type);

class cLevel;

class cLevelDrawer : public CSpriteDrawer {
public:
	cLevelDrawer(cLevel* level);
	~cLevelDrawer();

	void Draw(sf::RenderTarget& target) override;

	friend class cLevel;

private:
	cLevel* mLevel;
};

class cLevel {
public:
	cLevel();
	~cLevel();

	int Generate();
	void LoadContent(CResourceController<sf::Texture>& c);
	void LoadFromPacket(sf::Packet p);
	sf::Packet ContructPacket(int xOffset);
	char GetBlock(int x, int y);
	char GetBlockFree(int x, int y);
	void SetBlock(int x, int y, char block);
	void Render(CDrawModule& draw);
	int Width();
	bool LevelCollide(int x, int y, int w, int h);
	void GenerateBiomeColumn(int x, int biome, int& height);
	void GenerateStructure(int x, int y, int type);

	friend class cLevelDrawer;

private:
	char* mData;
	int mWidth;
	int mHeight;
	CCleanResource<sf::Texture> mTexture;
	cLevelDrawer mDrawer;

};
