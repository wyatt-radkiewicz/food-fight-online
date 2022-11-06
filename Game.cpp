#include <SFML/Graphics.hpp>

#include "Game.h"

#include "GameModules.h"

//
// CGameModule Class Functions
//
// Public Functions //
CGameModule::CGameModule(CGame& game)
	: m_game(game)
{

}
CGameModule::~CGameModule()
{

}

//
// CGameModuleController Class Functions
//
// Public Functions //
CGameModuleController::CGameModuleController()
{

}
CGameModuleController::~CGameModuleController()
{

}
void CGameModuleController::AddModule(std::unique_ptr<CGameModule> module,
									  const std::string& moduleName)
{
	// Add the module if it isnt there already
	if (m_modules.find(moduleName) == m_modules.end())
		m_modules[moduleName] = std::move(module);
}
CGameModule& CGameModuleController::GetModule(const std::string& moduleName)
{
	return *m_modules[moduleName].get();
}
void CGameModuleController::HandleEvent(const CGameEvent& event)
{
	for (auto& module : m_modules) {
		module.second.get()->HandleEvent(event);
	}
}
void CGameModuleController::Tick(const DeltaTime_t& deltaTime)
{
	for (auto& module : m_modules) {
		module.second.get()->Tick(deltaTime);
	}
}
void CGameModuleController::EndTick(const DeltaTime_t& deltaTime)
{
	for (auto& module : m_modules) {
		module.second.get()->EndTick(deltaTime);
	}
}
void CGameModuleController::Draw()
{
	for (auto& module : m_modules) {
		module.second.get()->Draw();
	}
}

//
// CGameState Class Functions
//
// Public Functions //
CGameState::CGameState(CGame& game)
	: m_game(game)
{

}
CGameState::~CGameState()
{

}

//
// CGame Class Functions
//
// Public Functions //
CGame::CGame()
	: m_running(false), m_ticks(0), m_frameRate(0),
	m_modules(std::make_unique<CGameModuleController>()),
	m_stack(std::make_unique<std::stack<std::unique_ptr<CGameState>>>())
{
	m_config.LoadFromFile("Assets/MainConfig.cfg");

	// Add the modules //
	AddModules();
}
void CGame::SetTick(uint64_t tick) {
	m_ticks = 0;
}
CGame::~CGame()
{
	m_stack.reset(nullptr);
	m_modules.reset(nullptr);
}
void CGame::PushState(std::unique_ptr<CGameState> newState)
{
	m_stackMutex.lock();

	if (!m_stack->empty())
		TopState().OnExit();

	// Add a state
	m_stack->push(std::move(newState));

	m_stackMutex.unlock();
}
void CGame::PopState()
{
	m_stackMutex.lock();

	// Remove a state if we can
	if (!m_stack->empty())
		m_stack->pop();

	if (!m_stack->empty())
		TopState().OnEnter();

	m_stackMutex.unlock();
}
bool CGame::StatesEmpty() const
{
	return m_stack->empty();
}
const CGameState& CGame::TopState() const
{
	return *m_stack->top().get();
}
CGameState& CGame::TopState()
{
	return *m_stack->top().get();
}
void CGame::StartGame()
{
	// Set up some variables
	m_running = true;

	// Start the game loop
	while (m_running) {
		auto frameStart = std::chrono::high_resolution_clock::now();

		// Run some code
		Events();
		if (CheckAbortTick())
			break;
		Tick();
		if (CheckAbortTick())
			break;
		Draw();
		if (CheckAbortTick())
			break;

		// Frame capping
		auto frameEnd = std::chrono::high_resolution_clock::now();

		m_unhingedDeltaTime = std::chrono::duration_cast<DeltaTime_t>(frameEnd - frameStart);

		if (m_frameRate > 0) {
			auto timeElapsed =
				std::chrono::duration_cast<std::chrono::milliseconds>
				(frameEnd - frameStart).count();
			long long fps = (long long)((1.0f / (float)m_frameRate) * 1000.0f);
			if (timeElapsed < fps) {
				std::this_thread::sleep_for(
					std::chrono::milliseconds(fps - timeElapsed)
				);
			}
		}

		// Getting the delta time
		frameEnd = std::chrono::high_resolution_clock::now();
		m_deltaTime = std::chrono::duration_cast<DeltaTime_t>(frameEnd - frameStart);
		m_ticks++;
	}
}
void CGame::EndGame()
{
	m_running = false;
}
const std::chrono::duration<float>& CGame::GetDeltaTime() const
{
	return m_deltaTime;
}
const DeltaTime_t& CGame::GetUnhingedDeltaTime() const
{
	return m_unhingedDeltaTime;
}
bool CGame::IsRunning() const
{
	return m_running;
}
uint16_t CGame::GetFrameRate() const
{
	return m_frameRate;
}
void CGame::SetFrameRate(uint16_t frameRate)
{
	m_frameRate = frameRate;
}
uint64_t CGame::GetTick() const
{
	return m_ticks;
}
const CConfig& CGame::GetConfig() const
{
	return m_config;
}
void CGame::HandleEvent(const CGameEvent& event)
{
	switch (event.GetEvent()) {
		case EGameEvent::Game_PopState:
			PopState();
			break;
		case EGameEvent::Game_End:
			EndGame();
			break;
		default:
			break;
	}
}
void CGame::PushEvent(std::unique_ptr<CGameEvent> event)
{
	m_eventQueue.PushEvent(std::move(event));
}
CGameEvent& CGame::TopEvent()
{
	return m_eventQueue.TopEvent();
}
void CGame::PopEvent()
{
	m_eventQueue.PopEvent();
}
CGameModule& CGame::GetModule(const std::string& module)
{
	return m_modules->GetModule(module);
}
CGameModule* CGame::GetModulePtr(const std::string& module)
{
	return &m_modules->GetModule(module);
}

// Private Functions //
void CGame::AddModules()
{
	m_modules->AddModule(
		std::make_unique<CDrawModule>(*this, m_config),
		"CDrawModule"
	);
	m_modules->AddModule(
		std::make_unique<CInputModule>(*this, m_config),
		"CInputModule"
	);
	m_modules->AddModule(
		std::make_unique<CAudioModule>(*this),
		"CAudioModule"
	);
	m_modules->AddModule(
		std::make_unique<CNetworkModule>(*this),
		"CNetworkModule"
	);
}
void CGame::Events()
{
	// Game Events
	while (!m_eventQueue.EventsEmpty()) {
		m_modules->HandleEvent(m_eventQueue.TopEvent());
		TopState().HandleEvent(m_eventQueue.TopEvent());
		HandleEvent(m_eventQueue.TopEvent());
		m_eventQueue.PopEvent();
	}
}
void CGame::Tick()
{
	m_modules->Tick(m_deltaTime);
	TopState().Tick(m_deltaTime);
	m_modules->EndTick(m_deltaTime);
}
void CGame::Draw()
{
	TopState().Draw();
	m_modules->Draw();
}
bool CGame::CheckAbortTick()
{
	// Check if there is any more game states
	if (StatesEmpty() ||
		!IsRunning()) {
		EndGame();
		return true;
	}
	else {
		return false;
	}
}
