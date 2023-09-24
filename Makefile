NAME		= webserv
CC			= c++
CFLAGS		= -Wall -Wextra -Werror -std=c++11
RM			= rm -f -r
OBJDIR		= obj
INCDIR  	= inc
SRCDIR  	= src

SRC     	= $(shell find $(SRCDIR) -name '*.cpp')
OBJ     	= $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
HEAD    	= $(wildcard inc/*.hpp)

all :	$(NAME)

run :	all 
		./webserv

$(NAME) : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEAD)
			$(CC) -c $(CFLAGS) -o $@ $<

$(OBJ) : | $(OBJDIR)

$(OBJDIR) :
	mkdir $@

re :	fclean all

clean :
	${RM} ${OBJDIR}

fclean : clean
	${RM} ${NAME} 

.PHONY: all clean fclean re run