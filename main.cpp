/// By Goia Alexia Maria, 27/11/22
/// Written following clean coding principles and CLang Tidy Guidelines.

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <tuple>
#include <cfloat>

double g_timeLimit = 10000;

struct Planet {
    int x;
    int y;
    int bonus;
    int strength;
    int worth; // Computed as bonus/strength.
};

struct Spaceship {
    float movementSpeed = 1.0;
    int strength = 0;
};

int closestFriendlyPlanet(Planet hostile, Planet planets[], int arrayLength) {
    /** Finds the closest friendly planet to a hostile planet.
     *
     *  Used for finding the starting point of the spaceship.
     *
     *  @param hostile - the current hostile planet
     *  @param planets - the array containing all the planets
     *  @param arrayLength - the length of the planet array
     *  @return index of the friendly planet that is the closest.
     */
    int index = 0;
    auto smallestDistance = DBL_MAX;
    for (int i = 0; i < arrayLength; i++) {
        double currentDistance = sqrt((hostile.x - planets[i].x) * (hostile.x - planets[i].x) +
                                      (hostile.y - planets[i].y) * (hostile.y - planets[i].y)); // geometric distance
        if (currentDistance < smallestDistance) {
            smallestDistance = currentDistance;
            index = i;
        }
    }
    return index;
}

double distanceBetweenPlanets(Planet p1, Planet p2) {
    /** Finds the geometric distance between two planets.
     *
     *  Used for computing the time needed for movement between the planets.
     *
     *  @param p1 - the first planet
     *  @param p2 - the second planet
     *  @return the geometric distance between them.
     */
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

int main() {
    int scenarios, budget, friendlyPlanets, hostilePlanets;
    double averageHostileStrength = 0;
    Spaceship enterprise;
    std::ifstream readFile(R"(C:\Users\lexig\CLionProjects\tora\input.txt)");
    std::ofstream outFile("out.txt");

    readFile >> scenarios;
    outFile << scenarios << std::endl;

    // Going through scenarios.
    for (int i = 0; i < scenarios; i++) {

        readFile >> budget >> friendlyPlanets >> hostilePlanets;
        Planet planets[friendlyPlanets + hostilePlanets];

        // Reading planets.
        for (int j = 0; j < friendlyPlanets + hostilePlanets; j++) {
            // Friendly planets.
            if (j < friendlyPlanets) {
                readFile >> planets[j].x >> planets[j].y;
                planets[j].bonus = planets[j].strength = 0;
            }
                // Hostile planets.
            else {
                readFile >> planets[j].x >> planets[j].y >> planets[j].bonus >> planets[j].strength;

                if (planets[j].strength == 0) {
                    // To avoid an arithmetic error.
                    planets[j].worth = INT_MAX;
                } else {
                    planets[j].worth = planets[j].bonus / planets[j].strength;
                }
                averageHostileStrength += planets[j].strength;
            }
        }

        // Computing the average hostile strength as an arithmetic mean.
        averageHostileStrength /= hostilePlanets;


        // Sort hostile planets by worth to determine the order of visitation.
        for (int k = friendlyPlanets; k < friendlyPlanets + hostilePlanets - 1; k++) {
            for (int l = k + 1; l < friendlyPlanets + hostilePlanets; l++) {
                if (planets[k].worth < planets[l].worth) {
                    Planet p = planets[k];
                    planets[k] = planets[l];
                    planets[l] = p;
                }
            }
        }

        // Reinitializing Enterprise's stats.
        enterprise.movementSpeed = 1;
        enterprise.strength = 0;


        // Distributing the budget with the priority in strength.
        if (averageHostileStrength >= budget) {
            averageHostileStrength = budget;
            budget = 0;
        } else {
            budget -= int(averageHostileStrength);
        }
        enterprise.strength = int(averageHostileStrength);
        enterprise.movementSpeed += float(0.1) * float(budget);

        int nearestFriendlyPlanetIndex = closestFriendlyPlanet(planets[friendlyPlanets], planets, friendlyPlanets);
        outFile << nearestFriendlyPlanetIndex << std::endl;

        // Saving the executed moves in a vector of tuples, representing planet positions.
        std::vector<std::tuple<int, int>> moves;
        int currentHostilePlanetIndex = friendlyPlanets; // first hostile planet is on that position in the planet array.

        int currentPlanetIndex = nearestFriendlyPlanetIndex;
        int nextPlanetIndex = currentHostilePlanetIndex;

        while (g_timeLimit > 0 and currentHostilePlanetIndex < friendlyPlanets + hostilePlanets) {
            // Checking to see if we can travel to the next planet.
            if (enterprise.strength >= planets[nextPlanetIndex].strength) {
                // Adding the move.
                auto aux = std::make_tuple(planets[nextPlanetIndex].x, planets[nextPlanetIndex].y);
                moves.push_back(aux);
                // Modifying the time.
                g_timeLimit -=
                        distanceBetweenPlanets(planets[currentPlanetIndex], planets[nextPlanetIndex]) /
                        enterprise.movementSpeed;
                // The actual travel.
                currentHostilePlanetIndex++;
                currentPlanetIndex = nextPlanetIndex;
                nextPlanetIndex = currentHostilePlanetIndex;
            }
                // Otherwise, we skip it.
            else {
                currentHostilePlanetIndex++;
                nextPlanetIndex = currentHostilePlanetIndex;
            }
        }
        // Writing to the output.
        outFile << (enterprise.movementSpeed - 1) * 10 << " " << enterprise.strength << std::endl;
        outFile << moves.size() << std::endl;
        for (auto m: moves) {
            outFile << "M " << std::get<0>(m) << " " << std::get<1>(m) << std::endl;
        }

        // Resetting the time.
        g_timeLimit = 10000;


    }

    // Closing the files and exiting with the good ol' 0 code.
    readFile.close();
    outFile.close();
    return 0;
}