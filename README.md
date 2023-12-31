Pathfinding Algorithm Implementation with A*
---
Using [Raylib](https://www.raylib.com/) and [RayGui](https://www.raylib.com/), I have implemented a basic A* implementation, 
<br>
It allows the user to choose whether to use diagonals or not in the calculation of the area.

> In a worst-case, the algorithm can be O(b^d), where b is the branching factor – the average number of edges from each node, and d is the number of nodes on the resulting path

User can also change the variables and colors used in the implementation by changing some parts of the code


```c
#define WIDTH 720
#define WIN_HEIGHT WIDTH
#define WIN_WIDTH (WIDTH + WIDTH * 0.3)
#define ROWS (int)(WIDTH/20)
#define FPS 60

Color baseTileColor = DARKBLUE;
Color startColor = WHITE;
Color goalColor = PINK;
Color endingColor = WHITE;
Color gridLineColor = BLACK;
Color barrierColor = BLACK;
Color pathColor = RED;
Color visitedColor = ORANGE;

```
<br>

To run the Code in Windows
> 1. Clone the Repository
> 2. Open Terminal
> 3. ```build.bat```

To run the Code on Bash
> 1. Clone the repository
> 2. Open terminal
> 3. ```chmod +x build.sh```
> 4. ```build.sh```

