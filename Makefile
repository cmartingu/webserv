# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mllamas- <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/19 17:20:49 by mllamas-          #+#    #+#              #
#    Updated: 2023/12/28 15:22:56 by mllamas-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	webserv

SRC		=	ParseConfigFile/Server/Server.cpp \
			ParseConfigFile/Config.cpp \
			ParseConfigFile/Location/Location.cpp \
			ParseResponse/Client.cpp \
			ParseResponse/HttpRequest.cpp \
			webserv.cpp

OBJS	=	$(SRC:.cpp=.o)

COMPILE	=	c++

FLAGS	=	-Wall -Wextra -Werror -std=c++98 -g3

all: $(NAME)

$(NAME): $(OBJS)
	$(COMPILE) $(FLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)
	
re:	fclean all

.cpp.o:
	${COMPILE} ${FLAGS} -c $< -o ${<:.cpp=.o}

.PHONY:	all clean fclean re