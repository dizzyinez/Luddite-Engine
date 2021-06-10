#include "ObjectInterfacePerModule.h"
#include "IObject.h"
#include "Luddite/Core/GameInstance.hpp"
#include "Luddite/Core/SystemTable.h"
#include "ISimpleSerializer.h"
#include "IRuntimeObjectSystem.h"
#include "IObjectFactorySystem.h"

// #include "RuntimeSourceDependency.h"

enum InterfaceIDEnumLuddite
{
        IID_IRCCPP_GAME_INSTANCE = IID_ENDInterfaceID, // IID_ENDInterfaceID from IObject.h InterfaceIDEnum

        IID_ENDInterfaceIDEnumLuddite
};

struct GameInstance : GameInstanceI, TInterface<IID_IRCCPP_GAME_INSTANCE, IObject>
{
        GameInstance()
        {
                PerModuleInterface::g_pSystemTable->pGameInstanceI = this;
                g_pSys->pRuntimeObjectSystem->GetObjectFactorySystem()->SetObjectConstructorHistorySize(10);
        }

        // void Serialize(ISimpleSerializer *pSerializer) override
        // {
        //         // SERIALIZE(show_demo_window);
        //         // SERIALIZE(compileStartTime);
        //         // SERIALIZE(compileEndTime);
        // }
        virtual void Initialize() override
        {
                // LD_LOG_INFO("TEST");
        }
};

REGISTERSINGLETON(GameInstance, true);