NAME = webserv
CC = c++
CFLAGS = -Wall -Wextra -Werror 
RM			= rm -f -r
OBJDIR		= obj
INCDIR  	= inc
SRCDIR  	= src

SRC     	= $(shell find $(SRCDIR) -name '*.cpp')
OBJ     	= $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
HEAD    	= $(wildcard inc/*.hpp)

# FILES	=	main.cpp \
# 			Request.cpp 

# OBJ			= ${FILES:%.cpp=${OBJDIR}/%.o}

all : $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

# $(OBJDIR)/%.o: %.cpp
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

.PHONY: all clean fclean re
