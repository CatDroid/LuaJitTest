extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include "demo_select.h"

#include <iostream>

using namespace std;

int n = 0;

int func(lua_State *L)
{
	printf("func_top=%d top=%s\n", lua_gettop(L), lua_tostring(L, -1));

	if (!n)
	{
		++n;
		lua_pushinteger(L, 123);
		int result = lua_yield(L, 1); // -1 luajit会继续执行后面的c++代码
		printf("continue running  %d\n", result);
		return result;// 应该要返回 -1 
	}
	else // luajit 第二次resume 不会执行后面的代码了, 而是会崩溃
	{
		printf("continue running branch 2 ? \n");
		return 1;
	}
}


#ifdef TEST_COROUTINE 
int main(int argc, char* const argv[])
#else 
int test1(int argc, char* const argv[])
#endif 
{

	lua_State *L = luaL_newstate();

	/* init lua library */
	lua_pushcfunction(L, luaopen_base);
	if (lua_pcall(L, 0, 0, 0) != 0)
	{
		return 1;
	}


	lua_pushcfunction(L, luaopen_package);
	if (lua_pcall(L, 0, 0, 0) != 0)
	{
		return 2;
	}

	/* create the coroutine */
	lua_State *LL = lua_newthread(L);

	lua_pushcfunction(LL, func); // 主函数以及它需要的参数压入线程栈
	lua_pushstring(LL, "hello world"); // 空栈 

	const char * str = lua_tostring(LL, -1);
	std::cout << "栈顶第一个(空栈)" << str << std::endl;    // 从栈中读取字符串
	std::cout << "栈顶第二个是否c函数 " << (lua_iscfunction(LL, -2) ? "True" : "False") << std::endl;
	std::cout << "begin top " << lua_gettop(LL) << std::endl;

	/* first time resume */
	int res = 0;
 
	// 返回 LUA_YIELD 如果协程yield 
	// 返回 0 如果协程完成 并没有错误
	// 返回 错误码 
	int status = lua_resume(LL, 1);
	if (status == LUA_YIELD)
	{
 
		printf("lua_resume res = %d, current top=%d\n", res, lua_gettop(LL)); // 2, cfunction已经退出栈

		//if (lua_isinteger(LL, -1))
		if (lua_isnumber(LL, -1))
		{
			printf("top one is %d\n", lua_tointeger(LL, -1));
		}

		if (lua_isstring(LL, -2))
		{
			printf("top two is %s\n", lua_tostring(LL, -2));
		}

		lua_pop(LL, 1);// lua层push的123弹出来  hhl?c++到c++不会自动弹栈

		printf("after lua_pop %d, current top=%d\n", res, lua_gettop(LL));

		/* twice resume */
		status = lua_resume(LL, 0); // 这里会崩溃, 因为直接resume cfunction 即使上次返回LUA_YEILD, 也会执行完cfunction的
		if (status == LUA_OK)
		{
			printf("second_top=%d top_string=%s\n", lua_gettop(LL), lua_tostring(LL, -1));
		}
	}

	lua_close(L);

	return 0;
}