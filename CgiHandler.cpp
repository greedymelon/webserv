#include "CgiHandler.hpp"
#include <cstring>
#include <iostream>

// CgiHandler::CgiHandler(Request &request)
// {
//     if (request.get_uri().find(".jpeg") == std::string::npos)
//         _is_img = false;
//     else
//         _is_img = true;
// }


void CgiHandler::feed(const char *chunk)
{
   _page.push_back(chunk);
   


}


