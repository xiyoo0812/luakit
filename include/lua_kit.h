#pragma once
#include "lua_table.h"
#include "lua_class.h"

namespace luakit {

    class kit_state {
    public:
        kit_state() {
            m_L = luaL_newstate();
            luaL_openlibs(m_L);
        }
        kit_state(lua_State* L) : m_L(L) {}

        template<typename T>
        void set(const char* name, T obj) {
            native_to_lua(m_L, obj);
            lua_setglobal(m_L, name);
        }

        template<typename T>
        T get(const char* name) {
            lua_guard g(m_L);
            lua_getglobal(m_L, name);
            return lua_to_native<T>(m_L, -1);
        }

        template <typename F>
        void set_function(const char* function, F func) {
            lua_push_function(m_L, func);
            lua_setglobal(m_L, function);
        }

        template <typename... ret_types, typename... arg_types>
        bool call(const char* function, exception_handler handler, std::tuple<ret_types&...>&& rets, arg_types... args) {
            return call_global_function(m_L, function, handler, std::forward<std::tuple<ret_types&...>>(rets), std::forward<arg_types>(args)...);
        }

        bool call(const char* function, exception_handler handler = nullptr) {
            return call(function, handler, std::tie());
        }

        bool run_file(const char* filename, exception_handler handler = nullptr) {
            lua_guard g(m_L);
            if (luaL_loadfile(m_L, filename) || lua_pcall(m_L, 0, 0, -1)) {
                if (handler) {
                    handler(lua_tostring(m_L, -1));
                }
                return false;
            }
            return true;
        }

        bool run_script(const char* script, exception_handler handler = nullptr) {
            lua_guard g(m_L);
            if (luaL_loadstring(m_L, script) || lua_pcall(m_L, 0, 0, -1)) {
                if (handler) {
                    handler(lua_tostring(m_L, -1));
                }
                return false;
            }
            return true;
        }

        lua_table new_table(const char* name = nullptr) {
            lua_guard g(m_L);
            lua_newtable(m_L);
            if (name) {
                lua_pushvalue(m_L, -1);
                lua_setglobal(m_L, name);
            }
            return lua_table(m_L);
        }

        template <typename... arg_types>
        lua_table new_table(const char* name, arg_types... args) {
            lua_table table = new_table(name);
            table.create_with(std::forward<arg_types>(args)...);
            return table;
        }

        template <typename... enum_value>
        lua_table new_enum(const char* name, enum_value... args) {
            lua_table table = new_table(name);
            table.create_with(std::forward<enum_value>(args)...);
            return table;
        }

        template<typename T, typename... arg_types>
        void new_class(arg_types... args) {
            lua_wrap_class<T>(m_L, std::forward<arg_types>(args)...);
        }

        template <typename T>
        reference new_reference(T v) {
            lua_guard g(m_L);
            native_to_lua(m_L, v);
            return reference(m_L);;
        }

        template <typename sequence_type, typename T>
        reference new_reference(sequence_type v) {
            lua_guard g(m_L);
            lua_new_reference<sequence_type, T>(m_L, v);
            return reference(m_L);
        }

        template <typename associate_type, typename T, typename V>
        reference new_reference(associate_type v) {
            lua_guard g(m_L);
            lua_new_reference<associate_type, K, V>(m_L, v);
            return reference(m_L);;
        }

    protected:
        lua_State* m_L = nullptr;
    };

}