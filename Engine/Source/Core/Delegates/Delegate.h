#pragma once

#include <functional>
#include "Container/Array.h"

#define FUNC_DECLARE_DELEGATE( DelegateName, ReturnType, ... ) \
    typedef TDelegate<ReturnType, __VA_ARGS__> DelegateName;

#define DECLARE_DELEGATE( DelegateName ) FUNC_DECLARE_DELEGATE( DelegateName, void )
#define DECLARE_DELEGATE_OneParam( DelegateName, Param1Type ) FUNC_DECLARE_DELEGATE( DelegateName, void, Param1Type )
// TODO: TwoParams 이상은 테스트 못해봄
#define DECLARE_DELEGATE_TwoParams( DelegateName, Param1Type, Param2Type ) FUNC_DECLARE_DELEGATE( DelegateName, void, Param1Type, Param2Type )
#define DECLARE_DELEGATE_ThreeParams( DelegateName, Param1Type, Param2Type, Param3Type ) FUNC_DECLARE_DELEGATE( DelegateName, void, Param1Type, Param2Type, Param3Type )
#define DECLARE_DELEGATE_FourParams( DelegateName, Param1Type, Param2Type, Param3Type, Param4Type ) FUNC_DECLARE_DELEGATE( DelegateName, void, Param1Type, Param2Type, Param3Type, Param4Type )
#define DECLARE_DELEGATE_FiveParams( DelegateName, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type ) FUNC_DECLARE_DELEGATE( DelegateName, void, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type )
#define DECLARE_DELEGATE_SixParams( DelegateName, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type ) FUNC_DECLARE_DELEGATE( DelegateName, void, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type )
#define DECLARE_DELEGATE_SevenParams( DelegateName, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type, Param7Type ) FUNC_DECLARE_DELEGATE( DelegateName, void, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type, Param7Type )
#define DECLARE_DELEGATE_EightParams( DelegateName, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type, Param7Type, Param8Type ) FUNC_DECLARE_DELEGATE( DelegateName, void, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type, Param7Type, Param8Type )
#define DECLARE_DELEGATE_NineParams( DelegateName, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type, Param7Type, Param8Type, Param9Type ) FUNC_DECLARE_DELEGATE( DelegateName, void, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type, Param7Type, Param8Type, Param9Type )


template<typename InRetValType, typename... ParamTypes>
class TDelegate
{
    using Callback = std::function<void(ParamTypes...)>;
    using CallbackHandle = uint32;

    struct FCallbackEntry
    {
        CallbackHandle Handle;
        Callback Func;
        void* Object;
    };

public:
    CallbackHandle BindLambda(void* Object, Callback func)
    {
        callbacks.Add({NextHandle, func, Object});
        return NextHandle++;
    }

    template<typename T, typename... InParamTypes>
    CallbackHandle Bind(T* Object, void (T::*Func)(InParamTypes...))
    {
         return BindLambda(
             Object,
            [Object, Func](InParamTypes... args)
            {
                (Object->*Func)(args...);
            }
        );
    }

    void Broadcast(ParamTypes... args)
    {
        for (FCallbackEntry& callback : callbacks)
        {
            callback.Func(args...);
        }
    }

    void UnbindAllByObject(void* Object)
    {
        callbacks.RemoveAll(
            [Object](const FCallbackEntry& entry)
            {
                return entry.Object == Object;
            }
        );
    }

    void Unbind(CallbackHandle Handle)
    {
        callbacks.RemoveAll(
            [Handle](const FCallbackEntry& entry)
            {
                return entry.Handle == Handle;
            }
        );
    }

    void UnbindAll()
    {
        callbacks.Empty();
        NextHandle = 0;
    }

private:
    TArray<FCallbackEntry> callbacks;
    CallbackHandle NextHandle = 0;
};
