//
// Created by dsklyadnev on 5/28/15.
//

#include "ServerApplication.h"
#include "MqttListener.h"

#include <vector>

ServerApplication::ServerApplication()
    : Poco::Util::ServerApplication()
{
    addSubsystem(new MqttListener);
}

int ServerApplication::main(const std::vector <std::string> &args)
{
    waitForTerminationRequest();
    return 0;
}
