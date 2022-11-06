#include <iostream>

#include "DrawModule.h"

#include <SFML/System/Utf.hpp>

//
// CTextEntered Class Functions
//
// Public Functions //
CTextEnteredEvent::CTextEnteredEvent(EGameEvent event,
									 uint32_t unicode)
	: CGameEvent(event)
{
	sf::String str;
	str += unicode;
	m_key = str.toAnsiString()[0];
}
CTextEnteredEvent::~CTextEnteredEvent()
{

}
uint8_t CTextEnteredEvent::GetKey() const
{
	return m_key;
}

//
// CDrawLayer
//
// Public Functions //
CDrawLayer::CDrawLayer()
{

}
CDrawLayer::~CDrawLayer()
{

}
void CDrawLayer::DrawLayer(sf::RenderTarget& screen)
{
	for (auto& drawer : m_drawers) {
		drawer->Draw(screen);
	}

	// Flush for next frame
	m_drawers.clear();
}
void CDrawLayer::Draw(CDrawer& drawer)
{
	m_drawers.push_back(&drawer);
}
bool CDrawLayer::IsEmpty()
{
	return m_drawers.empty();
}

//
// CDrawModule Class Functions
//
// Public Functions //
CDrawModule::CDrawModule(CGame& game, CConfig& config)
	: CGameModule(game)
{
	sf::ContextSettings settings;
	settings.depthBits = 0;
	settings.stencilBits = 0;
	settings.antialiasingLevel = 0;
	settings.majorVersion = 2;
	settings.minorVersion = 1;
	settings.attributeFlags = sf::ContextSettings::Default;
	settings.sRgbCapable = 0;

	m_window.create(sf::VideoMode(640,
									  480),
						"Food Fight Online", 7U, settings);
}
CDrawModule::~CDrawModule()
{

}
void CDrawModule::HandleEvent(const CGameEvent& event)
{
	
}
void CDrawModule::Tick(const DeltaTime_t& deltaTime)
{
	// Sfml2 window events
	sf::Event sfEvent;
	while (m_window.pollEvent(sfEvent)) {
		if (sfEvent.type == sf::Event::Closed)
			m_game.PushEvent(std::make_unique<CGameEvent>(EGameEvent::Game_End));
		if (sfEvent.type == sf::Event::LostFocus)
			m_game.PushEvent(std::make_unique<CGameEvent>(EGameEvent::Window_LostFocus));
		if (sfEvent.type == sf::Event::GainedFocus)
			m_game.PushEvent(std::make_unique<CGameEvent>(EGameEvent::Window_GainedFocus));
		if (sfEvent.type == sf::Event::TextEntered)
			m_game.PushEvent(std::make_unique<CTextEnteredEvent>(
				EGameEvent::TextEntered, sfEvent.text.unicode
				));
	}
}
void CDrawModule::EndTick(const DeltaTime_t& deltaTime)
{
	m_window.clear();
}
void CDrawModule::Draw()
{
	for (auto& layer : m_layers) {
		if (!layer.IsEmpty())
			layer.DrawLayer(m_window);
	}

	m_window.display();
}
void CDrawModule::SetCamPos(int x, int y) {
	sf::View view = m_window.getDefaultView();
	view.setCenter(x, y);
	m_window.setView(view);
}
Vector2i CDrawModule::GetPos() {
	sf::View view = m_window.getView();
	return Vector2i(view.getCenter().x, view.getCenter().y);
}
CDrawLayer& CDrawModule::GetLayer(uint8_t layer)
{
	return m_layers.at(layer);
}
void CDrawModule::DrawToLayer(CDrawer& drawer)
{
	drawer.Draw(m_window);
	//m_layers.at(drawer.GetLayer()).Draw(drawer);
}
sf::RenderWindow& CDrawModule::GetWindow() {
	return m_window;
}
