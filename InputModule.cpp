#include "InputModule.h"

//
// CKeyEvent Class Functions
//
// Public Functons //
CKeyEvent::CKeyEvent(EGameEvent event,
					 const std::string& name,
					 sf::Keyboard::Key key)
	: CGameEvent(event), m_key(key), m_name(name)
{

}
CKeyEvent::~CKeyEvent()
{

}
sf::Keyboard::Key CKeyEvent::GetKey() const
{
	return m_key;
}
const std::string& CKeyEvent::GetName() const
{
	return m_name;
}

//
// CKeyBind Class Functions
//
// Public Functions //
CKeyBind::CKeyBind()
	: m_name(""), m_key(sf::Keyboard::Unknown),
	m_currentState(false), m_lastState(false)
{

}
CKeyBind::CKeyBind(const std::string& name, sf::Keyboard::Key key)
	: m_name(name), m_key(key), m_currentState(false), m_lastState(false)
{

}
CKeyBind::~CKeyBind()
{

}
const std::string& CKeyBind::GetName() const
{
	return m_name;
}
void CKeyBind::SetKey(sf::Keyboard::Key key)
{
	m_key = key;
}
sf::Keyboard::Key CKeyBind::GetKey() const
{
	return m_key;
}
bool CKeyBind::IsPressed() const
{
	return m_currentState && !m_lastState;
}
bool CKeyBind::IsDown() const
{
	return m_currentState;
}
bool CKeyBind::IsUp() const
{
	return !m_currentState;
}
bool CKeyBind::IsReleased() const
{
	return !m_currentState && m_lastState;
}

// Private Functions //
void CKeyBind::SetName(const std::string& name)
{
	m_name = name;
}

//
// CInputModule Class Functions
//
// Public Functions //
CInputModule::CInputModule(CGame& game, CConfig& config)
	: CGameModule(game), m_collectInput(true)
{
	// Get the key binds
	AddBindFromConfig(config, "Forward");
	AddBindFromConfig(config, "Backward");
	AddBindFromConfig(config, "Left");
	AddBindFromConfig(config, "Right");
	AddBindFromConfig(config, "Jump");
	AddBindFromConfig(config, "Build");
	AddBindFromConfig(config, "Break");
	AddBindFromConfig(config, "Select");
	AddBindFromConfig(config, "Special");
	AddBindFromConfig(config, "Talk");
}
CInputModule::~CInputModule()
{

}
void CInputModule::HandleEvent(const CGameEvent& event)
{
	switch (event.GetEvent()) {
		case EGameEvent::Window_LostFocus:
			SetCollectInput(false);
			break;
		case EGameEvent::Window_GainedFocus:
			SetCollectInput(true);
			break;
		default:
			break;
	}
}
void CInputModule::Tick(const DeltaTime_t& deltaTime)
{
	
}
void CInputModule::EndTick(const DeltaTime_t& deltaTime)
{
	for (auto& bind : m_binds) {
		if (m_collectInput) {
			bind.second.m_lastState = bind.second.m_currentState;
			bind.second.m_currentState =
				sf::Keyboard::isKeyPressed(bind.second.GetKey());

			// Events
			if (bind.second.IsPressed()) {
				m_game.PushEvent(std::make_unique<CKeyEvent>(
					EGameEvent::Key_Pressed,
					bind.second.GetName(),
					bind.second.GetKey()
					));
			}
			if (bind.second.IsDown()) {
				m_game.PushEvent(std::make_unique<CKeyEvent>(
					EGameEvent::Key_Down,
					bind.second.GetName(),
					bind.second.GetKey()
					));
			}
			if (bind.second.IsReleased()) {
				m_game.PushEvent(std::make_unique<CKeyEvent>(
					EGameEvent::Key_Released,
					bind.second.GetName(),
					bind.second.GetKey()
					));
			}
		}
		else {
			bind.second.m_lastState = false;
			bind.second.m_currentState = false;
		}
	}
}
void CInputModule::Draw()
{

}
void CInputModule::SetCollectInput(bool collect)
{
	m_collectInput = collect;
}
void CInputModule::AddBind(const std::string& name, sf::Keyboard::Key key)
{
	if (m_binds.find(name) == m_binds.end()) {
		m_binds[name].SetKey(key);
		m_binds[name].SetName(name);
	}
}
void CInputModule::AddBind(const CKeyBind& bind)
{
	if (m_binds.find(bind.GetName()) == m_binds.end()) {
		m_binds[bind.GetName()].SetKey(bind.GetKey());
		m_binds[bind.GetName()].SetName(bind.GetName());
	}
}
CKeyBind& CInputModule::GetBind(const std::string& name)
{
	return m_binds.at(name);
}

// Private Functions //
void CInputModule::AddBindFromConfig(CConfig& config, const std::string& bindName)
{
	if (config.FindConfig("keys_bind" + bindName))
		AddBind(bindName, (sf::Keyboard::Key)config.GetConfig("keys_bind" + bindName));
}
