#include "includes.h"
#include <sstream>
#include <string.h>


// data allocation
void* d3d9Device[119]; //used for d3d9x api
BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;
D3DCOLOR espcol, snapcol;

extern LPDIRECT3DDEVICE9 pDevice = nullptr;

Hack* hack;

// hook function for graphic overlays
void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice) {
	if (!pDevice)
		pDevice = o_pDevice;

	// Drawing option menu
	DrawText("HACK", windowWidth / 2, windowHeight - 20, D3DCOLOR_ARGB(255,255,255,255));

	int menuOffX = windowWidth / 2;
	int menuOffY = 50;
	D3DCOLOR enabled = D3DCOLOR_ARGB(255, 0, 255, 0);
	D3DCOLOR disabled = D3DCOLOR_ARGB(255, 255, 0, 0);

	if (!hack->settings.showMenu) {
		DrawText("Show Menu (INS)", menuOffX, menuOffY, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	else {
		DrawText("Show Teamates (F1)",	menuOffX, menuOffY + 0 * 12, hack->settings.showTeamates ? enabled : disabled );
		DrawText("Snaplines (F2)",	menuOffX, menuOffY + 1 * 12, hack->settings.snaplines ? enabled : disabled);
		DrawText("Aimbot (F3)", menuOffX, menuOffY + 2 * 12, hack->settings.aimbot ? enabled : disabled);
		DrawText("Statusbars (F4)",	menuOffX, menuOffY + 3 * 12, hack->settings.status2D ? enabled : disabled);
		DrawText("Status Text (F5)",	menuOffX, menuOffY + 4 * 12, hack->settings.statusText ? enabled : disabled);
		DrawText("3D Box (F6)",	menuOffX, menuOffY + 5 * 12, hack->settings.box3D ? enabled : disabled);
		DrawText("Hide Menu (INS)",		menuOffX, menuOffY + 6 * 12, D3DCOLOR_ARGB(255,255,255,255));
	}
	
	//update the rest by reading the settings

	for (int i = 1; i < 32; i++) {
		Ent* curEnt = hack->entList->ents[i].ent;
		if (!hack->CheckValidEnt(curEnt))
			continue;

		
		if (curEnt->OwnTeam == hack->localEnt->OwnTeam) {
			espcol = hack->color.team.esp;
			snapcol = hack->color.team.snapline;
		}
		else {
			espcol = hack->color.enemy.esp;
			snapcol = hack->color.enemy.snapline;
		}
			

		if(!hack->settings.showTeamates && (curEnt->OwnTeam == hack->localEnt->OwnTeam))
			continue;

		Vec3 entHead3D = hack->GetBonePos(curEnt, 8);
		entHead3D.z += 8;
		Vec2 entPos2D, entHead2D;
		// snapline
		if (hack->WSTransform(curEnt->vecOrigin, entPos2D)) {
			if (hack->settings.snaplines) DrawLine(entPos2D.x, entPos2D.y, windowWidth / 2, windowHeight, 2, snapcol);

			if (hack->WSTransform(entHead3D, entHead2D)) {
				//draw player box
				if (hack->settings.box3D) 
					DrawBox(entHead3D, curEnt->vecOrigin, curEnt->angEyeAnglesY, 25, 2, espcol);
				//draw hp bar
				if (hack->settings.status2D) {
					int height = ABS(entPos2D.y - entHead2D.y);
					int dX = (entPos2D.x - entHead2D.x);

					float healthPerc = curEnt->iHealth / 100.f;
					float armorPerc = curEnt->ArmorValue / 100.f;

					Vec2 botHealth, topHealth, botArmor, topArmor;
					int healthHeight = height * healthPerc;
					int armorHeight = height * armorPerc;

					botHealth.y = botArmor.y = entPos2D.y;

					botHealth.x = entPos2D.x - (height / 4) - 2;
					botArmor.x = entPos2D.x + (height / 4) + 2;

					topHealth.y = entHead2D.y + height - healthHeight;
					topArmor.y = entHead2D.y + height - armorHeight;

					topHealth.x = entPos2D.x - (height / 4) - 2 - (dX * healthPerc);
					topArmor.x = entPos2D.x + (height / 4) + 2 - (dX * armorPerc);

					DrawLine(botHealth, topHealth, 2, hack->color.health);
					DrawLine(botArmor, topArmor, 2, hack->color.armor);
				}

				//draw text stats
				if (hack->settings.statusText) {
					std::stringstream s1, s2;
					s1 << curEnt->iHealth;
					s2 << curEnt->ArmorValue;
					std::string t1 = "hp: " + s1.str();
					std::string t2 = "ap: " + s2.str();
					char* healthMsg = (char*)t1.c_str();
					char* armorMsg = (char*)t2.c_str();

					DrawText(healthMsg, entPos2D.x, entPos2D.y, D3DCOLOR_ARGB(255, 255, 255, 255));
					DrawText(armorMsg, entPos2D.x, entPos2D.y + 12, D3DCOLOR_ARGB(255, 255, 255, 255));

					if(!curEnt->bHasHelmet)
						DrawText("ez hs", entPos2D.x, entPos2D.y + 24, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
			}
		}
	}
	oEndScene(pDevice);
}

DWORD WINAPI OnAttach(HMODULE hModule) {
	//open console
#ifdef _DEBUG
	AllocConsole();
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	SetConsoleTitleA("HACK CONSOLE");
#endif

	// hook
	if (GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {
		memcpy(EndSceneBytes, (char*)d3d9Device[42], 7);

		oEndScene = (tEndScene)TrampHook((char*)d3d9Device[42], (char*)hkEndScene, 7);
	}

	hack = new Hack();
	hack->Init();

	// hack loop
	while (!GetAsyncKeyState(VK_END)) {
		hack->Update();
		if (hack->settings.aimbot)
			hack->autoaim(); //autoaim is not graphic mode hence seperated, graphs are done using hooks
		Vec3 pAng = hack->localEnt->aimPunchAngle;
		Sleep(1);
	}

	// unhook
	Patch((BYTE*)d3d9Device[42], EndSceneBytes, 7);

	Sleep(1000);

	// uninject
	FreeLibraryAndExitThread(hModule, 0);
}

VOID WINAPI OnDetach()
{
#ifdef _DEBUG
	fclose((FILE*)stdin);
	fclose((FILE*)stdout);

	HWND hw_ConsoleHwnd = GetConsoleWindow();
	FreeConsole();
	PostMessageW(hw_ConsoleHwnd, WM_CLOSE, 0, 0);
#endif
}
//main func
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpr) {
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)OnAttach, hModule, NULL, NULL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		OnDetach();
	}
	return TRUE;
}