#include "Packetizer.h"

namespace Fog {
namespace Network {

    CPacketizer::CPacketizer(PropertiesPtr properties ):
    hdrLength_(0)
    {
        if (!initialize(properties))
            throw std::runtime_error("Unable to initailize CPacketizer");
    }

    bool CPacketizer::initialize(PropertiesPtr properties)
    {
        Properties::iterator ret;
        ret=properties->find(std::string("hdrLength"));
        if (ret == properties->end())
        {
            LOG_ERROR("hdrLength not present in config file");
            return false;
        }
        else
        {
            int* hdr=boost::any_cast<int>(&(ret->second));
            if (hdr == NULL)
            {
                LOG_ERROR("hdrLength not an integer value");
            return false;
            }

            hdrLength_=*hdr;
        }

#if 0
        ret=(*serverDetails)->find(std::string("bodyLength"));
        if (ret == (*serverDetails)->end())
        {
            LOG_ERROR("bodyLength not present in config file");
            return false;
        }
        else
        {
            int* bodyLen=boost::any_cast<int>(&(ret->second));
            if (bodyLen == NULL)
            {
                LOG_ERROR("bodyLength not an integer value");
                return false;
            }

            bodyLength_=*bodyLen;
        }
#endif

        std::ostringstream tmp;
        tmp << "Cpacketize initialized with header length " << hdrLength_;
        LOG_DEBUG(tmp.str());
        return true;
    }

    void CPacketizer::operator() (char* buffer, size_t bufSize)
    {
        // copy over the input buffer into existing buffer.
        std::copy( (const boost::uint8_t *)buffer, 
                (const boost::uint8_t *)buffer + bufSize, 
                std::back_inserter(buffer_) );

        int32_t bytesProcessed=0;
        int32_t bytesRemaining=buffer_.size();

        // process the block
        while ( bytesRemaining > 0 )
        {
            if (bytesRemaining < hdrLength_)
            {
                // get more data
                return;
            }

            char hdr[hdrLength_+1];
            memset(hdr, 0, sizeof(hdr));

            // copy first headerlength from buffer_ to character array
            for (int32_t i=0; i<hdrLength_; i++)
            {
                hdr[i]=buffer_[i];
            }

            int32_t bodyLen=0;
            int32_t n=sscanf(hdr, "%d", &bodyLen);
            std::ostringstream tmp;
            tmp << "Header is - " <<hdr << " " << n << std::endl;
            LOG_ERROR(tmp.str());
            if ( n != 1 )
            {
                // something is wrong
                buffer_.clear(); // drop the buffer

                std::ostringstream errorStr;
                errorStr << "Error - " << strerror(errno) << 
                    " in scanning header buffer " << hdr << 
                    " . for body length - " << std::endl;
                LOG_ERROR(errorStr.str());
                return;
            }

            bytesProcessed += hdrLength_;
            bytesRemaining -= hdrLength_;

            if (bytesRemaining < bodyLen )
            {
                return; // get more data to complete body
            }

            char body[bodyLen + 1];
            memset(body, 0, sizeof(body));
            for(int i=0; i<bodyLen; i++)
            {
                body[i]=buffer_[i+bytesProcessed];
            }

            // just print out what you received
            std::ostringstream debugStr;
            debugStr<< "Body - " << body << std::endl;
            LOG_DEBUG(debugStr.str());

            // raise signal for passing data to client
            // TODO - its fine now since we are using a single thread, going
            // forward we need to ensure that the buffers are unique enough !!

            OnPacket(body, bodyLen );

            bytesProcessed += bodyLen;

            // clean up buffer_ by erasing the values already processed
            // note that we are erasing elements only after the body is
            // processed. This ensures that incomplete body is processed in the
            // same loop
            buffer_.erase(buffer_.begin(), 
                          buffer_.begin() + bytesProcessed);

            bytesRemaining -= bodyLen;
        }
    }

} // Network
} // Fog
