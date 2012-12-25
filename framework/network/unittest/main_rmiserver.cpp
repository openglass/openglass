#include <boost/shared_ptr.hpp>

#include "OpenGlass.h"

int main(int argc, char* argv[])
{
    try
    {
        Fog::OpenGlass openg("test_config_rmiserver", "server.log");
        openg.initialize();
        openg.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "Unknown Exception: in main" <<"\n";
    }
}
