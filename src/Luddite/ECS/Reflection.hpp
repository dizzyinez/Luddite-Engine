#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/MetaInfo.hpp"
#include "Luddite/Core/map.h"

#define REM_PRIMITIVE(...) __VA_ARGS__
#define EAT_PRIMITIVE(...)
#define EAT(x) EAT_PRIMITIVE x
#define REM(x) REM_PRIMITIVE x
#define TYPEOF(x) DETAIL_TYPEOF(DETAIL_TYPEOF_PROBE x, )
#define DETAIL_TYPEOF(...) DETAIL_TYPEOF_HEAD(__VA_ARGS__)
#define DETAIL_TYPEOF_HEAD(x, ...) REM_PRIMITIVE x
#define DETAIL_TYPEOF_PROBE(...) (__VA_ARGS__),

#define COMMA_PRIMITIVE(x) , x
#define COMMA(x) COMMA_PRIMITIVE EAT_PRIMITIVE() (x)
#define TAIL_PRIMITIVE(x, ...) __VA_ARGS__
#define TAIL(...) TAIL_PRIMITIVE(__VA_ARGS__)

#define STRINGIZE_PRIMITIVE(x) #x
#define STRINGIZE(x) STRINGIZE_PRIMITIVE(x)

#define MEMBER_DEFINE_PRIMITIVE(v, i, ...) REM(v) = REM(i);
#define MEMBER_DEFINE_INNER(...) MEMBER_DEFINE_PRIMITIVE(__VA_ARGS__)
#define MEMBER_DEFINE(v) MEMBER_DEFINE_INNER(REM(v))

#define MEMBER_META_TYPES_PRIMITIVE(v, i, ...)
#define MEMBER_META_TYPES_INNER(...) MEMBER_META_TYPES_PRIMITIVE(__VA_ARGS__)
#define MEMBER_META_TYPES(v) MEMBER_META_TYPES_INNER(REM(v))
#define GET_META_TYPES(...) TAIL(throwaway MAP(MEMBER_META_TYPES, __VA_ARGS__))

#define MEMBER_META_DEFINE_PRIMITIVE(v, i, ...) inline static const MetaTypeInfo<TYPEOF(v)> EAT(v) {.name = STRINGIZE(EAT(v)), __VA_ARGS__};
#define MEMBER_META_DEFINE_INNER(...) MEMBER_META_DEFINE_PRIMITIVE(__VA_ARGS__)
#define MEMBER_META_DEFINE(v) MEMBER_META_DEFINE_INNER(REM(v))

#define MEMBER_META_APPLY_PRIMITIVE(v, i, ...) f(EAT(v), c.EAT(v));
#define MEMBER_META_APPLY_INNER(...) MEMBER_META_APPLY_PRIMITIVE(__VA_ARGS__)
#define MEMBER_META_APPLY(v) MEMBER_META_APPLY_INNER(REM(v))

#define LD_COMPONENT_DEFINE(Name, Internal, ...) \
        struct Name { \
                REM(Internal) \
                MAP(MEMBER_DEFINE, __VA_ARGS__) \
                struct Meta { \
                        MAP(MEMBER_META_DEFINE, __VA_ARGS__) \
                        template <typename F> \
                        static void apply(const struct Name& c, F && f) { \
                                MAP(MEMBER_META_APPLY, __VA_ARGS__) \
                        } \
                        template <typename F> \
                        static void apply(struct Name & c, F && f) { \
                                MAP(MEMBER_META_APPLY, __VA_ARGS__) \
                        } \
                }; \
        };
#define LD_COMPONENT_REGISTER(Name, StringName, w) \
        w.component<Name>(StringName) \
        .set<Luddite::ReflectionData>({[](void* data){ \
                                               Name::Meta::apply(*reinterpret_cast<Name*>(data), [](const auto& type_info, auto& value){ \
                                type_info.IMGuiElement(value); \
                        }); \
                                       }});
