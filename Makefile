
#Library Name ----------------------------

NAME = webserv

#Command variables ------------------------

CC = c++
FLAGS = -Wall -Wextra -Werror -g -std=c++98 #-fsanitize=address
MK = mkdir -p
RM = rm -rf
USER = $(shell whoami)

#Sources directories

INCDIR = inc
SRCDIR = server
SRC = main.cpp Request.cpp Response.cpp initialization.cpp utils.cpp serverConfig.cpp location.cpp configFile.cpp configParser.cpp getMethod.cpp postMethod.cpp deleteMethod.cpp parsingMethodUtils.cpp cgi.cpp
		
VPATH = ${SRCDIR} ${SRCDIR2}

#Object directories

OBJDIR = server_obj


OBJS = $(addprefix ${OBJDIR}/, ${SRC:%.cpp=%.o})

${OBJDIR}/%.o : %.cpp
	${CC} ${FLAGS} -I${INCDIR} -c $< -o $@ 
	
all: $(NAME) $(NAME2)

$(NAME): $(OBJDIR) $(OBJS)
	$(CC) ${FLAGS} $(OBJS) -o $(NAME)


$(OBJDIR):
	$(MK) $(OBJDIR)

clean:
	$(RM) $(OBJDIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY:
	fclean re bonus all