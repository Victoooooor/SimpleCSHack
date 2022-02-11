#pragma once
#include "hack.h"
//basic draw wrappers for all needed elements
void DrawRect(int x, int y, int w, int h, D3DCOLOR col);
void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR col);
void DrawLine(Vec2 src, Vec2 dst, int thickness, D3DCOLOR col);
void DrawBox(Vec3 top, Vec3 bot, float a, int width, int thickness, D3DCOLOR col);
void DrawText(const char* text, float x, float y, D3DCOLOR col);