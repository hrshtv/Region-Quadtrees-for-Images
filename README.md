# Region Quadtrees for Image Operations  
A `C++` implementation of region quadtrees for performing various operations on binary images  

---
Operations supported:
- `set(int x1, int y1, int x2, int y2, int b)` : Set all pixels in the sub-matrix/sub-image with rows `x1` to `x2` and columns `y1` to `y2` (inclusive) to the value `b`
- `get(int x1, int y1)` : Return the value of the pixel at `(x1, y1)`
- `overlap(quad_tree const &Q)` : Pixel-wise boolean `OR` / Image union
- `intersect(quad_tree &Q)` : Pixel-wise boolean `AND` / Image intersection 
- `complement()` : Complement all the entries in the binary image
- `resize(int m)` : Resizes the image to `2^m * 2^m`
- `extract(int x1, int y1, int m)` : Extracts the `2^m * 2^m` submatrix with rows from `x1` to `x1+2^m-1` and columns `y1` to `y1+2^m-1` / Image cropping

---
(*This was done as a course assignment for CS 213: Data Structures and Algorithms, IIT Bombay*)  
