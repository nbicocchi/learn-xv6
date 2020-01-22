# Tree

Usage: 
tree <i>path</i>

The path can either be absolute or relative. 
If no arguments are provided the tree command will execute in the current directory. 

strcat library function was introduced to implement the tree command. 

The idea behind the implementation is to have a structure that represents a node of the tree. </br>
The structure is formed by:
<ul>
  <li>name</li>
  <li>another struct that represents the next node</li>
  <li>another struct that represents the child</li>
</ul>

To install: <i>./generate.sh -l Tree</i>
