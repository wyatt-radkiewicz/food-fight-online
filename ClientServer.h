#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Level.h"
#include "Game.h"
#include "GameModules.h"

const unsigned short gServerPort = 3331;
const int gAnimalsMax = 2024;
extern bool gBURN;
extern bool gNoBurn;
extern bool gCoolCows;
extern bool gTBuild;

struct sClientPlayerSkin {
	float speed = 0;
	float jump = 0;
	int damage = 0;
	std::string texture = "NULLPATH";
	int special = 0;
};

struct sClientPlayer {
	CSpriteDrawer spr;
	CTextDrawer foodtext;
	CTextDrawer nametext;
	std::string name;
	CCleanResource<sf::Texture> tex;
	float x;
	float y;
	float food;
	int blocks = 0;
	bool dead = false;


	float xsp;
	float ysp;
	float grv;
	float jmp;
	bool grd;
	bool jumping;
};

struct sServerPlayer {
	sf::IpAddress ip;
	std::string name;
	unsigned short port;
	int x;
	int y;
	int blocks = 0;
	float food;
	int timeAlive;
	int foodEarned;
	bool dead = false;
	int worldProgress = 0;
	sf::Image img;
	std::string imgPath = "";
	bool hasImg = false;
};

class cServerState;
class cClientAnimal;

std::unique_ptr<cClientAnimal> MakeNewClientAnimal(int type, CResourceController<sf::Texture>& textures,
	sf::Font& fonts);

class cClientAnimal
{
public:
	cClientAnimal(CResourceController<sf::Texture>& textures,
		sf::Font& fonts, int type);
	virtual ~cClientAnimal();

	virtual void OnUpdate();
	virtual void OnDraw(CDrawModule& drawer);

public:
	CTextDrawer mHpText;
	CCleanResource<sf::Texture> mTexture;
	CSpriteDrawer mSprite;

	Vector2f pos;

	bool mFriendly = true;
	int mType = -1;
	float mXSpeed = 0;
	float mYSpeed = 0;
	float mDamage = 0;
	float mFood = 10;
	int mHp = 10;
};

class cServerAnimal
{
public:
	cServerAnimal();
	virtual ~cServerAnimal();

	virtual void OnUpdate(int id, cLevel& level, std::stack<int>& deletes,
		std::map<int, std::unique_ptr<cServerAnimal>>& animals, std::map<int, sServerPlayer>& players, bool daytime, cServerState& state);

public:
	Vector2f pos;

	bool mFriendly = true;
	int mType = -1;
	float mDamage = 0;
	float mFood = 10;
	int mHp = 10;
	int mBlocks = 0;

	float mXSpeed = 0;
	float mYSpeed = 0;
};

class cClientPig : public cClientAnimal
{
public:
	cClientPig(CResourceController<sf::Texture>& textures,
		sf::Font& fonts, int type);
	virtual ~cClientPig();

	virtual void OnDraw(CDrawModule& drawer);
};

class cServerPig : public cServerAnimal
{
public:
	cServerPig();
	virtual ~cServerPig();

	virtual void OnUpdate(int id, cLevel& level, std::stack<int>& deletes, std::map<int, std::unique_ptr<cServerAnimal>>& animals,
		std::map<int, sServerPlayer>& players, bool daytime, cServerState& state);

	int mTargetHeight;
	float mFacing;
};

class cClientCow : public cClientAnimal
{
public:
	cClientCow(CResourceController<sf::Texture>& textures,
		sf::Font& fonts, int type);
	virtual ~cClientCow();

	virtual void OnDraw(CDrawModule& drawer);
};

class cServerCow : public cServerAnimal
{
public:
	cServerCow();
	virtual ~cServerCow();

	virtual void OnUpdate(int id, cLevel& level, std::stack<int>& deletes, std::map<int, std::unique_ptr<cServerAnimal>>& animals,
		std::map<int, sServerPlayer>& players, bool daytime, cServerState& state);

	float mFacing;
	float mYsp;
	float mXsp;
};

class cClientChicken : public cClientAnimal
{
public:
	cClientChicken(CResourceController<sf::Texture>& textures,
		sf::Font& fonts, int type);
	virtual ~cClientChicken();

	virtual void OnDraw(CDrawModule& drawer);
};

class cServerChicken : public cServerAnimal
{
public:
	cServerChicken();
	virtual ~cServerChicken();

	virtual void OnUpdate(int id, cLevel& level, std::stack<int>& deletes, std::map<int, std::unique_ptr<cServerAnimal>>& animals,
		std::map<int, sServerPlayer>& players, bool daytime, cServerState& state);

	float mFacing;
	int mState;
	float mSpeed;
};

class cClientZombie : public cClientAnimal
{
public:
	cClientZombie(CResourceController<sf::Texture>& textures,
		sf::Font& fonts, int type);
	virtual ~cClientZombie();

	virtual void OnDraw(CDrawModule& drawer);
};

class cServerZombie : public cServerAnimal
{
public:
	cServerZombie();
	virtual ~cServerZombie();

	virtual void OnUpdate(int id, cLevel& level, std::stack<int>& deletes, std::map<int, std::unique_ptr<cServerAnimal>>& animals,
		std::map<int, sServerPlayer>& players, bool daytime, cServerState& state);

	float mFacing;
	float mSpeed;
	float mXsp;
	float mYsp;
	int mFindtick;
};

class cClientCropduster : public cClientAnimal
{
public:
	cClientCropduster(CResourceController<sf::Texture>& textures,
		sf::Font& fonts, int type);
	virtual ~cClientCropduster();

	virtual void OnDraw(CDrawModule& drawer);
};

class cServerCropduster : public cServerAnimal
{
public:
	cServerCropduster();
	virtual ~cServerCropduster();

	virtual void OnUpdate(int id, cLevel& level, std::stack<int>& deletes, std::map<int, std::unique_ptr<cServerAnimal>>& animals,
		std::map<int, sServerPlayer>& players, bool daytime, cServerState& state);

	int mSpawnTimer;
	float mFacing;
};

class cClientAcid : public cClientAnimal
{
public:
	cClientAcid(CResourceController<sf::Texture>& textures,
		sf::Font& fonts, int type);
	virtual ~cClientAcid();

	virtual void OnDraw(CDrawModule& drawer);
};

class cServerAcid : public cServerAnimal
{
public:
	cServerAcid();
	virtual ~cServerAcid();

	virtual void OnUpdate(int id, cLevel& level, std::stack<int>& deletes, std::map<int, std::unique_ptr<cServerAnimal>>& animals,
		std::map<int, sServerPlayer>& players, bool daytime, cServerState& state);

	int mXsp;
	int mYsp;
};
