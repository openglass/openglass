#include <boost/shared_ptr.hpp>

#include "OpenGlass.h"

int main(int argc, char* argv[])
{
    try
    {
        // TODO - make this as input args to the main function
        Fog::OpenGlass openg("../cfg/openGlass.cfg", "../log/openGlass.log");
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
