function CorTest()
    print("coroutine begin")
    local re = coroutine.yield(100, "hello c++") -- 压入两个参数到栈中  从左到右顺序入栈  所以-2是100 -1是"hello c++" 
    print("coroutine continue: re = " .. re) -- c++ resume压入栈并指示有多少参数  就会自动pop并给到re
    local re2 = Stop("call c yield!") -- c函数会yeild, 下次返回到这里 ret是 lua_resume传递的值
    print("coroutine continue after yield re2 = "..tostring(re2))
    print("coroutine end")
	return "LuaEndStr"
end