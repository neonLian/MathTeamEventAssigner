#include <iostream>
#include <vector>
#include <array>
#include <queue>
#include <map>
#include <limits>
#include <numeric>

#define For(i,a,b) for(int i = a; i < b; i++)
using namespace std;

struct DPState
{
    int i;
    int pts;
    int e1;
    int e2;
    int e3;
    int e4;
    int e5;
    int e6;
};

bool operator <(const DPState& x, const DPState& y) {
    int ypts = -1*y.pts;
    int xpts = -1*x.pts;
    return std::tie(x.i, xpts, x.e1, x.e2, x.e3, x.e4, x.e5, x.e6) > std::tie(y.i, ypts, y.e1, y.e2, y.e3, y.e4, y.e5, y.e6);
}
ostream& operator<<(ostream& os, const DPState& dps) {
    return os << "<" << dps.i << ", "
                    << dps.pts << ", "
                    << dps.e1 << ", "
                    << dps.e2 << ", "
                    << dps.e3 << ", "
                    << dps.e4 << ", "
                    << dps.e5 << ", "
                    << dps.e6 << ">";
}
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

int main() {
    int N = 18, M = 6; // N = # of people, M = # of events
    int eventMax = (N/2)+1;
    int numSelectedEvents = 3;

    // Data format: 2D array, A[person][event #] = preference
    // Goal: maximize score (since it's easier to work that way)
    // Below: real math team event 5 data
    vector<vector<int>> A = {{2,6,3,5,4,1},
                             {4,6,3,5,1,2},
                             {3,6,2,5,4,1},
                             {1,6,2,3,5,4},
                             {1,5,2,4,3,6},
                             {3,6,2,5,4,1},
                             {5,6,2,1,4,3},
                             {2,4,1,5,6,3},
                             {1,4,5,2,6,3},
                             {6,1,2,5,3,4},
                             {1,2,3,4,5,6},
                             {5,4,3,1,2,6},
                             {1,6,2,3,5,4},
                             {4,1,3,6,2,5},
                             {1,3,5,6,4,2},
                             {4,1,5,2,6,3},
                             {4,1,5,2,6,3},
                             {2,4,6,1,5,3}};

    bool visited [eventMax+1][eventMax+1][eventMax+1][eventMax+1][eventMax+1][eventMax+1];

    bool findMin = true;
    if (findMin)
        For(i, 0, A.size())
            For (j, 0, A.at(i).size())
                A.at(i).at(j) *= -1;

    priority_queue<DPState> pq;
    pq.emplace(0,0,0,0,0,0,0,0);


    vector<int> size3bitmasks;
    For(i, 0, M-2)
        For(j, i+1, M-1)
            For(k, j+1, M)
                size3bitmasks.push_back((1 << i) + (1 << j) + (1 << k));

    /** SPEED OPTIMIZATION: Identify the minimum and maximum points that can be scored in any given range **/
    vector<int> prefixMin(N, 0);
    vector<int> prefixMax(N, 0);
    For(i, 0, N)
    {
        // Sort preferences to identify the top M events and low M events
        vector<int> person = A.at(i);
        sort(person.begin(), person.end());
        prefixMin.at(i) = (i > 0 ? prefixMin.at(i-1) : 0) + accumulate(person.begin(), person.begin()+numSelectedEvents, 0);
        prefixMax.at(i) = (i > 0 ? prefixMax.at(i-1) : 0) + accumulate(person.end()-numSelectedEvents, person.end(), 0);
    }

    /** MAIN LOOP **/

    int best = numeric_limits<int>::min();
    int plannedBest = prefixMin.at(N-1);
    vector<int> bestCapacities(M, 0);
    int DEBUG_lasti = -1;
    int DEBUG_previts = 0;
    while (!pq.empty())
    {
        DPState curstate = pq.top();
        pq.pop();

        if (DEBUG_lasti < curstate.i)
        {
            cout << "On person " << curstate.i << " (previts = " << DEBUG_previts << ", pq size = " << pq.size() << ")" << endl;
            DEBUG_lasti = curstate.i;
            DEBUG_previts = 0;
            fill(&visited[0][0][0][0][0][0], &visited[0][0][0][0][0][0] + sizeof(visited), false);
        }
        DEBUG_previts++;

        if (visited[curstate.e1][curstate.e2][curstate.e3][curstate.e4][curstate.e5][curstate.e6])
            continue;
        visited[curstate.e1][curstate.e2][curstate.e3][curstate.e4][curstate.e5][curstate.e6] = true;

        /** SPEED OPTIMIZATION continued **/
        if (curstate.pts + prefixMax.at(N-1) - (curstate.i > 0 ? prefixMax.at(curstate.i-1) : 0) < plannedBest)
            continue;

        vector<int> eventCapacity {curstate.e1, curstate.e2, curstate.e3, curstate.e4, curstate.e5, curstate.e6};

        // Iterate through all possible event selections for person i
        for (int bmi = 0; bmi < size3bitmasks.size(); bmi++)
        {
            const int& bm = size3bitmasks.at(bmi);
            // Check event capacity for selected bitmask
            bool eject = false;
            for (int j = 0; j < M; j++)
                if (eventCapacity.at(j) + ((bm & (1 << j)) > 0) > eventMax) {
                    eject = true;
                    break;
                }
            if (eject) continue;

            // Add points for this selected bitmask
            int addedPts = 0;
            for (int j = 0; j < M; j++)
                if (bm & (1 << j))
                    addedPts += A.at(curstate.i).at(j);

            /** SPEED OPTIMIZATION continued **/
            plannedBest = max(plannedBest, curstate.pts + prefixMin.at(N-1) - (curstate.i > 0 ? prefixMin.at(curstate.i-1) : 0));

            // Send this fine event selection off to the next person
            int newE1 = curstate.e1 + ((bm & (1 << 0)) > 0);
            int newE2 = curstate.e2 + ((bm & (1 << 1)) > 0);
            int newE3 = curstate.e3 + ((bm & (1 << 2)) > 0);
            int newE4 = curstate.e4 + ((bm & (1 << 3)) > 0);
            int newE5 = curstate.e5 + ((bm & (1 << 4)) > 0);
            int newE6 = curstate.e6 + ((bm & (1 << 5)) > 0);
            if (curstate.i < N - 1)
                pq.emplace(curstate.i + 1, curstate.pts + addedPts, newE1, newE2, newE3, newE4, newE5, newE6);
            else if (curstate.pts + addedPts > best) {
                best = curstate.pts + addedPts;
                bestCapacities = {newE1, newE2, newE3, newE4, newE5, newE6};
            }
        }
    }
    cout << "final i iters = " << DEBUG_previts << endl;

    cout << best * (findMin ? -1 : 1) << endl;
    cout << bestCapacities << endl;
}
