#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Global Values

// FORKS SHOULD NOT CHANGE THE MAJOR/MINOR VERSION! The major/minor version
// here is used by scripts to determine whether they're compatible with the
// base headcrab library. For the sake of compatibility, forks should
// keep this intact and change the "mod" table based on the comment below.
#define VERSION_MAJOR 0
#define VERSION_MINOR 1

#define BITS sizeof(void *) * 8

#if defined(_WIN32)
#define OS "Windows"
#elif defined(__APPLE__)
#define OS "macOS"
#elif defined(__linux__)
#define OS "Linux"
#elif defined(BSD)
#define OS "BSD"
#else
#error "OS detection failed! Please make sure you're compiling on a supported platform!"
#endif

static lua_State *HEADCRAB;

enum HEADCRAB_TYPES {
    FLOAT32,
    FLOAT64,
    SIGNED8,
    SIGNED16,
    SIGNED32,
    SIGNED64,
    UNSIGNED8,
    UNSIGNED16,
    UNSIGNED32,
    UNSIGNED64,
};

static char SCRIPT_PATH[4096] = {0, 0xB3, 0xDC, 0xF5}; // I'd really like to know if people use paths longer than this

// Native Lua Library

static int headcrab_peek(lua_State *L) {
    void *addr;
    int argc = lua_gettop(L);
    enum HEADCRAB_TYPES type;
    if(argc == 2) {
        type = lua_tointeger(L, 1);
        addr = (void *)lua_tointeger(L, 2);
        switch(type) {
            case FLOAT32:
                lua_pushnumber(L, *(float *)addr);
                return 1;
            case FLOAT64:
                lua_pushnumber(L, *(double *)addr);
                return 1;
            case SIGNED8:
                lua_pushinteger(L, *(int8_t *)addr);
                return 1;
            case SIGNED16:
                lua_pushinteger(L, *(int16_t *)addr);
                return 1;
            case SIGNED32:
                lua_pushinteger(L, *(int32_t *)addr);
                return 1;
            case SIGNED64:
                lua_pushinteger(L, *(int64_t *)addr);
                return 1;
            case UNSIGNED8:
                lua_pushinteger(L, *(uint8_t *)addr);
                return 1;
            case UNSIGNED16:
                lua_pushinteger(L, *(uint16_t *)addr);
                return 1;
            case UNSIGNED32:
                lua_pushinteger(L, *(uint32_t *)addr);
                return 1;
            case UNSIGNED64:
                lua_pushinteger(L, *(uint64_t *)addr);
                return 1;
            default:
                return 0;
        }
    } else {
        return 0;
    }
}

static int headcrab_poke(lua_State *L) {
    void *addr;
    int argc = lua_gettop(L);
    enum HEADCRAB_TYPES type;
    if(argc == 3) {
        type = lua_tointeger(L, 1);
        addr = (void *)lua_tointeger(L, 2);
        switch(type) {
            case FLOAT32:
                *(float *)addr = lua_tonumber(L, 3);
                break;
            case FLOAT64:
                *(double *)addr = lua_tonumber(L, 3);
                break;
            case SIGNED8:
                *(int8_t *)addr = lua_tointeger(L, 3);
                break;
            case SIGNED16:
                *(int16_t *)addr = lua_tointeger(L, 3);
                break;
            case SIGNED32:
                *(int32_t *)addr = lua_tointeger(L, 3);
                break;
            case SIGNED64:
                *(int64_t *)addr = lua_tointeger(L, 3);
                break;
            case UNSIGNED8:
                *(uint8_t *)addr = lua_tointeger(L, 3);
                break;
            case UNSIGNED16:
                *(uint16_t *)addr = lua_tointeger(L, 3);
                break;
            case UNSIGNED32:
                *(uint32_t *)addr = lua_tointeger(L, 3);
                break;
            case UNSIGNED64:
                *(uint64_t *)addr = lua_tointeger(L, 3);
                break;
        }
    }
    return 0;
}

// Lua State Initialization

void headcrab_init_lua_platform() {
    lua_pushstring(HEADCRAB, "platform");
    lua_createtable(HEADCRAB, 0, 2);

    // hc.platform.bits
    lua_pushstring(HEADCRAB, "bits");
    lua_pushinteger(HEADCRAB, BITS);
    lua_settable(HEADCRAB, -3);

    // hc.platform.os
    lua_pushstring(HEADCRAB, "os");
    lua_pushstring(HEADCRAB, OS);
    lua_settable(HEADCRAB, -3);

    lua_settable(HEADCRAB, -3);
}

void headcrab_init_lua_process() {
    lua_pushstring(HEADCRAB, "process");
    lua_createtable(HEADCRAB, 0, 1);

    // TODO: Process arguments

    // TODO: Process name

    // hc.process.id
    lua_pushstring(HEADCRAB, "id");
    #ifdef _WIN32
    lua_pushinteger(HEADCRAB, GetCurrentProcessId());
    #else
    lua_pushinteger(HEADCRAB, getpid());
    #endif
    lua_settable(HEADCRAB, -3);

    lua_settable(HEADCRAB, -3);
}

void headcrab_init_lua_type() {
    lua_pushstring(HEADCRAB, "type");
    lua_createtable(HEADCRAB, 0, 10);

    // hc.type.signed8
    lua_pushstring(HEADCRAB, "i8");
    lua_pushinteger(HEADCRAB, SIGNED8);
    lua_settable(HEADCRAB, -3);
    // hc.type.unsigned8
    lua_pushstring(HEADCRAB, "u8");
    lua_pushinteger(HEADCRAB, UNSIGNED8);
    lua_settable(HEADCRAB, -3);
    // hc.type.signed16
    lua_pushstring(HEADCRAB, "i16");
    lua_pushinteger(HEADCRAB, SIGNED16);
    lua_settable(HEADCRAB, -3);
    // hc.type.unsigned16
    lua_pushstring(HEADCRAB, "u16");
    lua_pushinteger(HEADCRAB, UNSIGNED16);
    lua_settable(HEADCRAB, -3);
    // hc.type.signed32
    lua_pushstring(HEADCRAB, "i32");
    lua_pushinteger(HEADCRAB, SIGNED32);
    lua_settable(HEADCRAB, -3);
    // hc.type.unsigned32
    lua_pushstring(HEADCRAB, "u32");
    lua_pushinteger(HEADCRAB, UNSIGNED32);
    lua_settable(HEADCRAB, -3);
    // hc.type.signed64
    lua_pushstring(HEADCRAB, "i64");
    lua_pushinteger(HEADCRAB, SIGNED64);
    lua_settable(HEADCRAB, -3);
    // hc.type.unsigned64
    lua_pushstring(HEADCRAB, "u64");
    lua_pushinteger(HEADCRAB, UNSIGNED64);
    lua_settable(HEADCRAB, -3);
    // hc.type.float32
    lua_pushstring(HEADCRAB, "f32");
    lua_pushinteger(HEADCRAB, FLOAT32);
    lua_settable(HEADCRAB, -3);
    // hc.type.float64
    lua_pushstring(HEADCRAB, "f64");
    lua_pushinteger(HEADCRAB, FLOAT64);
    lua_settable(HEADCRAB, -3);

    lua_settable(HEADCRAB, -3);
}

void headcrab_init_lua_version() {
    lua_pushstring(HEADCRAB, "version");
    lua_createtable(HEADCRAB, 0, 3);

    // hc.version.major
    // FORKS SHOULD NOT CHANGE THE MAJOR/MINOR VERSION! The major/minor version
    // here is used by scripts to determine whether they're compatible with the
    // base headcrab library. For the sake of compatibility, forks should
    // keep this intact and change the "mod" table based on the comment below.
    lua_pushstring(HEADCRAB, "major");
    lua_pushinteger(HEADCRAB, VERSION_MAJOR);
    lua_settable(HEADCRAB, -3);

    // hc.version.minor
    lua_pushstring(HEADCRAB, "minor");
    lua_pushinteger(HEADCRAB, VERSION_MINOR);
    lua_settable(HEADCRAB, -3);

    // hc.version.mod
    // This should be used by any forks of the software to indicate what
    // modifications have been made to the interpreter.
    //
    // Example: hc.version.mod.modname = {major = 1, minor = 0}
    lua_pushstring(HEADCRAB, "mod");
    lua_createtable(HEADCRAB, 0, 0);
    lua_settable(HEADCRAB, -3);

    lua_settable(HEADCRAB, -3);
}

void headcrab_init_lua() {
    // Create the headcrab table
    lua_createtable(HEADCRAB, 0, 6);

    // hc.peek
    lua_pushstring(HEADCRAB, "peek");
    lua_pushcfunction(HEADCRAB, headcrab_peek);
    lua_settable(HEADCRAB, -3);
    // hc.poke
    lua_pushstring(HEADCRAB, "poke");
    lua_pushcfunction(HEADCRAB, headcrab_poke);
    lua_settable(HEADCRAB, -3);

    // Initialize the APIs
    headcrab_init_lua_platform();
    headcrab_init_lua_process();
    headcrab_init_lua_type();
    headcrab_init_lua_version();

    // Wrap everything up and make it accessible
    lua_setglobal(HEADCRAB, "hc");
}

// Entry point

#ifdef _WIN32
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    switch(reason) {
        case DLL_PROCESS_ATTACH:
            headcrab_init();
            break;
        case DLL_PROCESS_DETACH:
            headcrab_fini();
            break;
    }
}
#endif

#ifndef _WIN32
__attribute__((constructor))
#endif
void headcrab_init() {
    // Create the Lua state
    HEADCRAB = luaL_newstate();
    luaL_openlibs(HEADCRAB);
    // Load the headcrab library
    headcrab_init_lua();
    // Execute the Lua script
    if(luaL_dofile(HEADCRAB, SCRIPT_PATH) != LUA_OK) {
        luaL_error(HEADCRAB, "%s", lua_tostring(HEADCRAB, -1));
    }
}

#ifndef _WIN32
__attribute__((destructor))
#endif
void headcrab_fini() {
    // Close the Lua state nicely
    lua_close(HEADCRAB);
}