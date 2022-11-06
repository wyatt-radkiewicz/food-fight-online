#pragma once

#include <string>
#include <unordered_map>

#include <SFML/Window.hpp>

#include "Game.h"

//
// Overview: A event that holds data about the key pressed, held down, or released.
//
class CKeyEvent : public CGameEvent
{
public:
	CKeyEvent(EGameEvent event,
			  const std::string& name,
			  sf::Keyboard::Key key);
	~CKeyEvent() override;

	sf::Keyboard::Key GetKey() const;
	const std::string& GetName() const;

private:
	std::string m_name;
	sf::Keyboard::Key m_key;
};

//
// Overview: A class that binds a bind or button name
// to a key scancode.
//
class CKeyBind
{
public:
	CKeyBind();
	CKeyBind(const std::string& name, sf::Keyboard::Key key);
	~CKeyBind();

	const std::string& GetName() const;
	void SetKey(sf::Keyboard::Key key);
	sf::Keyboard::Key GetKey() const;

	bool IsPressed() const;
	bool IsDown() const;
	bool IsUp() const;
	bool IsReleased() const;

	friend class CInputModule;

private:
	void SetName(const std::string& name);

private:
	std::string m_name;
	sf::Keyboard::Key m_key;

	bool m_lastState;
	bool m_currentState;
};

//
// Overview: This game module class runs all the inputs for the
// game. This includes controllers, mouse, keyboard, mic, ect.
// To see if a bind is pressed, you can check directly, or
// handle a EGameEvent::KeyPressed, EGameEvent::KeyDown, 
// or EGameEvent::KeyReleased event.
//
class CInputModule : public CGameModule
{
public:
	CInputModule(CGame& game, CConfig& config);
	~CInputModule() override;

	void HandleEvent(const CGameEvent& event) override;
	void Tick(const DeltaTime_t& deltaTime) override;
	void EndTick(const DeltaTime_t& deltaTime) override;
	void Draw() override;

	void SetCollectInput(bool collect);

	void AddBind(const std::string& name, sf::Keyboard::Key key);
	void AddBind(const CKeyBind& bind);
	CKeyBind& GetBind(const std::string& name);

private:
	void AddBindFromConfig(CConfig& config, const std::string& bindName);

private:
	std::unordered_map<std::string, CKeyBind> m_binds;

	bool m_collectInput;
};
