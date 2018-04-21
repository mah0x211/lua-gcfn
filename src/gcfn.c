/*
 *  Copyright (C) 2017 Masatoshi Teruya
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

#include <lua.h>
#include <lauxlib.h>


#define MODULE_MT "gcfn"


typedef struct {
    int ref;
    int call;
} gcfn_t;


static int disable_lua( lua_State *L )
{
    gcfn_t *fn = luaL_checkudata( L, 1, MODULE_MT );
    fn->call = 0;
    return 0;
}


static int enable_lua( lua_State *L )
{
    gcfn_t *fn = luaL_checkudata( L, 1, MODULE_MT );
    fn->call = 1;
    return 0;
}


static int rungcfn_lua( lua_State *L )
{
    lua_State *co = NULL;

    // push upvalues to stack
    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    co = lua_tothread( L, 1 );

    // run thread
    if(
#if LUA_VERSION_NUM >= 502
        lua_resume( co, L, lua_gettop( co ) - 1 )
#else
        lua_resume( co, lua_gettop( co ) - 1 )
#endif
    ){
        fprintf( stderr, "%s\n", lua_tostring( co, -1 ) );
    }

    return 0;
}


static int gc_lua( lua_State *L )
{
    gcfn_t *fn = (gcfn_t*)lua_touserdata( L, 1 );

    // call closure
    if( fn->call )
    {
        lua_rawgeti( L, LUA_REGISTRYINDEX, fn->ref );
        // call gcfn
        if( lua_pcall( L, 0, 0, 0 ) ){
            fprintf( stderr, "%s\n", lua_tostring( L, -1 ) );
        }
    }

    // release closure
    luaL_unref( L, LUA_REGISTRYINDEX, fn->ref );

    return 0;
}


static int tostring_lua( lua_State *L )
{
    lua_pushfstring( L, MODULE_MT ": %p", lua_touserdata( L, 1 ) );
    return 1;
}


static int new_lua( lua_State *L )
{
    gcfn_t *fn = NULL;
    lua_State *co = NULL;
    int ref = LUA_NOREF;

    luaL_checktype( L, 1, LUA_TFUNCTION );
    // move all argument to coroutine
    co = lua_newthread( L );
    ref = luaL_ref( L, LUA_REGISTRYINDEX );
    lua_xmove( L, co, lua_gettop( L ) );

    // add closure function
    lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
    luaL_unref( L, LUA_REGISTRYINDEX, ref );
    lua_pushcclosure( L, rungcfn_lua, 1 );
    // retain closure reference
    ref = luaL_ref( L, LUA_REGISTRYINDEX );

    fn = (gcfn_t*)lua_newuserdata( L, sizeof( gcfn_t ) );
    *fn = (gcfn_t){
        .ref = ref,
        .call = 1
    };
    luaL_getmetatable( L, MODULE_MT );
    lua_setmetatable( L, -2 );

    return 1;
}


LUALIB_API int luaopen_gcfn( lua_State *L )
{
    struct luaL_Reg mmethod[] = {
        { "__tostring", tostring_lua },
        { "__gc", gc_lua },
        { NULL, NULL }
    };
    struct luaL_Reg method[] = {
        { "enable", enable_lua },
        { "disable", disable_lua },
        { NULL, NULL }
    };
    struct luaL_Reg *ptr = mmethod;

    // create gcfn metatable
    luaL_newmetatable( L, MODULE_MT );
    // metamethods
    while( ptr->name ){
        lua_pushstring( L, ptr->name );
        lua_pushcfunction( L, ptr->func );
        lua_rawset( L, -3 );
        ptr++;
    }
    // metamethods
    ptr = method;
    lua_pushstring( L, "__index" );
    lua_newtable( L );
    while( ptr->name ){
        lua_pushstring( L, ptr->name );
        lua_pushcfunction( L, ptr->func );
        lua_rawset( L, -3 );
        ptr++;
    }
    lua_rawset( L, -3 );
    lua_pop( L, 1 );

    lua_pushcfunction( L, new_lua );

    return 1;
}
