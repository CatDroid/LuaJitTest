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
	lua_State* newL = CreateCoroutine(L, "CorTest"); // �� corTest.lua �еĺ��� CorTest �ŵ��µ�Э��ջ
	lua_register(newL, "Stop", Stop);// pushcfunction+setglobal ��C�������õ����Э����
	
	 
	int re = lua_resume(newL, 0); // 0 û�д��κβ��� 
	if (re != LUA_YIELD) 
	{
		printf("ERROR 1\n");
		return 0;
	}

	// Lua���lua_yeild����2���������ظ�c++��lua_resume
	int rint = luaL_checknumber(newL, -2);
	const char* str = lua_tostring(newL, -1);
	printf("stack value: %d,  %s,now stack size:%d\n", rint, str, lua_gettop(newL)); // 100,  hello c++,now stack size:2


	lua_pushstring(newL, "from c++");
	printf("stack size: %d\n", lua_gettop(newL)); // 3 -- ����������lua���صĲ��� 1���Ǹո�push�� 


	re = lua_resume(newL, 1); // ����һ��������Lua��lua_yeild
	if (re != LUA_YIELD) 
	{
		printf("ERROR 2\n");
		return 0;
	}
 
	const char* str1 = lua_tostring(newL, -1);
	printf("stack value: %s,  now stack size:%d\n", str1, lua_gettop(newL));// "StopFuncionYeildString" 1 

	int status = lua_status(newL); // coroutine.status(co)
	printf("lua_status status = %d\n", status); // suspended, running, and dead


	lua_pushstring(newL, "cFunctionYeildResultStr"); // �������c�������ص�lua���ֵ(ѹջ����lua��)
	re = lua_resume(newL, 1); 
	if (re != LUA_OK)
	{
		printf("ERROR 3\n");
		return 0;
	}

	int status2 = lua_status(newL); // coroutine.status(co)
	printf("lua_status status2 = %d\n", status2); // suspended, running, and dead

	const char* str2 = lua_tostring(newL, -1); // lua�ű�ִ�����return��ֵ 
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
	printf("C Function Stop:stack size: %d\n", lua_gettop(lp)); //  lua����C++ֻ������һ������ ���� size=1
 
	const char* str = lua_tostring(lp, -1);
	printf("C Function Stop: argument 1 from LUA: %s\n", str); 

	lua_pushstring(lp, "StopFuncionYeildString");
	printf("Stop func stack size: %d\n", lua_gettop(lp)); // ����һ����ջ ������2 

	int ret = lua_yield(lp, 1);
	printf("c++ resume from lua_yield  ret = %d\n", ret);  // -1 luaJit��������ִ��  Lua5.4���ﲻ��ִ��
	return ret;
}

lua_State* CreateCoroutine(lua_State* gL, const char* corName)
{
	lua_State* newL = lua_newthread(gL);
	lua_getglobal(newL, corName);
	return newL;
}


