#include "demo_select.h" 


extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <stdio.h>


static int Stop(lua_State* L);
lua_State* CreateCoroutine(lua_State* gL, const char* corName);

#ifdef TEST_LUA_TO_C_TO_LUA  
int main(int argc, char* const argv[])
#else 
int test3(int argc, char* const argv[])
#endif 
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);


	luaL_dofile(L, "corTest.lua");
	lua_State* newL = CreateCoroutine(L, "CorTest"); // 把 corTest.lua 中的函数 CorTest 放到新的协程栈
	lua_register(newL, "Stop", Stop);// pushcfunction+setglobal 把C函数设置到这个协程上
	
	 
	int re = lua_resume(newL, 0); // 0 没有带任何参数 
	if (re != LUA_YIELD) 
	{
		printf("ERROR 1\n");
		return 0;
	}

	// Lua层的lua_yeild传递2个参数返回给c++的lua_resume
	int rint = luaL_checknumber(newL, -2);
	const char* str = lua_tostring(newL, -1);
	printf("stack value: %d,  %s,now stack size:%d\n", rint, str, lua_gettop(newL)); // 100,  hello c++,now stack size:2


	lua_pushstring(newL, "from c++");
	printf("stack size: %d\n", lua_gettop(newL)); // 3 -- 其中两个是lua返回的参数 1个是刚刚push的 


	re = lua_resume(newL, 1); // 返回一个参数给Lua的lua_yeild
	if (re != LUA_YIELD) 
	{
		printf("ERROR 2\n");
		return 0;
	}
 
	const char* str1 = lua_tostring(newL, -1);
	printf("stack value: %s,  now stack size:%d\n", str1, lua_gettop(newL));// "StopFuncionYeildString" 1 

	int status = lua_status(newL); // coroutine.status(co)
	printf("lua_status status = %d\n", status); // suspended, running, and dead


	lua_pushstring(newL, "cFunctionYeildResultStr"); // 这个会变成c函数返回到lua层的值(压栈给到lua层)
	re = lua_resume(newL, 1); 
	if (re != LUA_OK)
	{
		printf("ERROR 3\n");
		return 0;
	}

	int status2 = lua_status(newL); // coroutine.status(co)
	printf("lua_status status2 = %d\n", status2); // suspended, running, and dead

	const char* str2 = lua_tostring(newL, -1); // lua脚本执行完后return的值 
	printf("stack value: %s,  now stack size:%d\n", str2, lua_gettop(newL)); // "LuaEndStr" 1

	lua_close(L);
	return 0;
}

class MyObj
{
private:
	int index = 0;
public:
	MyObj(int i):index(i)
	{
		printf("MyObj %p index %d\n", this, index);
	}
	~MyObj()
	{
		printf(">>>>>>>>>> ~MyObj %p index %d\n", this, index);
	}
};

static int Stop(lua_State* lp)
{
	MyObj temp(123);
	printf("C Function Stop:stack size: %d\n", lua_gettop(lp)); //  lua调用C++只传递了一个参数 所以 size=1
 
	const char* str = lua_tostring(lp, -1);
	printf("C Function Stop: argument 1 from LUA: %s\n", str); 

	lua_pushstring(lp, "StopFuncionYeildString");
	printf("Stop func stack size: %d\n", lua_gettop(lp)); // 增加一个入栈 这里是2 

	int ret = lua_yield(lp, 1);
	printf("c++ resume from lua_yield  ret = %d\n", ret);  // -1 luaJit这里会继续执行  Lua5.4这里不会执行
	return ret;
}

lua_State* CreateCoroutine(lua_State* gL, const char* corName)
{
	lua_State* newL = lua_newthread(gL);
	lua_getglobal(newL, corName);
	return newL;
}


