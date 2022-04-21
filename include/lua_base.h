#pragma once

#include <set>
#include <map>
#include <list>
#include <tuple>
#include <string>
#include <vector>
#include <utility>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <assert.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

namespace luakit {

#define MAX_LUA_OBJECT_KEY 128

    //异常处理器
    using exception_handler = std::function<void(std::string err)>;

    //定义全局函数和类函数
    using global_function = std::function<int(lua_State*)>;
    using object_function = std::function<int(void*, lua_State*)>;

    //类成员（变量、函数）包装器
    using member_wrapper = std::function<void(lua_State*, void*)>;

    //类成员元素的声
    struct class_member {
        bool is_function = false;
        member_wrapper getter = nullptr;
        member_wrapper setter = nullptr;
    };

    template<typename T>
    class class_meta
    {
    public:
        static bool is_register() {
            return m_register;
        }
        static void register_meta(const char* name) {
            if (!m_register) {
                m_meta_name.append(name).append("__");
                m_register = true;
            }
        }
        static const char* get_meta_name() {
            return m_meta_name.c_str();
        }
        static const char* get_object_key(void* obj) {
            static char objkey[MAX_LUA_OBJECT_KEY];
            snprintf(objkey, MAX_LUA_OBJECT_KEY, "%p@%s", obj, m_meta_name.c_str());
            return objkey;
        }
    private:
        static bool m_register;
        static std::string m_meta_name;
    };

    template<typename T>
    bool class_meta<T>::m_register = false;
    template<typename T>
    std::string class_meta<T>::m_meta_name = "__class_meta_";

    class lua_guard {
    public:
        lua_guard(lua_State* L) : m_L(L) { m_top = lua_gettop(L); }
        ~lua_guard() { lua_settop(m_L, m_top); }
        lua_guard(const lua_guard& other) = delete;
        lua_guard(lua_guard&& other) = delete;
        lua_guard& operator =(const lua_guard&) = delete;
    private:
        int m_top = 0;
        lua_State* m_L = nullptr;
    };

}
