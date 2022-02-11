#pragma once
#include <Windows.h>
#include <cstdint>

#include "Offsets.h"
#include <math.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include "includes.h"

//this defines most of functions for grabing Player data
Player* Player::GetPlayer(int index)
{
	static uint32_t moduleBase = (uint32_t)GetModuleHandle("client.dll");
	static uint32_t entityList = moduleBase + hazedumper::signatures::dwEntityList;

	return (Player*)(entityList + index * 0x10);
}

int* Player::GetMaxPlayer()
{
	static uint32_t moduleBase = (uintptr_t)GetModuleHandle("engine.dll");
	return (int*)(*(uint32_t*)(moduleBase + hazedumper::signatures::dwClientState) + hazedumper::signatures::dwClientState_MaxPlayer);
}

V3* Player::GetOrigin()
{
	return (V3*)(*(uintptr_t*)this + hazedumper::netvars::m_vecOrigin);
}

V3* Player::GetViewOffset()
{

	return (V3*)(*(uintptr_t*)this + hazedumper::netvars::m_vecViewOffset);
}

V3* Player::GetBonePos(int boneID)
{
	uint32_t boneMatrix = *(uint32_t*)(*(uint32_t*)this + hazedumper::netvars::m_dwBoneMatrix);
	static V3 bonePos;
	bonePos.x = *(float*)(boneMatrix + 0x30 * boneID + 0x0C);
	bonePos.y = *(float*)(boneMatrix + 0x30 * boneID + 0x1C);
	bonePos.z = *(float*)(boneMatrix + 0x30 * boneID + 0x2C);
	return &bonePos;
}

int* Player::GetHealth()
{
	return (int*)(*(uint32_t*)this + hazedumper::netvars::m_iHealth);
}

int* Player::GetTeam()
{
	return (int*)(*(uint32_t*)this + hazedumper::netvars::m_iTeamNum);
}

LocalPlayer* LocalPlayer::Get()
{
	static uint32_t clientModule = (uint32_t)(GetModuleHandle("client.dll"));
	static LocalPlayer* localPlayer = (LocalPlayer*)(clientModule + hazedumper::signatures::dwLocalPlayer);

	return localPlayer;
}

V3* LocalPlayer::GetOrigin()
{
	return (V3*)(*(uintptr_t*)this + hazedumper::netvars::m_vecOrigin);
}

V3* LocalPlayer::GetViewOffset()
{

	return (V3*)(*(uintptr_t*)this + hazedumper::netvars::m_vecViewOffset);
}

int* LocalPlayer::GetHealth()
{
	return (int*)(*(uintptr_t*)this + hazedumper::netvars::m_iHealth);
}

int* LocalPlayer::GetTeam()
{
	return (int*)(*(uintptr_t*)this + hazedumper::netvars::m_iTeamNum);
}

float LocalPlayer::GetDistance(V3* other)
{
	V3* myPos = GetOrigin();
	V3 delta = V3(other->x - myPos->x, other->y - myPos->y, other->z - myPos->z);

	return sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
}

//get aim point
void LocalPlayer::Aim(V3* target)
{
	//initiate module
	static uint32_t engineModule = (uint32_t)GetModuleHandle("engine.dll");
	static V3* viewAngles = (V3*)(*(uint32_t*)(engineModule + hazedumper::signatures::dwClientState) + hazedumper::signatures::dwClientState_ViewAngles);

	//allocation
	V3 origin = *GetOrigin();
	V3 viewOffset = *GetViewOffset();
	V3* myPos = &(origin + viewOffset);

	//algebra
	V3 deltaVec = { target->x - myPos->x, target->y - myPos->y, target->z - myPos->z };
	float deltaVecLength = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);

	float pitch = -asin(deltaVec.z / deltaVecLength) * (180 / M_PI);
	float yaw = atan2(deltaVec.y, deltaVec.x) * (180 / M_PI);

	if (pitch >= -89 && pitch <= 89 && yaw >= -180 && yaw <= 180) //thresholded to prevent hard ban
	{
		viewAngles->x = pitch;
		viewAngles->y = yaw;
	}
}