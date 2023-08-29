#include "../inc/Configuration.hpp"
#include "../inc/Server.hpp"

int main(int argc, char **argv) {
	std::string filePath;

	if (argc == 1)
    {
		filePath = "Conf/test.conf";
    }
	else if (argc != 2)
    {
		std::cerr << "Invalid argument amount" << std::endl;
		std::exit(EXIT_FAILURE);
    }
	else
    {
		filePath = argv[1];
    }
	Configuration conf(filePath);
	Server(conf).start();
	return (EXIT_SUCCESS);
}