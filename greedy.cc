#include <iostream>
#include <vector>
#include <ctime>
#include <climits>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Rectangle
{
    int width, height; // Dimensions of the rectangle
};

struct Solution
{
    int x1, y1, x2, y2; // Coordinates of the rectangle (top-left and bottom-right)
};

// Function to check if a rectangle can be placed at position (x, y)
bool can_place(const vector<vector<bool>> &grid, int x, int y, int W, int H)
{
    // Out of bounds check
    if (x + W > (int)grid.size() || y + H > (int)grid[0].size())
        return false;

    // Check for overlapping with already placed rectangles
    for (int i = x; i < x + W; ++i)
    {
        for (int j = y; j < y + H; ++j)
        {
            if (grid[i][j])
                return false;
        }
    }
    return true;
}

// Function to mark (action = True) or unmark (action = False) the grid when placing or removing a rectangle
void place_or_remove(vector<vector<bool>> &grid, int x, int y, int W, int H, bool action)
{
    // Iterate over the rectangle's cells and mark/unmark them
    for (int i = x; i < x + W; ++i)
    {
        for (int j = y; j < y + H; ++j)
        {
            grid[i][j] = action;
        }
    }
}

// Comparator function to sort rectangles by descending area
bool sort_rectangles(const Rectangle &a, const Rectangle &b)
{
    return (a.width * a.height) > (b.width * b.height);
}

void read_input_file(const string &filename, int &W, vector<Rectangle> &rectangles)
{
    ifstream input_file(filename);
    if (!input_file)
    {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    // Read the roll width and number of rectangles
    int number_of_rectangles;
    input_file >> W >> number_of_rectangles;

    // Read the number of rectangles and their dimensions
    string line;
    getline(input_file, line);

    while (getline(input_file, line))
    {
        istringstream iss(line);
        int count, rw, rh;
        if (iss >> count >> rw >> rh)
        {
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
    input_file.close();
}

// Function to perform greedy placement of rectangles
int do_greedy_placement(int W, const vector<Rectangle> &rectangles, vector<Solution> &solution)
{
    // Calculate maximum possible height
    int max_dim = 0;
    for (const auto &r : rectangles)
        max_dim += max(r.width, r.height);

    // Initialize grid with W columns and max_dim rows, all set to false (unoccupied)
    vector<vector<bool>> grid(W, vector<bool>(max_dim, false));
    int best_length = 0;

    // Iterate through each rectangle
    for (size_t i = 0; i < rectangles.size(); i++)
    {
        int rw = rectangles[i].width;
        int rh = rectangles[i].height;
        bool placed = false;

        // Prioritize larger dimension as width and smaller as height
        int w = max(rw, rh);
        int h = min(rw, rh);

        // Attempt to place the rectangle in both orientations
        for (int y = 0; y < max_dim && !placed; y++)
        {
            for (int x = 0; x < W && !placed; x++)
            {
                // Try original orientation (w, h)
                if (can_place(grid, x, y, w, h))
                {
                    place_or_remove(grid, x, y, w, h, true);
                    solution.push_back(Solution{x, y, x + w - 1, y + h - 1});
                    best_length = max(best_length, y + h);
                    placed = true;
                }
                else
                {
                    // If rectangle is not a square, try the rotated orientation (h, w)
                    if (rw != rh)
                    {
                        if (can_place(grid, x, y, h, w))
                        {
                            place_or_remove(grid, x, y, h, w, true);
                            solution.push_back(Solution{x, y, x + h - 1, y + w - 1});
                            best_length = max(best_length, y + w);
                            placed = true;
                        }
                    }
                }
            }
        }
    }
    return best_length;
}

// Function to output the solution to a file
void output_solution(const string &filename, double elapsed_time, int best_length, const vector<Solution> &solution)
{
    ofstream outfile(filename);
    if (!outfile)
    {
        cerr << "Error opening output file: " << filename << endl;
        exit(1);
    }

    outfile << fixed << setprecision(1) << elapsed_time << endl;
    outfile << best_length << endl;
    for (const auto &sol : solution)
    {
        outfile << sol.x1 << " " << sol.y1 << " "
                << sol.x2 << " " << sol.y2 << endl;
    }
    outfile.close();
}

int main(int argc, char *argv[])
{
    // Check for correct number of arguments
    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }

    // Start timing
    clock_t start_time = clock();

    int W;
    vector<Rectangle> rectangles;

    // Read and sort rectangles from the input file
    read_input_file(argv[1], W, rectangles);

    // Sort rectangles by descending area
    sort(rectangles.begin(), rectangles.end(), sort_rectangles);

    vector<Solution> solution;
    int best_length = do_greedy_placement(W, rectangles, solution);

    // End timing
    clock_t end_time = clock();
    double elapsed_time = double(end_time - start_time) / CLOCKS_PER_SEC;

    // Output the solution to the output file
    output_solution(argv[2], elapsed_time, best_length, solution);

    return 0;
}
