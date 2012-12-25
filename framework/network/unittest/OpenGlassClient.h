#ifndef OPENGLASSCLIENT_H
#define OPENGLASSCLIENT_H

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "common/DataSet.h"
#include "common/TextSerializer.h"
#include "BaseGlass.h"

namespace Fog {

class OpenGlassClient : public BaseGlass
{
public:
    OpenGlassClient(const char* configFileName, const char* logFileName);
    bool initialize();
    void run();
};

}// Fog

#endif // OPENGLASS_H
