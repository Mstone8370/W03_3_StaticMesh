#pragma once

#include <functional>
#include "Container/Array.h"

#define FUNC_DECLARE_DELEGATE( DelegateName, ReturnType, ... ) \
    typedef TDelegate<ReturnType, __VA_ARGS__> DelegateName;

#define DECLARE_DELEGATE( DelegateName ) FUNC_DECLARE_DELEGATE( DelegateName, void )

#define DECLARE_DELEGATE_OneParam( DelegateName, Param1Type ) FUNC_DECLARE_DELEGATE( DelegateName, void, Param1Type )


template<typename InRetValType, typename... ParamTypes>
class TDelegate
{
public:
    using Callback = std::function<void(ParamTypes...)>;

    void BindLambda(Callback func)
    {
        callbacks.Add(func);
    }

    template<typename T, typename... InParamTypes>
    void Bind(T* Object, void (T::*Func)(InParamTypes...))
    {
        BindLambda(
            [Object, Func](InParamTypes... args)
            {
                (Object->*Func)(args...);
            }
        );
    }

    void Broadcast(ParamTypes... args)
    {
        for (auto& callback : callbacks)
        {
            callback(args...);
        }
    }

private:
    TArray<Callback> callbacks;
};
