#pragma once

#include <stdint.h>

#include <memory>
#include <mutex>
#include <stack>
#include <string>
#include <chrono>
#include <map>
#include <unordered_map>

#include "GameEvent.h"
#include "Config.h"

typedef std::chrono::duration<float> DeltaTime_t;

class CGame;

//
// Overview: A abstact class that runs certain static, main
// aspects of the game such rendering, audio, file management, ect.
//
// Constructor: CGameModule(CGame& game);
//
// Virtual Functions:
//  ~CGameModule();
//  void HandleEvent(const CGameEvent& event);
//  void Tick(const DeltaTime_t& deltaTime);
//  void EndTick(const DeltaTime_t& deltaTime);
//  void Draw();
//
class CGameModule
{
public:
	CGameModule(CGame& game);
	virtual ~CGameModule();

	virtual void HandleEvent(const CGameEvent& event) = 0;
	virtual void Tick(const DeltaTime_t& deltaTime) = 0;
	virtual void EndTick(const DeltaTime_t& deltaTime) = 0;
	virtual void Draw() = 0;

protected:
	CGame& m_game;
};

//
// Overview: Controls all the modules within the game, and
// allows any you to access them (provided they are there).
//
class CGameModuleController
{
public:
	CGameModuleController();
	~CGameModuleController();

	void AddModule(std::unique_ptr<CGameModule> module,
				   const std::string& moduleName);
	CGameModule& GetModule(const std::string& moduleName);

	void HandleEvent(const CGameEvent& event);
	void Tick(const DeltaTime_t& deltaTime);
	void EndTick(const DeltaTime_t& deltaTime);
	void Draw();

protected:
	std::unordered_map<std::string, std::unique_ptr<CGameModule>> m_modules;
};

//
// Overview: A abstract class that runs certain
// code for that state of the game.
// Eg. Main menu, main game, ect.
//
// Constructor: CGameState(CGame& game);
// 
// Virtual Functions: 
//  ~CGameState();
//  void OnEnter();
//  void Tick(const DeltaTime_t& deltaTime);
//  void Draw();
//  void OnExit();
//
class CGameState
{
public:
	CGameState(CGame& game);
	virtual ~CGameState();

	virtual void OnEnter() = 0;
	virtual void HandleEvent(const CGameEvent& event) = 0;
	virtual void Tick(const DeltaTime_t& deltaTime) = 0;
	virtual void Draw() = 0;
	virtual void OnExit() = 0;

protected:
	CGame& m_game;
};

//
// Overview: Contains the game states and runs them accordingly.
//
class CGame
{
public:
	CGame();
	~CGame();

	void PushState(std::unique_ptr<CGameState> newState);
	void PopState();
	bool StatesEmpty() const;
	const CGameState& TopState() const;
	CGameState& TopState();

	void StartGame();
	void EndGame();
	const DeltaTime_t& GetDeltaTime() const;
	const DeltaTime_t& GetUnhingedDeltaTime() const;
	bool IsRunning() const;
	uint16_t GetFrameRate() const;
	void SetFrameRate(uint16_t frameRate);
	uint64_t GetTick() const;
	void SetTick(uint64_t tick);

	const CConfig& GetConfig() const;

	void HandleEvent(const CGameEvent& event);
	void PushEvent(std::unique_ptr<CGameEvent> event);
	CGameEvent& TopEvent();
	void PopEvent();

	CGameModule& GetModule(const std::string& module);
	CGameModule* GetModulePtr(const std::string& module);

private:
	void AddModules();

	void Events();
	void Tick();
	void Draw();

	bool CheckAbortTick();

private:
	uint64_t m_ticks;
	uint16_t m_frameRate;

	std::unique_ptr<std::stack<std::unique_ptr<CGameState>>> m_stack;

	CGameEventQueue m_eventQueue;
	std::unique_ptr<CGameModuleController> m_modules;
	CConfig m_config;
	DeltaTime_t m_deltaTime;
	DeltaTime_t m_unhingedDeltaTime;

	bool m_running;

	// Multithreading
	std::mutex m_stackMutex;
};
