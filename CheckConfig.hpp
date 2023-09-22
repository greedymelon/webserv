#ifndef CHECKCONFIG_HPP
#define CHECKCONFIG_HPP

#include <vector>
#include <iostream>
#include <fstream>

class CheckConfig {

private:
	bool serverBrackets;
	bool locationBrackets;

public:
	CheckConfig(const std::string path);
	~CheckConfig();

	void	checkBrackets(std::vector<std::string> tokens);
	bool	checkServerKey(std::string word);
	bool	checkLocationKey(std::string word);
	void	checkKey(const std::string path);
	std::vector<std::string>	split(std::string str, std::string delimiter) ;
	std::string	deleteSpace(std::string word);
	void	checkArgument(const std::string line);

	void	setServerBrackets(bool isOpen);
	void	setLocationBrackets(bool isOpen);
	bool	getLocationBrackets();
	bool	getServerBrackets();
};

#endif