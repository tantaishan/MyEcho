/*
 * Copyright 2002, Log4cpp Project. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PassThroughLayout.hh"
#include "FactoryParams.hh"
#include <memory>

namespace log4cpp
{
   std::auto_ptr<Layout> create_pass_through_layout(const FactoryParams& params)
   {
      return std::auto_ptr<Layout>(new PassThroughLayout);
   }
}
