#ifndef MODEL_PROTOCOL_H
#define MODEL_PROTOCOL_H

#include <opencommon/ModelInfo.h>
#include "common/DataSet.h"
#include "common/TextSerializer.h"

namespace Fog {
namespace Model {

class Protocol
{
public:
    Protocol(int protocolId, 
             std::string protoName,
             std::string ip,
             int portNum_){}
    bool initialize() {return true; }
private:
    int protocolId_;
    std::string protocolName_;
    std::string ip_;
    int portNum_;
};

} // Model
} // Fog

#endif // MODEL_PROTOCOL_H
