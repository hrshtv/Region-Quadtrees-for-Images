#include <iostream>
/*
    Description: Region quadtrees implemented in C++ for handling binary images
*/

// A helper function to print things
template <class T>
void debug(T v){
    std::cout << v << std::endl;
}

class quad_tree{

    // Stores the data about the current node and the pointers to it's children
    
    // public: // Comment this before submission

    private:

    static long long int twoPows[21]; // Stores first 20 powers of 2, static, common to all objs of the class

    // Private member variables
    quad_tree* tr; // Top-right
    quad_tree* tl; // Top-left
    quad_tree* br; // Bottom-right
    quad_tree* bl; // Bottom-left

    int value = -1; // default -1, can be 0 or 1 if the node is a root node 
    bool isLeaf; // True if the node is a leaf, can do the same things via value too, but this makes the code readable and easier to debug
    int level; // Not the standard tree level, 2^level * 2^level is the size of the submatrix stored by the node (can be atmost 20)

    // Extra function 1: To help in destruction, also, we can call this manually
    void deleteTree();

    // Extra constructor to initialize an img with a value
    quad_tree(int n, int val);

    // Extra function 2: Used in resize() to increase the size of the img
    void upscale(int m);

    // Extra function 3: Used in resize() to decrease the size of the img
    void downscale(int n, int m);

    // Extra function 4: Used in downscale() to count number of whites
    long long int countWhites();

    // Extra function 5: Used in almost every operation to trim down the redundant nodes, if any
    void postProcess();

    // Extra function 6: The main logic for extract is in here
    void extract2(quad_tree &q, int x1, int y1, int m);

    // Extra operator: Assignment operator, used in almost every function to update values
    quad_tree& operator=(quad_tree const &Q);

    public:

    // 1. Constructor
    quad_tree(int n); // n is atmost 20, "height" of the quad_tree

    // 2. Destructor
    ~quad_tree();

    // 3. Copy Constructor
    quad_tree(quad_tree const &Q);

    // 4. Sets all pixels in the sub-matrix with rows x1 to x2 and columns y1 to y2 (inclusive) to the value b.
    void set(int x1, int y1, int x2, int y2, int b);

    // 5. Returns the value of the pixel (x1,y1)
    int get(int x1, int y1) const;

    // 6. Returns the height n
    int size() const;

    // 7. Pixelwise boolean OR
    void overlap(quad_tree const &Q);

    // 8. Pixelwise boolean AND
    void intersect(quad_tree &Q);

    // 9. Complement all the entries in the matrix
    void complement();

    // 10. Change the size of the matrix to 2^m * 2^m
    void resize(int m);

    // 11. the new value is the 2^m * 2^m submatrix with rows from x1 to x1+2^m-1 and columns y1 to y1+2^m-1
    void extract(int x1, int y1, int m);

};

// Initialize first 20 powers of 2:
long long int quad_tree::twoPows[21] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576};

// 1. (Done) Constructor
quad_tree::quad_tree(int n){
    level = n;
    isLeaf = true;
    value = 0; // as black image
    tr = nullptr;
    tl = nullptr;
    br = nullptr;
    bl = nullptr;
}

// Extra constructor
quad_tree::quad_tree(int n, int val){
    level = n;
    isLeaf = true;
    value = val;
    tr = nullptr;
    tl = nullptr;
    br = nullptr;
    bl = nullptr;
}

// Extra function 1:
void quad_tree::deleteTree(){
    if (!isLeaf){
        delete tl;
        delete tr;
        delete bl;
        delete br;
    }
}

// 2. (Done) Destructor:
quad_tree::~quad_tree(){
    deleteTree();
}

// 3. (Done) Copy constructor
quad_tree::quad_tree(quad_tree const &Q){

    level = Q.level;
    isLeaf = Q.isLeaf;
    value = Q.value;

    if (Q.isLeaf){
        tr = nullptr;
        tl = nullptr;
        br = nullptr;
        bl = nullptr;
    }
    else{
        tr = new quad_tree((*Q.tr));
        tl = new quad_tree((*Q.tl));
        br = new quad_tree((*Q.br));
        bl = new quad_tree((*Q.bl));
    }
}

// Extra operator: Assignment
quad_tree& quad_tree::operator=(quad_tree const &Q){
    if (this != &Q){ // check self assignment for small inc in efficiency

        this->deleteTree();
        level = Q.level;
        isLeaf = Q.isLeaf;
        value = Q.value;

        if (Q.isLeaf){
            tr = nullptr;
            tl = nullptr;
            br = nullptr;
            bl = nullptr;
        }
        else{
            tr = new quad_tree((*Q.tr));
            tl = new quad_tree((*Q.tl));
            br = new quad_tree((*Q.br));
            bl = new quad_tree((*Q.bl));
        }
    }
    return (*this);
}

// Extra function 5 
void quad_tree::postProcess(){
    // Post processing
    if (!isLeaf && tl->isLeaf && tl->value == tr->value && tr->value == bl->value && bl->value == br->value && (br->value == 0 || br->value == 1)){
        // Redundant node found
        int n = this->level;
        int val = tl->value;
        quad_tree temp(n, val);
        (*this) = temp; // assignment operator used
    }
    return;
}

// 4. (Done) Set all pixels in the sub-matrix with rows x1 to x2 and columns y1 to y2 (inclusive) to the value b.
void quad_tree::set(int x1, int y1, int x2, int y2, int b){

    // Sanity check
    if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0 || x2 < x1 || y2 < y1 || x2 >= twoPows[level] || y2 >= twoPows[level])
        return;

    // Small optimization, when setting entire node
    if (x1 == 0 && y1 == 0 && x2 == (twoPows[level]-1) && y2 == x2){
        quad_tree temp(level, b);
        (*this) = temp;
        return; // since it's a leaf, no need for post-processing
    }
    else{
        if (isLeaf){
            if (level==0){
                value = b; // just change the value
                return;
            }
            else{
                // Create temporary redundant nodes filled with the previous value 
                tl = new quad_tree(level-1, value);
                tr = new quad_tree(level-1, value);
                bl = new quad_tree(level-1, value);
                br = new quad_tree(level-1, value);
                // Remove leaf status, level remains same
                isLeaf = false;
                value = -1;
            }
        }

        int mid = twoPows[level-1];
        
        if (x1 < mid && x2 >= mid && y1 < mid && y2 >= mid){
            // In all 4
            tl->set(x1, y1, mid-1, mid-1, b);
            tr->set(x1, 0, mid-1, y2-mid, b);
            bl->set(0, y1, x2-mid, mid-1, b);
            br->set(0, 0, x2-mid, y2-mid, b);
        }
        else if (x1 < mid && x2 >= mid && y2 < mid){
            // In tl and bl
            tl->set(x1, y1, mid-1, y2, b);
            bl->set(0, y1, x2-mid, y2, b);
        }
        else if (x1 < mid && x2 >= mid && y1 >= mid){
            // In tr and br 
            tr->set(x1, y1-mid, mid-1, y2-mid, b);
            br->set(0, y1-mid, x2-mid, y2-mid, b);
        }
        else if (x2 < mid && y1 < mid && y2 >= mid){
            // In tl and tr
            tl->set(x1, y1, x2, mid-1, b);
            tr->set(x1, 0, x2, y2-mid, b);
        }
        else if (x1 >= mid && y1 < mid && y2 >= mid){
            // In bl and br
            bl->set(x1-mid, y1, x2-mid, mid-1, b);
            br->set(x1-mid, 0, x2-mid, y2-mid, b);
        }
        else if (x2 < mid && y2 < mid){
            // In tl
            tl->set(x1, y1, x2, y2, b);
        }
        else if (x2 < mid && y1 >= mid){
            // In tr
            tr->set(x1, y1-mid, x2, y2-mid, b);
        }
        else if (x1 >= mid && y2 < mid){
            // In bl
            bl->set(x1-mid, y1, x2-mid, y2, b);
        }
        else if (x1 >= mid && y1 >= mid){
            // In br
            br->set(x1-mid, y1-mid, x2-mid, y2-mid, b);
        }
    }

    postProcess();
}

// 5. (Done) O(height) Return the value of the pixel (x1,y1)
int quad_tree::get(int x1, int y1) const{
    if (isLeaf)
        return value;
    else{
        int mid = twoPows[level-1]; // can be stored inside int
        if (x1 >= mid){
            if (y1 >= mid)
                return br->get(x1-mid, y1-mid);
            else
                return bl->get(x1-mid, y1);
        }
        else{
            if (y1 >= mid)
                return tr->get(x1, y1-mid);
            else
                return tl->get(x1, y1);
        }
    }
}

// 6. (Done) Returns the height n
int quad_tree::size() const{
    return level;
}

// 7. (Done) Pixelwise boolean OR
void quad_tree::overlap(quad_tree const &Q){
    if (this->isLeaf){
        if (this->value == 0){
            (*this) = Q; // since boolean OR with a full black image is the other image 
            return;
        }
        else // this is a full white image, result will be full white too
            return;
    }
    else if (Q.isLeaf){
        if (Q.value == 0) // no change since boolean OR with a full black image
            return;
        else{
            (*this) = Q; // since boolean OR with a full white image is a full white image 
            return;
        }
    }
    else{
        // The only remaining case is when both are not leaves, recurse on children
        tl->overlap(*(Q.tl));
        tr->overlap(*(Q.tr));
        bl->overlap(*(Q.bl));
        br->overlap(*(Q.br));
    }

    postProcess();
}

// 8. (Done) Pixelwise boolean AND
void quad_tree::intersect(quad_tree &Q){
    if (this->isLeaf){
        if (this->value == 1){
            (*this) = Q; // since boolean AND with a full white image is the other image 
            return;
        }
        else // this is a full black image, result will be full black too
            return;
    }
    else if (Q.isLeaf){
        if (Q.value == 1) // no change since boolean AND with a full white image
            return;
        else{
            (*this) = Q; // since boolean OR with a full black image is a full black image 
            return;
        }
    }
    else{
        // The only remaining case is when both are not leaves, recurse on children
        tl->intersect(*(Q.tl));
        tr->intersect(*(Q.tr));
        bl->intersect(*(Q.bl));
        br->intersect(*(Q.br));
    }

    postProcess();
}

// 9. (Done) Complement all the entries in the matrix
void quad_tree::complement(){
    if (isLeaf)
        value ^= 1; // toggles
    else{
        tl->complement();
        tr->complement();
        bl->complement();
        br->complement();
    }
}

// Extra function 2
void quad_tree::upscale(int m){
    // Only the level/capacity of a node changes
    if (m < 0)
        return;
    level = m;
    if (!isLeaf){
        tl->upscale(m-1); // m-1 as level will be level-1 for children trees
        tr->upscale(m-1);
        br->upscale(m-1);
        bl->upscale(m-1);
    }   
    else
        return;
}

// Extra function 4
long long int quad_tree::countWhites(){
    if (isLeaf){
        if (value == 1)
            return twoPows[level]*twoPows[level];
        else
            return 0;
    }
    else
        return (tl->countWhites() + tr->countWhites() + bl->countWhites() + br->countWhites());
}

// Extra function 3
void quad_tree::downscale(int n, int m){
    if (level == (n-m)){
        long long int n_whites = countWhites();
        long long int n_blacks = twoPows[level]*twoPows[level] - n_whites;
        if (n_whites >= n_blacks){
            quad_tree temp(0, 1); // Must be of 0 level, and white
            (*this) = temp; // assignment operator used
        }
        else{
            quad_tree temp(0); // Must be of 0 level, and black
            (*this) = temp; // assignment operator used
        }
    }
    else if (level > (n-m)){
        level -= (n-m);
        if (!isLeaf){
            tl->downscale(n, m);
            tr->downscale(n, m);
            bl->downscale(n, m);
            br->downscale(n, m);
        }
    }
    postProcess();
}

/*
    Alternative for countWhites():
    Store the n_whites inside each node, which measures the number of white pixels in the submatrix represented by the node.
    This may be difficult to maintain.
*/

// 10. Change the size of the matrix to 2^m * 2^m
void quad_tree::resize(int m){
    if (m == level)
        return;
    else if (m > level)
        upscale(m);
    else
        downscale(level, m);
}

// Extra function 6: Assigns the extracted tree to q
void quad_tree::extract2(quad_tree &q, int x1, int y1, int m){
    if (isLeaf){
        q.isLeaf = true;
        q.value = this->value;
        return;
    }
    if (m == 0){
        // A single pixel
        q.isLeaf = true;
        q.value = this->get(x1, y1);
        return;
    }
    else if (m == level){
        q = (*this);
        return;
    }
    else if (m > 0){

        int mid = twoPows[m-1];
        int mid_main = twoPows[level-1];
        int x2 = x1 + twoPows[m] - 1;
        int y2 = y1 + twoPows[m] - 1;

        if (x2 < mid_main && y2 < mid_main)
            this->tl->extract2(q, x1, y1, m);
        else if (x1 >= mid_main && y1 >= mid_main)
            this->br->extract2(q, x1-mid_main, y1-mid_main, m);
        else if (x2 < mid_main && y1 >= mid_main)
            this->tr->extract2(q, x1, y1-mid_main, m);
        else if (x1 >= mid_main && y2 < mid_main)
            this->bl->extract2(q, x1-mid_main, y1, m);
        else{
            // Lies in all 4: Divide extracted area into subareas
            // q can be a nonLeaf, if not it's handled in postProcess
            q.value = -1;
            q.isLeaf = false;

            q.tl = new quad_tree(m-1, -1);
            q.tr = new quad_tree(m-1, -1);
            q.bl = new quad_tree(m-1, -1);
            q.br = new quad_tree(m-1, -1);

            this->extract2(*q.tl, x1, y1, m-1);
            this->extract2(*q.tr, x1, y1+mid, m-1);
            this->extract2(*q.bl, x1+mid, y1, m-1);
            this->extract2(*q.br, x1+mid, y1+mid, m-1);

            q.postProcess();
            return;
        }
    }
    else
        return;
}

// 11. the new value is the 2^m * 2^m submatrix with rows from x1 to x1+2^m-1 and columns y1 to y1+2^m-1
void quad_tree::extract(int x1, int y1, int m){
    if (m == level)
        return;
    // Minor optimization:
    if (!isLeaf){
         // A single leaf of capacity m, all nodes are nullptr
        quad_tree q(m); // final quad_tree which will store the extracted matrix
        this->extract2(q, x1, y1, m);
        (*this) = q; // final deepcopy
        return;
    }
    else {
        // Only level of the leaf changes
        level = m;
        return;
    }
}