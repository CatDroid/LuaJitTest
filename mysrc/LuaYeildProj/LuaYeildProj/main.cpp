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
		int result = lua_yield(L, 1); // -1 luajit�����ִ�к����c++����
		printf("continue running  %d\n", result);
		return result;// Ӧ��Ҫ���� -1 
	}
	else // luajit �ڶ���resume ����ִ�к���Ĵ�����, ���ǻ����
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

	lua_pushcfunction(LL, func); // �������Լ�����Ҫ�Ĳ���ѹ���߳�ջ
	lua_pushstring(LL, "hello world"); // ��ջ 

	const char * str = lua_tostring(LL, -1);
	std::cout << "ջ����һ��(��ջ)" << str << std::endl;    // ��ջ�ж�ȡ�ַ���
	std::cout << "ջ���ڶ����Ƿ�c���� " << (lua_iscfunction(LL, -2) ? "True" : "False") << std::endl;
	std::cout << "begin top " << lua_gettop(LL) << std::endl;

	/* first time resume */
	int res = 0;
 
	// ���� LUA_YIELD ���Э��yield 
	// ���� 0 ���Э����� ��û�д���
	// ���� ������ 
	int status = lua_resume(LL, 1);
	if (status == LUA_YIELD)
	{
 
		printf("lua_resume res = %d, current top=%d\n", res, lua_gettop(LL)); // 2, cfunction�Ѿ��˳�ջ

		//if (lua_isinteger(LL, -1))
		if (lua_isnumber(LL, -1))
		{
			printf("top one is %d\n", lua_tointeger(LL, -1));
		}

		if (lua_isstring(LL, -2))
		{
			printf("top two is %s\n", lua_tostring(LL, -2));
		}

		lua_pop(LL, 1);// lua��push��123������  hhl?c++��c++�����Զ���ջ

		printf("after lua_pop %d, current top=%d\n", res, lua_gettop(LL));

		/* twice resume */
		status = lua_resume(LL, 0); // ��������, ��Ϊֱ��resume cfunction ��ʹ�ϴη���LUA_YEILD, Ҳ��ִ����cfunction��
		if (status == LUA_OK)
		{
			printf("second_top=%d top_string=%s\n", lua_gettop(LL), lua_tostring(LL, -1));
		}
	}

	lua_close(L);

	return 0;
}