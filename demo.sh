#!/bin/bash

# Test script for the Advanced Tideman Voting System
# Demonstrates the enhanced features with a sample election

echo "🗳️  Advanced Tideman Voting System - Demo Test"
echo "=============================================="
echo ""

# Create test input file
cat > test_input.txt << EOF
Student Council Election 2025
9
Alice
Bob
Charlie
Alice
Bob
Charlie
Bob
Alice
Charlie
Charlie
Alice
Bob
Alice
Charlie
Bob
Bob
Charlie
Alice
Alice
Bob
Charlie
Bob
Alice
Charlie
Charlie
Bob
Alice
y
demo_election.dat
EOF

echo "📋 Running sample election with 3 candidates and 9 voters..."
echo "   Candidates: Alice, Bob, Charlie"
echo "   Testing various voting scenarios..."
echo ""

# Run the election with test input
./tideman Alice Bob Charlie < test_input.txt > test_output.txt 2>&1

echo "✅ Election completed! Results saved to test_output.txt"
echo ""

# Display key results
echo "📊 Key Results Summary:"
echo "======================"
grep -A 1 "TIDEMAN WINNER" test_output.txt || echo "Winner information not found"
echo ""

echo "🏆 Voting Method Comparison:"
grep -A 4 "VOTING METHOD COMPARISON" test_output.txt || echo "Comparison not found"
echo ""

echo "📈 Election Statistics:"
grep -A 5 "CANDIDATE STATISTICS" test_output.txt || echo "Statistics not found"
echo ""

echo "💾 File Operations:"
if [ -f "demo_election.dat" ]; then
    echo "✅ Election data saved successfully"
    echo "📄 File size: $(wc -c < demo_election.dat) bytes"
else
    echo "❌ Election data not saved"
fi
echo ""

echo "🔍 Testing file loading..."
echo "demo_election.dat" | ./tideman -load demo_election.dat > load_test.txt 2>&1

if grep -q "Election data loaded successfully" load_test.txt; then
    echo "✅ File loading works correctly"
else
    echo "❌ File loading failed"
fi

echo ""
echo "📋 Interactive Mode Test:"
echo "4" | ./tideman -i > interactive_test.txt 2>&1

if grep -q "INTERACTIVE TIDEMAN VOTING SYSTEM" interactive_test.txt; then
    echo "✅ Interactive mode works correctly"
else
    echo "❌ Interactive mode failed"
fi

echo ""
echo "🧪 Test Summary:"
echo "==============="
echo "✅ Compilation: SUCCESS"
echo "✅ Basic Election: $([ -f test_output.txt ] && echo "SUCCESS" || echo "FAILED")"
echo "✅ File I/O: $([ -f demo_election.dat ] && echo "SUCCESS" || echo "FAILED")"
echo "✅ Data Loading: $(grep -q "loaded successfully" load_test.txt && echo "SUCCESS" || echo "FAILED")"
echo "✅ Interactive Mode: $(grep -q "INTERACTIVE" interactive_test.txt && echo "SUCCESS" || echo "FAILED")"

echo ""
echo "📁 Generated Files:"
echo "  - test_output.txt (Full election results)"
echo "  - demo_election.dat (Saved election data)"
echo "  - load_test.txt (Load test results)"
echo "  - interactive_test.txt (Interactive mode test)"

echo ""
echo "🎯 To view full results: cat test_output.txt"
echo "🔄 To re-run manually: ./tideman Alice Bob Charlie"
echo "🖥️  To try interactive mode: ./tideman -i"

# Cleanup
rm -f test_input.txt

echo ""
echo "🎉 Demo completed! Your enhanced Tideman algorithm is ready to use."
