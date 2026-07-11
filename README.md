# zozincube
A software rendered spinning wire cube in C++ inspired by tsoding's video

# steps
- make every pixel a point in a 2D boolean array
- markPoint method that takes x, y, size. finds the x,y point and marks a "size" sized square
- draw the entire board, marked pixels are a different color

# next
- now do this on a loop, transform the marked x,y coords for every frame and create movement
- implement the tsoding algorithm 