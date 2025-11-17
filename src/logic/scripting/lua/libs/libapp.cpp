#include "api_lua.hpp"

#include "io/io.hpp"
#include "io/devices/MemoryDevice.hpp"
#include "engine/Engine.hpp"
#include "content/ContentControl.hpp"
#include "logic/scripting/scripting.hpp"

using namespace scripting;

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

static int l_get_content_sources(lua::State* L) {
    const auto& sources = engine->getContentControl().getContentSources();
    lua::createtable(L, static_cast<int>(sources.size()), 0);
    for (size_t i = 0; i < sources.size(); i++) {
        lua::pushlstring(L, sources[i].string());
        lua::rawseti(L, static_cast<int>(i + 1));
    }
    return 1;
}

static int l_set_content_sources(lua::State* L) {
    engine->getContentControl().resetContentSources();
    return 0;
}

const luaL_Reg applib[] = {
    {"create_memory_device", lua::wrap<l_create_memory_device>},
    {"get_content_sources", lua::wrap<l_get_content_sources>},
    {"set_content_sources", lua::wrap<l_set_content_sources>},
    // see libcore.cpp an stdlib.lua
    {nullptr, nullptr}
};
