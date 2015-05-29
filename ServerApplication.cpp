//
// Created by dsklyadnev on 5/28/15.
//

#include "ServerApplication.h"
#include <vector>

ServerApplication::ServerApplication()
    : Poco::Util::ServerApplication()
{
}

int ServerApplication::main(const std::vector <std::string> &args)
{
    return 1;
}
