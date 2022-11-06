#include "AudioModule.h"

#include <SFML/Audio.hpp>

CAudioModule::CAudioModule(CGame& game)
	: CGameModule(game)
{

}
CAudioModule::~CAudioModule()
{

}
void CAudioModule::HandleEvent(const CGameEvent& event)
{

}
void CAudioModule::Tick(const DeltaTime_t& deltaTime)
{

}
void CAudioModule::EndTick(const DeltaTime_t& deltaTime)
{

}
void CAudioModule::Draw()
{

}
void CAudioModule::SetAudioVolume(float volume)
{
	sf::Listener::setGlobalVolume(volume);
}
float CAudioModule::GetAudioVolume() const
{
	return sf::Listener::getGlobalVolume();
}
void CAudioModule::SetListeningPos(float x, float y)
{
	sf::Listener::setPosition(sf::Vector3f(x, y, 0.0f));
}
void CAudioModule::SetListeningPos(Vector2f pos)
{
	sf::Listener::setPosition(sf::Vector3f(pos.x, pos.y, 0.0f));
}
Vector2f CAudioModule::GetListeningPos()
{
	return Vector2f(sf::Listener::getPosition().x,
					sf::Listener::getPosition().y);
}
