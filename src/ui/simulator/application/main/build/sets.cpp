// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "../../../windows/sets.h"

#include "../main.h"

using namespace Yuni;

namespace Antares::Forms
{
void ApplWnd::createNBSets()
{
    assert(pNotebook);

    pSets = new Window::Sets(pNotebook);
    pNotebook->add(pSets, wxT("sets"), wxT(""));
}

} // namespace Antares::Forms
