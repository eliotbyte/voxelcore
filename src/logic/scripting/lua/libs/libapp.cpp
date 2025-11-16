#include "api_lua.hpp"

#include "io/io.hpp"
#include "io/devices/MemoryDevice.hpp"
#include "logic/scripting/scripting.hpp"
#include "engine/Engine.hpp"
#include "engine/EnginePaths.hpp"
#include "network/Network.hpp"
#include "util/platform.hpp"
#include "window/Window.hpp"

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

static int l_focus(lua::State* L) {
    engine->getWindow().focus();
    return 0;
}

static int l_create_memory_device(lua::State* L) {
    std::string name = lua::require_string(L, 1);
    if (io::get_device(name)) {
        throw std::runtime_error(
            "entry-point '" + name + "' is already used"
        );
    }
    if (name.find(':') != std::string::npos) {
        throw std::runtime_error("invalid entry point name");
    }
    
    io::set_device(name, std::make_unique<io::MemoryDevice>());
    return 0;
}

const luaL_Reg applib[] = {
    {"start_debug_instance", lua::wrap<l_start_debug_instance>},
    {"focus", lua::wrap<l_focus>},
    {"create_memory_device", lua::wrap<l_create_memory_device>},
    // for other functions see libcore.cpp and stdlib.lua
    {nullptr, nullptr}
};
