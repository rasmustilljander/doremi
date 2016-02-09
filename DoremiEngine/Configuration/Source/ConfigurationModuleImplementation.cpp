#include <Internal/ConfigurationModuleImplementation.hpp>
#include <Internal/InterpretCofigurationMap.hpp>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
namespace DoremiEngine
{
    namespace Configuration
    {
        ConfigurationModuleImplementation::ConfigurationModuleImplementation(const Core::SharedContext& p_sharedContext)
            : m_sharedContext(p_sharedContext)
        {
        }

        ConfigurationModuleImplementation::~ConfigurationModuleImplementation() {}

        void ConfigurationModuleImplementation::Startup() {}

        void ConfigurationModuleImplementation::Shutdown() {}

        void ConfigurationModuleImplementation::ReadConfigurationValuesFromFile(const std::string p_fileName)
        {
            // Get the true path
            std::string t_truePath = m_sharedContext.GetWorkingDirectory() + "ConfigurationFiles/" + p_fileName;
            // input stream
            std::ifstream t_file(t_truePath);
            if(!t_file.is_open())
            {
                // COuldnt open file, log error
                std::cout << "Couldn't open file " + p_fileName << std::endl;
            }
            std::map<std::string, std::string> values;
            std::string line;
            // Fetch the line, each time the file pointer will step to the next line.
            while(std::getline(t_file, line))
            {
                // Create a stream for the line fetched
                std::istringstream t_lineStream(line);
                std::string key;
                // Splits the line at the first encounter of ' ' and puts the filepointer there. Also saves the steped over text to key
                if(std::getline(t_lineStream, key, ' '))
                {
                    if(key != "//") // If not a comment
                    {
                        std::string value;
                        // The last ' ' is to remove whitespace errors at the end of a line. This does however constrains
                        // the value to only beeing one word meaning no whitespace in a value!
                        if(std::getline(t_lineStream, value, ' ')) values[key] = value; // Saves the values to the map at key
                    }
                }
            }
            // Close the file when we are done reading
            t_file.close();
            // Saves the red data to the given configuration info
            InterpretMap(values, m_configInfo);
        }

        void ConfigurationModuleImplementation::WriteConfigurationValuesToFile(const std::string p_fileName)
        {
            std::string t_truePath = m_sharedContext.GetWorkingDirectory() + "ConfigurationFiles/" + p_fileName;
            // Take the input file to save the values at the correct location in file
            std::ifstream t_inputFile(t_truePath);
            bool shouldFill = false;
            if(!t_inputFile.is_open())
            {
                // COuldnt open file, log error
                std::cout << "Couldn't open file, creating new one which will be filled with all config values " + p_fileName << std::endl;
                shouldFill = true;
            }
            // Get all the values from config
            std::map<std::string, std::string> values;
            values = SaveConfigToMap(m_configInfo);
            // Needed to save the values at the correct location in file
            std::map<int, std::string> valuesSaveOrder;
            std::string line;
            int i = 0;
            // Fetch the line, each time the file pointer will step to the next line.
            while(std::getline(t_inputFile, line))
            {
                // Create a stream for the line fetched
                std::istringstream t_lineStream(line);
                std::string key;
                // If this isnt a comment it will be overwritten
                valuesSaveOrder[i] = line;
                if(std::getline(t_lineStream, key, ' '))
                {
                    if(key != "//") // If no comment
                    {
                        if(values.count(key))
                        {
                            // Take the string from the values map
                            valuesSaveOrder[i] = values.find(key)->first + " " + values.find(key)->second;
                            // Erase it so we know which keys didnt exist in the file
                            values.erase(key);
                        }
                        else
                        {
                            // The config file have spelling errors
                            std::cout << "Spelling errors in " << p_fileName << std::endl;
                            valuesSaveOrder[i] = "";
                        }
                    }
                }
                i++;
            }
            t_inputFile.close();

            // Open stream for output
            std::ofstream t_outputFile(t_truePath);
            size_t length = valuesSaveOrder.size();
            // Write all values that already had a key in the file
            for(size_t i = 0; i < length; i++)
            {
                t_outputFile << valuesSaveOrder.at(i) << std::endl;
            }
            // Write the rest, Should perhaps not be done?
            if(shouldFill)
            {
                for(auto pairs : values)
                {
                    t_outputFile << pairs.first + " " + pairs.second << std::endl;
                }
            }
            t_outputFile.close();
        }
    }
}

DoremiEngine::Configuration::ConfigurationModule* CreateConfigurationModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Configuration::ConfigurationModule* configuration = new DoremiEngine::Configuration::ConfigurationModuleImplementation(p_sharedContext);
    return configuration;
}