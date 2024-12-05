#pragma once

// CPP Includes
# include <iostream>
# include <iomanip>
# include <sstream>
# include <fstream>
# include <string>
# include <cstring>
# include <limits>
# include <cstdio>
# include <dirent.h>

// CPP Containers
# include <map>
# include <set>
# include <vector>
# include <algorithm>
# include <iterator>
# include <list>
# include <utility>


// C Includes
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <fcntl.h>
# include <time.h>
# include <limits.h>
# include <errno.h>
# include <dirent.h>
# include <poll.h>

// C System
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/time.h>

// C Network
# include <sys/socket.h>
# include <sys/select.h>
# include <arpa/inet.h>
# include <netinet/in.h>

# include "ParseConfigFile/Config.hpp"
# include "ParseConfigFile/Server/Server.hpp"
# include "ParseResponse/HttpRequest.hpp"
# include "ParseResponse/Client.hpp"

// void	startWebServer(Server &serv);