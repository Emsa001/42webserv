NAME			= webserv

# compiler settings
CXX				= c++
CXXFLAGS		= -Wall -Wextra
CXXFLAGS		+= -std=c++98 -pedantic
# CXXFLAGS		+= -fsanitize=address
CXXFLAGS		+= -g
CXXFLAGS		+= -I$(INCDIR)

# source files
INCDIR			= includes
SRCDIR			= src
OBJDIR			= .obj
SRC				= $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp) $(wildcard $(SRCDIR)/*/*/*.cpp)
OBJ				= $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))

all: $(NAME)

run: $(NAME)
	./$(NAME)

clean:
	$(RM) -rf $(OBJDIR)

fclean: clean
	$(RM) -f $(NAME)
	$(RM) -f test

re: fclean
	$(MAKE)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o "$@" $^ $(LDFLAGS)

$(OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o "$@" "$<"

setup:
	docker build -t pre-commit -f assets/pre-commit.Dockerfile .

format:
	docker run -v.:/mnt -it --rm pre-commit

# ===== DOCKER =====
docker:
	docker build -t webserv .

docker-test:
	docker run --rm webserv ./entrypoint.sh test

docker-run:
	docker run -p 8080:80 webserv

.PHONY: run all clean fclean re test t setup format docker docker-test docker-run

# ===== TESTING =====
TEST_CXXFLAGS		= -std=c++11 -I$(GTESTDIR)/googletest/include -g -I$(INCDIR)
TEST_LDFLAGS		+= -L$(GTESTDIR)/build/lib
TEST_LDLIBS			+= -lgtest -lgtest_main -pthread
TEST_MAIN			= $(wildcard tests/*.cpp)
TEST_OBJ			= $(filter-out %/main.o, $(OBJ))

GTESTDIR = gtest
GTEST = $(GTESTDIR)/build/lib/libgtest.a

test: $(GTEST) $(TEST_OBJ) $(TEST_MAIN)
	$(CXX) -o "$@" $(TEST_CXXFLAGS) $(TEST_MAIN) $(TEST_OBJ) $(TEST_LDFLAGS) $(TEST_LDLIBS)

t: test
	./$<

$(GTESTDIR):
	git clone --depth=1 --branch release-1.12.1 https://github.com/google/googletest "$@"

$(GTEST): $(GTESTDIR)
	cd $(GTESTDIR) && \
	mkdir -p build && \
	cd build && \
	cmake -DCMAKE_CXX_STANDARD=11 .. && \
	make -j$(shell nproc)

.PHONY: all clean fclean re run leak t
