function CorTest()
    print("coroutine begin")
    local re = coroutine.yield(100, "hello c++") -- ѹ������������ջ��  ������˳����ջ  ����-2��100 -1��"hello c++" 
    print("coroutine continue: re = " .. re) -- c++ resumeѹ��ջ��ָʾ�ж��ٲ���  �ͻ��Զ�pop������re
    local re2 = Stop("call c yield!") -- c������yeild, �´η��ص����� ret�� lua_resume���ݵ�ֵ
    print("coroutine continue after yield re2 = "..tostring(re2))
    print("coroutine end")
	return "LuaEndStr"
end