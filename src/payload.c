#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define VERSION_MAJOR 0
#define VERSION_MINOR 1

// Lua state

static lua_State *HEADCRAB;

// Data from injector

static char SCRIPT_PATH[4096] = {0, 0xB3, 0xDC, 0xF5}; // I'd really like to know if people use paths longer than this

// Native Lua Library

// ...

void headcrab_init_lua() {
    // Create the headcrab table
    lua_createtable(HEADCRAB, 0, 1);
    // Store the version number
    lua_pushstring(HEADCRAB, "version");
    lua_createtable(HEADCRAB, 0, 3);
    // FORKS SHOULD NOT CHANGE THE MAJOR/MINOR VERSION! The major/minor version
    // here is used by scripts to determine whether they're compatible with the
    // base headcrab library. For the sake of compatibility, forks should
    // keep this intact and change the "mod" table based on the comment below.
    lua_pushstring(HEADCRAB, "major");
    lua_pushinteger(HEADCRAB, VERSION_MAJOR);
    lua_settable(HEADCRAB, -3);
    lua_pushstring(HEADCRAB, "minor");
    lua_pushinteger(HEADCRAB, VERSION_MINOR);
    lua_settable(HEADCRAB, -3);
    // This should be used by any forks of the software to indicate what
    // modifications have been made to the interpreter.
    //
    // Example: hc.version.mod.modname = {major = 1, minor = 0}
    lua_pushstring(HEADCRAB, "mod");
    lua_createtable(HEADCRAB, 0, 0);
    lua_settable(HEADCRAB, -3);
    lua_settable(HEADCRAB, -3);
    // ...
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