#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "lualib.h"
#include "lauxlib.h"

void* luabox_malloc(size_t n);
void luabox_free(void* p);

void luabox_init(void);
void* luabox_register_cb(void*, size_t);

int seluaL_dostring(lua_State* L, char* s) {
    size_t len = strlen(s);
    char* sbx_s = luabox_malloc(len);
    memcpy(sbx_s, s, len);
    int ok = luaL_dostring(L, sbx_s);
    luabox_free(sbx_s);
    return ok;
}

void selua_setglobal(lua_State* L, char* s) {
    size_t len = strlen(s);
    char* sbx_s = luabox_malloc(len);
    memcpy(sbx_s, s, len);
    lua_setglobal(L, sbx_s);
    luabox_free(sbx_s);
}

int foo(lua_State* L) {
    lua_Number arg = lua_tonumber(L, -1);
    printf("foo: %d\n", (int) arg);
    return 0;
}

int main() {
    luabox_init();

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    void* sbx_foo = luabox_register_cb(&foo, 0);
    lua_pushcfunction(L, sbx_foo);
    selua_setglobal(L, "foo");

    char* line;
    while ((line = readline("luabox> ")) != NULL) {
        add_history(line);
        if (seluaL_dostring(L, line) != 0)
            fprintf(stderr, "error during evaluation");
        free(line);
    }

    lua_close(L);

    return 0;
}
