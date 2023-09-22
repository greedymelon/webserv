#include "../inc/Config.hpp"
#include "../inc/CheckConfig.hpp"

Config::Config(const std::string &path) :_path(path) {
	try {
		CheckConfig confCheck(path);
		this->parse();
	} catch (const std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}

Config::~Config() {}

void Config::parse() {

	std::ifstream ifs(_path.c_str());
	if (!ifs.is_open()) {
		std::cerr << "File does not exist: " << _path << std::endl;
		exit(1);
	}
	
	std::vector<std::string> tokens;
	std::string token;
	while (ifs >> token) {
		if (token == "{" || token == "}") //如果是{或}，就跳过
			continue;
		else if (!token.empty())
			tokens.push_back(token);
	}
	ifs.close();

	// for (size_t i = 0; i != tokens.size(); ++i) {
	// 	printf("token%lu: %s\n", i, tokens[i].c_str());
	// }

	for (size_t i = 0; i != tokens.size(); ++i) {
		size_t found = tokens[i].find(";");
		if (found)
			tokens[i] = tokens[i].substr(0, found);
	}

	for (size_t i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "server" || tokens[i] == "?server")
			parseServer(tokens, i);
		if (tokens[i] == "server")
			i--;
	}
}

void Config::parseServer(std::vector<std::string> &tokens, size_t &i) {
	VirtualServer server;
	i++;
	for(; i < tokens.size() - 1; ) {
		if (tokens[i] == "server")
			break;
		else if (tokens[i] == "listen") {
			i++;
			paresHostPort(server.host, server.port, tokens[i]);
			CheckPort(server.port);
		}
		else if (tokens[i] == "server_name") {
			i++;
			CheckServerName(tokens[i]);
			server.name = tokens[i];
		}
		else if (tokens[i] == "root") {
			i++;
			server.root = tokens[i];
		}
		else if (tokens[i] == "cgi_ext") {
			i++;
			parseCgiExt(server.cgi_exit, tokens, i);
		}
		else if (tokens[i] == "error_page") {
			parseErrorPage(server.error_pages, tokens, i);
		}
		else if (tokens[i] == "location") {
			parseLocation(server.locations, tokens, i);
			CheckLocation(server.locations);
		}
		else
			i++;
	}
	bool newServer = true;
	for (size_t i = 0; i < servers.size(); i++) {
		if (servers[i].port == server.port)
			newServer = false;
	}
	if (newServer)
		servers.push_back(server);
}

void Config::paresHostPort(std::string &host, std::string &port, std::string &token) {
	std::stringstream ss;
	size_t found = token.find(":");
	host = token.substr(0, found);
	ss << token.substr(found + 1);
	ss >> port;
}

bool Config::check_word(const std::string &word) {
	if (word.empty())
		return true;
	if (word == "cgi_ext" || word == "cgi_path" || word == "index" || word == "listen" || word == "location"  || word == "server_name" || word == "error_page" || word == "root" || word == "index" || word == "return" || word == "autoindex" || word == "upload_dir" || word == "max_body_size")
		return true;
	return false;
}

void Config::parseCgiExt(std::vector<std::string> &cgi_ext, const std::vector<std::string> &tokens, size_t &i) {
	for (; i < tokens.size(); i++) {
		if (!check_word(tokens[i]))
			cgi_ext.push_back(tokens[i]);
		else
			break;
	}
}

static bool str_isdigit(std::string str) {
	for (size_t i = 0; i < str.length(); i++)
		if (!isdigit(str[i]))
			return (0);
	return (1);
}

void Config::parseErrorPage(std::map<int, std::string> &error_pages, std::vector<std::string> &tokens, size_t &i) {
	++i;
	int j = i;
	while (tokens[j] != "\0" && !check_word(tokens[j]))
		++j;
	std::string path = tokens[--j];
	while (str_isdigit(tokens[i])) {
		try {
			error_pages[std::atoi(tokens[i].c_str())] = path;
		} catch (const std::exception &e) {
			throw std::invalid_argument("Error: Error page!");
		}
		i++;
	}
}

void Config::parseLocation(std::vector<Location> &locations, const std::vector<std::string> &tokens, size_t &i) {
	Location location;
	location.autoindex = 0;
	location.redirect_cd = 0;
	location.path = tokens[++i];
	for (; i < tokens.size() - 1 && (tokens[i] != "location" || tokens[i] != "server");) {
		if (tokens[i] == "location" || tokens[i] == "server")
			break;
		else if ( tokens[i] == "allow" ) {
			for ( ++i; !check_word(tokens[i]); ++i) {
				// printf("allow is: %s\n", tokens[i].c_str());
				location.methods.push_back(tokens[i]);
				}
		}
		else if ( tokens[i] == "root" )
			location.root = tokens[++i];
		else if ( tokens[i] == "autoindex" ) {
			std::string str = tokens[++i];
			if (str.compare("on") == 0 || str.compare("off") == 0){
			if (str == "on")
				location.autoindex = 1;
			}
			else
				throw std::invalid_argument("Error: Autoindex");
		}
		else if (tokens[i] == "cgi_path") {
			location.cgi_path = tokens[++i];
		}
		else if (tokens[i] == "upload_dir") {
			location.upload_dir = tokens[++i];
		}
		else if (tokens[i] == "index")
		{
			for (++i;!check_word(tokens[i]); ++i)
				location.index.push_back(tokens[i]);			
		}
		else if (tokens[i] == "return") {
			location.redirect_cd = stoi(tokens[++i]);
			location.redirect_url = tokens[++i];
		}
		else if(tokens[i] == "max_body_size")
		{
			CheckClientMaxBodySize(tokens[++i]);
			location.max_body_size = tokens[i];
		}
		else
			i++;
	}
	locations.push_back(location);	
}

/*
CHECK
*/

//atoi(const char*) -> need to convert by c_str()
//stoi(const string*)
void Config::CheckPort(std::string port) {
	try {
		std::stoi(port);
	}
	catch (const std::exception &e) {
		throw std::invalid_argument("Error: Port!");
	}
}

void Config::CheckServerName(std::string serverName) {
	if (check_word(serverName))
		throw std::invalid_argument("Error: Server name!");
}

void Config::CheckPath(std::string path) {
	if (path.empty())
		throw std::invalid_argument("Error: Path!");
}

void Config::CheckAllow(std::vector<std::string> methods) {
	for (size_t i = 0; i < methods.size(); i++) {
		if (!(methods[i].compare("PUT") == 0 || methods[i].compare("GET") == 0 || methods[i].compare("HEAD") == 0 || methods[i].compare("CONNECT") == 0 || methods[i].compare("GET") == 0 || methods[i].compare("POST") == 0 || methods[i].compare("DELETE") == 0))
			throw std::invalid_argument("Error: Allow!");
	}
}

void Config::CheckLocation(std::vector<Location> location) {
	for (size_t i = 0; i < location.size(); i++) {
		CheckPath(location[i].path);
		CheckAllow(location[i].methods);
	}
}

void Config::CheckClientMaxBodySize (std::string maxBodySize){
    std::string txt;
    if(maxBodySize.back() != 'm')
        txt = maxBodySize;
    else
        txt = maxBodySize.substr(0, maxBodySize.size() - 1);
    for (size_t i = 0; i < txt.length(); i++)
    {
        if (!isdigit(txt[i]))
            throw std::invalid_argument("Error: CheckClientMaxBodySize!");
    }
}

void Config::display() {
    std::cout << "Config path: " << _path << std::endl;
    std::cout << "Servers: " << std::endl;
    for (std::vector<VirtualServer>::const_iterator it = servers.begin(); it != servers.end(); it++) {
        const VirtualServer server = *it;
        std::cout << "\tHost: " << server.host << std::endl;
        std::cout << "\tPort: " << server.port << std::endl;
        std::cout << "\tName: " << server.name << std::endl;
        std::cout << "\tRoot: " << server.root << std::endl;
		std::cout << "\tCGI EXT " << server.cgi_exit.size() << std::endl;
		for (size_t i = 0; server.cgi_exit.size() > i ; i++) {
            std::cout << "\t\tcgiExit " << server.cgi_exit[i]<< std::endl;
        }
        std::cout << "\tError Pages: " << std::endl;
        for (std::map<int, std::string>::const_iterator it_err = server.error_pages.begin(); it_err != server.error_pages.end(); ++it_err) {
            std::cout << "\t\tCode: " << it_err->first << std::endl;
            std::cout << "\t\tPath: " << it_err->second << std::endl;
        }
        std::cout << "\tLocations: " << std::endl;
        for (std::vector<Location>::const_iterator it_loc = server.locations.begin(); it_loc != server.locations.end(); ++it_loc) {
            const Location &location = *it_loc;
	        std::cout << "\tlocation: " << std::endl;
            std::cout << "\t\tPath: " << location.path << std::endl;
            std::cout << "\t\tmax body size: " << location.max_body_size << std::endl;
            std::cout << "\t\tMethods: " << std::endl;
            for (std::vector<std::string>::const_iterator it_meth = location.methods.begin(); it_meth != location.methods.end(); ++it_meth) {
                std::cout << "\t\t\t" << *it_meth << std::endl;
            }
            std::cout << "\t\tRoot: " << location.root << std::endl;
            std::cout << "\t\tAuto Index: " << location.autoindex << std::endl;
            // std::cout << "\t\tCGI Ext: " << location.cgi_ext << std::endl;
            std::cout << "\t\tCGI Path: " << location.cgi_path << std::endl;
            std::cout << "\t\tUpload Dir: " << location.upload_dir << std::endl;
            std::cout << "\t\tIndex: " << std::endl;
            for (std::vector<std::string>::const_iterator it_ind = location.index.begin(); it_ind != location.index.end(); ++it_ind) {
                std::cout << "\t\t\t" << *it_ind << std::endl;
            }
			std::cout << "\t\tRedirection code: " << location.redirect_cd << std::endl;
			std::cout << "\t\tRedirection url: " << location.redirect_url << std::endl;
        }
	}
}
