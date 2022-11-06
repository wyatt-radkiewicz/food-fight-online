#pragma once

#include <stack>

#include <SFML/Network.hpp>

#include "Game.h"

//
// Overview: A event that is used in multiplayer games.
//
class CNetworkEvent : public CGameEvent
{
public:
	CNetworkEvent();
	~CNetworkEvent() override;

	void AddTypeToPacket(EGameEvent event);
	sf::Packet& GetPacket();
	const sf::Packet& GetPacket() const;

	const sf::IpAddress& GetSenderIp() const;
	uint16_t GetSenderPort() const;

	friend class CNetworkModule;

private:
	sf::Packet m_packet;
	sf::IpAddress m_senderIp;
	uint16_t m_senderPort;
};

//
// Overview: Holds the ip and port to send the data to.
//
struct SNetworkSendData
{
	std::unique_ptr<CNetworkEvent> event;
	sf::IpAddress ip;
	uint16_t port;
};

//
// Overview: Sends the events recieved from the udp port to all
// parts of the game system.
//
class CNetworkModule : public CGameModule
{
public:
	CNetworkModule(CGame& game);
	~CNetworkModule() override;

	void HandleEvent(const CGameEvent& event) override;
	void Tick(const DeltaTime_t& deltaTime) override;
	void EndTick(const DeltaTime_t& deltaTime) override;
	void Draw() override;

	bool BindSocket(uint16_t port);
	const sf::UdpSocket& GetSocket() const;
	void SendEvent(std::unique_ptr<CNetworkEvent> event,
				   const sf::IpAddress& ip, uint16_t port);
	void SendEventImmidient(std::unique_ptr<CNetworkEvent> event,
							const sf::IpAddress& ip, uint16_t port);

	void SetTickRate(uint16_t tickRate);

private:
	std::chrono::high_resolution_clock::time_point m_lastTick;

	std::stack<SNetworkSendData> m_queue;
	sf::UdpSocket m_socket;
	uint16_t m_tickRate;

	bool m_binded;
};
