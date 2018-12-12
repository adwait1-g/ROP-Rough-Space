Compile in this way to get the tool: 

	$ g++ main.cpp elfparse.cpp -o elfparse -std=c++11 -lcapstone

```trying.cpp``` contains the skeleton code for the trie that will store the harvested gadgets in ```main.cpp```.
It is working and there are a few example gadgets that have been inserted into a try and some simple test cases have been searched for.
to execute ```trying.cpp```, run the following command: 

``` g++ -std=c++11  trying.cpp ```
