# Tideman Voting Algorithm

A comprehensive implementation of the Tideman (ranked pairs) voting method with advanced analytics, multiple voting system comparisons, and election simulation capabilities.

## 🏆 Overview

The Tideman voting method is a ranked-choice voting system that guarantees to elect the Condorcet winner when one exists. This implementation extends beyond the basic algorithm to provide a complete voting analysis platform with professional-grade features and insights.

## ✨ Key Features

- **🗳️ Tideman Algorithm**: Full implementation with cycle detection and graph optimization
- **📊 Advanced Analytics**: Comprehensive statistics, competitiveness scoring, and margin analysis  
- **🔍 Multiple Methods**: Compare results from Tideman, Plurality, Borda Count, and Condorcet voting
- **🎲 Election Simulation**: Monte Carlo simulation with statistical analysis across thousands of elections
- **💾 Data Persistence**: Save and load election results for future analysis
- **🖥️ Interactive Interface**: User-friendly command-line interface with multiple operation modes
- **⚡ Performance Monitoring**: Algorithm optimization with execution time and complexity analysis

## 🚀 Quick Start

### Compilation
```bash
gcc -o tideman tideman.c
```

### Basic Usage
```bash
# Run an election with candidates
./tideman Alice Bob Charlie

# Interactive mode
./tideman -i

# Load saved election data
./tideman -load election_data.txt

# Run demo with test cases
./demo.sh
```

## 📊 Example Output

```
🏆 TIDEMAN WINNER: Alice

═══ VOTING METHOD COMPARISON ═══
Plurality Winner: Alice
Borda Count Winner: Bob  
Condorcet Winner: Alice

═══ CANDIDATE STATISTICS ═══
Candidate     H2H Wins  H2H Losses  Condorcet%  Condorcet Winner
────────────────────────────────────────────────────────────────
Alice              2           0      100.0%              YES
Bob                1           1       50.0%               NO  
Charlie            0           2        0.0%               NO

Election Competitiveness Score: 7.2/10.0
```

## 🎯 Use Cases

- **Academic Research**: Analyze voting theory and social choice mechanisms
- **Educational Tool**: Demonstrate different voting methods and their outcomes
- **Election Analysis**: Compare how different voting systems would affect real elections
- **Algorithm Study**: Understand graph algorithms, cycle detection, and optimization techniques

## 📁 Project Structure

```
tideman-voting-algorithm/
├── README.md              # This file - basic overview
├── DOCUMENTATION.md       # Detailed project explanation and theory
├── tideman.c             # Main implementation
├── tideman               # Compiled executable  
└── demo.sh               # Demonstration script
```

## 🔬 Technical Highlights

- **Algorithm Complexity**: O(n³) cycle detection with optimization techniques
- **Memory Efficient**: Optimized data structures for large candidate sets
- **Robust Error Handling**: Comprehensive input validation and edge case management
- **Modular Design**: Clean separation of concerns with well-documented functions

## 📚 Learn More

For detailed information about the project background, voting theory, implementation challenges, and technical deep-dive, see [Tideman_explanation.md](Tideman_explanation.md).

## 🎓 Academic Context

This project demonstrates advanced computer science concepts including:
- Graph algorithms and cycle detection
- Computational social choice theory
- Algorithm optimization and performance analysis
- Statistical simulation and Monte Carlo methods
- Software engineering best practices

---

**Note**: This implementation is designed for educational and research purposes, showcasing advanced programming techniques and voting theory concepts.
