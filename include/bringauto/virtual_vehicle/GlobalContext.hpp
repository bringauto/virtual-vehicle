#pragma once

#include <boost/asio.hpp>

namespace bringauto::virtual_vehicle {
    struct GlobalContext {
        boost::asio::io_context ioContext;
        bool cruise{false};
        double stopWaitSeconds{10.0};
    };
}