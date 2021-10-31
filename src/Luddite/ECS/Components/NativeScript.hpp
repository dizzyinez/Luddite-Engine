#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/Scripting/NativeScript.hpp"

struct C_NativeScript
{
	Luddite::INativeScript* m_pScript;
	Luddite::INativeScript*(*InstantiateScript)();
	void (*DestroyScript)(C_NativeScript*);
	template <typename T>
	void Bind()
	{
		InstantiateScript = []() {return static_cast<Luddite::INativeScript*>(new T());};
		DestroyScript = [](C_NativeScript* nsc) {delete static_cast<T*>(nsc->m_pScript);};
	}
};
