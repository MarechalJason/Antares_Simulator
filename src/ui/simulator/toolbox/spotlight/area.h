// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_SPOTLIGHT_AREA_H__
#define __ANTARES_TOOLBOX_SPOTLIGHT_AREA_H__

#include <memory>
#include <ui/common/component/spotlight.h>

#include <yuni/yuni.h>

#include <antares/study/study.h>

namespace Antares::Toolbox::Spotlight
{
class ItemArea: public Component::Spotlight::IItem
{
public:
    using Ptr = std::shared_ptr<ItemArea>;

    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    explicit ItemArea(Data::Area* a);
    //! Destructor
    virtual ~ItemArea();
    //@}

public:
    Data::Area* area;
};

} // namespace Antares::Toolbox::Spotlight

#endif // __ANTARES_TOOLBOX_SPOTLIGHT_AREA_H__
