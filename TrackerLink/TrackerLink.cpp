#include "pch.h"
#include "TrackerLink.h"
#include <windows.h>
#include <shellapi.h>
using namespace std;


BAKKESMOD_PLUGIN(TrackerLink, "This plugin allows you to open RLTracker of all players in game with 'F5' key.", plugin_version, PLUGINTYPE_FREEPLAY)

std::map<int, std::string> PlatformMap{
	{ 0, "[Unknown]" },
	{ 1,  "steam" },
	{ 2,  "psn" },
	{ 3,  "psn" },
	{ 4,  "xbl" },
	{ 6,  "switch" },
	{ 7,  "switch" },
	{ 9,  "[Deleted]" },
	{ 11, "epic" },
};

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void TrackerLink::onLoad()
{
	_globalCvarManager = cvarManager;
	LOG("Plugin TrackerLink loaded !");
	 //!! Enable debug logging by setting DEBUG_LOG = true in logging.h !!
	DEBUGLOG("TrackerLink debug mode enabled");
	gameWrapper->HookEvent("Function Engine.GameViewportClient.Tick", bind(&TrackerLink::onTick, this, std::placeholders::_1));
}
void TrackerLink::RenderSettings()
{
	ImGui::TextUnformatted("Noting to see here.");
}
void TrackerLink::onTick(std::string eventName)
{

	static std::chrono::steady_clock::time_point lastExecutionTime;
	static bool canPress = true;

	if (gameWrapper->IsKeyPressed(gameWrapper->GetFNameIndexByString("F5"))) {

		// Delay between each execution
		std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsedSeconds = currentTime - lastExecutionTime;
		if (!canPress || elapsedSeconds.count() < 3.0) {
			return;
		}
		lastExecutionTime = currentTime;


		// getting players
		if (!gameWrapper->IsInOnlineGame() || gameWrapper->IsInGame()) return;
		ServerWrapper sw = gameWrapper->GetOnlineGame();
		ArrayWrapper<PriWrapper> players = sw.GetPRIs();


		for (size_t i = 0; i < players.Count(); i++) {
			PriWrapper priw = players.Get(i);
			OnlinePlatform plateform = priw.GetPlatform();
			LOG("" + priw.GetPlayerName().ToString() + " " + PlatformMap.at(plateform));


			string s = std::format("https://rocketleague.tracker.network/rocket-league/profile/{0}/{1}", PlatformMap.at(plateform), priw.GetPlayerName().ToString());

			std::wstring stemp = std::wstring(s.begin(), s.end());
			LPCWSTR url = stemp.c_str();

			ShellExecute(0, 0, url, 0, 0, SW_SHOW);
		}
	}
}
