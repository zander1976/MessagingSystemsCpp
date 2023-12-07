CXX = g++
CXXFLAGS = -g -Wall -Wextra 
LDFLAGS = -lrt -pthread

all: client server editor

client: client.cpp
	$(CXX) $(CXXFLAGS) -o client client.cpp 

server: server.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o server server.cpp

editor: editor.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o editor editor.cpp

clean:
	rm -f client server editor

