#pragma once
#ifdef FOCUS_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

#include<memory>

#include"Platform/WindowSystem.h"
#include"Function/Render/RenderSystem.h"
#include"Editor/Overlay.h"

namespace FOCUS
{
	class GlobalContext
	{
	public:
		GlobalContext();
		void tick(bool* running);
	};
}

extern "C" __declspec(dllexport) int getSceneObjCount();
