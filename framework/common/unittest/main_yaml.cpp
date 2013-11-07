#include <iostream>
#include <yaml-cpp/yaml.h>
#include <unistd.h>
#include <string.h>


int main()
{
    YAML::Node config = YAML::LoadFile("test_yaml_config");
    std::list<std::string> incFileList;

    YAML::Node includeFiles;
    if ( config["include"] )
    {
        YAML::Node includeFiles=config["include"];

        // iterate through the include list and load them as Nodes into the
        // config node

        YAML::const_iterator iter=includeFiles.begin();
        while (iter!=includeFiles.end())
        {
            // If files are present load the same into a node
            std::string file=iter->as<std::string>();
            std::cout << file << "\n";
            YAML::Node m_config = YAML::LoadFile(file);
            config[file]=m_config;
            incFileList.push_back(file);
            ++iter;
        }
    }

    char hostname[80];
    int ret=gethostname(hostname, sizeof(hostname));
    if ( ret != 0 )
    {
        std::cout <<"error for hostname - ret is " << ret << "\n";
    }

    std::string machineName(hostname, strlen(hostname));
    std::cout << "machineName is:" << machineName << "\n";
    std::string name="opra1";
    // parse the machine_config and get the required info about opra1
    if ( config[incFileList.front()] )
    {
        if (config[incFileList.front()][machineName])
        {
            if ( config[incFileList.front()][machineName][name] )
            {
                YAML::Node parser = config[incFileList.front()][machineName][name];
                std::cout <<  "conn is " << parser["conn"] << "\n";
                std::cout <<  "realtime is" << parser["realtime"] << "\n";
            }
        }
    }
    else { std::cout << "Error in machine" << "\n"; }
}

