// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "../../../windows/notes.h"

#include "../main.h"

using namespace Yuni;

namespace Antares::Forms
{
void ApplWnd::createNBNotes()
{
    assert(pNotebook);

    pUserNotes = new Window::Notes(pNotebook);
    pNotebook->add(pUserNotes, wxT("notes"), wxT("User's Notes"));
}

} // namespace Antares::Forms
