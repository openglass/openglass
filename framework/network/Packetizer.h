#ifndef PACKETIZER_H
#define PACKETIZER_H
#include <errno.h>
#include <string.h>

#include <iostream>
#include <deque>
#include <list>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/signal.hpp>

#include "common/Logger.h"
#include "common/CommonDefs.h"

typedef std::deque< boost::uint8_t > Buffer;

namespace Fog {
namespace Network {
/**
* @brief This class acts as a classical counted packetizer. The length of
* data to be read is provided in the header. The header is read first and
* then the remaining data gleaned from information in header
*/
class CPacketizer
{
public:
    CPacketizer(PropertiesPtr properties );
    ~CPacketizer() { }

    virtual bool initialize(PropertiesPtr properties);

    /**
     * @brief gets emitted when data is read from the socket 
     * and packetized.
     */
    boost::signal< void (char* data, size_t length) > OnPacket;

    /**
     * @brief will be used to bind to connection 
     *
     * @param buffer
     * @param bufSize
     */
    void operator() (char* buffer, size_t bufSize);

protected:
    Buffer buffer_;
    int32_t hdrLength_;
};

} // Network
} // Fog

#endif  // PACKETIZER_H
