#include <algorithm>
#include <chrono>
#include <climits>
#include <csignal>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>
#include <ctime>

using namespace std;

struct Rectangle
{
    int width;
    int height;
    bool placed;
};

struct Solution
{
    int x1, y1, x2, y2;
};

clock_t start_time;
string output_filename;
vector<Rectangle> rectangles;
vector<Solution> best_solution;

// Global so the signal handler can easily access it
int best_length = INT_MAX;

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

// Reads the file, returns W; fills the rectangles vector
int read_input_file(const string &filename)
{
    ifstream ifs(filename);
    if (!ifs)
    {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    best_length = INT_MAX; // Initialize best_length to a large value so the signal handler knows if a solution has been found
    int W, n;
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
                rectangles.emplace_back(Rectangle{rw, rh, false});
            }
        }
        else
        {
            cerr << "Error parsing line: " << line << endl;
        }
    }
    ifs.close();
    return W;
}

// Places a rectangle in the solution and updates the variables
void place_rectangle(int &width_occupied,
                     int &width_remaining,
                     int &previous_height,
                     int W,
                     int x,
                     int y,
                     Rectangle &r,
                     vector<Solution> &solutions,
                     bool first_in_row)
{
    solutions.push_back({x, y, x + r.width - 1, y + r.height - 1});
    r.placed = true;
    if (first_in_row)
    {
        width_occupied = r.width;
        previous_height = r.height;
        width_remaining = W - width_occupied;
    }
    else
    {
        width_occupied += r.width;
        width_remaining = W - width_occupied;
    }
}

// Builds a single solution row by row
int grasp(vector<Rectangle> &rects,
          vector<Solution> &solutions,
          int &roll_length,
          int W,
          int &width_occupied,
          int &width_remaining,
          int &previous_height)
{
    for (auto &big : rects)
    {
        if (!big.placed)
        {
            int x = 0, y = roll_length;
            bool placed_big = false;

            // Try the two orientations for the "big" rectangle
            for (int rot = 0; rot < 2 && !placed_big; ++rot)
            {
                swap(big.width, big.height);
                if (big.width <= W)
                {
                    place_rectangle(width_occupied,
                                    width_remaining,
                                    previous_height,
                                    W,
                                    x, y,
                                    big,
                                    solutions,
                                    true);
                    roll_length += big.height;
                    placed_big = true;
                }
            }

            // Fill leftover space if big rectangle was placed
            if (placed_big)
            {
                bool row_full = false;
                for (auto &small : rects)
                {
                    if (!small.placed && !row_full)
                    {
                        x = width_occupied;
                        bool placed_small = false;
                        // Rotate the small rectangle
                        for (int rot = 0; rot < 2 && !placed_small; ++rot)
                        {
                            swap(small.width, small.height);
                            if (small.width <= width_remaining && small.height <= previous_height)
                            {
                                place_rectangle(width_occupied,
                                                width_remaining,
                                                previous_height,
                                                W,
                                                x, y,
                                                small,
                                                solutions,
                                                false);
                                placed_small = true;
                            }
                        }
                        // If leftover space is fully occupied, mark it
                        if (width_occupied == W)
                        {
                            row_full = true;
                        }
                    }
                }
            }
        }
    }
    return roll_length;
}

// Tries random permutations to find better solutions
void vns_search(int iterations, int W)
{
    // Random number generator
    random_device rd;
    mt19937 g(rd());

    for (int i = 0; i < iterations; ++i)
    {
        // Shuffle rectangles
        shuffle(rectangles.begin(), rectangles.end(), g);

        // Reset all parameters
        for (auto &r : rectangles)
        {
            r.placed = false;
        }

        int width_occupied_local = 0;
        int width_remaining_local = W;
        int previous_height_local = 0;

        vector<Solution> candidate;
        int roll_length = 0;

        int new_length = grasp(rectangles,
                               candidate,
                               roll_length,
                               W,
                               width_occupied_local,
                               width_remaining_local,
                               previous_height_local);

        if (new_length < best_length)
        {
            best_length = new_length;
            best_solution = candidate;
        }
    }
}

// Sort rectangles by descending area
bool compare_rectangles(const Rectangle &a, const Rectangle &b)
{
    return (a.width * a.height) > (b.width * b.height);
}

int main(int argc, char *argv[])
{
    start_time = clock();
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }
    output_filename = argv[2];

    // Read roll width
    int W = read_input_file(argv[1]);

    sort(rectangles.begin(), rectangles.end(), compare_rectangles);

    vns_search(600000, W);

    write_solution();
    return 0;
}
