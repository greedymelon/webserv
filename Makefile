NAME = webserv
CC = c++
CFLAGS = -Wall -Wextra -Werror 
RM			= rm -f -r
OBJDIR		= obj
#SRCDIR		= src

FILES	=	main.cpp \
			Request.cpp \

OBJ			= ${FILES:%.cpp=${OBJDIR}/%.o}

all : $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJDIR)/%.o: %.cpp
			$(CC) -c $(CFLAGS) -o $@ $<

$(OBJ) : | $(OBJDIR)

$(OBJDIR) :
	mkdir $@

re :	fclean all

clean :
	${RM} ${OBJDIR}

fclean : clean
	${RM} ${NAME} 

.PHONY: all clean fclean re
