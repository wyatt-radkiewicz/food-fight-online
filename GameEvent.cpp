#include "GameEvent.h"

//
// CGameEvent Class Functions
//
// Public Functions //
CGameEvent::CGameEvent()
	: m_type(EGameEvent::Defualt)
{

}
CGameEvent::CGameEvent(EGameEvent type)
	: m_type(type)
{

}
CGameEvent::~CGameEvent()
{

}
EGameEvent CGameEvent::GetEvent() const
{
	return m_type;
}

//
// CGameEventQueue Class Functions
//
// Public Functions //
CGameEventQueue::CGameEventQueue()
{

}
CGameEventQueue::~CGameEventQueue()
{

}
void CGameEventQueue::PushEvent(std::unique_ptr<CGameEvent> event)
{
	m_eventMutex.lock();

	// Push an event
	m_events.push(std::move(event));

	m_eventMutex.unlock();
}
const CGameEvent& CGameEventQueue::TopEvent() const
{
	return *m_events.top().get();
}
CGameEvent& CGameEventQueue::TopEvent()
{
	return *m_events.top().get();
}
void CGameEventQueue::PopEvent()
{
	m_eventMutex.lock();

	// Push an event
	if (!m_events.empty())
		m_events.pop();

	m_eventMutex.unlock();
}
bool CGameEventQueue::EventsEmpty()
{
	return m_events.empty();
}
