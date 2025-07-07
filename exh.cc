#include <iostream>
#include <vector>
#include <ctime>
#include <climits>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <csignal>

using namespace std;

struct Rectangle
{
    int width, height; // Dimensions of the rectangle
};

struct Solution
{
    int x1, y1, x2, y2; // Coordinates of the rectangle (top-left and bottom-right)
};

// Initializer backtrack function so "try_place_rectangle" can be called recursively
void backtrack(int index, int current_length, vector<vector<bool>> &grid, vector<Solution> &solution);

// Global variables
vector<Rectangle> rectangles;   // List of rectangles to place
int W, best_length;             // Roll width and the best length found
vector<Solution> best_solution; // Best solution (list of rectangle placements)
clock_t start_time;             // Start time of the program
string output_filename;         // Output file name (global for access in signal handler)

// Function to write the current best solution to the output file.
void write_solution()
{
    ofstream out_file_trunc(output_filename, ios::out | ios::trunc);
    if (!out_file_trunc)
    {
        cerr << "Error opening output file: " << output_filename << endl;
        return;
    }

    double elapsed = double(clock() - start_time) / CLOCKS_PER_SEC;
    out_file_trunc << fixed << setprecision(1) << elapsed << endl;
    out_file_trunc << best_length << endl;

    for (const auto &sol : best_solution)
    {
        out_file_trunc << sol.x1 << " " << sol.y1 << " "
                       << sol.x2 << " " << sol.y2 << endl;
    }

    out_file_trunc.close();
}

// Signal handler to catch interrupt signals
void signal_handler(int signum)
{
    if (best_length < INT_MAX)
    {
        write_solution();
    }
    exit(signum);
}

// Check if a rectangle can be placed at position (x, y) in the grid.
bool can_place(const vector<vector<bool>> &grid,
               int x, int y,
               int width, int height)
{
    if (x + width > W || y + height > static_cast<int>(grid[0].size()))
        return false;

    for (int i = x; i < x + width; ++i)
    {
        for (int j = y; j < y + height; ++j)
        {
            if (grid[i][j])
                return false;
        }
    }
    return true;
}

// Marks or unmarks the cells in the grid occupied by the rectangle.
void place_or_remove(vector<vector<bool>> &grid,
                     int x, int y,
                     int width, int height,
                     bool action)
{
    for (int i = x; i < x + width; ++i)
    {
        for (int j = y; j < y + height; ++j)
        {
            grid[i][j] = action;
        }
    }
}

// Finds the lowest y-coordinate where the rectangle can be placed.
int find_lowest_free_y(const vector<vector<bool>> &grid,
                       int x, int width, int height,
                       int current_length)
{
    for (int y = 0; y <= current_length; ++y)
    {
        if (can_place(grid, x, y, width, height))
            return y;
    }
    return -1; // No valid position found within current_length
}

// Helper function to attempt placing a rectangle and handle recursion
void try_place_rectangle(int index,
                         int x,
                         int y,
                         int width,
                         int height,
                         int new_length,
                         vector<vector<bool>> &grid,
                         vector<Solution> &solution)
{
    // Place the rectangle on the grid
    place_or_remove(grid, x, y, width, height, true);

    // Add the rectangle's coordinates to the solution
    solution.push_back({x, y, x + width - 1, y + height - 1});

    backtrack(index + 1, new_length, grid, solution);

    // Remove the rectangle from the grid
    place_or_remove(grid, x, y, width, height, false);

    // Remove the rectangle's coordinates from the solution
    solution.pop_back();
}

// Recursive backtrack function
void backtrack(int index,
               int current_length,
               vector<vector<bool>> &grid,
               vector<Solution> &solution)
{
    // Prune branches that cannot yield a better solution
    if (current_length >= best_length)
        return;

    // If all rectangles have been placed, update the best solution
    if (index == static_cast<int>(rectangles.size()))
    {
        best_length = current_length;
        best_solution = solution;
        return;
    }

    // Get the current rectangle to place
    Rectangle rect = rectangles[index];

    // Iterate over all possible x-coordinates
    for (int x = 0; x < W; ++x)
    {
        // Attempt to place the rectangle in its original orientation
        int y = find_lowest_free_y(grid, x, rect.width, rect.height, current_length);
        if (y != -1)
        {
            int new_length = max(current_length, y + rect.height);
            try_place_rectangle(index, x, y, rect.width, rect.height, new_length, grid, solution);
        }

        // If the rectangle is not a square, attempt to place it rotated
        if (rect.width != rect.height)
        {
            int y_rotated = find_lowest_free_y(grid, x, rect.height, rect.width, current_length);
            if (y_rotated != -1)
            {
                int new_length_rotated = max(current_length, y_rotated + rect.width);
                try_place_rectangle(index, x, y_rotated, rect.height, rect.width, new_length_rotated, grid, solution);
            }
        }
    }
}

// Initializer backtrack function to set up the grid and solution
void backtrack_init()
{
    // Compute an upper bound for grid height (sum of max dimensions of all rectangles)
    int max_dim = 0;
    for (const auto &r : rectangles)
        max_dim += max(r.width, r.height);

    // Initialize the grid with W columns and max_dim rows, all set to false (unoccupied)
    vector<vector<bool>> grid(W, vector<bool>(max_dim, false));

    vector<Solution> solution;

    backtrack(0, 0, grid, solution);
}

// Reads input from a file
void read_input_file(const string &filename)
{
    ifstream ifs(filename);
    if (!ifs)
    {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    best_length = INT_MAX; // Initialize best_length to a large value so the signal handler knows if a solution has been found

    int n;
    ifs >> W >> n;

    string line;
    getline(ifs, line);
    while (getline(ifs, line))
    {
        istringstream iss(line);
        int count, rw, rh;
        if (iss >> count >> rw >> rh)
        {
            // Add 'count' number of rectangles with dimensions rw x rh
            for (int i = 0; i < count; ++i)
            {
                rectangles.emplace_back(Rectangle{rw, rh});
            }
        }
        else
        {
            cerr << "Error parsing line: " << line << endl;
        }
    }
    ifs.close();
}

// Comparator function to sort rectangles by descending area
bool compare_rectangles(const Rectangle &a, const Rectangle &b)
{
    return (a.width * a.height) > (b.width * b.height);
}

// Main function
int main(int argc, char *argv[])
{

    start_time = clock();
    // Register signal handlers for interrupt and termination signals
    signal(SIGINT, signal_handler);  // Handle Ctrl+C
    signal(SIGTERM, signal_handler); // Handle termination signals

    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }

    // Store the output filename globally for access in write_solution and signal_handler
    output_filename = argv[2];

    read_input_file(argv[1]);

    // Sort rectangles by descending area (width * height)
    sort(rectangles.begin(), rectangles.end(), compare_rectangles);

    // Start the backtracking process by calling the initializer backtrack function
    backtrack_init();
    write_solution();

    return 0;
}
