#pragma once

#include <array>
#include <limits>
#include <memory>

#include <SFML/Graphics.hpp>

#include "Game.h"
#include "Drawer.h"

//
// Overview: A event that holds the keycode.
//
class CTextEnteredEvent : public CGameEvent
{
public:
	CTextEnteredEvent(EGameEvent event,
					  uint32_t unicode);
	~CTextEnteredEvent() override;

	uint8_t GetKey() const;

private:
	uint8_t m_key;
};


//
// Overview: This class collects pointers to all the CDrawer classes that
// need to be drawn and draws them for that frame.
//
// Note: You cannot give Draw(CDrawer& drawer) a CDrawer
// that will go out of scope by the end of the frame.
//
class CDrawLayer
{
public:
	CDrawLayer();
	~CDrawLayer();

	void DrawLayer(sf::RenderTarget& screen);

	void Draw(CDrawer& drawer);

	bool IsEmpty();

private:
	std::vector<CDrawer*> m_drawers;
};

//
// Overview: A module that draws "things" onto the screen using
// a view coordinate, and layer number.
//
class CDrawModule : public CGameModule
{
public:
	CDrawModule(CGame& game, CConfig& config);
	~CDrawModule() override;

	void HandleEvent(const CGameEvent& event) override;
	void Tick(const DeltaTime_t& deltaTime) override;
	void EndTick(const DeltaTime_t& deltaTime) override;
	void Draw() override;

	CDrawLayer& GetLayer(uint8_t layer);
	void DrawToLayer(CDrawer& drawer);

	void SetCamPos(int x, int y);
	Vector2i GetPos();

	sf::RenderWindow& GetWindow();

private:
	std::array<CDrawLayer, std::numeric_limits<uint8_t>::max()> m_layers;

	sf::RenderWindow m_window;
};
