#include <set>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>

using namespace std;

bool debug = false;

bool global_constraint(set<int> chests, multiset<int> keys, int key_req[], multiset<int> keys_inside[]) {
  multiset<int> keys_available;
  multiset<int> keys_needed;

  keys_available.insert(keys.begin(), keys.end());

  for (set<int>::iterator c=chests.begin(); c != chests.end(); *c ++) {
    keys_available.insert(keys_inside[*c].begin(), keys_inside[*c].end());
    keys_needed.insert(key_req[*c]);
  }

  for (multiset<int>::iterator k=keys_needed.begin(); k!= keys_needed.end(); *k ++) {
    if (keys_needed.count(*k) > keys_available.count(*k)) {
      if (debug)
	cout << "Need " << keys_needed.count(*k) << " of key " << *k << " but only have " << keys_available.count(*k) << "." << endl;
      return false;
    }

  }
  return true;
}


bool still_possible(set<int> chests, multiset<int> keys, int key_req[], multiset<int> keys_inside[]) {
  bool flag = true;

  while (flag) {
    flag = false;
    
    for (set<int>::iterator c=chests.begin(); c != chests.end(); *c ++) {
      
      // check whether we can open the chest
      int key = key_req[*c];
      if (keys.count(key)==0)
	continue;
      
      // open it without destroying the key
      keys.insert(keys_inside[*c].begin(), keys_inside[*c].end());
      chests.erase(*c);
      flag = true; // there is still hope
      break;
    }
  }

  return chests.empty();
}

int solve(set<int> chests, multiset<int> keys, int key_req[], multiset<int> keys_inside[]) {

  int n_chests = chests.size();
  vector<int> solution;

  if (!global_constraint(chests, keys, key_req, keys_inside) ||
      !still_possible(chests, keys, key_req, keys_inside)) {
    cout << "IMPOSSIBLE" << endl;
    return 0;
  }

  while (!chests.empty()) {
    for (set<int>::iterator c=chests.begin(); c != chests.end(); *c ++) {
      // Skip this chest if we can't open it

      int key = key_req[*c];
      if (keys.count(key)==0) {
	if (debug) 
	  cout << "Can't open chest " << *c + 1 << " because I don't have key " << key << endl;
	continue;
      }

      // Open the chest
      keys.erase(keys.find(key));
      int chest = *c;
      c = chests.erase(c);  // requires C++11

      if (debug) {
	cout << "After removing chest " << *c + 1 << ", chests contains: ";
	for (set<int>::iterator x=chests.begin(); x != chests.end(); *x ++) 
	  cout << *x + 1 << " ";
	cout << endl;
      }

      keys.insert(keys_inside[chest].begin(), keys_inside[chest].end());

      if (debug) {
	cout << "I have these keys: ";
	copy(keys.begin(), keys.end(),
	     ostream_iterator<int>(cout, " "));
	cout << endl;
      }

      // Test
      if (still_possible(chests, keys, key_req, keys_inside)) {
	solution.push_back(chest + 1);
	break;
      }
      if (debug)
	cout << "Had to unopen chest " << chest + 1 << endl;
      // Rewind
      keys.insert(key);
      c = chests.insert(c, chest);
      // why is it so hard to do "keys -= keys_inside[chest]" in C++ ?!
      multiset<int> temp;
      std::set_difference(keys.begin(), keys.end(), 
			  keys_inside[chest].begin(), keys_inside[chest].end(), 
			  std::inserter(temp, temp.begin()));
      keys.swap(temp);

    }
  }

  if (debug) 
    cout << "Solution: ";
  copy(solution.begin(), solution.end(),
       ostream_iterator<int>(cout, " "));
  cout << endl;

  return 0;
}


int main(int argc, char **argv) {
  int T;
  cin >> T;
  for (int t=0; t<T; t++) {
    cout << "Case #" << t+1 << ": ";
    if (debug) cout << endl;

    int K, N;
    cin >> K >> N;
    
    multiset<int> keys;
    for (int k=0; k<K; k++) {
      int temp;
      cin >> temp;
      keys.insert(temp);
    }
    if (debug) {
      cout << "I start with these keys: ";
      copy(keys.begin(), keys.end(),
	   ostream_iterator<int>(cout, " "));
      cout << endl;
    }

    int key_req[N];
    multiset<int> keys_inside[N];
    set<int> chests;

    for (int n=0; n<N; n++) {
      chests.insert(n);

      int Ti, Ki;
      cin >> Ti >> Ki;
      key_req[n] = Ti;
      for (int k=0; k<Ki; k++) {
	int temp;
	cin >> temp;
	keys_inside[n].insert(temp);
      }
      if (debug) {
	cout << "Chest " << n+1 << " needs key " << Ti << 
	  " and contains keys: ";
	copy(keys_inside[n].begin(), keys_inside[n].end(),
	     ostream_iterator<int>(cout, " "));
	cout << endl;
      }
    }
    
    solve(chests, keys, key_req, keys_inside);
  }

  return 0;
}
