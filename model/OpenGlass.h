#ifndef OPENGLASS_H
#define OPENGLASS_H

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "common/DataSet.h"
#include "common/TextSerializer.h"
#include "datasource/DataSourceMgr.h"
#include "BaseGlass.h"


typedef boost::shared_ptr<DataSet<TextSerializer> > DSptr;
namespace Fog {

class OpenGlass : public BaseGlass
{
public:
    OpenGlass(const char* configFileName, const char* logFileName);
    bool initialize();
    void run();
private:
    void testMethod(DataSet<TextSerializer>& ds);
    void testMethod2(DataSet<TextSerializer>& ds);
    void onStop(DataSet<TextSerializer>& ds);
    boost::condition stopEvent_;
    boost::mutex stop_;

    boost::shared_ptr<Fog::Datasource::DataSourceMgr> dsMgrptr_;
};

}// Fog

#endif // OPENGLASS_H
