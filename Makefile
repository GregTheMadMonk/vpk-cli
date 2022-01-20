all: *.cpp
	c++ -o vpk-cli *.cpp -g

clean:
	-rm vpk-cli
