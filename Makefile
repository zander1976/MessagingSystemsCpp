CXX = g++
CXXFLAGS = -g -Wall -Wextra

all: client server editor

client: client.cpp
	$(CXX) $(CXXFLAGS) -o client client.cpp

server: server.cpp
	$(CXX) $(CXXFLAGS) -o server server.cpp

editor: editor.cpp
	$(CXX) $(CXXFLAGS) -o editor editor.cpp

clean:
	rm -f client server editor

