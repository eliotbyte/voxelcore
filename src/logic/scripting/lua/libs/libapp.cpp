#include "api_lua.hpp"

#include "logic/scripting/scripting.hpp"
#include "engine/Engine.hpp"
#include "engine/EnginePaths.hpp"
#include "network/Network.hpp"
#include "util/platform.hpp"

using namespace scripting;

static int l_start_debug_instance(lua::State* L) {
    int port = lua::tointeger(L, 1);
    if (port == 0) {
        port = engine->getNetwork().findFreePort();
        if (port == -1) {
            throw std::runtime_error("could not find free port");
        }
    }
    const auto& paths = engine->getPaths();

    std::vector<std::string> args {
        "--res", paths.getResourcesFolder().u8string(),
        "--dir", paths.getUserFilesFolder().u8string(),
        "--dbg-server",  "tcp:" + std::to_string(port),
    };
    platform::new_engine_instance(std::move(args));
    return lua::pushinteger(L, port);
}

const luaL_Reg applib[] = {
    {"start_debug_instance", lua::wrap<l_start_debug_instance>},
    // for other functions see libcore.cpp and stdlib.lua
    {nullptr, nullptr}
};
