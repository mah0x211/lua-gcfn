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
    int narg;
} gcfn_t;


static int rungcfn_lua( lua_State *L )
{
    int narg = lua_tointeger( L, 1 );
    int i = 1;

    // push upvalues
    for(; i <= narg; i++ ){
        lua_pushvalue( L, lua_upvalueindex( i ) );
    }

    // call gcfn
    printf("call gcfn\n");
    switch( lua_pcall( L, narg - 1, 0, 0 ) ){
        case 0:
        break;

        default:
            printf( "%s\n", lua_tostring( L, -1 ) );
    }

    return 0;
}


static int gc_lua( lua_State *L )
{
    gcfn_t *fn = (gcfn_t*)lua_touserdata( L, 1 );

    // release closure
    lua_rawgeti( L, LUA_REGISTRYINDEX, fn->ref );
    luaL_unref( L, LUA_REGISTRYINDEX, fn->ref );
    // run closure
    lua_pushinteger( L, fn->narg );
    lua_call( L, 1, 0 );

    return 0;
}


static int new_lua( lua_State *L )
{
    int narg = lua_gettop( L );
    gcfn_t *fn = NULL;

    luaL_checktype( L, 1, LUA_TFUNCTION );
    lua_pushcclosure( L, rungcfn_lua, narg );

    // retain closure reference
    fn = (gcfn_t*)lua_newuserdata( L, sizeof( gcfn_t ) );
    lua_pushvalue( L, -2 );
    fn->ref = luaL_ref( L, LUA_REGISTRYINDEX );
    fn->narg = narg;

    luaL_getmetatable( L, MODULE_MT );
    lua_setmetatable( L, -2 );

    return 1;
}


LUALIB_API int luaopen_gcfn( lua_State *L )
{
    // create gcfn metatable
    luaL_newmetatable( L, MODULE_MT );
    lua_pushstring( L, "__gc" );
    lua_pushcfunction( L, gc_lua );
    lua_rawset( L, -3 );
    lua_pop( L, 1 );

    lua_pushcfunction( L, new_lua );

    return 1;
}
