#include "api_lua.hpp"

#include "io/io.hpp"
#include "io/devices/MemoryDevice.hpp"

static int l_create_memory_device(lua::State* L) {
    auto name = lua::require_string(L, 1);
    if (io::get_device(name)) {
        throw std::runtime_error(
            "entry-point '" + std::string(name) + "' is already used"
        );
    }
    io::set_device(name, std::make_unique<io::MemoryDevice>());
    return 0;
}

const luaL_Reg applib[] = {
    {"create_memory_device", lua::wrap<l_create_memory_device>},
    // see libcore.cpp an stdlib.lua
    {nullptr, nullptr}
};
