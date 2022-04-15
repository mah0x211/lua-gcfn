/**
 *  Copyright (C) 2017-2022 Masatoshi Fukunaga
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  gcfn.c
 *  lua-gcfn
 *  Created by Masatoshi Teruya on 17/10/13.
 */

#include <lauxlib.h>
#include <lua.h>

#define MODULE_MT "gcfn"

static int REF_DEBUG_TRACEBACK = LUA_NOREF;

typedef struct {
    int ref;
    int call;
} gcfn_t;

static int disable_lua(lua_State *L)
{
    gcfn_t *fn = luaL_checkudata(L, 1, MODULE_MT);
    fn->call   = 0;
    return 0;
}

static int enable_lua(lua_State *L)
{
    gcfn_t *fn = luaL_checkudata(L, 1, MODULE_MT);
    fn->call   = 1;
    return 0;
}

static int rungcfn_lua(lua_State *L)
{
    int farg = 0;

    // push arguments
    lua_pushvalue(L, lua_upvalueindex(1));
    farg = lua_tointeger(L, -1);
    lua_pop(L, 1);
    for (int i = 1; i <= farg; i++) {
        lua_pushvalue(L, lua_upvalueindex(i + 1));
        lua_insert(L, i);
    }

    lua_call(L, lua_gettop(L) - 1, 0);

    return 0;
}

static int gc_lua(lua_State *L)
{
    gcfn_t *fn = (gcfn_t *)lua_touserdata(L, 1);

    // call closure
    if (fn->call) {
        int errfn = 0;

        if (REF_DEBUG_TRACEBACK != LUA_NOREF) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, REF_DEBUG_TRACEBACK);
            errfn = lua_gettop(L);
        }

        // call gcfn
        lua_rawgeti(L, LUA_REGISTRYINDEX, fn->ref);
        if (lua_pcall(L, 0, 0, errfn)) {
            fprintf(stderr, "%s\n", lua_tostring(L, -1));
        }
    }

    // release closure
    luaL_unref(L, LUA_REGISTRYINDEX, fn->ref);

    return 0;
}

static int tostring_lua(lua_State *L)
{
    lua_pushfstring(L, MODULE_MT ": %p", lua_touserdata(L, 1));
    return 1;
}

static int new_lua(lua_State *L)
{
    gcfn_t *fn = NULL;
    int ref    = LUA_NOREF;

    luaL_checktype(L, 1, LUA_TFUNCTION);
    // create closure function
    lua_pushinteger(L, lua_gettop(L));
    lua_insert(L, 1);
    lua_pushcclosure(L, rungcfn_lua, lua_gettop(L));
    ref = luaL_ref(L, LUA_REGISTRYINDEX);

    fn = (gcfn_t *)lua_newuserdata(L, sizeof(gcfn_t));
    luaL_getmetatable(L, MODULE_MT);
    lua_setmetatable(L, -2);
    fn->ref     = ref;
    fn->call    = 1;

    return 1;
}

LUALIB_API int luaopen_gcfn(lua_State *L)
{
    struct luaL_Reg mmethod[] = {
        {"__tostring", tostring_lua},
        {"__gc",       gc_lua      },
        {NULL,         NULL        }
    };
    struct luaL_Reg method[] = {
        {"enable",  enable_lua },
        {"disable", disable_lua},
        {NULL,      NULL       }
    };
    struct luaL_Reg *ptr = mmethod;

    // create gcfn metatable
    luaL_newmetatable(L, MODULE_MT);
    // metamethods
    while (ptr->name) {
        lua_pushstring(L, ptr->name);
        lua_pushcfunction(L, ptr->func);
        lua_rawset(L, -3);
        ptr++;
    }
    // metamethods
    ptr = method;
    lua_pushstring(L, "__index");
    lua_newtable(L);
    while (ptr->name) {
        lua_pushstring(L, ptr->name);
        lua_pushcfunction(L, ptr->func);
        lua_rawset(L, -3);
        ptr++;
    }
    lua_rawset(L, -3);
    lua_pop(L, 1);

    // get debug.traceback function
    lua_getglobal(L, "debug");
    if (lua_type(L, -1) == LUA_TTABLE) {
        lua_getfield(L, -1, "traceback");
        if (lua_type(L, -1) == LUA_TFUNCTION) {
            REF_DEBUG_TRACEBACK = luaL_ref(L, LUA_REGISTRYINDEX);
        }
    }

    lua_pushcfunction(L, new_lua);

    return 1;
}
