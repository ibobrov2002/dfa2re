#include "api.hpp"
#include <string>
#include <iostream>
#include <set>
#include <map>
using namespace std;

void set_trains_and_del_duplet(DFA &d, map<pair<string, string>, string> &trains) {
  for(auto state1 : d.get_states()) {
    for(auto state2 : d.get_states()) {
      set<string> jumps;
      for(auto symbol : d.get_alphabet()) {
        // cout << symbol << ' ' << state1 << ' ' << state2 << endl;
        string s;
        if (d.has_trans(state1, symbol) && d.get_trans(state1, symbol) == state2) {
          s = symbol;
          jumps.insert(s);
        }
      }
      if (jumps.size() == 1) {
        trains[make_pair(state1, state2)] = *jumps.begin();
      } else if (jumps.size() > 1) {
        string s = "";
        for(auto symbol : jumps) {
          if (s == "") {
            s = symbol;
          } else {
            s = s + "|" + symbol;
          }
        }
        trains[make_pair(state1, state2)] = s;
      }
    }
  }
}

void exclusion(DFA &d, string &state, map<pair<string, string>, string> &trains){
  string s = "";
  if (trains.find(make_pair(state, state)) != trains.end()) {
    s = trains[make_pair(state, state)];
  }
  
  for(auto train1 : trains) {
    if (train1.first.second == state && train1.first.first != state) {
      string q = train1.second;
      for(auto train2 : trains) {
        if (train2.first.first == state && train2.first.second != state) {
          string p = train2.second;
          if (trains.find(make_pair(train1.first.first, train2.first.second)) != trains.end()) {
            if (s.size() > 1) {
              trains[make_pair(train1.first.first, train2.first.second)] = trains[make_pair(train1.first.first, train2.first.second)] + "|(" + q + ")(" + s + ")*(" + p + ")";
            } else if (s.size() == 1) {
              trains[make_pair(train1.first.first, train2.first.second)] = trains[make_pair(train1.first.first, train2.first.second)] + "|(" + q + ")" + s + "*(" + p + ")";
            } else {
              trains[make_pair(train1.first.first, train2.first.second)] = trains[make_pair(train1.first.first, train2.first.second)] + "|(" + q + ")(" + p + ")";
            }
          } else {
            if (s.size() > 1) {
              trains[make_pair(train1.first.first, train2.first.second)] = "(" + q + ")(" + s + ")*(" + p + ")";
            } else if (s.size() == 1) {
              trains[make_pair(train1.first.first, train2.first.second)] = "(" + q + ")" + s + "*(" + p + ")";
            } else {
              trains[make_pair(train1.first.first, train2.first.second)] = "(" + q + ")(" + p + ")";
            }
          }
        }
      }
    }
  }
  set<pair<string, string>> del;
  d.delete_state(state);
  for(auto train : trains) {
    if (train.first.first == state || train.first.second == state) {
      del.insert(train.first);
    }
  }
  for(auto train : del) {
    trains.erase(train);
  }
  return;
}

string x_initial(DFA d, map<pair<string, string>, string> trains) {
  string initial = d.get_initial_state();
  for(auto state : d.get_states()) {
    if (state != initial) {
      exclusion(d, state, trains);
    }
  }
  if (trains.size() == 1) {
    if (trains.begin()->second.size() == 1) {
      return trains.begin()->second + "*";
    } else {
      return "(" + trains.begin()->second + ")*";
    }
  } else {
    return "$";
  }
}

string x_final(DFA d, map<pair<string, string>, string> trains, string & st) {
  string initial = d.get_initial_state();
  for(auto state : d.get_states()) {
    if (state != st && state != initial) {
      exclusion(d, state, trains);
    }
  }
  string r = "", s = "", u = "", t = "";
  if (trains.find(make_pair(initial, initial)) != trains.end()) {
    r = trains[make_pair(initial, initial)];
  }
  if (trains.find(make_pair(st, st)) != trains.end()) {
    u = trains[make_pair(st, st)];
  }
  if (trains.find(make_pair(initial, st)) != trains.end()) {
    s = trains[make_pair(initial, st)];
  }
  if (trains.find(make_pair(st, initial)) != trains.end()) {
    t = trains[make_pair(st, initial)];
  }
  // for(auto train : trains) {
  //   // cout << train.first.first << ' ' << train.first.second << ' ' << train.second << endl;
  //   if (train.first.first == initial && train.first.second == initial) {
  //     r = train.second;
  //   } else if (train.first.first == st && train.first.second == st) {
  //     u = train.second;
  //   } else if (train.first.first == initial && train.first.second == st) {
  //     s = train.second;
  //   } else {
  //     t = train.second;
  //   }
  // }
  // cout << r << "=r " <<  s << "=s " << u << "=u " << t << "=t " << endl;
  if (s != "") {
    if (r != "") {
      if (u != "") {
        if (t != "") {
          return "(" + r + "|(" + s + ")(" + u + ")*(" + t + "))*(" + s + ")(" + u + ")*";
        } else {
          return "(" + r + ")*(" + s + ")(" + u + ")*";
        }
      } else {
        if (t != "") {
          return "(" + r + "|(" + s + ")(" + t + "))*(" + s + ")";
        } else {
          return "(" + r + ")*(" + s + ")";
        }
      }
    } else {
      if (u != "") {
        if (t != "") {
          return "((" + s + ")(" + u + ")*(" + t + "))*(" + s + ")(" + u + ")*";
        } else {
          return "(" + s + ")(" + u + ")*";
        }
      } else {
        if (t != "") {
          return "((" + s + ")(" + t + "))*(" + s + ")";
        } else {
          return "(" + s + ")";
        }
      }
    }
  } else {
    return "";
  }


  // if (r == "") {
  //   if (s == "") {
  //     if (u == "") {
  //       if (t == "") {
  //         return "";
  //       } else {
  //         return "";
  //       }
  //     } else {
  //       if (t == "") {
  //         return "";
  //       } else {
  //         return "";
  //       }
  //     }
  //   } else {
  //     if (u == "") {
  //       if (t == "") {
  //         return "(" + s + ")";
  //       } else {
  //         return "((" + s + ")(" + t + "))*(" + s + ")";
  //       }
  //     } else {
  //       if (t == "") {
  //         return "(" + s + ")(" + u + ")*";
  //       } else {
  //         return "((" + s + ")(" + u + ")*(" + t + "))*(" + s + ")(" + u + ")*";
  //       }
  //     }
  //   }
  // } else {
  //   if (s == "") {
  //     if (u == "") {
  //       if (t == "") {
  //         return "";
  //       } else {
  //         return "";
  //       }
  //     } else {
  //       if (t == "") {
  //         return "";
  //       } else {
  //         return "";
  //       }
  //     }
  //   } else {
  //     if (u == "") {
  //       if (t == "") {
  //         return "(" + r + ")*(" + s + ")";
  //       } else {
  //         return "(" + r + "|(" + s + ")(" + t + "))*(" + s + ")";
  //       }
  //     } else {
  //       if (t == "") {
  //         return "(" + r + ")*(" + s + ")(" + u + ")*";
  //       } else {
  //         return "(" + r + "|(" + s + ")(" + u + ")*(" + t + "))*(" + s + ")(" + u + ")*";
  //       }
  //     }
  //   }
  // }
}

std::string dfa2re(DFA &d) {
  map<pair<string, string>, string> trains;
  // for(auto state : d.get_states()) {
  //   for(auto symbol : d.get_alphabet()) {
  //     string s(1, symbol);
  //     if (d.has_trans(state, symbol)) {
  //       trains.insert(make_pair(make_pair(state, d.get_trans(state, symbol)), s));
  //     }
  //   }
  // }
  set_trains_and_del_duplet(d, trains);
  for(auto state : d.get_states()) {
    if (!d.is_final(state) && d.get_initial_state() != state) {
      exclusion(d, state, trains);
    }
  }
  // for(auto train : trains) {
  //   cout << train.first.first << ' ' << train.first.second << ' ' << train.second << endl;
  // }
  string x_str = "";
  set<string> x;
  if (d.is_final(d.get_initial_state())) {
    x_str = x_initial(d, trains);
  }
  for(auto state : d.get_states()) {
    if (state != d.get_initial_state()) {
      x.insert(x_final(d, trains, state));
    }
  }
  if (x_str != "") {
    if (x_str == "$") {
      x_str = "";
    }
    for(auto xi : x) {
      x_str = x_str + "|" + xi;
    }
  } else {
    for(auto xi : x) {
      if (x_str != "") {
        x_str = x_str + "|" + xi;
      } else {
        x_str = x_str + xi;
      }
    }
  }
  return x_str;
}
