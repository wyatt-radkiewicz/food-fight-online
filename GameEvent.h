#pragma once

#include <memory>
#include <mutex>
#include <stack>

//
// Overview: An enum defining all the different game events.
//
enum class EGameEvent
{
	Defualt = -1,
	// Input Module
	Key_Pressed,
	Key_Down,
	Key_Released,
	// Game
	Game_PopState,
	Game_End,
	TextEntered,
	// Draw Module
	Window_LostFocus,
	Window_GainedFocus,
	// Network Module
	Net_SentPacket,
	Net_RecvPacket,
	// Custom events for server networking
	NetServer_PlayerPos,
	NetServer_PlayerConnected,
	NetServer_PlayerDisconnected,
	NetServer_PlayerSaid,
	NetServer_PlayerFoodUpdate,
	NetServer_AnimalHurt,
	NetServer_PlayerDied,
	NetServer_BuildBlock,
	NetServer_BreakBlock,
	NetServer_Joinable,
	NetServer_PlayerSkinPic,
	// Custom events for client networking
	NetClient_PlayerPos,
	NetClient_PlayerConnected,
	NetClient_PlayerDisconnected,
	NetClient_ConnectionSuccess,
	NetClient_PlayerSaid,
	NetClient_WorldUpdate,
	NetClient_PlayerFoodUpdate,
	NetClient_UpdateAnimals,
	NetClient_GiveFood,
	NetClient_Daytime,
	NetClient_Nighttime,
	NetClient_PlayerDied,
	NetClient_AddAnimalsFrame,
	NetClient_ResultsScreen,
	NetClient_SetFood,
	NetClient_UpdateTimer,
	NetClient_GiveBlocks,
	NetClient_CanJoin,
	NetClient_PlayerSkinPic
};

//
// Overview: A class that holds data for a given game event.
// Note: You can inherit from this class to provide additional parameters.
//
// Constructor:
//  CGameEvent();
//  CGameEvent(EGameEvent type);
//
// Virtual Functions: 
//  ~CGameEvent();
//
class CGameEvent
{
public:
	CGameEvent();
	CGameEvent(EGameEvent type);
	virtual ~CGameEvent();

	EGameEvent GetEvent() const;

protected:
	EGameEvent m_type;
};

//
// A class that holds events in a queue to be processed one at a time.
//
class CGameEventQueue
{
public:
	CGameEventQueue();
	~CGameEventQueue();

	void PushEvent(std::unique_ptr<CGameEvent> event);
	const CGameEvent& TopEvent() const;
	CGameEvent& TopEvent();
	void PopEvent();
	bool EventsEmpty();

protected:
	std::stack<std::unique_ptr<CGameEvent>> m_events;

	// Multithreading
	std::mutex m_eventMutex;
};
