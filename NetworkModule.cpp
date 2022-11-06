#include "NetworkModule.h"

#include <iostream>

//
// CNetworkEvent Class Functions
//
// Public Functions //
CNetworkEvent::CNetworkEvent()
	: CGameEvent()
{

}
CNetworkEvent::~CNetworkEvent()
{

}
void CNetworkEvent::AddTypeToPacket(EGameEvent event)
{
	m_packet << int32_t(event);
	m_type = event;
}
sf::Packet& CNetworkEvent::GetPacket()
{
	return m_packet;
}
const sf::Packet& CNetworkEvent::GetPacket() const
{
	return m_packet;
}
const sf::IpAddress& CNetworkEvent::GetSenderIp() const
{
	return m_senderIp;
}
uint16_t CNetworkEvent::GetSenderPort() const
{
	return m_senderPort;
}

//
// CNetworkModule
//
// Public Functions //
CNetworkModule::CNetworkModule(CGame& game)
	: CGameModule(game), m_socket(), m_tickRate(64), m_binded(false)
{
	m_socket.setBlocking(false);
	m_lastTick = std::chrono::high_resolution_clock::now();
}
CNetworkModule::~CNetworkModule()
{

}
void CNetworkModule::HandleEvent(const CGameEvent& event)
{

}
void CNetworkModule::Tick(const DeltaTime_t& deltaTime)
{
	// Abort if we are not connected to anything
	if (!m_binded)
		return;

	// See if we can receive (tick rate)
	std::chrono::duration<float> et =
		std::chrono::high_resolution_clock::now() - m_lastTick;
	float etTarget = 1.0f / (float)m_tickRate;

	if (et.count() > etTarget || m_tickRate == 0) {
		m_lastTick = std::chrono::high_resolution_clock::now();

		// You may now send data
		while (!m_queue.empty()) {
			if (m_socket.send(m_queue.top().event->GetPacket(),
							  m_queue.top().ip,
							  m_queue.top().port) != sf::Socket::Done) {
				std::cout <<
					"CNetworkModule::Tick::Error\tCould not send event to port: "
					<< m_queue.top().port << " and ip: " << m_queue.top().ip.toString() << "!" << std::endl;
			}
			m_queue.pop();
		}

		// You may now receive data
		sf::IpAddress ip;
		uint16_t port;
		sf::Packet packet;
		while (m_socket.receive(packet, ip, port) == sf::Socket::Done) {
			sf::Int32 eventType;
			packet >> eventType;
			auto event = std::make_unique<CNetworkEvent>();
			event->AddTypeToPacket((EGameEvent)eventType);
			event->GetPacket() = packet;
			event->m_senderIp = ip;
			event->m_senderPort = port;
			m_game.PushEvent(std::move(event));
		}
	}
}
void CNetworkModule::EndTick(const DeltaTime_t& deltaTime)
{

}
void CNetworkModule::Draw()
{

}
bool CNetworkModule::BindSocket(uint16_t port)
{
	m_binded = true;

	if (m_socket.bind(port) != sf::Socket::Done) {
		std::cout <<
			"CNetworkModule::BindSocket::Error\tCould not bind socket to port: "
			<< port << "!" << std::endl;

		m_binded = false;
	}

	return m_binded;
}
const sf::UdpSocket& CNetworkModule::GetSocket() const
{
	return m_socket;
}
void CNetworkModule::SendEvent(std::unique_ptr<CNetworkEvent> event,
							   const sf::IpAddress& ip, uint16_t port)
{
	// Abort if we are not connected to anything
	if (!m_binded)
		return;

	m_queue.push(SNetworkSendData());
	m_queue.top().event = std::move(event);
	m_queue.top().ip = ip;
	m_queue.top().port = port;
}
void CNetworkModule::SendEventImmidient(std::unique_ptr<CNetworkEvent> event,
										const sf::IpAddress& ip, uint16_t port)
{
	// Abort if we are not connected to anything
	if (!m_binded)
		return;

	if (m_socket.send(event->GetPacket(), ip, port) != sf::Socket::Done) {
		std::cout <<
			"CNetworkModule::Tick::Error\tCould not send event to port: "
			<< port << " and ip: " << ip.toString() << "!" << std::endl;
	}
}
void CNetworkModule::SetTickRate(uint16_t tickRate)
{
	m_tickRate = tickRate;
}
