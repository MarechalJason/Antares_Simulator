// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <action/behavior.h>
#include <cassert>

#include <yuni/yuni.h>

namespace Antares::Action
{
const char* BehaviorToString(Behavior behavior)
{
    static const char* const str[] = {"skip", "merge", "overwrite"};
    assert((uint)behavior < (uint)bhMax);
    return str[(uint)behavior];
}

} // namespace Antares::Action
