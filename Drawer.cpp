#include "Drawer.h"

//
// CDrawer Class Functions
//
// Public Functions //
CDrawer::CDrawer()
	: m_layer(0u)
{

}
CDrawer::CDrawer(uint8_t layer)
	: m_layer(layer)
{

}
CDrawer::~CDrawer()
{

}
uint8_t CDrawer::GetLayer() const
{
	return m_layer;
}
void CDrawer::SetLayer(uint8_t layer)
{
	m_layer = layer;
}

cSkyDrawer::cSkyDrawer(sf::Color color)
	: CDrawer() {
	this->color = color;
	m_layer = 0;
}
cSkyDrawer::~cSkyDrawer() {

}
void cSkyDrawer::SetColor(sf::Color color) {
	this->color = color;
}

void cSkyDrawer::Draw(sf::RenderTarget& target) {
	target.clear(color);
}

//
// CSpriteDrawer Class Functions
//
// Public Functions //
CSpriteDrawer::CSpriteDrawer()
	: CDrawer()
{

}
CSpriteDrawer::CSpriteDrawer(uint8_t layer)
	: CDrawer(layer)
{

}
CSpriteDrawer::CSpriteDrawer(sf::Texture& texture)
	: m_sprite(texture)
{

}
CSpriteDrawer::CSpriteDrawer(sf::Texture& texture, uint8_t layer)
	: CDrawer(layer), m_sprite(texture)
{

}
CSpriteDrawer::~CSpriteDrawer()
{

}
sf::Sprite& CSpriteDrawer::GetSprite()
{
	return m_sprite;
}
const sf::Sprite& CSpriteDrawer::GetSprite() const
{
	return m_sprite;
}
void CSpriteDrawer::Draw(sf::RenderTarget& target)
{
	target.draw(m_sprite);
}
void CSpriteDrawer::SetPos(int32_t x, int32_t y)
{
	m_sprite.setPosition(sf::Vector2f((float)x, (float)y));
}
void CSpriteDrawer::SetPos(Vector2i vec)
{
	m_sprite.setPosition(sf::Vector2f((float)vec.x, (float)vec.y));
}
Vector2i CSpriteDrawer::GetPos() const
{
	return Vector2i((int32_t)m_sprite.getPosition().x,
					(int32_t)m_sprite.getPosition().y);
}
void CSpriteDrawer::SetTexture(sf::Texture& texture)
{
	m_sprite.setTexture(texture);
}

//
// CBoxDrawer Class Functions
//
// Public Functions //
CBoxDrawer::CBoxDrawer()
	: CDrawer(), m_rectangle()
{

}
CBoxDrawer::CBoxDrawer(uint8_t layer)
	: CDrawer(layer), m_rectangle()
{

}
CBoxDrawer::CBoxDrawer(Vector2i pos, Vector2i size, Color color)
	: CDrawer(), m_rectangle(sf::Vector2f((float)size.x, (float)size.y))
{
	m_rectangle.setPosition(sf::Vector2f((float)pos.x, (float)pos.y));
	m_rectangle.setFillColor(sf::Color(
		(sf::Uint8)(color.r * (float)std::numeric_limits<uint8_t>::max()),
		(sf::Uint8)(color.g * (float)std::numeric_limits<uint8_t>::max()),
		(sf::Uint8)(color.b * (float)std::numeric_limits<uint8_t>::max()),
		(sf::Uint8)(color.a * (float)std::numeric_limits<uint8_t>::max())
	));
}
CBoxDrawer::CBoxDrawer(Vector2i pos, Vector2i size, Color color, uint8_t layer)
	: CDrawer(layer), m_rectangle(sf::Vector2f((float)size.x, (float)size.y))
{
	m_rectangle.setPosition(sf::Vector2f((float)pos.x, (float)pos.y));
	m_rectangle.setFillColor(sf::Color(
		(sf::Uint8)(color.r * (float)std::numeric_limits<uint8_t>::max()),
		(sf::Uint8)(color.g * (float)std::numeric_limits<uint8_t>::max()),
		(sf::Uint8)(color.b * (float)std::numeric_limits<uint8_t>::max()),
		(sf::Uint8)(color.a * (float)std::numeric_limits<uint8_t>::max())
	));
}
CBoxDrawer::~CBoxDrawer()
{

}
void CBoxDrawer::Draw(sf::RenderTarget& target)
{
	target.draw(m_rectangle);
}
void CBoxDrawer::SetColor(Color color)
{
	m_rectangle.setFillColor(sf::Color(
		(sf::Uint8)(color.r * (float)std::numeric_limits<uint8_t>::max()),
		(sf::Uint8)(color.g * (float)std::numeric_limits<uint8_t>::max()),
		(sf::Uint8)(color.b * (float)std::numeric_limits<uint8_t>::max()),
		(sf::Uint8)(color.a * (float)std::numeric_limits<uint8_t>::max())
	));
}
Color CBoxDrawer::GetColor() const
{
	sf::Color oldColor = m_rectangle.getFillColor();

	Color color(
		((float)oldColor.r / (float)std::numeric_limits<uint8_t>::max()),
		((float)oldColor.g / (float)std::numeric_limits<uint8_t>::max()),
		((float)oldColor.b / (float)std::numeric_limits<uint8_t>::max()),
		((float)oldColor.a / (float)std::numeric_limits<uint8_t>::max())
	);

	return color;
}
void CBoxDrawer::SetPos(int32_t x, int32_t y)
{
	m_rectangle.setPosition(sf::Vector2f((float)x, (float)y));
}
void CBoxDrawer::SetPos(Vector2i vec)
{
	m_rectangle.setPosition(sf::Vector2f((float)vec.x, (float)vec.y));
}
Vector2i CBoxDrawer::GetPos() const
{
	return Vector2i(
		(uint32_t)m_rectangle.getPosition().x,
		(uint32_t)m_rectangle.getPosition().y
	);
}
void CBoxDrawer::SetSize(Vector2i size)
{
	m_rectangle.setPosition(sf::Vector2f((float)size.x, (float)size.y));
}
Vector2i CBoxDrawer::GetSize() const
{
	return Vector2i(
		(uint32_t)m_rectangle.getSize().x,
		(uint32_t)m_rectangle.getSize().y
	);
}

//
// CTextDrawer Class Functions
//
// Public Functions //
CTextDrawer::CTextDrawer()
	: CDrawer(), m_align(ETextAlign::Left), m_position()
{

}
CTextDrawer::CTextDrawer(uint8_t layer)
	: CDrawer(layer), m_align(ETextAlign::Left), m_position()
{

}
CTextDrawer::CTextDrawer(sf::Font& font)
	: CDrawer(), m_text("", font), m_align(ETextAlign::Left), m_position()
{

}
CTextDrawer::CTextDrawer(sf::Font& font, uint8_t layer)
	: CDrawer(layer), m_text("", font), m_align(ETextAlign::Left), m_position()
{

}
CTextDrawer::CTextDrawer(sf::Font& font, uint32_t size)
	: CDrawer(), m_text("", font, size), m_align(ETextAlign::Left),
	m_position()
{

}
CTextDrawer::CTextDrawer(sf::Font& font, uint32_t size, uint8_t layer)
	: CDrawer(layer), m_text("", font, size), m_align(ETextAlign::Left),
	m_position()
{

}
CTextDrawer::~CTextDrawer()
{

}
sf::Text& CTextDrawer::GetText()
{
	return m_text;
}
const sf::Text& CTextDrawer::GetText() const
{
	return m_text;
}
void CTextDrawer::SetColor(Color color)
{
	m_text.setFillColor(sf::Color(
		(sf::Uint8)(color.r * (float)std::numeric_limits<uint8_t>::max()),
		(sf::Uint8)(color.g * (float)std::numeric_limits<uint8_t>::max()),
		(sf::Uint8)(color.b * (float)std::numeric_limits<uint8_t>::max()),
		(sf::Uint8)(color.a * (float)std::numeric_limits<uint8_t>::max())
	));
}
Color CTextDrawer::GetColor() const
{
	sf::Color oldColor = m_text.getFillColor();

	Color color(
		((float)oldColor.r / (float)std::numeric_limits<uint8_t>::max()),
		((float)oldColor.g / (float)std::numeric_limits<uint8_t>::max()),
		((float)oldColor.b / (float)std::numeric_limits<uint8_t>::max()),
		((float)oldColor.a / (float)std::numeric_limits<uint8_t>::max())
	);

	return color;
}
ETextAlign CTextDrawer::GetAlign() const
{
	return m_align;
}
void CTextDrawer::SetAlign(ETextAlign align)
{
	m_align = align;
}
void CTextDrawer::Draw(sf::RenderTarget& target)
{
	switch (m_align) {
		case ETextAlign::Left:
			m_text.setPosition(sf::Vector2f(
				(float)m_position.x,
				(float)m_position.y
			));
			break;
		case ETextAlign::Middle:
			m_text.setPosition(sf::Vector2f(
				(float)m_position.x - m_text.getGlobalBounds().width / 2.0f,
				(float)m_position.y
			));
			break;
		case ETextAlign::Right:
			m_text.setPosition(sf::Vector2f(
				(float)m_position.x - m_text.getGlobalBounds().width,
				(float)m_position.y
			));
			break;
	}

	target.draw(m_text);
}
void CTextDrawer::SetPos(int32_t x, int32_t y)
{
	m_position.x = x;
	m_position.y = y;
}
void CTextDrawer::SetPos(Vector2i vec)
{
	m_position = vec;
}
Vector2i CTextDrawer::GetPos() const
{
	return m_position;
}
uint32_t CTextDrawer::GetSize() const
{
	return m_text.getCharacterSize();
}
void CTextDrawer::SetSize(uint32_t size)
{
	m_text.setCharacterSize(size);
}
void CTextDrawer::SetFont(sf::Font& font)
{
	m_text.setFont(font);
}
std::string CTextDrawer::GetString() const
{
	return m_text.getString();
}
void CTextDrawer::SetString(const std::string& string)
{
	m_text.setString(string);
}
