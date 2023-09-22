#include "../inc/CheckConfig.hpp"

CheckConfig::~CheckConfig() {}

/** Check Semicolon ************************************************************************************/

bool	wrongSemicolon(const std::string &str) {
	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] == ';' && i != str.length() - 1) {
			return true;
		}
	}
	return false;
}

/** Constructor ************************************************************************************/

CheckConfig::CheckConfig(const std::string path) : serverBrackets(false), locationBrackets(false) {
	std::ifstream ifs(path.c_str());
	if (!ifs.is_open()) {
	std::cerr << "File does not exist: " << path << std::endl;
	exit(1);
	}

	std::vector<std::string> tokens;
	std::string token;
	while (ifs >> token) {
		if (token.find(";") != std::string::npos) {
			if (wrongSemicolon(token)) {
				throw std::invalid_argument("Error: missing semicolor or invalid semicolon");
			}
		}
		if (token.find("{") != std::string::npos && token.size() > 1)
		{
			if (token.size() != token.find("{") + 1)
				throw std::invalid_argument("Error : Brackets");
			token.resize(token.find("{"));
			tokens.push_back(token);
			tokens.push_back("{");
		}
		else
			tokens.push_back(token);
	}
	checkBrackets(tokens);
	checkKey(path);
}

/** Brackets Getter & Setter ************************************************************************************/

void CheckConfig::setServerBrackets(bool isOpen) {
	serverBrackets = isOpen;
}

void CheckConfig::setLocationBrackets(bool isOpen) {
	locationBrackets = isOpen;
}

bool CheckConfig::getLocationBrackets() {
	return locationBrackets;
}

bool CheckConfig::getServerBrackets() {
	return serverBrackets;
}

/** Check Brackets ************************************************************************************/

void CheckConfig::checkBrackets(std::vector<std::string> tokens) {
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].compare("{") == 0) {
			if (i == 0) //??????????????????
				throw std::invalid_argument("Brackets error! 1");
			if (tokens[i - 1].compare("server") == 0 && getServerBrackets() == false) {
				setServerBrackets(true);
			}
			else if (tokens[i - 2].compare("location") == 0 && getLocationBrackets() == false) {
				setLocationBrackets(true);
			}
			else
				throw std::invalid_argument("Error : Brackets");
		}
		if (tokens[i].compare("}") == 0) {
			if (getLocationBrackets() == true) {
				setLocationBrackets(false);
				if (i < tokens.size()) {
					if (!(tokens[i + 1].compare("}") == 0 || tokens[i + 1].compare("location") == 0))
						throw std::invalid_argument("Error : Brackets");
				}
			}
			else if (getServerBrackets() == true) {
				setServerBrackets(false);
				if (i < tokens.size()) {
					if ((tokens[i + 1].compare("server") != 0) && (i + 1) < tokens.size())
						throw std::invalid_argument("Error : Brackets");
				}
			}
			else
				throw std::invalid_argument("Error : Brackets");
		}
	}
	if (getServerBrackets() || getLocationBrackets())
		throw std::invalid_argument("Error : Brackets");
}

/** Check Key ************************************************************************************/

std::string CheckConfig::deleteSpace(std::string word)
{
	size_t count = 0;
	for (int i = 0; word[i]; i++) {
		if (word[i] == ' ' || word[i] == '\t')
			count++;
		else
			break;
	}
	if (word.size() == count)
		return "";
	word.erase(0, count);
	return word;
}

bool CheckConfig::checkLocationKey(std::string word) {
	if (!getLocationBrackets())
		return false;
	word = deleteSpace(word);
	if(word.empty())
		return true;
	if ( word == "max_body_size" || word == "cgi_path" || word == "root" || word == "allow" || word == "autoindex" || word == "index" || word == "upload_dir" || word == "cgi_ext"  || word == "return")
		return true;
	return false;
}

bool CheckConfig::checkServerKey(std::string word) {
	word = deleteSpace(word);
	if (word.empty())
		return true;
	if (word == "{")
		return true;
	else if(word == "}") {
		setLocationBrackets(false);
		return true;
	}
	if (word == "location") {
		if (getLocationBrackets())
			return false;
		else {
			setLocationBrackets(true);
			return true;
		}
	}
	else if (getLocationBrackets())
		return checkLocationKey(word);
	else if (word == "root" || word == "server" || word == "{" || word == "}" || word == "listen" || word == "location" || word == "server_name" || word == "error_page" || word == "cgi_ext" )
		return true;
	return false;
}

void CheckConfig::checkKey(const std::string path) {
	std::ifstream ifs(path.c_str());
	if (!ifs.is_open()) {
		std::cerr << "File does not exits:" << path << std::endl;
		exit(1);
	}
	std::string line;
	while (getline(ifs, line))
	{
		if (!checkServerKey(split(line, " ")[0]))
			throw std::invalid_argument("Error: Invalid Key");
		checkArgument(line);
	}
}

/** Check Argument ************************************************************************************/

std::vector<std::string> CheckConfig::split(std::string str, std::string delimiter) {
    std::vector<std::string> words;
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        if (!token.empty()) {
            words.push_back(token);
        }
        str.erase(0, pos + delimiter.length());
    }
    if (!str.empty()) {
        words.push_back(str);
    }
    if (words.size() == 0)
        words.push_back("");
    return words;
}

void CheckConfig::checkArgument(const std::string line) {
    std::vector<std::string> parse = split(line, " ");
    for (size_t i = 0; parse.size() > i; i++)
        parse[i] = deleteSpace(parse[i]);
    if (parse[0].compare("listen") == 0 || parse[0].compare("server_name") == 0 || parse[0].compare("root") == 0 || parse[0].compare("client_max_body_size") == 0 ||  parse[0].compare("autoindex") == 0 ) {
        if (parse.size() != 2)
            throw std::invalid_argument("Error: " + parse[0] + ": argument error");
    }
    if (parse[0].compare("return") == 0 || parse[0].compare("error_page") == 0) {
        if (parse.size() != 3)
            throw std::invalid_argument("Error: " + parse[0] + ": argument error");
    }
}
