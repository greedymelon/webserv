#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include <string>

class Configuration
{
    public:

        Configuration (const std::string &path);
        ~Configuration(void);
        std::string fake ;
};

#endif