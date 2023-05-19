#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <random>

// --- Dzialanie algorytmu genetycznego ---
// 1. Tworzymy populacje losowych rozwiązań dla danych wejściowych
// 2. Greedy - Używamy algorytmu zachłannego dla każdego rozwiązania
// 3. Selection - Wybór najlepszych rozwiązan z populacji (im mniejszy czas zakończenia tym lepsze rozwiązanie)
// 4. Crossover - Krzyżowane rozwiązan z kroku 3 w celu stworzenia potomków. Wymiana pewnych części rozwiązań.
// 5. Mutation - Losowe modyfikowanie rozwiązan potomnych (aby zróżnicować rozwiązania)
// 6. Nowoutworzone rozwiązania zastępuja starsze rozwiązania w populacji
// 7. Warunek stopu - Powyższe kroki (2-6) są powtarzane przez określona liczbę iteracji lub do osiągnięcia zadowalającego czasu zakończenia
// 8. Po zakończeniu wybierane jest najlepsze rozwiązanie

// --- Parametry metaheurestyki genetycznej ---
// populationSize - Liczba populacji z losowymi rozwiązaniami (krok 1)
// maxGenerations - Ile razy przejdzie przez populacje (krok 7)
// mutationRate - Jest to wartość z zakresu od 0.0 do 1.0, która reprezentuje prawdopodobieństwo wystąpienia mutacji dla każdego osobnika w populacji.
//      Niższa wartość oznacza mniejsze prawdopodobieństwo mutacji, podczas gdy wyższa wartość może prowadzić do większego wpływu mutacji na populację.

int greedy(
    std::vector<int>processors,
    std::vector<int>tasks
) {
    std::vector<int>::size_type processorsCount = processors.size();
    std::vector<int>::size_type tasksCount = tasks.size();

    // Zaplanowanie zadan
    for (int i = 0; i < tasksCount; i++) {
        int minFinishTime = std::numeric_limits<int>::max();
        int minFinishTimeProcessorIndex = 0;

        // Znalezienie procesora, ktory ma najmniejszy czas ukonczenia
        for (int j = 0; j < processorsCount; j++) {
            int finishTime = processors[j];

            if (finishTime < minFinishTime) {
                minFinishTime = finishTime;
                minFinishTimeProcessorIndex = j;
            }
        }

        // Dodanie zadania do procesora, ktory ma najmniejszy czas ukonczenia
        processors[minFinishTimeProcessorIndex] += tasks[i];
    }

    // Wyswietlanie wynikow
    // - Czas ukonczenia kazdego z procesorow
    // - Czas procesora, ktory ukonczy sie jako ostatni
    int maxFinishTime = 0;
    for (int j = 0; j < processorsCount; j++) {
        // std::cout << "Processor " << j + 1 << " finish time: " << processors[j] << std::endl;
        if (processors[j] > maxFinishTime) {
            maxFinishTime = processors[j];
        }
    }

    return maxFinishTime;
}

// Mutation - Losowe modyfikowanie rozwiązan potomnych (aby zróżnicować rozwiązania)
void mutation(std::vector<int>& plan) {
    std::vector<int>::size_type planSize = plan.size();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, planSize - 1);

    // Wylosuj dwa indexy dla populacji
    int index1 = dis(gen);
    int index2 = dis(gen);

    // Zamien ze soba wylosowane elementy
    int temp = plan[index1];
    plan[index1] = plan[index2];
    plan[index2] = temp;
}

// Crossover - Krzyżowane rozwiązan z kroku 3 w celu stworzenia potomków. Wymiana pewnych części rozwiązań.
std::vector<int> crossover(const std::vector<int>& parent1, const std::vector<int>& parent2) {
    std::vector<int>::size_type planSize = parent1.size();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, planSize - 1);

    // Parent 1: 5 2 3 4 1
    // Parent 2: 1 3 2 5 4

    // Losuje punkt startowy
    //   | 
    // 5 2 3 4 1
    int crossoverStart = dis(gen);

    std::uniform_int_distribution<> dis2(crossoverStart, planSize - 1);

    // Losuje punkt koncowy
    //   |   |
    // 5 2 3 4 1
    int crossoverEnd = dis2(gen);

    std::vector<int> parent2RemainingElements = parent2;
    std::vector<int> child(planSize);


    // Skopiuj pierwszą część parent1 do dziecka
    //
    // 0 2 3 4 0
    for (int i = crossoverStart; i < crossoverEnd; i++) {
        child[i] = parent1[i];
        std::vector<int>::size_type parent2RemainingElementsSize = parent2RemainingElements.size();

        // Usuwa skopiowane elementy z parent2
        //   | |   |
        // 1 3 2 5 4
        for (int j = 0; j < parent2RemainingElementsSize; j++) {
            if (parent2RemainingElements[j] == parent1[i]) {
                parent2RemainingElements.erase(parent2RemainingElements.begin() + j);
                break;
            }
        }
    }

    // Skopiuj pozostałe elementy parent2 do dziecka
    // |       |
    // 1 2 3 4 5
    for (int i = 0; i < planSize; i++) {
        if (i >= crossoverStart && i < crossoverEnd) {
            continue;
        }

        child[i] = parent2RemainingElements[0];
        parent2RemainingElements.erase(parent2RemainingElements.begin());
    }

    return child;
}

// Selection - Wybór najlepszych rozwiązan z populacji (im mniejszy czas zakończenia tym lepsze rozwiązanie)
std::vector<std::vector<int>> selection(const std::vector<std::vector<int>>& population, const std::vector<int>& tasks) {
    std::vector<std::pair<int, int>> completionTimes;
    std::vector<int>::size_type populationSize = population.size();

    // Oblicz czas zakończenia dla każdego rozwiązania w populacji
    for (int i = 0; i < populationSize; i++) {
        int completionTime = greedy(population[i], tasks);
        completionTimes.push_back(std::make_pair(i, completionTime));
    }

    // Posortuj rosnąco czasy zakończenia
    for (int i = 0; i < populationSize - 1; i++) {
        for (int j = 0; j < populationSize - i - 1; j++) {
            if (completionTimes[j].second > completionTimes[j + 1].second) {
                std::swap(completionTimes[j], completionTimes[j + 1]);
            }
        }
    }

    std::vector<std::vector<int>> selectedPopulation;

    // Pierwszą połowe najlepszych rozwiązań przekazujemy do następnej generacji
    for (int i = 0; i < populationSize / 2; i++) {
        selectedPopulation.push_back(population[completionTimes[i].first]);
    }

    return selectedPopulation;
}

void genetic(
    std::vector<int>processors,
    std::vector<int>tasks
) {
    int populationSize;
    int maxGenerations;
    double mutationRate;

    std::cout << "Liczba pokolen: ";
    std::cin >> populationSize;

    std::cout << "Maksymalna ilosc generacji: ";
    std::cin >> maxGenerations;

    std::cout << "Wspolczynnik mutacji (0.0-1.0): ";
    std::cin >> mutationRate;

    std::vector<int>::size_type tasksCount = tasks.size();

    // Wylosowanie pierwszej populacji rozwiązań
    std::vector<std::vector<int>> population(populationSize, std::vector<int>(tasksCount));

    for (int i = 0; i < populationSize; i++) {
        for (int j = 0; j < tasksCount; j++) {
            population[i][j] = tasks[j];
        }

        std::random_device rd;
        std::mt19937 g(rd());

        std::shuffle(population[i].begin(), population[i].end(), g);
    }

    int generation = 0;
    int bestCompletionTime = std::numeric_limits<int>::max();
    std::vector<int> bestIndividual;

    while (generation < maxGenerations) {
        std::vector<std::vector<int>> selectedPopulation = selection(population, tasks);

        std::vector<std::vector<int>> newPopulation;

        // Najlepszą populacje dodajemy do nowej generacji, a resztę populacji dalej modyfikujemy
        newPopulation.push_back(selectedPopulation[0]);
        selectedPopulation.erase(selectedPopulation.begin());

        // Krzyżowanie i mutacja rozwiązań do nowej populacji
        while (newPopulation.size() < populationSize) {
            // Wylosuj indexy dwóch populacji rodziców
            int parentIndex1 = rand() % selectedPopulation.size();
            int parentIndex2 = rand() % selectedPopulation.size();

            std::vector<int> parent1 = selectedPopulation[parentIndex1];
            std::vector<int> parent2 = selectedPopulation[parentIndex2];

            // Krzyżowanie wylosowanych rodziców
            std::vector<int> child = crossover(parent1, parent2);

            // Mutacja populacji dziecka z siłą na podstawie parametru
            double mutationProbability = (double)rand() / RAND_MAX;
            if (mutationProbability <= mutationRate) {
                mutation(child);
            }

            newPopulation.push_back(child);
        }

        population = newPopulation;

        // Znajdz najlepsze rozwiązanie
        for (int i = 0; i < populationSize; i++) {
            int completionTime = greedy(population[i], tasks);

            if (completionTime < bestCompletionTime) {
                bestCompletionTime = completionTime;
                bestIndividual = population[i];
            }
        }

        generation++;
    }

    std::cout << "Najlepsze rozwiązanie: ";
    for (int i = 0; i < tasksCount; i++) {
        std::cout << bestIndividual[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Najlepszy czas: " << bestCompletionTime << std::endl;

}

int main()
{
    std::cout
        << "Co chcesz zrobic?\n"
        << "1. Wygenerowac\n"
        << "2. Wczytac i uzyc algorytmu zachlannego\n"
        << "3. Wczytac i uzyc metaheurestyki genetycznej\n"
        << std::endl;

    int programOption;
    std::cin >> programOption;

    if (programOption == 1) {
        // Generowanie instancji
        int numberOfInstances;
        int numberOfProcessors;

        std::cout << "Liczba procesorow: ";
        std::cin >> numberOfProcessors;

        std::cout << "Liczba instancji: ";
        std::cin >> numberOfInstances;

        std::ofstream outfile("m" + std::to_string(numberOfProcessors) + ".txt");

        outfile << numberOfProcessors << std::endl;
        outfile << numberOfInstances << std::endl;

        for (int i = 0; i < numberOfInstances; i++) {
            outfile << rand() % (numberOfInstances - numberOfProcessors / 2 + 1) + numberOfProcessors / 2 << std::endl;
        }

        outfile.close();
    }
    else if (programOption == 2 || programOption == 3) {
        // Wczytywanie instancji
        std::string fileName;

        std::cout << "Nazwa pliku z instancjami: ";
        std::cin >> fileName;

        std::ifstream instances(fileName);

        std::vector<int> tasks;
        std::vector<int> processors;

        if (instances.is_open()) {
            std::string instance;
            int i = 0;

            while (std::getline(instances, instance)) {
                if (i == 0) { // 1 linia to liczba procesorów
                    processors.resize(std::stoi(instance), 0);
                }
                else if (i == 1) { // 2 linia to liczba zadań
                    tasks.resize(std::stoi(instance));
                }
                else { // kazda kolejna linia to zadanie
                    tasks[i - 2] = std::stoi(instance);
                }

                i++;
            }
        }
        else {
            std::cerr << "Couldn't open file\n";
            return 1;
        }

        if (programOption == 2) {
            int maxFinishTime = greedy(processors, tasks);
            std::cout << "Najlepszy czas: " << maxFinishTime << std::endl;
        }
        else {
            genetic(processors, tasks);
        }
    }

    return 0;
}