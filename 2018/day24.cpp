// Advent of Code 2018
// Day 24: Immune System Simulator 20XX
// #include <bits/stdc++.h>
// #include <array>
// #include <bitset>
// #include <cassert>
// #include <deque>
// #include <map>
// #include <sstream>
// #include <string>
// #include <tuple>
// #include <istream>
// #include <unordered_map>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <list>
#include <numeric>
#include <regex>
#include <set>
#include <vector>
using namespace std;

// global constants/variables
const string AttackT[]{"cold", "fire", "slashing", "radiation", "bludgeoning"};
const string ArmyT[]{"Immune System:", "Infection:"};
const string IMM = ArmyT[0], INF = ArmyT[1];
size_t ArmiesIndex{0};

// a single group in an army
struct Group {
   private:
    string armyType, attackType;
    int units, hp, damage, initiative;
    set<string> immunities, weaknesses;

   public:
    // army alliance checking
    bool isType(const string& armyT) const { return armyType == armyT; }

    // for attacking ordering
    int intv() const { return initiative; }

    // is this Group still fighting (with effective power)?
    int unitsAlive() const { return units; }

    // the total damage this Group can do
    int effectivePower() const { return units * damage; }

    // increase the base damage dealt by this group
    void increaseDamage(int amount) { damage += amount; }

    // reduces unit counts, and ensures units is non-negative
    int takeDamage(int amount) {
        int killCount = min(amount / hp, units);  // ensure units >= 0
        units -= killCount;
        return killCount;
    }

    // function for attacking and target selection ordering
    // The attacking group targets the group in the enemy army to which it would
    // deal the most damage (after accounting for weaknesses and immunities)
    int dmgDealt(const Group& enemy) const {
        int multiplier = 1;
        if (enemy.immunities.count(attackType))
            multiplier = 0;
        else if (enemy.weaknesses.count(attackType))
            multiplier = 2;
        return multiplier * effectivePower();
    }

    // comparator for target selection ordering
    // In decreasing order of effective power, groups choose their targets
    // in a tie, the group with the higher initiative chooses first
    bool operator<(const Group& rhs) const {
        return effectivePower() < rhs.effectivePower() ||
               (effectivePower() == rhs.effectivePower() &&
                initiative < rhs.initiative);
    }

    // print a Group's state
    friend ostream& operator<<(ostream& os, const Group& g) {
        os << g.armyType << " " << g.units << " units, " << g.hp << " hp, "
           << g.attackType << ": " << g.damage << " dmg, " << g.effectivePower()
           << " ep, " << g.initiative << " init ";
        os << "(immune to: ";
        for (auto& type : g.immunities)
            os << type << ", ";
        os << ") (weak to: ";
        for (auto& type : g.weaknesses)
            os << type << ", ";
        os << ")" << endl;
        return os;
    }

    // parse a single line of input Group data
    friend istream& operator>>(istream& is, Group& g) {
        static const regex re{
            R"(^(\d+) units each with (\d+) hit points(?: \((.*)\))?)"
            R"( with an attack that does (\d+) (\w+) damage at initiative (\d+)$)"};

        string line, immu_weak;
        stringstream ss;

        getline(is, line);
        // cerr << "L: " << line << endl;
        if (line.empty()) {
            is.setstate(ios_base::failbit);  // one army set done
            return is;
        }
        // smatch matches;
        // if (!regex_match(line, matches, re)) {
        //     is.setstate(ios_base::failbit); // one army set done
        //     return is;
        // }

        // put the regex submatches in a string stream (immu/weak at end)
        copy(sregex_token_iterator(line.begin(), line.end(), re,
                                   {1, 2, 4, 5, 6, 3}),
             {}, ostream_iterator<string>(ss, "\n"));
        // cerr << ss.str() << endl;

        // initialize the new Group (note: operator >> reuses same object ref)
        g.armyType = ArmyT[ArmiesIndex];             // from globals
        g.immunities.clear(), g.weaknesses.clear();  // clear old sets
        ss >> g.units >> g.hp >> g.damage >> g.attackType >> g.initiative;

        // parse immunities/weaknesses and initialize sets
        while (getline(ss, immu_weak, ';'))
            for (const string& attack : AttackT)
                if (immu_weak.find(attack) != string::npos)
                    if (immu_weak.find("immune") != string::npos)
                        g.immunities.insert(attack);  // "immune to"
                    else
                        g.weaknesses.insert(attack);  // "weak to"
        return is;
    }
};  // end struct Group

// type definitions for convenience/readability
typedef Group* GroupPtr;
typedef vector<Group> Army;
typedef list<GroupPtr> ArmyPtrs;  // list for O(1) removal
struct AttackerCmp {  // for attacking ordering (insertion sort in target map)
    bool operator()(const GroupPtr& lhs, const GroupPtr& rhs) const {
        return lhs->intv() > rhs->intv();  // the defined attacking order
    }
};
typedef map<GroupPtr, GroupPtr, AttackerCmp> TargetMap;  // (attacker->target)

// comparison (lambda) for pointers to Groups (for list::sort())
auto GroupPtrCmp = [](const GroupPtr& lhs, const GroupPtr& rhs) {
    return *rhs < *lhs;  // reverse call the Group < operator overload
};                       // could also use list::reverse() after list::sort()

// parses only one entire army type at a time (historical reasons)
istream& operator>>(istream& is, Army& army) {
    string line;
    getline(is, line);
    cerr << "Parsing army #" << ArmiesIndex << " " << line << endl;
    move(istream_iterator<Group>{is}, {}, back_inserter(army));
    if (!ArmiesIndex++)  // dont clear after parsing second army
        is.clear();
    return is;
}

// prints an entire army/armies
ostream& operator<<(ostream& os, const Army& army) {
    os << "--- Army ---" << endl;
    for (const Group& g : army)
        os << g;
    return os;
}

// does this army type still have surviving units? (ie. losers?)
bool anyGroupsAlive(Army& armies, const string& armyType) {
    return any_of(armies.begin(), armies.end(), [&](Group& g) {
        return g.unitsAlive() && g.isType(armyType);
    });
}

// do all(both) armies still have surviving units? (ie. fight on?)
bool survivingGroupsInAll(Army& armies) {
    return all_of(begin(ArmyT), end(ArmyT), [&](const string& armyType) {
        return anyGroupsAlive(armies, armyType);
    });
}

// sum up the survivoring groups unit counts of a battle's outcome
// (losing army groups are all at exactly 0 units each)
int tallySurvivors(Army& battleResult) {
    return accumulate(
        battleResult.begin(), battleResult.end(), 0,
        [](int& sum, const Group& g) { return sum + g.unitsAlive(); });
}

// phase 1: target selection
// - groups choose 1 enemy group each (or 0 if dead, impotent, or all chosen)
// - groups choose in dec. order of their effective power (then initiative)
// - groups choose the enemy group to which it would deal the most damage
// - tie: choose the group with largest effective power (then initiative)
TargetMap targetSelect(ArmyPtrs& armies) {
    TargetMap targets;

    // first do one full sort of all the groups by the predefined choosing order
    // sort(armies.rbegin(), armies.rend(), GroupPtrCmp); // (rev for dec order)
    armies.sort(GroupPtrCmp);  // list

    // then for each army type, choose enemy targets
    for (const string& offense : ArmyT) {
        // start with a working list of surviving enemy groups (still sorted)
        ArmyPtrs defense;
        copy_if(armies.begin(), armies.end(), back_inserter(defense),
                [&](GroupPtr e) {
                    return e->unitsAlive() and not e->isType(offense);
                });

        // for each surviving group, choose a target (if able & any left)
        for (GroupPtr& attacker : armies) {
            if (not(attacker->unitsAlive() and attacker->isType(offense)))
                continue;
            if (defense.empty())
                break;

            // preserves predefined choice order, so ok in event of a choice tie
            // always returns an element (even if 0 damage dealt) so do a check
            auto targetChoice = max_element(
                defense.begin(), defense.end(),
                [&](const GroupPtr& lhs, const GroupPtr& rhs) {
                    return attacker->dmgDealt(*lhs) < attacker->dmgDealt(*rhs);
                });

            // make sure the attacker can actually deal damage to the choice
            if (attacker->dmgDealt(**targetChoice)) {
                targets[attacker] = *targetChoice;  // insertion sorted
                defense.erase(targetChoice);
            }
        }
    }
    return targets;
}

// phase 2: attacking
// - each group deals damage to the enemy group target it selected (if any)
// - groups attack in decreasing order of initiative (simultaneously)
// - if a group contains no units, it cannot attack (killed during battle)
// - the defending group only loses whole units from damage (1 per full hp)
// - at phase end, if both armies still contain units, a new fight begins
// - combat ends only once one army has lost all of its units
// - stalemates (no deaths) can occur, so check for them

// attack the chosen enemy group and tally the casualties
// no need to sort since already sorted (set insertion: AttackerCmp)
// no need to check for dead Groups (0 units -> no effective power)
int attack(TargetMap& targets) {
    return accumulate(
        targets.begin(), targets.end(), 0, [&](int& sum, const auto& choice) {
            auto& [attacker, enemy] = choice;
            int casualties = enemy->takeDamage(attacker->dmgDealt(*enemy));
            return sum + casualties;
        });
}

// each single fight consists of 2 phases: target selection & attacking
// note: call-by-value (deep) copies of the armies (since many battles)
pair<Army&, bool> fight(Army armies, int boost = 0) {
    // process both armies at the same time as pointers to groups
    ArmyPtrs armyPtrs;
    transform(armies.begin(), armies.end(), back_inserter(armyPtrs),
              [&](Group& g) { return &g; });

    // boost the immune system army, if any
    for_each(armies.begin(), armies.end(), [&](Group& g) {
        if (boost && g.isType(IMM))
            g.increaseDamage(boost);
    });

    // keep fighting until a definite winner emerges or a stalemate occurs
    // the fight is won when any army doesn't have any groups left to fight
    bool stalemate{false};
    while (survivingGroupsInAll(armies) and not stalemate) {
        TargetMap targets =
            targetSelect(armyPtrs);        // phase 1: target selection
        int casualties = attack(targets);  // phase 2: attacking
        stalemate = not casualties;
    }
    return {armies, stalemate};
}

// simulate the battle according to the given rules
void solve() {
    Army armies;  // both armies in one list
    while (cin >> armies)
        ;

    // part 1 fight the battle as is (without any boost)
    // part 2 fight with an increasingly stronger immune system (until it wins)
    bool infectionArmyWon{true};
    int survivorTally_boost[]{0, 0}, boost{-1};
    while (infectionArmyWon) {
        // fight a full battle at the current boost level
        auto [battleResult, stalemate] = fight(armies, ++boost);

        // we need a definite winner
        if (stalemate)
            continue;

        // who won this round?
        infectionArmyWon = anyGroupsAlive(battleResult, INF);

        // memo the 2 solutions (part 1: no boost) (part 2: +45 boost)
        survivorTally_boost[boost > 0] = tallySurvivors(battleResult);
    }

    // output solutions
    // Part 1: As it stands now, how many units would the winning army have?
    cout << "[Part 01] = " << survivorTally_boost[false] << endl;  // 16747

    // Part 2: How many units does the immune system have left after getting
    // the smallest boost it needs to win?
    cout << "[Part 02] = " << survivorTally_boost[true] << endl;  // 5923
}

// Main: Time the solver.
int main() {
    // Speed up C++ io by unsyncing with C stdio and untie cin/cout.
    ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    // Time the solve() function in milliseconds.
    auto start_time = std::chrono::high_resolution_clock::now();
    solve();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto ms_count = std::chrono::duration_cast<std::chrono::milliseconds>(
                        end_time - start_time)
                        .count();
    std::clog << "[Runtime] = " << ms_count << "ms" << std::endl;
}