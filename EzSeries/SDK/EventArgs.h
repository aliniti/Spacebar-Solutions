#pragma once

#include "PluginSDK.h"

struct OnProcessSpellEventArgs
{
	IGameObject* Target;
	ISpellData* SpellData;

	Vector StartPosition;
	Vector EndPosition;

	SpellSlot SpellSlot;

	int32_t CastedSpellCount;
	int32_t Level;

	bool IsAutoAttack;
};

struct OnTeleportEventArgs
{
	enum class TeleportType : int
	{
		Recall,
		SuperRecall,
		Teleport,
		TwistedFate,
		Shen
	};

	enum class TeleportStatus : int
	{
		Start,
		Finish,
		Abort
	};

	TeleportType Type;
	TeleportStatus Status;

	float Start;
	float Duration;
};

struct OnStopCastEventArgs
{
	bool StopAnimation;
	bool ExecuteCastFrame;
	bool ForceStop;
	bool DestroyMissile;

	int32_t MissileNetworkdId;
	int32_t Counter;
};

struct OnNewPathEventArgs
{
	bool IsDash;
	float Speed;

	std::vector<Vector> Path;
};

struct OnCastSpellEventArgs
{
	SpellSlot SpellSlot;

	Vector CastPointStart;
	Vector CastPointEnd;

	IGameObject* Target;

	bool Process;
	bool IsUserCall;
};

struct OnIssueOrderEventArgs
{
	IGameObject* Target;

	Vector Position;
	IssueOrderType IssueOrderType;

	bool IsAttackMove;
	bool IsPetCommand;
	bool IsUserCall;
	bool Process;
};

struct OnBuffEventArgs
{
	IGameObject* Caster;
	IBuffInstance Buff;
	bool IsBuffGain;
};

struct OnWndProcEventArgs
{
	UINT uMsg;
	WPARAM wParam;
	LPARAM lParam;
};

struct OnEventHandlerEventArgs
{
	const char* EventName;
	int32_t Argument1;
	int32_t Argument2;
	int32_t Argument3;
	int32_t Argument4;
};

struct OnPreCreateObjectEventArgs
{
	std::string ObjectName;
	std::string ChampionName;

	int* SkinId;
};

struct OnPlayAnimationEventArgs
{
	std::string AnimationName;
	int AnimHash;
	bool Process;
};

struct OnUpdateChargeableSpellEventArgs
{
	SpellSlot SpellSlot;
	Vector Position;

	bool IsUserCall;
	bool ReleaseCast;
	bool Process;
};

struct BeforeAttackOrbwalkerArgs
{
	IGameObject* Target;
	bool Process;
};

struct OnSurrenderVoteEventArgs
{
	IGameObject* Sender;
	SurrenderVoteType Type;
};