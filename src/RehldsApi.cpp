/*
 *  Copyright (C) 2018-2020 SPMod Development Team
 *
 *  This file is part of SPMod.
 *
 *  SPMod is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  SPMod is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with SPMod.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "spmod.hpp"

IRehldsApi *gRehldsApi;
const RehldsFuncs_t *gRehldsFuncs;
IRehldsHookchains *gRehldsHookchains;
IRehldsServerStatic *gRehldsServerStatic;
IRehldsServerData *gRehldsServerData;

static void SV_DropClientHook(IRehldsHook_SV_DropClient *chain, IGameClient *client, bool crash, const char *string)
{
    PlayerMngr *plrMngr = gSPGlobal->getPlayerManager();
    Player *plr = plrMngr->getPlayer(client->GetEdict());

    Forward *forward = gSPGlobal->getForwardManager()->getForward(ForwardMngr::FWD_PLAYER_DISCONNECT);
    forward->pushInt(plr->basePlayer()->edict()->getIndex());
    forward->pushInt(crash);
    forward->pushString(string);
    forward->execFunc(nullptr);

    chain->callNext(client, crash, string);

    PlayerMngr::m_playersNum--;
    plr->disconnect();

    forward = gSPGlobal->getForwardManager()->getForward(ForwardMngr::FWD_PLAYER_DISCONNECTED);
    forward->pushInt(plr->basePlayer()->edict()->getIndex());
    forward->pushInt(crash);
    forward->pushString(string);
    forward->execFunc(nullptr);
}

static void Cvar_DirectSetHook(IRehldsHook_Cvar_DirectSet *chain, cvar_t *cvar, const char *value)
{
    auto cachedCvar = gSPGlobal->getCvarManager()->getCvar(cvar->name);

    if (!cachedCvar)
    {
        chain->callNext(cvar, value);
        return;
    }

    // If cached cvar is the same, do not update cached value
    if (cachedCvar && cachedCvar->asString() != value)
    {
        cachedCvar->setValue(value);
    }

    chain->callNext(cvar, value);
}

static bool _initRehldsApi(CSysModule *module, std::string *error = nullptr)
{
    if (!module)
    {
        if (error)
            *error = "Failed to locate engine module";

        return false;
    }

    CreateInterfaceFn ifaceFactory = Sys_GetFactory(module);
    if (!ifaceFactory)
    {
        if (error)
            *error = "Failed to locate interface factory in engine module";

        return false;
    }

    std::int32_t retCode = 0;
    gRehldsApi = reinterpret_cast<IRehldsApi *>(ifaceFactory(VREHLDS_HLDS_API_VERSION, &retCode));
    if (!gRehldsApi)
    {
        if (error)
        {
            *error = "Failed to retrieve rehlds api interface, code ";
            *error += std::to_string(retCode);
        }

        return false;
    }

    auto majorVersion = gRehldsApi->GetMajorVersion();
    auto minorVersion = gRehldsApi->GetMinorVersion();

    if (majorVersion != REHLDS_API_VERSION_MAJOR)
    {
        std::stringstream msg;

        msg << "ReHLDS API Major version mismatch; expected " << std::to_string(REHLDS_API_VERSION_MAJOR);
        msg << ", got " << std::to_string(majorVersion);

        if (error)
            *error = msg.str();

        return false;
    }

    if (minorVersion < REHLDS_API_VERSION_MINOR)
    {
        std::stringstream msg;

        msg << "ReHLDS API minor version mismatch; excpected at least " << std::to_string(REHLDS_API_VERSION_MINOR);
        msg << ", got " << std::to_string(minorVersion);

        if (error)
            *error = msg.str();

        return false;
    }

    gRehldsFuncs = gRehldsApi->GetFuncs();
    gRehldsHookchains = gRehldsApi->GetHookchains();
    gRehldsServerStatic = gRehldsApi->GetServerStatic();
    gRehldsServerData = gRehldsApi->GetServerData();

    return true;
}

bool initRehldsApi()
{
    std::string errorMsg;

#if defined SP_POSIX
    CSysModule *engineModule = Sys_LoadModule("engine_i486.so");
    if (!_initRehldsApi(engineModule, &errorMsg))
    {
        gSPGlobal->getLoggerManager()->getLogger(gSPModLoggerName)->logToBoth(LogLevel::Error, errorMsg);
        return false;
    }
#else
    CSysModule *engineModule = Sys_LoadModule("swds.dll");
    if (!_initRehldsApi(engineModule))
    {
        engineModule = Sys_LoadModule("filesystem_stdio.dll");
        if (!_initRehldsApi(engineModule, &errorMsg))
        {
            gSPGlobal->getLoggerManager()->getLogger(gSPModLoggerName)->logToBoth(LogLevel::Error, errorMsg);
            return false;
        }
    }
#endif

    return true;
}

void installRehldsHooks()
{
    gRehldsHookchains->SV_DropClient()->registerHook(SV_DropClientHook);
    gRehldsHookchains->Cvar_DirectSet()->registerHook(Cvar_DirectSetHook);
}

void uninstallRehldsHooks()
{
    gRehldsHookchains->SV_DropClient()->unregisterHook(SV_DropClientHook);
    gRehldsHookchains->Cvar_DirectSet()->unregisterHook(Cvar_DirectSetHook);
}
