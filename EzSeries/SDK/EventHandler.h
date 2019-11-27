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
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnTick"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnTick"), pCallback); }
};

template < >
struct EventHandler<Events::OnProcessSpellCast>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnProcessSpellEventArgs* args)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnProcessSpellCast"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnProcessSpellEventArgs* args)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnProcessSpellCast"), pCallback); }
};

template < >
struct EventHandler<Events::OnEndScene>
{
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnEndScene"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnEndScene"), pCallback); }
};

template < >
struct EventHandler<Events::OnWndProc>
{
	static void AddEventHandler(void(*pCallback)(OnWndProcEventArgs* args)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnWndProc"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(OnWndProcEventArgs* args)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnWndProc"), pCallback); }
};

template < >
struct EventHandler<Events::OnPreReset>
{
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnPreReset"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnPreReset"), pCallback); }
};

template < >
struct EventHandler<Events::OnPostReset>
{
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnPostReset"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnPostReset"), pCallback); }
};

template < >
struct EventHandler<Events::OnHudDraw>
{
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnHudDraw"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnHudDraw"), pCallback); }
};

template < >
struct EventHandler<Events::OnHPBarDraw>
{
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnHPBarDraw"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnHPBarDraw"), pCallback); }
};

template < >
struct EventHandler<Events::OnBuff>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnBuffEventArgs* args)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnBuff"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnBuffEventArgs* args)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnBuff"), pCallback); }
};

template < >
struct EventHandler<Events::OnCastSpell>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnCastSpellEventArgs* args)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnCastSpell"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnCastSpellEventArgs* args)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnCastSpell"), pCallback); }
};

template < >
struct EventHandler<Events::OnCreateObject>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnCreateObject"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnCreateObject"), pCallback); }
};

template < >
struct EventHandler<Events::OnDeleteObject>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnDeleteObject"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnDeleteObject"), pCallback); }
};

template < >
struct EventHandler<Events::OnDoCast>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnProcessSpellEventArgs* args)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnDoCast"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnProcessSpellEventArgs* args)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnDoCast"), pCallback); }
};

template < >
struct EventHandler<Events::OnIssueOrder>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnIssueOrderEventArgs* args)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnIssueOrder"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnIssueOrderEventArgs* args)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnIssueOrder"), pCallback); }
};

template < >
struct EventHandler<Events::OnNewPath>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnNewPathEventArgs* args)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnNewPath"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnNewPathEventArgs* args)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnNewPath"), pCallback); }
};

template < >
struct EventHandler<Events::OnPlayAnimation>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnPlayAnimationEventArgs* args)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnPlayAnimation"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnPlayAnimationEventArgs* args)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnPlayAnimation"), pCallback); }
};

template < >
struct EventHandler<Events::OnPreCreateObject>
{
	static void AddEventHandler(void(*pCallback)(OnPreCreateObjectEventArgs* args)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnPreCreateObject"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(OnPreCreateObjectEventArgs* args)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnPreCreateObject"), pCallback); }
};

template < >
struct EventHandler<Events::OnStopCast>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnStopCastEventArgs* args)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnStopCast"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnStopCastEventArgs* args)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnStopCast"), pCallback); }
};

template < >
struct EventHandler<Events::OnTeleport>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnTeleportEventArgs* args)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnTeleport"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnTeleportEventArgs* args)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnTeleport"), pCallback); }
};

template < >
struct EventHandler<Events::OnUpdateChargeableSpell>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* sender, OnUpdateChargeableSpellEventArgs* args)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnUpdateChargeableSpell"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* sender, OnUpdateChargeableSpellEventArgs* args)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnUpdateChargeableSpell"), pCallback); }
};

template < >
struct EventHandler<Events::OnGlowRender>
{
	static void AddEventHandler(void(*pCallback)()) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnGlowRender"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)()) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnGlowRender"), pCallback); }
};

template < >
struct EventHandler<Events::OnBeforeAttackOrbwalker>
{
	static void AddEventHandler(void(*pCallback)(BeforeAttackOrbwalkerArgs* args)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnBeforeAttackOrbwalker"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(BeforeAttackOrbwalkerArgs* args)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnBeforeAttackOrbwalker"), pCallback); }
};

template < >
struct EventHandler<Events::OnAfterAttackOrbwalker>
{
	static void AddEventHandler(void(*pCallback)(IGameObject* target)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnAfterAttackOrbwalker"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(IGameObject* target)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnAfterAttackOrbwalker"), pCallback); }
};

template < >
struct EventHandler<Events::OnEventHandler>
{
	static void AddEventHandler(void(*pCallback)(OnEventHandlerEventArgs* args)) { g_PluginSDK->GetEventManager()->AddEventCallback(fnv_hash("OnEventHandler"), pCallback); }
	static void RemoveEventHandler(void(*pCallback)(OnEventHandlerEventArgs* args)) { g_PluginSDK->GetEventManager()->RemoveEventCallback(fnv_hash("OnEventHandler"), pCallback); }
};