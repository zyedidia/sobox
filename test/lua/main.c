#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "lualib.h"
#include "lauxlib.h"

void* liblua_malloc(size_t n);
void liblua_free(void* p);

void liblua_init(void);

int seluaL_dostring(lua_State* L, char* s) {
    size_t len = strlen(s);
    char* sbx_s = liblua_malloc(len);
    memcpy(sbx_s, s, len);
    int ok = luaL_dostring(L, sbx_s);
    liblua_free(sbx_s);
    return ok;
}

int main() {
    liblua_init();

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    seluaL_dostring(L, "print('hello world')");

    lua_close(L);

    return 0;
}
