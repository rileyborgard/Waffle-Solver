
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <unordered_set>
#include <array>

std::vector<std::string> dict;
std::unordered_set<std::string> set;
std::vector<std::string> letters, colors;

std::vector<std::string> solution;
std::array<int, 26> counts;

// Determine whether a cell is present in a horizontal word (H), vertical word (V), or both (2)
std::vector<std::string> type = {
    "2H2H2",
    "V V V",
    "2H2H2",
    "V V V",
    "2H2H2",
};

int grid[5][5];

bool is_consistent() {
    // consistent with colors
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if(letters[i][j] == ' ') continue;
            switch(colors[i][j]) {
            case 'G': {
                if(solution[i][j] != letters[i][j] && solution[i][j] != '*') {
                    return false;
                }
                break;
            }
            case 'Y': {
                if(solution[i][j] == letters[i][j]) {
                    // If this was true, the cell should be colored green instead
                    return false;
                }
                // We need at least one letter in a shared word to be letters[i][j]
                bool found = false;
                for(int r = 0; r < 5; r++) {
                    if ((type[i][j] == 'V' || type[i][j] == '2') && solution[r][j] == letters[i][j] || solution[r][j] == '*') {
                        found = true;
                        break;
                    }
                    if ((type[i][j] == 'H' || type[i][j] == '2') && solution[i][r] == letters[i][j] || solution[i][r] == '*') {
                        found = true;
                        break;
                    }
                }
                if(!found) {
                    return false;
                }
                break;
            }
            case '.': {
                if(solution[i][j] == letters[i][j]) {
                    // If this was true, the cell should be colored green instead
                    return false;
                }
                // Don't check anything else, because special cases are complicated
                // TODO: at least check for these special cases when the board is complete
                break;
            }
            default:
                assert(false);
            }
        }
    }

    // consistent with letter set
    std::fill(counts.begin(), counts.end(), 0);
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if(letters[i][j] == ' ') continue;
            counts[letters[i][j] - 'A']++;
        }
    }
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if(letters[i][j] == ' ' || solution[i][j] == '*') continue;
            if(--counts[solution[i][j] - 'A'] < 0) {
                return false;
            }
        }
    }

    // consistent with dictionary
    std::string temp(5, '*');
    for(int r = 0; r < 5; r += 2) {
        if(set.count(solution[r]) == 0) {
            return false;
        }
        for(int i = 0; i < 5; i++) {
            temp[i] = solution[i][r];
        }
        if(set.count(temp) == 0) {
            return false;
        }
    }
    return true;
}

// Backtracking algorithm to solve for what the final board should look like
// Does not tell you what swaps to make
bool solve() {
    int best_options = INT_MAX;
    int best_i = -1, best_j = -1;
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if(letters[i][j] == ' ' || solution[i][j] != '*') continue;
            int options = 0;
            for(char c = 'A'; c <= 'Z'; c++) {
                solution[i][j] = c;
                if(is_consistent()) {
                    options++;
                    if(options >= best_options) {
                        // We already know this isn't the best cell to pick next
                        // No need to keep checking other options
                        break;
                    }
                }
            }
            solution[i][j] = '*';
            if(options == 0) {
                return false;
            }
            if(options < best_options) {
                best_options = options;
                best_i = i;
                best_j = j;
            }
        }
    }
    if(best_options == INT_MAX) {
        return true;
    }
    int i = best_i, j = best_j;
    for(char c = 'A'; c <= 'Z'; c++) {
        solution[i][j] = c;
        if(is_consistent() && solve()) {
            return true;
        }
    }
    solution[i][j] = '*';
    return false;
}

std::vector<std::pair<int, int>> min_swaps() {
    // For each letter, store the grid positions where it occurs in the input and output configurations
    std::vector<std::vector<int>> input(26), output(26);
    int ngrid = 0;
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if(letters[i][j] == ' ') continue;
            ngrid = std::max(ngrid, grid[i][j]);
            if(colors[i][j] == 'G') continue;
            input[letters[i][j] - 'A'].push_back(grid[i][j]);
            output[solution[i][j] - 'A'].push_back(grid[i][j]);
        }
    }

    // For each letter, there is a mapping from its input positions to its output positions
    // We will iterate over all combinations of them with brute force
    std::vector<std::vector<int>> perm(26);
    for(int c = 0; c < 26; c++) {
        perm[c].resize(input[c].size());
        assert(input[c].size() == output[c].size());
        for(int i = 0; i < (int) perm[c].size(); i++) {
            perm[c][i] = i;
        }
    }

    // Iterate through all permutations that move the letters correctly
    // Save the one with the fewest cycles, that is the one that requires the fewest swaps
    std::vector<bool> vis(26);
    std::vector<int> p(ngrid, -1);
    int maxcyc = -1;
    std::vector<int> bestp(ngrid, -1);
    while(true) {
        std::fill(vis.begin(), vis.end(), false);
        for(int c = 0; c < 26; c++) {
            for(int i = 0; i < (int) perm[c].size(); i++) {
                p[input[c][i]] = output[c][perm[c][i]];
            }
        }
        int cyc = 0;
        for(int i = 0; i < ngrid; i++) {
            if(p[i] == -1 || vis[i]) continue;
            cyc++;
            int x = i;
            do {
                vis[x] = true;
                x = p[x];
            }while(x != i);
        }
        if(cyc > maxcyc) {
            maxcyc = cyc;
            bestp = p;
        }
        int c = 0;
        while(c < 26 && !std::next_permutation(perm[c].begin(), perm[c].end())) c++;
        if(c == 26) break;
    }

    // Create the sequence of swaps based on the best permutation
    std::vector<std::pair<int, int>> ans;
    std::fill(vis.begin(), vis.end(), false);
    for(int i = 0; i < ngrid; i++) {
        if(bestp[i] == -1 || vis[i]) continue;
        int x = i;
        do {
            if(x != i) {
                ans.push_back({i, x});
            }
            vis[x] = true;
            x = bestp[x];
        }while(x != i);
    }
    return ans;
}

int main() {
    // Read dictionary of valid words, only include ones that are 5 letters
    std::ifstream dict_in("res/words5.txt");
    std::string line;
    while(std::getline(dict_in, line)) {
        if(line.length() != 5) continue;
        for(char &c : line) {
            c = toupper(c);
            assert(c >= 'A' && c <= 'Z');
        }
        dict.push_back(line);
    }
    dict_in.close();

    // Preprocess dictionary so we can check if a word with wildcards exists
    std::string temp(5, '*');
    for(std::string& s : dict) {
        for(int mask = 0; mask < (1 << 5); mask++) {
            for(int i = 0; i < 5; i++) {
                if(mask >> i & 1) {
                    temp[i] = '*';
                }else {
                    temp[i] = s[i];
                }
            }
            set.insert(temp);
        }
    }

    // Read input puzzle
    while(std::getline(std::cin, line)) {
        if(line.empty() || line[0] == '#') continue;
        assert(line.length() == 5);
        if((int) letters.size() < 5) {
            letters.push_back(line);
        }else {
            colors.push_back(line);
        }
    }
    assert((int) letters.size() == 5);
    assert((int) colors.size() == 5);

    solution.assign(5, std::string(5, '*'));
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if(colors[i][j] == 'G') {
                solution[i][j] = letters[i][j];
            }else if(letters[i][j] == ' ') {
                solution[i][j] = ' ';
            }
        }
    }
    bool is_solved = solve();
    if(!is_solved) {
        std::cout << "No solution found\n";
        return 0;
    }

    std::cout << "Solution:\n";
    for(int i = 0; i < 5; i++) {
        std::cout << solution[i] << '\n';
    }
    std::cout << std::endl;

    int index = 1;
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if(solution[i][j] == ' ') {
                std::cout << "   ";
            }else {
                grid[i][j] = index++;
                std::cout << grid[i][j] / 10 << grid[i][j] % 10 << ' ';
            }
        }
        std::cout << '\n';
    }
    std::cout << std::endl;

    std::vector<std::pair<int, int>> swaps = min_swaps();

    std::cout << "Swaps:\n";
    for(auto &pa : swaps) {
        std::cout << pa.first << ' ' << pa.second << '\n';
    }
}
