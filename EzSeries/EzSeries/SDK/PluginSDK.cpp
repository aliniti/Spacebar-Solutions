#include "PluginSDK.h"
#include <cctype>

IPluginsSDK* g_PluginSDK = nullptr;
IObjectManager* g_ObjectManager = nullptr;
ICamera* g_Camera = nullptr;
INavMesh* g_NavMesh = nullptr;
IRenderer* g_Renderer = nullptr;
IShop* g_Shop = nullptr;
IGameObject* g_LocalPlayer = nullptr;
IDrawing* g_Drawing = nullptr;
IMainMenu* g_Menu = nullptr;
IOrbwalker* g_Orbwalker = nullptr;
ICommon* g_Common = nullptr;
IChampionManager* g_ChampionManager = nullptr;
IHealthPrediction* g_HealthPrediction = nullptr;
IItemManager* g_ItemManager = nullptr;
ILogSystem* g_Log = nullptr;

// Default empty entrypoint
int __stdcall DllMain(void*, unsigned long, void*) { return 1; }

// Plugin won't load in case of mismatch
PLUGIN_API std::int32_t	PLUGIN_TARGET_SDK = PLUGIN_SDK_VERSION;

bool StringContains(const char* strA, const char* strB, bool ignore_case)
{
	std::string str1 = strA;
	std::string str2 = strB;

	return std::search(
		str1.begin(), str1.end(),
		str2.begin(), str2.end(),
		[ignore_case](char ch1, char ch2)
		{
			if (ignore_case)
			{
				return std::toupper(ch1) == std::toupper(ch2);
			}

			return ch1 == ch2;
		}
	) != str1.end();
}

bool StringEquals(const char* strA, const char* strB, bool ignore_case)
{
	if (!ignore_case)
		return strcmp(strA, strB) == 0;
	return strlen(strA) == strlen(strB) && _stricmp(strA, strB) == 0;
}

void ICommon::Log(const char* _fmt, ...)
{
	va_list va;
	va_start(va, _fmt);
	g_Log->PrintVA(_fmt, va);
	va_end(va);
}
