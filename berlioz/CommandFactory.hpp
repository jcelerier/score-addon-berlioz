#pragma once
#include <score/command/Command.hpp>

namespace berlioz
{
inline const CommandGroupKey& CommandFactoryName()
{
    static const CommandGroupKey key{"berlioz"};
    return key;
}
}
