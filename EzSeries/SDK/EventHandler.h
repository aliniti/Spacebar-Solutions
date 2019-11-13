#pragma once

#include "PluginSDK.h"
#include "EventArgs.h"

template < Events Event >
struct EventHandler
{
	static void AddEventHandler() { }
	static void RemoveEventHandler() { }
};

template < >
struct EventHandler<Events::GameUpdate>
{
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->AddEventHandler(Events::GameUpdate, pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->RemoveEventHandler(Events::GameUpdate, pCallback); }
};

template < >
struct EventHandler<Events::OnProcessSpellCast>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnProcessSpellEventArgs* args)) { g_PluginSDK->AddEventHandler(Events::OnProcessSpellCast, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnProcessSpellEventArgs* args)) { g_PluginSDK->RemoveEventHandler(Events::OnProcessSpellCast, pCallback); }
};

template < >
struct EventHandler<Events::OnEndScene>
{
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->AddEventHandler(Events::OnEndScene, pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->RemoveEventHandler(Events::OnEndScene, pCallback); }
};

template < >
struct EventHandler<Events::OnWndProc>
{
	static void AddEventHandler(void(*pCallback)(OnWndProcEventArgs* args)) { g_PluginSDK->AddEventHandler(Events::OnWndProc, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(OnWndProcEventArgs* args)) { g_PluginSDK->RemoveEventHandler(Events::OnWndProc, pCallback); }
};

template < >
struct EventHandler<Events::OnPreReset>
{
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->AddEventHandler(Events::OnPreReset, pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->RemoveEventHandler(Events::OnPreReset, pCallback); }
};

template < >
struct EventHandler<Events::OnPostReset>
{
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->AddEventHandler(Events::OnPostReset, pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->RemoveEventHandler(Events::OnPostReset, pCallback); }
};

template < >
struct EventHandler<Events::OnHudDraw>
{
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->AddEventHandler(Events::OnHudDraw, pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->RemoveEventHandler(Events::OnHudDraw, pCallback); }
};

template < >
struct EventHandler<Events::OnHPBarDraw>
{
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->AddEventHandler(Events::OnHPBarDraw, pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->RemoveEventHandler(Events::OnHPBarDraw, pCallback); }
};

template < >
struct EventHandler<Events::OnBuff>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnBuffEventArgs* args)) { g_PluginSDK->AddEventHandler(Events::OnBuff, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnBuffEventArgs* args)) { g_PluginSDK->RemoveEventHandler(Events::OnBuff, pCallback); }
};

template < >
struct EventHandler<Events::OnCastSpell>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnCastSpellEventArgs* args)) { g_PluginSDK->AddEventHandler(Events::OnCastSpell, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnCastSpellEventArgs* args)) { g_PluginSDK->RemoveEventHandler(Events::OnCastSpell, pCallback); }
};

template < >
struct EventHandler<Events::OnCreateObject>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender)) { g_PluginSDK->AddEventHandler(Events::OnCreateObject, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender)) { g_PluginSDK->RemoveEventHandler(Events::OnCreateObject, pCallback); }
};

template < >
struct EventHandler<Events::OnDeleteObject>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender)) { g_PluginSDK->AddEventHandler(Events::OnDeleteObject, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender)) { g_PluginSDK->RemoveEventHandler(Events::OnDeleteObject, pCallback); }
};

template < >
struct EventHandler<Events::OnDoCast>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnProcessSpellEventArgs* args)) { g_PluginSDK->AddEventHandler(Events::OnDoCast, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnProcessSpellEventArgs* args)) { g_PluginSDK->RemoveEventHandler(Events::OnDoCast, pCallback); }
};

template < >
struct EventHandler<Events::OnIssueOrder>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnIssueOrderEventArgs* args)) { g_PluginSDK->AddEventHandler(Events::OnIssueOrder, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnIssueOrderEventArgs* args)) { g_PluginSDK->RemoveEventHandler(Events::OnIssueOrder, pCallback); }
};

template < >
struct EventHandler<Events::OnNewPath>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnNewPathEventArgs* args)) { g_PluginSDK->AddEventHandler(Events::OnNewPath, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnNewPathEventArgs* args)) { g_PluginSDK->RemoveEventHandler(Events::OnNewPath, pCallback); }
};

template < >
struct EventHandler<Events::OnPlayAnimation>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnPlayAnimationEventArgs* args)) { g_PluginSDK->AddEventHandler(Events::OnPlayAnimation, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnPlayAnimationEventArgs* args)) { g_PluginSDK->RemoveEventHandler(Events::OnPlayAnimation, pCallback); }
};

template < >
struct EventHandler<Events::OnCryptString>
{
	static void AddEventHandler(void(*pCallback)(const char* str, int hash)) { g_PluginSDK->AddEventHandler(Events::OnCryptString, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(const char* str, int hash)) { g_PluginSDK->RemoveEventHandler(Events::OnCryptString, pCallback); }
};

template < >
struct EventHandler<Events::OnPreCreateObject>
{
	static void AddEventHandler(void(*pCallback)(OnPreCreateObjectEventArgs* args)) { g_PluginSDK->AddEventHandler(Events::OnPreCreateObject, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(OnPreCreateObjectEventArgs* args)) { g_PluginSDK->RemoveEventHandler(Events::OnPreCreateObject, pCallback); }
};

template < >
struct EventHandler<Events::OnStopCast>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnStopCastEventArgs* args)) { g_PluginSDK->AddEventHandler(Events::OnStopCast, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnStopCastEventArgs* args)) { g_PluginSDK->RemoveEventHandler(Events::OnStopCast, pCallback); }
};

template < >
struct EventHandler<Events::OnTeleport>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnTeleportEventArgs* args)) { g_PluginSDK->AddEventHandler(Events::OnTeleport, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnTeleportEventArgs* args)) { g_PluginSDK->RemoveEventHandler(Events::OnTeleport, pCallback); }
};

template < >
struct EventHandler<Events::OnTurretRange>
{
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->AddEventHandler(Events::OnTurretRange, pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->RemoveEventHandler(Events::OnTurretRange, pCallback); }
};

template < >
struct EventHandler<Events::OnUpdateChargeableSpell>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnUpdateChargeableSpellEventArgs* args)) { g_PluginSDK->AddEventHandler(Events::OnUpdateChargeableSpell, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnUpdateChargeableSpellEventArgs* args)) { g_PluginSDK->RemoveEventHandler(Events::OnUpdateChargeableSpell, pCallback); }
};

template < >
struct EventHandler<Events::OnGlowRender>
{
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->AddEventHandler(Events::OnGlowRender, pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->RemoveEventHandler(Events::OnGlowRender, pCallback); }
};

template < >
struct EventHandler<Events::OnMenuDraw>
{
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->AddEventHandler(Events::OnMenuDraw, pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->RemoveEventHandler(Events::OnMenuDraw, pCallback); }
};

template < >
struct EventHandler<Events::OnBeforeAttackOrbwalker>
{
	static void AddEventHandler(void(*pCallback)(BeforeAttackOrbwalkerArgs* args)) { g_PluginSDK->AddEventHandler(Events::OnBeforeAttackOrbwalker, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(BeforeAttackOrbwalkerArgs* args)) { g_PluginSDK->RemoveEventHandler(Events::OnBeforeAttackOrbwalker, pCallback); }
};

template < >
struct EventHandler<Events::OnAfterAttackOrbwalker>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* target)) { g_PluginSDK->AddEventHandler(Events::OnAfterAttackOrbwalker, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* target)) { g_PluginSDK->RemoveEventHandler(Events::OnAfterAttackOrbwalker, pCallback); }
};

template < >
struct EventHandler<Events::OnSurrenderVote>
{
	static void AddEventHandler(void(*pCallback)(OnSurrenderVoteEventArgs* args)) { g_PluginSDK->AddEventHandler(Events::OnSurrenderVote, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(OnSurrenderVoteEventArgs* args)) { g_PluginSDK->RemoveEventHandler(Events::OnSurrenderVote, pCallback); }
};

template < >
struct EventHandler<Events::OnEventHandler>
{
	static void AddEventHandler(void(*pCallback)(OnEventHandlerEventArgs* args)) { g_PluginSDK->AddEventHandler(Events::OnEventHandler, pCallback); }
	static void RemoveEventHandler(void(*pCallback)(OnEventHandlerEventArgs* args)) { g_PluginSDK->RemoveEventHandler(Events::OnEventHandler, pCallback); }
};