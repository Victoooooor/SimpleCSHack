#pragma once
//https://guidedhacking.com/threads/auto-padding-for-class-recreation.13478/
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}
//macro def
#define ABS(x) ((x < 0) ? (-x) : (x))
#define TORAD(x) ((x) * 0.01745329252)
#define W2S(x,y) hack->WSTransform(x, y) //most used function wrapper

//vector def & operators
struct Vec2 {
	float x, y;
};
struct Vec3 {
	float x, y, z;
	Vec3 operator+(Vec3 d) {
		return { x + d.x, y + d.y, z + d.z };
	}
	Vec3 operator*(float d) {
		return { x * d, y * d, z * d };
	}
};
struct Vec4 {
	float x, y, z, w;
};

class V3
{
public:
	float x, y, z;
	V3() { x = y = z = 0; }

	V3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	V3& operator+(V3 arg)
	{
		x += arg.x;
		y += arg.y;
		z += arg.z;
		return *this;
	}
};
//player class for aimbot geometry 
class Player
{
public:
	Player();
	static int* GetMaxPlayer();
	static Player* GetPlayer(int index);

	int* GetHealth();
	int* GetTeam();
	V3* GetOrigin();
	V3* GetViewOffset();
	V3* GetBonePos(int boneID);
};

class LocalPlayer
{

private:

	LocalPlayer();

public:

	static LocalPlayer* Get();

	V3* GetOrigin();
	V3* GetViewOffset();
	int* GetHealth();
	int* GetTeam();
	void Aim(V3* target);
	float GetDistance(V3*);

};



class Ent {
public:
	//Entitiy State Table from GuidedHacking
	union {
		// isDormant
		DEFINE_MEMBER_N(bool, isDormant, 0xED);
		// iHealth
		DEFINE_MEMBER_N(int, iHealth, 0x100);
		// vecOrigin
		DEFINE_MEMBER_N(Vec3, vecOrigin, 0x138);
		// OwnTeam
		DEFINE_MEMBER_N(int, OwnTeam, 0xF4);
		// boneMatrix
		DEFINE_MEMBER_N(int, boneMatrix, 0x26A8);
		// armorValue
		DEFINE_MEMBER_N(int, ArmorValue, 0xB368);
		// aimPunchAngle
		DEFINE_MEMBER_N(Vec3, aimPunchAngle, 0x302C);
		// angEyeAnglesX
		DEFINE_MEMBER_N(float, angEyeAnglesX, 0xB36C);
		// angEyeAnglesY
		DEFINE_MEMBER_N(float, angEyeAnglesY, 0xB370);
		// vecVelocity
		DEFINE_MEMBER_N(Vec3, vecVelocity, 0x114);
		// bHasHelmet
		DEFINE_MEMBER_N(bool, bHasHelmet, 0x1839);
	};
};

class EntListObj {
public:
	Ent* ent;
	char padding[12];
};

class EntList {
public:
	EntListObj ents[32];
};
//all functions directly used are integrated in Hack class
class Hack {
public:
	Player* GetClosestEnemy();
	uintptr_t engine;
	uintptr_t client;
	Ent* localEnt;
	EntList* entList;
	float viewMatrix[16];

	ID3DXLine* LineL;
	ID3DXFont* FontF;


	~Hack();

	void Init();
	void Update();
	void autoaim();
	void CheckButtons();

	bool CheckValidEnt(Ent* ent);
	bool WSTransform(Vec3 pos, Vec2& screen);
	Vec3 GetBonePos(Ent* ent, int bone);
	Vec3 TransformVec(Vec3 src, Vec3 ang, float d);

	struct Settings {
		bool showMenu = false;
		bool showTeamates = true;
		bool snaplines = false;
		bool aimbot = true;
		bool status2D = false;
		bool statusText = false;
		bool box3D = false;
		
	}settings;

	struct Buttons {
		DWORD showMenuBtn = VK_INSERT;
		DWORD showTeamatesBtn = VK_F1;
		DWORD snaplinesBtn =	VK_F2;
		DWORD aimbotBtn =		VK_F3;
		DWORD status2DBtn =		VK_F4;
		DWORD statusTextBtn =	VK_F5;
		DWORD box3DBtn =		VK_F6;
	}button;

	struct Colors {
		D3DCOLOR health = D3DCOLOR_ARGB(255, 45, 140, 90);
		D3DCOLOR armor = D3DCOLOR_ARGB(255, 30, 140, 255);
		struct Team {
			D3DCOLOR esp = D3DCOLOR_ARGB(255, 0, 255, 0);
			D3DCOLOR snapline = D3DCOLOR_ARGB(255, 0, 255, 0);
		}team;
		struct Enemy {
			D3DCOLOR esp = D3DCOLOR_ARGB(255, 255, 0, 0);
			D3DCOLOR snapline = D3DCOLOR_ARGB(255, 255, 0, 0);
		}enemy;
	}color;
};