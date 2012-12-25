#include "OpenGlassClient.h"
#include "common/Command.h"

int main(int argc, char* argv[])
{
    try
    {
        Fog::OpenGlassClient openg("test_config_rmiclient", "client.log");
        openg.initialize();
        openg.run();
        return true;
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
