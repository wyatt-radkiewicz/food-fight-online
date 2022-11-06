#pragma once

#include <SFML/Graphics.hpp>

#include "Utilities.h"

//
// Overview: An abstract class that is be used to
// draw things on layers for the game.
// 
// Contructor:
//  CDrawer();
//  CDrawer(uint16_t layer);
// 
// Virtual Functions:
//  void Draw(sf::RenderTarget& target);
//
class CDrawer
{
public:
	CDrawer();
	CDrawer(uint8_t layer);
	~CDrawer();

	virtual void Draw(sf::RenderTarget& target) = 0;

	uint8_t GetLayer() const;
	void SetLayer(uint8_t layer);

protected:
	uint8_t m_layer;
};

class cSkyDrawer : public CDrawer
{
public:
	cSkyDrawer(sf::Color color);
	~cSkyDrawer();

	void SetColor(sf::Color color);
	virtual void Draw(sf::RenderTarget& target);

protected:
	sf::Color color;

};

//
// Overview: A CDrawer that draws a sprite to the screen.
//
class CSpriteDrawer : public CDrawer
{
public:
	CSpriteDrawer();
	CSpriteDrawer(uint8_t layer);
	CSpriteDrawer(sf::Texture& texture);
	CSpriteDrawer(sf::Texture& texture, uint8_t layer);
	~CSpriteDrawer();

	sf::Sprite& GetSprite();
	const sf::Sprite& GetSprite() const;

	void Draw(sf::RenderTarget& target) override;

	void SetPos(int32_t x, int32_t y);
	void SetPos(Vector2i vec);
	Vector2i GetPos() const;
	void SetTexture(sf::Texture& texture);

private:
	sf::Sprite m_sprite;
};

//
// Overview: A CDrawer that draws a box to the screen.
//
class CBoxDrawer : public CDrawer
{
public:
	CBoxDrawer();
	CBoxDrawer(uint8_t layer);
	CBoxDrawer(Vector2i pos, Vector2i size, Color color);
	CBoxDrawer(Vector2i pos, Vector2i size, Color color, uint8_t layer);
	~CBoxDrawer();

	void Draw(sf::RenderTarget& target) override;

	void SetColor(Color color);
	Color GetColor() const;

	void SetPos(int32_t x, int32_t y);
	void SetPos(Vector2i vec);
	Vector2i GetPos() const;
	void SetSize(Vector2i size);
	Vector2i GetSize() const;

private:
	sf::RectangleShape m_rectangle;
};

//
// Overview: A enum that holds text alignment.
//
enum class ETextAlign
{
	Left,
	Middle,
	Right
};

//
// Overview: A CDrawer that draws text to the screen.
//
class CTextDrawer : public CDrawer
{
public:
	CTextDrawer();
	CTextDrawer(uint8_t layer);
	CTextDrawer(sf::Font& font);
	CTextDrawer(sf::Font& font, uint8_t layer);
	CTextDrawer(sf::Font& font, uint32_t size);
	CTextDrawer(sf::Font& font, uint32_t size, uint8_t layer);
	~CTextDrawer();

	sf::Text& GetText();
	const sf::Text& GetText() const;
	void SetColor(Color color);
	Color GetColor() const;
	ETextAlign GetAlign() const;
	void SetAlign(ETextAlign align);

	void Draw(sf::RenderTarget& target) override;

	void SetPos(int32_t x, int32_t y);
	void SetPos(Vector2i vec);
	Vector2i GetPos() const;
	uint32_t GetSize() const;
	void SetSize(uint32_t size);
	void SetFont(sf::Font& font);
	std::string GetString() const;
	void SetString(const std::string& string);

private:
	Vector2i m_position;
	sf::Text m_text;
	ETextAlign m_align;
};
