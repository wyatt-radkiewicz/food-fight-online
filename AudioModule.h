#pragma once

#include "Game.h"
#include "Utilities.h"

//
// Handles the listener's pov for audio.
//
class CAudioModule : public CGameModule
{
public:
	CAudioModule(CGame& game);
	~CAudioModule();

	void HandleEvent(const CGameEvent& event) override;
	void Tick(const DeltaTime_t& deltaTime) override;
	void EndTick(const DeltaTime_t& deltaTime) override;
	void Draw() override;

	void SetAudioVolume(float volume);
	float GetAudioVolume() const;
	void SetListeningPos(float x, float y);
	void SetListeningPos(Vector2f pos);
	Vector2f GetListeningPos();
};
