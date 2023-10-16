#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <queue>
#include <map>
#include <limits>
#include <sstream>

#define For(i,a,b) for(int i = a; i < b; i++)
using namespace std;
unsigned long long LLINF = numeric_limits<unsigned long long>::max();

struct AssignmentState
{
    int personIndex;
    int pts;
    int e1;
    int e2;
    int e3;
    int e4;
    int e5;
    int e6;
    unsigned long long aid;           // Index of this state in the allstates vector
    unsigned long long pid = LLINF;   // Index of the best state that led to this state
};

/*** Overloads the comparison operator for the custom AssignmentState struct to be processed in the right order for the priority queue.
 *   PQ elements will be processed in ascending order based on person index, or in descending order of points if two states are
 *   for the same person index.  ***/
bool operator <(const AssignmentState& x, const AssignmentState& y) {
    int ypts = -1*y.pts;
    int xpts = -1*x.pts;
    return std::tie(x.personIndex, xpts, x.e1, x.e2, x.e3, x.e4, x.e5, x.e6) > std::tie(y.personIndex, ypts, y.e1, y.e2, y.e3, y.e4, y.e5, y.e6);
}

// Overload << operator for debugging
ostream& operator<<(ostream& os, const AssignmentState& dps) {
    return os << "<" << dps.personIndex << ", "
              << dps.pts << ", "
              << dps.e1 << ", "
              << dps.e2 << ", "
              << dps.e3 << ", "
              << dps.e4 << ", "
              << dps.e5 << ", "
              << dps.e6 << ">";
}

// Overload the << operator for vectors to easily print vectors in the console.
template < class T >
std::ostream& operator << (std::ostream& os, const std::vector<T>& v)
{
    os << "[";
    for (auto ii = v.begin(); ii != v.end(); ++ii)
    {
        os << " " << *ii;
    }
    os << "]";
    return os;
}

int numberOfPeople;
int numberOfEvents = 6;
int eventMax;
int eventMin;

/*** Reads the preference rankings of a team from a comma separated values file. ***/
pair<vector<vector<int>>, vector<string>> readCSV(const string& filepath)
{
    ifstream fin(filepath);
    string line;
    getline(fin,line); // Skip the header line in the file

    vector<vector<int>> preferenceArray;
    vector<string> names;
    while(getline(fin, line))
    {
        istringstream linestream(line);
        preferenceArray.emplace_back(6,0);
        for (int i = 0; i <= numberOfEvents; i++)
        {
            string nextData;
            getline(linestream, nextData, ',');
            if (i == 0)
                names.push_back(nextData);
            else
                preferenceArray.at(preferenceArray.size()-1).at(i-1) = stoi(nextData);
        }
    }

    return make_pair(preferenceArray, names);
}

vector<vector<int>> eventRankings;
priority_queue<AssignmentState> processingQueue;
vector<AssignmentState> allStates;

int bestAssignmentScore = numeric_limits<int>::min();
vector<int> bestCapacities(numberOfEvents, 0);
unsigned long long bestPrevID = -1; // bestPrevID is the AssignmentState ID number used to match names to events at the end

/***  Adds a person to a team assignment and determines the resulting assignment score and event counts
 *    input: AssignmentState (person index, assignment score, # of students currently in events 1 to 6)
 *    output: Updates the processingQueue, allStates, bestAssignmentScore, bestCapacities, and bestPrevID global variables ***/
void processPerson(const AssignmentState& curstate)
{
    vector<int> eventCapacity {curstate.e1, curstate.e2, curstate.e3, curstate.e4, curstate.e5, curstate.e6};

    // Iterate through all possible event selections for current person
    for (int possibleEvent = 0; possibleEvent < numberOfEvents; possibleEvent++)
    {
        // Ensure that event capacity is within limit if this person takes possibleEvent
        if (eventCapacity.at(possibleEvent) + 1 > eventMax)
            break;

        // Add points for this person taking event in consideration
        int addedPts = 0;
        addedPts += eventRankings.at(curstate.personIndex).at(possibleEvent);

        // Determine the new event counts if this person takes possibleEvent
        int newE1 = curstate.e1 + (possibleEvent == 0 ? 1 : 0);
        int newE2 = curstate.e2 + (possibleEvent == 1 ? 1 : 0);
        int newE3 = curstate.e3 + (possibleEvent == 2 ? 1 : 0);
        int newE4 = curstate.e4 + (possibleEvent == 3 ? 1 : 0);
        int newE5 = curstate.e5 + (possibleEvent == 4 ? 1 : 0);
        int newE6 = curstate.e6 + (possibleEvent == 5 ? 1 : 0);
        AssignmentState newState {curstate.personIndex + 1, curstate.pts + addedPts,
                                  newE1, newE2, newE3, newE4, newE5, newE6, allStates.size(), curstate.aid};
        // The allstates vector is used later to match the names of the people with their events
        allStates.push_back(newState);

        if (curstate.personIndex < numberOfPeople - 1) {
            // Add this state to the priority queue to continue exploring if there are more people to process
            processingQueue.emplace(newState);
        }
        else if (curstate.pts + addedPts > bestAssignmentScore
                 && min({newE1, newE2, newE3, newE4, newE5, newE6}) >= eventMin) {
            // Mark this state as the best if it has the highest score and meets the event capacity criteria
            bestAssignmentScore = curstate.pts + addedPts;
            bestCapacities = {newE1, newE2, newE3, newE4, newE5, newE6 };
            bestPrevID = newState.aid;
        }
    }
}

int main() {

    /*** INPUT: Reading information from CSV file ***/
    cout << "Enter filepath of CSV data file to read: " << endl;
    string inputFilepath;
    getline(cin, inputFilepath);
    
    pair<vector<vector<int>>, vector<string>> csvdata = readCSV(inputFilepath);
    eventRankings = csvdata.first;
    vector<string> names = csvdata.second;

    numberOfPeople = eventRankings.size();
    eventMax = (numberOfPeople / numberOfEvents) + 1;
    eventMin = (numberOfPeople / numberOfEvents) - 1;

    bool ******visited = (bool******)malloc((eventMax+1)*sizeof(bool*****)); //[eventMax+1][eventMax+1][eventMax+1][eventMax+1][eventMax+1][eventMax+1];
    For(i, 0, eventMax+2)
    {
        visited[i] = (bool*****)malloc((eventMax+1)*sizeof(bool****));
        For(j, 0, eventMax+2)
        {
            visited[i][j] = (bool****)malloc((eventMax+1)*sizeof(bool***));
            For(k, 0, eventMax+2)
            {
                visited[i][j][k] = (bool***)malloc((eventMax+1)*sizeof(bool**));
                For(l, 0, eventMax+2)
                {
                    visited[i][j][k][l] = (bool**)malloc((eventMax+1)*sizeof(bool*));
                    For(m, 0, eventMax+2)
                    {
                        visited[i][j][k][l][m] = (bool*)malloc((eventMax+1)*sizeof(bool));
                        For(n, 0, eventMax+2)
                        {
                            visited[i][j][k][l][m][n] = false;
                        }
                    }
                }
            }
        }
    }
    /** The program will attempt to maximize the score, but inputs are given where #1 is best, #2 is worse, etc..
     *  Therefore, the input scores are multiplied by -1 so the program can correctly "maximize" the score. **/
    bool findMin = true;
    if (findMin)
        For(i, 0, eventRankings.size())
            For (j, 0, eventRankings.at(i).size())
                eventRankings.at(i).at(j) *= -1;

    // Initial state (first person with no events selected)
    AssignmentState starting {0, 0, 0, 0, 0, 0, 0, 0, 0, LLINF};
    processingQueue.emplace(starting);
    allStates.push_back(starting);

    /** MAIN LOOP **/

    int lastPersonProcessed = -1;

    while (!processingQueue.empty())
    {
        AssignmentState curstate = processingQueue.top();
        processingQueue.pop();

        // Reset the visited array once reaching a new person
        if (lastPersonProcessed < curstate.personIndex)
        {
            lastPersonProcessed = curstate.personIndex;
            fill(&visited[0][0][0][0][0][0], &visited[0][0][0][0][0][0] + sizeof(visited), false);
        }

        // Make sure to not process a state multiple times
        if (visited[curstate.e1][curstate.e2][curstate.e3][curstate.e4][curstate.e5][curstate.e6])
            continue;
        visited[curstate.e1][curstate.e2][curstate.e3][curstate.e4][curstate.e5][curstate.e6] = true;

        processPerson(curstate);
    }

    cout << bestAssignmentScore * (findMin ? -1 : 1) << endl;
    cout << bestCapacities << endl;

    /*** OUTPUT ***/
    cout << endl << "Assignment list:" << endl;
    while(bestPrevID != LLINF)
    {
        // Retrieve the previous state based on the ID
        AssignmentState* bestPrev = &allStates.at(bestPrevID);
        AssignmentState* prev = nullptr;
        if (bestPrev-> pid != LLINF)
            prev = &allStates.at(bestPrev->pid);

        // Deduce the events that were selected for the current person based on the changes in event counts
        vector<int> selectedEvents;
        vector<int> curEventCapacity = {bestPrev->e1, bestPrev->e2, bestPrev->e3, bestPrev->e4, bestPrev->e5, bestPrev->e6};
        vector<int> prevEventCapacity;
        if (bestPrev->pid == LLINF) break;
        else prevEventCapacity = {prev->e1, prev->e2, prev->e3, prev->e4, prev->e5, prev->e6};
        For(i, 0, numberOfEvents)
            if (curEventCapacity.at(i) > prevEventCapacity.at(i))
                selectedEvents.push_back(i+1);

        // Print to console
        cout << names.at(bestPrev->personIndex - 1) << ": " << selectedEvents << endl;

        bestPrevID = bestPrev->pid;
    }

    cout << "Done!" << endl;
}
