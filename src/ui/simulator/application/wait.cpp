// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "wait.h"

#include <cassert>
#include <ui/common/wx-wrapper.h>

#include <yuni/core/atomic/int.h>

#include <antares/logs/logs.h>

#include "main/main.h"

namespace Antares::WIP
{
void Enter(bool immediate)
{
    Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();
    if (immediate)
    {
        mainFrm.showWIP();
    }
    else
    {
        Dispatcher::GUI::Post(&mainFrm, &Forms::ApplWnd::showWIP);
    }
}

void Leave()
{
    Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();
    Dispatcher::GUI::Post(&mainFrm, &Forms::ApplWnd::hideWIP);
}

} // namespace Antares::WIP
