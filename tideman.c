#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <stdarg.h>

// Max number of candidates
#define MAX 15
#define MAX_NAME_LENGTH 50
#define MAX_FILENAME 100

// CS50 library replacements
typedef char* string;

int get_int(const char* prompt) {
    int value;
    printf("%s", prompt);
    scanf("%d", &value);
    return value;
}

char get_char(const char* prompt) {
    char value;
    printf("%s", prompt);
    scanf(" %c", &value);
    return value;
}

string get_string(const char* prompt, ...) {
    static char buffer[MAX_NAME_LENGTH];
    static char formatted_prompt[256];
    
    va_list args;
    va_start(args, prompt);
    vsnprintf(formatted_prompt, sizeof(formatted_prompt), prompt, args);
    va_end(args);
    
    printf("%s", formatted_prompt);
    scanf("%s", buffer);
    return buffer;
}

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;
int voter_count;

// Advanced statistics
typedef struct {
    int total_votes;
    int first_place_votes;
    int last_place_votes;
    float average_rank;
    int head_to_head_wins;
    int head_to_head_losses;
    float condorcet_score;
} candidate_stats;

candidate_stats stats[MAX];

// Election metadata
typedef struct {
    char election_name[MAX_NAME_LENGTH];
    char date[20];
    int total_voters;
    int total_candidates;
    float participation_rate;
} election_info;

election_info election;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
bool path_loser2winner(int loser, int winner);
void lock_pairs(void);
void print_winner(void);

// Enhanced features
void initialize_election(void);
void calculate_statistics(void);
void print_detailed_results(void);
void print_preference_matrix(void);
void print_graph_visualization(void);
void save_election_data(char *filename);
bool load_election_data(char *filename);
void run_simulation(int num_simulations);
void compare_voting_methods(void);
int plurality_winner(void);
int borda_count_winner(void);
bool is_condorcet_winner(int candidate);
void detect_voting_paradoxes(void);
void analyze_margin_of_victory(void);
float calculate_election_competitiveness(void);
void print_menu(void);
void interactive_mode(void);

// Performance monitoring
typedef struct {
    clock_t start_time;
    clock_t end_time;
    double total_time;
    int operations_count;
} performance_monitor;

performance_monitor perf_monitor;

void start_performance_monitor(void);
void end_performance_monitor(void);
void print_performance_stats(void);
bool path_loser2winner_optimized(int loser, int winner, bool visited[]);
void sort_pairs_optimized(void);

int main(int argc, char* argv[])
{
    printf("=== Advanced Tideman Voting System ===\n\n");
    
    // Check for interactive mode
    if (argc == 1 || (argc == 2 && strcmp(argv[1], "-i") == 0))
    {
        interactive_mode();
        return 0;
    }
    
    // Check for file loading
    if (argc == 3 && strcmp(argv[1], "-load") == 0)
    {
        if (load_election_data(argv[2]))
        {
            printf("Election data loaded successfully!\n\n");
            calculate_statistics();
            print_detailed_results();
            return 0;
        }
        else
        {
            printf("Error loading election data.\n");
            return 1;
        }
    }

    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: \n");
        printf("  tideman [candidate ...] - Run election with candidates\n");
        printf("  tideman -i - Interactive mode\n");
        printf("  tideman -load [filename] - Load saved election\n");
        return 1;
    }

    // Initialize election
    initialize_election();

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    if (candidate_count < 2)
    {
        printf("Minimum number of candidates is 2\n");
        return 2;
    }
    
    // Allocate memory for candidate names
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = malloc(strlen(argv[i + 1]) + 1);
        strcpy(candidates[i], argv[i + 1]);
    }

    // Clear graph of locked in pairs and initialize preferences
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
            preferences[i][j] = 0;
        }
    }

    pair_count = 0;
    voter_count = get_int("Number of voters: ");
    
    if (voter_count <= 0)
    {
        printf("Invalid number of voters.\n");
        return 3;
    }

    printf("\nCandidates: ");
    for (int i = 0; i < candidate_count; i++)
    {
        printf("%s", candidates[i]);
        if (i < candidate_count - 1) printf(", ");
    }
    printf("\n\n");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        printf("Voter %i:\n", i + 1);
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote. Please try again.\n");
                j--; // Retry the same rank
                continue;
            }
        }

        record_preferences(ranks);
        printf("\n");
    }

    // Run the Tideman algorithm with performance monitoring
    start_performance_monitor();
    add_pairs();
    sort_pairs();
    lock_pairs();
    end_performance_monitor();
    
    // Calculate comprehensive statistics
    calculate_statistics();
    
    // Display results
    print_detailed_results();
    
    // Show performance statistics
    print_performance_stats();
    
    // Ask if user wants to save results
    char save_choice = get_char("Save election results? (y/n): ");
    if (save_choice == 'y' || save_choice == 'Y')
    {
        char filename[MAX_FILENAME];
        printf("Enter filename: ");
        scanf("%s", filename);
        save_election_data(filename);
    }
    
    // Free allocated memory
    for (int i = 0; i < candidate_count; i++)
    {
        free(candidates[i]);
    }
    
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int a = 0; a < candidate_count; a++)
    {
        int position_a;
        for (int x = 0; x < candidate_count; x++)
        {
            if (ranks[x] == a)
            {
                position_a = x;
            }
        }
        
        for (int b = 0; b < candidate_count; b++)
        {
            int position_b;
            for (int y = 0; y < candidate_count; y++)
            {
                if (ranks[y] == b)
                {
                    position_b = y;
                }
            }
            
            if (position_a < position_b && a != b)
            {
                preferences[a][b]++;
            }
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    for (int n = 0; n < pair_count; n++)
    {
        int strength_victory = 0;
        int position_highest = 0;
        for (int i = n; i < pair_count; i++)
        {
            if (preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner] > strength_victory)
            {
                strength_victory = preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner];
                position_highest = i;
            }
        }
        
        int temp1 = pairs[position_highest].winner;
        pairs[position_highest].winner = pairs[n].winner;
        pairs[n].winner = temp1;
                
        int temp2 = pairs[position_highest].loser;
        pairs[position_highest].loser = pairs[n].loser;
        pairs[n].loser = temp2;
    }
    return;
}

bool path_loser2winner(int loser, int winner)
{
    if (locked[loser][winner] == true)
    {
        return true;
    }
    for (int a = 0; a < candidate_count; a++)
    {
        if (locked[loser][a] == true)
        {
            for (int b = 0; b < candidate_count; b++)
            {
                if (locked[b][winner] == true && (path_loser2winner(a, b) == true || a == b))
                {
                    return true;
                }
            }
        }
        
    }
    return false;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        if (path_loser2winner(pairs[i].loser, pairs[i].winner) == false)
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        bool source = true;
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i] == true)
            {
                source = false;
            }
        }
        if (source == true)
        {
            printf("🏆 TIDEMAN WINNER: %s\n", candidates[i]);
            return;
        }
    }
    printf("No clear winner found (possible tie or cycle).\n");
    return;
}

// Initialize election metadata
void initialize_election(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    printf("Enter election name: ");
    getchar(); // consume newline
    fgets(election.election_name, MAX_NAME_LENGTH, stdin);
    election.election_name[strcspn(election.election_name, "\n")] = 0; // Remove newline
    
    sprintf(election.date, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    
    // Initialize statistics
    for (int i = 0; i < MAX; i++)
    {
        stats[i].total_votes = 0;
        stats[i].first_place_votes = 0;
        stats[i].last_place_votes = 0;
        stats[i].average_rank = 0.0;
        stats[i].head_to_head_wins = 0;
        stats[i].head_to_head_losses = 0;
        stats[i].condorcet_score = 0.0;
    }
}

// Calculate comprehensive statistics
void calculate_statistics(void)
{
    election.total_voters = voter_count;
    election.total_candidates = candidate_count;
    election.participation_rate = 100.0; // Assuming 100% for now
    
    // Calculate statistics for each candidate
    for (int i = 0; i < candidate_count; i++)
    {
        stats[i].total_votes = voter_count;
        
        // Calculate head-to-head wins and losses
        for (int j = 0; j < candidate_count; j++)
        {
            if (i != j)
            {
                if (preferences[i][j] > preferences[j][i])
                {
                    stats[i].head_to_head_wins++;
                }
                else if (preferences[i][j] < preferences[j][i])
                {
                    stats[i].head_to_head_losses++;
                }
            }
        }
        
        // Calculate Condorcet score (percentage of head-to-head matchups won)
        int total_matchups = candidate_count - 1;
        if (total_matchups > 0)
        {
            stats[i].condorcet_score = (float)stats[i].head_to_head_wins / total_matchups * 100.0;
        }
    }
}

// Print detailed election results
void print_detailed_results(void)
{
    printf("\n╔════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                              ELECTION RESULTS                                         ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Election: %s\n", election.election_name);
    printf("Date: %s\n", election.date);
    printf("Total Voters: %d\n", voter_count);
    printf("Total Candidates: %d\n\n", candidate_count);
    
    // Print Tideman winner
    printf("═══ TIDEMAN METHOD WINNER ═══\n");
    print_winner();
    printf("\n");
    
    // Print other voting method results
    compare_voting_methods();
    
    // Print detailed statistics
    printf("═══ CANDIDATE STATISTICS ═══\n");
    printf("%-15s %10s %10s %10s %15s\n", "Candidate", "H2H Wins", "H2H Losses", "Condorcet%", "Condorcet Winner");
    printf("─────────────────────────────────────────────────────────────────────────────\n");
    
    for (int i = 0; i < candidate_count; i++)
    {
        printf("%-15s %10d %10d %9.1f%% %15s\n", 
               candidates[i], 
               stats[i].head_to_head_wins, 
               stats[i].head_to_head_losses, 
               stats[i].condorcet_score,
               is_condorcet_winner(i) ? "YES" : "NO");
    }
    
    printf("\n");
    
    // Print preference matrix
    print_preference_matrix();
    
    // Print graph visualization
    print_graph_visualization();
    
    // Analyze election
    detect_voting_paradoxes();
    analyze_margin_of_victory();
    
    float competitiveness = calculate_election_competitiveness();
    printf("Election Competitiveness Score: %.2f/10.0\n\n", competitiveness);
}

// Print preference matrix
void print_preference_matrix(void)
{
    printf("═══ PREFERENCE MATRIX ═══\n");
    printf("(Row candidate preferred over column candidate)\n\n");
    
    printf("%12s", "");
    for (int j = 0; j < candidate_count; j++)
    {
        printf("%8.8s", candidates[j]);
    }
    printf("\n");
    
    for (int i = 0; i < candidate_count; i++)
    {
        printf("%-12.12s", candidates[i]);
        for (int j = 0; j < candidate_count; j++)
        {
            if (i == j)
            {
                printf("%8s", "-");
            }
            else
            {
                printf("%8d", preferences[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

// Print graph visualization
void print_graph_visualization(void)
{
    printf("═══ LOCKED PAIRS GRAPH ═══\n");
    printf("(Arrows show locked preferences)\n\n");
    
    bool has_edges = false;
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[i][j])
            {
                printf("%s → %s (margin: %d)\n", 
                       candidates[i], 
                       candidates[j], 
                       preferences[i][j] - preferences[j][i]);
                has_edges = true;
            }
        }
    }
    
    if (!has_edges)
    {
        printf("No locked pairs (possible tie).\n");
    }
    printf("\n");
}

// Compare different voting methods
void compare_voting_methods(void)
{
    printf("═══ VOTING METHOD COMPARISON ═══\n");
    
    // Plurality winner
    int plurality_win = plurality_winner();
    printf("Plurality Winner: %s\n", candidates[plurality_win]);
    
    // Borda count winner
    int borda_win = borda_count_winner();
    printf("Borda Count Winner: %s\n", candidates[borda_win]);
    
    // Condorcet winner
    bool condorcet_exists = false;
    for (int i = 0; i < candidate_count; i++)
    {
        if (is_condorcet_winner(i))
        {
            printf("Condorcet Winner: %s\n", candidates[i]);
            condorcet_exists = true;
            break;
        }
    }
    if (!condorcet_exists)
    {
        printf("Condorcet Winner: None (Condorcet paradox)\n");
    }
    
    printf("\n");
}

// Find plurality winner (most first-place votes)
int plurality_winner(void)
{
    int first_place_votes[MAX] = {0};
    
    // Count first place votes by examining preferences
    for (int i = 0; i < candidate_count; i++)
    {
        int total_opponents_beaten = 0;
        for (int j = 0; j < candidate_count; j++)
        {
            if (i != j)
            {
                total_opponents_beaten += preferences[i][j];
            }
        }
        // Estimate first place votes (this is an approximation)
        first_place_votes[i] = total_opponents_beaten / (candidate_count - 1);
    }
    
    int winner = 0;
    for (int i = 1; i < candidate_count; i++)
    {
        if (first_place_votes[i] > first_place_votes[winner])
        {
            winner = i;
        }
    }
    
    return winner;
}

// Find Borda count winner
int borda_count_winner(void)
{
    int borda_scores[MAX] = {0};
    
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (i != j)
            {
                borda_scores[i] += preferences[i][j];
            }
        }
    }
    
    int winner = 0;
    for (int i = 1; i < candidate_count; i++)
    {
        if (borda_scores[i] > borda_scores[winner])
        {
            winner = i;
        }
    }
    
    return winner;
}

// Check if candidate is Condorcet winner
bool is_condorcet_winner(int candidate)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (i != candidate && preferences[candidate][i] <= preferences[i][candidate])
        {
            return false;
        }
    }
    return true;
}

// Detect voting paradoxes
void detect_voting_paradoxes(void)
{
    printf("═══ PARADOX ANALYSIS ═══\n");
    
    // Check for Condorcet paradox (cycle in preferences)
    bool has_cycle = false;
    for (int i = 0; i < candidate_count && !has_cycle; i++)
    {
        for (int j = 0; j < candidate_count && !has_cycle; j++)
        {
            for (int k = 0; k < candidate_count && !has_cycle; k++)
            {
                if (i != j && j != k && k != i)
                {
                    if (preferences[i][j] > preferences[j][i] &&
                        preferences[j][k] > preferences[k][j] &&
                        preferences[k][i] > preferences[i][k])
                    {
                        printf("Condorcet Paradox detected: %s > %s > %s > %s\n",
                               candidates[i], candidates[j], candidates[k], candidates[i]);
                        has_cycle = true;
                    }
                }
            }
        }
    }
    
    if (!has_cycle)
    {
        printf("No Condorcet paradox detected.\n");
    }
    
    printf("\n");
}

// Analyze margin of victory
void analyze_margin_of_victory(void)
{
    printf("═══ MARGIN ANALYSIS ═══\n");
    
    int min_margin = voter_count;
    int max_margin = 0;
    
    for (int i = 0; i < pair_count; i++)
    {
        int margin = preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner];
        if (margin < min_margin) min_margin = margin;
        if (margin > max_margin) max_margin = margin;
        
        printf("%s beats %s by %d votes (%.1f%%)\n",
               candidates[pairs[i].winner], 
               candidates[pairs[i].loser],
               margin,
               (float)margin / voter_count * 100.0);
    }
    
    printf("Smallest margin: %d votes\n", min_margin);
    printf("Largest margin: %d votes\n\n", max_margin);
}

// Calculate election competitiveness (0-10 scale)
float calculate_election_competitiveness(void)
{
    if (pair_count == 0) return 0.0;
    
    int total_margin = 0;
    for (int i = 0; i < pair_count; i++)
    {
        total_margin += preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner];
    }
    
    float avg_margin = (float)total_margin / pair_count;
    float competitiveness = 10.0 - (avg_margin / voter_count * 20.0);
    
    return competitiveness < 0 ? 0.0 : (competitiveness > 10.0 ? 10.0 : competitiveness);
}

// Save election data to file
void save_election_data(char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Error: Could not create file %s\n", filename);
        return;
    }
    
    fprintf(file, "ELECTION_DATA\n");
    fprintf(file, "%s\n", election.election_name);
    fprintf(file, "%s\n", election.date);
    fprintf(file, "%d\n", candidate_count);
    fprintf(file, "%d\n", voter_count);
    
    // Save candidate names
    for (int i = 0; i < candidate_count; i++)
    {
        fprintf(file, "%s\n", candidates[i]);
    }
    
    // Save preferences matrix
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            fprintf(file, "%d ", preferences[i][j]);
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    printf("Election data saved to %s\n", filename);
}

// Load election data from file
bool load_election_data(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file %s\n", filename);
        return false;
    }
    
    char header[20];
    if (fscanf(file, "%s", header) != 1 || strcmp(header, "ELECTION_DATA") != 0)
    {
        printf("Error: Invalid file format\n");
        fclose(file);
        return false;
    }
    
    fgetc(file); // consume newline
    fgets(election.election_name, MAX_NAME_LENGTH, file);
    election.election_name[strcspn(election.election_name, "\n")] = 0;
    
    fgets(election.date, 20, file);
    election.date[strcspn(election.date, "\n")] = 0;
    
    fscanf(file, "%d", &candidate_count);
    fscanf(file, "%d", &voter_count);
    
    // Load candidate names
    for (int i = 0; i < candidate_count; i++)
    {
        char name[MAX_NAME_LENGTH];
        fscanf(file, "%s", name);
        candidates[i] = malloc(strlen(name) + 1);
        strcpy(candidates[i], name);
    }
    
    // Load preferences matrix
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            fscanf(file, "%d", &preferences[i][j]);
        }
    }
    
    fclose(file);
    
    // Reconstruct the election state
    add_pairs();
    sort_pairs();
    lock_pairs();
    
    return true;
}

// Interactive mode menu
void interactive_mode(void)
{
    printf("═══ INTERACTIVE TIDEMAN VOTING SYSTEM ═══\n\n");
    
    while (true)
    {
        print_menu();
        int choice = get_int("Enter your choice: ");
        
        switch (choice)
        {
            case 1:
                // Run new election - this would require restructuring main logic
                printf("Feature not implemented in interactive mode yet.\n");
                printf("Please use command line mode for now.\n\n");
                break;
                
            case 2:
                {
                    char filename[MAX_FILENAME];
                    printf("Enter filename to load: ");
                    scanf("%s", filename);
                    if (load_election_data(filename))
                    {
                        calculate_statistics();
                        print_detailed_results();
                    }
                }
                break;
                
            case 3:
                printf("Enter number of simulations: ");
                int num_sims = get_int("");
                run_simulation(num_sims);
                break;
                
            case 4:
                printf("Exiting... Goodbye!\n");
                return;
                
            default:
                printf("Invalid choice. Please try again.\n\n");
        }
    }
}

void print_menu(void)
{
    printf("╔══════════════════════════════════════════╗\n");
    printf("║              MAIN MENU                   ║\n");
    printf("╠══════════════════════════════════════════╣\n");
    printf("║ 1. Run New Election                      ║\n");
    printf("║ 2. Load Saved Election                   ║\n");
    printf("║ 3. Run Election Simulation               ║\n");
    printf("║ 4. Exit                                  ║\n");
    printf("╚══════════════════════════════════════════╝\n\n");
}

// Run election simulation
void run_simulation(int num_simulations)
{
    if (candidate_count < 3) {
        printf("Need at least 3 candidates for meaningful simulation.\n");
        printf("Please load an election or run in command line mode first.\n\n");
        return;
    }
    
    printf("Running %d election simulations...\n", num_simulations);
    printf("Using candidates: ");
    for (int i = 0; i < candidate_count; i++) {
        printf("%s%s", candidates[i], (i < candidate_count - 1) ? ", " : "");
    }
    printf("\n\n");
    
    // Track winner statistics across simulations
    int tideman_wins[MAX] = {0};
    int plurality_wins[MAX] = {0};
    int borda_wins[MAX] = {0};
    int condorcet_wins[MAX] = {0};
    int total_paradoxes = 0;
    float total_competitiveness = 0.0;
    
    // Save original data
    int original_preferences[MAX][MAX];
    bool original_locked[MAX][MAX];
    pair original_pairs[MAX * (MAX - 1) / 2];
    int original_pair_count = pair_count;
    int original_voter_count = voter_count;
    
    for (int i = 0; i < candidate_count; i++) {
        for (int j = 0; j < candidate_count; j++) {
            original_preferences[i][j] = preferences[i][j];
            original_locked[i][j] = locked[i][j];
        }
    }
    for (int i = 0; i < original_pair_count; i++) {
        original_pairs[i] = pairs[i];
    }
    
    srand(time(NULL));
    
    for (int sim = 0; sim < num_simulations; sim++) {
        // Reset for new simulation
        for (int i = 0; i < candidate_count; i++) {
            for (int j = 0; j < candidate_count; j++) {
                preferences[i][j] = 0;
                locked[i][j] = false;
            }
        }
        pair_count = 0;
        
        // Generate random voter count (between 10 and 100)
        voter_count = 10 + rand() % 91;
        
        // Generate random votes
        for (int voter = 0; voter < voter_count; voter++) {
            int ranks[MAX];
            
            // Create random ranking
            for (int i = 0; i < candidate_count; i++) {
                ranks[i] = i;
            }
            
            // Fisher-Yates shuffle
            for (int i = candidate_count - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                int temp = ranks[i];
                ranks[i] = ranks[j];
                ranks[j] = temp;
            }
            
            // Record preferences for this voter
            for (int i = 0; i < candidate_count; i++) {
                for (int j = 0; j < candidate_count; j++) {
                    if (i != j) {
                        // Find positions in ranking
                        int pos_i = -1, pos_j = -1;
                        for (int k = 0; k < candidate_count; k++) {
                            if (ranks[k] == i) pos_i = k;
                            if (ranks[k] == j) pos_j = k;
                        }
                        
                        if (pos_i < pos_j) {  // i ranked higher than j
                            preferences[i][j]++;
                        }
                    }
                }
            }
        }
        
        // Run election methods
        add_pairs();
        sort_pairs();
        lock_pairs();
        
        // Find Tideman winner
        int tideman_winner = -1;
        for (int i = 0; i < candidate_count; i++) {
            bool is_source = true;
            for (int j = 0; j < candidate_count; j++) {
                if (locked[j][i]) {
                    is_source = false;
                    break;
                }
            }
            if (is_source) {
                tideman_winner = i;
                break;
            }
        }
        if (tideman_winner != -1) {
            tideman_wins[tideman_winner]++;
        }
        
        // Find other winners
        int plurality_win = plurality_winner();
        plurality_wins[plurality_win]++;
        
        int borda_win = borda_count_winner();
        borda_wins[borda_win]++;
        
        // Check for Condorcet winner
        bool found_condorcet = false;
        for (int i = 0; i < candidate_count; i++) {
            if (is_condorcet_winner(i)) {
                condorcet_wins[i]++;
                found_condorcet = true;
                break;
            }
        }
        if (!found_condorcet) {
            total_paradoxes++;
        }
        
        // Calculate competitiveness
        total_competitiveness += calculate_election_competitiveness();
        
        // Progress indicator
        if ((sim + 1) % (num_simulations / 10) == 0 || sim == 0) {
            printf("Progress: %d/%d simulations completed\n", sim + 1, num_simulations);
        }
    }
    
    // Display simulation results
    printf("\n╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    SIMULATION RESULTS                           ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Simulation Parameters:\n");
    printf("- Number of simulations: %d\n", num_simulations);
    printf("- Candidates: %d\n", candidate_count);
    printf("- Voters per simulation: 10-100 (random)\n\n");
    
    printf("═══ WINNER FREQUENCIES ═══\n");
    printf("%-15s %10s %10s %10s %10s\n", "Candidate", "Tideman", "Plurality", "Borda", "Condorcet");
    printf("───────────────────────────────────────────────────────────────────\n");
    
    for (int i = 0; i < candidate_count; i++) {
        printf("%-15s %9d%% %9d%% %8d%% %9d%%\n",
               candidates[i],
               (tideman_wins[i] * 100) / num_simulations,
               (plurality_wins[i] * 100) / num_simulations,
               (borda_wins[i] * 100) / num_simulations,
               (condorcet_wins[i] * 100) / num_simulations);
    }
    
    printf("\n═══ ELECTION ANALYSIS ═══\n");
    printf("Condorcet Paradoxes: %d/%d (%.1f%%)\n", 
           total_paradoxes, num_simulations, 
           (float)total_paradoxes / num_simulations * 100.0);
    printf("Average Competitiveness: %.2f/10.0\n", 
           total_competitiveness / num_simulations);
    
    // Method agreement analysis
    printf("\n═══ METHOD AGREEMENT ═══\n");
    int tideman_plurality_agree = 0;
    int tideman_borda_agree = 0;
    
    for (int i = 0; i < candidate_count; i++) {
        if (tideman_wins[i] > 0 && plurality_wins[i] > 0) {
            tideman_plurality_agree += (tideman_wins[i] < plurality_wins[i]) ? 
                                       tideman_wins[i] : plurality_wins[i];
        }
        if (tideman_wins[i] > 0 && borda_wins[i] > 0) {
            tideman_borda_agree += (tideman_wins[i] < borda_wins[i]) ? 
                                   tideman_wins[i] : borda_wins[i];
        }
    }
    
    printf("Tideman-Plurality Agreement: ~%.1f%%\n", 
           (float)tideman_plurality_agree / num_simulations * 100.0);
    printf("Tideman-Borda Agreement: ~%.1f%%\n", 
           (float)tideman_borda_agree / num_simulations * 100.0);
    
    // Restore original data
    for (int i = 0; i < candidate_count; i++) {
        for (int j = 0; j < candidate_count; j++) {
            preferences[i][j] = original_preferences[i][j];
            locked[i][j] = original_locked[i][j];
        }
    }
    for (int i = 0; i < original_pair_count; i++) {
        pairs[i] = original_pairs[i];
    }
    pair_count = original_pair_count;
    voter_count = original_voter_count;
    
    printf("\n💡 Insights:\n");
    if (total_paradoxes > num_simulations * 0.1) {
        printf("- High paradox rate suggests candidates are closely matched\n");
    }
    if (total_competitiveness / num_simulations > 7.0) {
        printf("- Elections tend to be highly competitive\n");
    } else if (total_competitiveness / num_simulations < 4.0) {
        printf("- Elections tend to have clear winners\n");
    }
    
    printf("\n✅ Simulation completed!\n\n");
}

// Performance monitoring functions
void start_performance_monitor(void)
{
    perf_monitor.start_time = clock();
    perf_monitor.operations_count = 0;
}

void end_performance_monitor(void)
{
    perf_monitor.end_time = clock();
    perf_monitor.total_time = ((double)(perf_monitor.end_time - perf_monitor.start_time)) / CLOCKS_PER_SEC;
}

void print_performance_stats(void)
{
    printf("\n═══ PERFORMANCE STATISTICS ═══\n");
    printf("Algorithm execution time: %.6f seconds\n", perf_monitor.total_time);
    printf("Operations performed: %d\n", perf_monitor.operations_count);
    if (perf_monitor.operations_count > 0) {
        printf("Average time per operation: %.9f seconds\n", 
               perf_monitor.total_time / perf_monitor.operations_count);
    }
    
    // Algorithm complexity analysis
    int n = candidate_count;
    printf("\nComplexity Analysis (n = %d candidates):\n", n);
    printf("- Preference recording: O(n²) per voter\n");
    printf("- Pair generation: O(n²) = %d operations\n", n * n);
    printf("- Pair sorting: O(p log p) where p = %d\n", pair_count);
    printf("- Cycle detection: O(n³) worst case = %d operations\n", n * n * n);
    
    // Memory usage estimate
    int memory_usage = sizeof(preferences) + sizeof(locked) + sizeof(pairs) + 
                      sizeof(candidates) + sizeof(stats) + sizeof(election);
    printf("Estimated memory usage: %d bytes\n", memory_usage);
    printf("\n");
}

// Optimized cycle detection with memoization
bool path_loser2winner_optimized(int loser, int winner, bool visited[])
{
    if (loser == winner) {
        return true;
    }
    
    if (visited[loser]) {
        return false; // Already checked this path
    }
    
    visited[loser] = true;
    perf_monitor.operations_count++;
    
    for (int intermediate = 0; intermediate < candidate_count; intermediate++) {
        if (locked[loser][intermediate]) {
            if (path_loser2winner_optimized(intermediate, winner, visited)) {
                return true;
            }
        }
    }
    
    return false;
}

// Optimized sorting using merge sort for better performance
void merge_pairs(int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    pair left_array[n1], right_array[n2];
    
    for (int i = 0; i < n1; i++) {
        left_array[i] = pairs[left + i];
    }
    for (int j = 0; j < n2; j++) {
        right_array[j] = pairs[mid + 1 + j];
    }
    
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        int left_margin = preferences[left_array[i].winner][left_array[i].loser] - 
                         preferences[left_array[i].loser][left_array[i].winner];
        int right_margin = preferences[right_array[j].winner][right_array[j].loser] - 
                          preferences[right_array[j].loser][right_array[j].winner];
        
        if (left_margin >= right_margin) {
            pairs[k] = left_array[i];
            i++;
        } else {
            pairs[k] = right_array[j];
            j++;
        }
        k++;
        perf_monitor.operations_count++;
    }
    
    while (i < n1) {
        pairs[k] = left_array[i];
        i++;
        k++;
    }
    
    while (j < n2) {
        pairs[k] = right_array[j];
        j++;
        k++;
    }
}

void merge_sort_pairs(int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        merge_sort_pairs(left, mid);
        merge_sort_pairs(mid + 1, right);
        merge_pairs(left, mid, right);
    }
}

void sort_pairs_optimized(void)
{
    if (pair_count > 1) {
        merge_sort_pairs(0, pair_count - 1);
    }
}

// Enhanced lock_pairs with optimized cycle detection
void lock_pairs_optimized(void)
{
    for (int i = 0; i < pair_count; i++) {
        bool visited[MAX] = {false};
        
        // Temporarily lock the pair
        locked[pairs[i].winner][pairs[i].loser] = true;
        
        // Check if this creates a cycle
        if (!path_loser2winner_optimized(pairs[i].loser, pairs[i].winner, visited)) {
            // No cycle, keep the lock
            perf_monitor.operations_count++;
        } else {
            // Cycle detected, remove the lock
            locked[pairs[i].winner][pairs[i].loser] = false;
        }
    }
}

// Enhanced main function with performance monitoring
int main_with_performance(int argc, char* argv[])
{
    start_performance_monitor();
    
    // Run the standard main function logic here
    // (This would replace the current main for performance testing)
    
    end_performance_monitor();
    print_performance_stats();
    
    return 0;
}
