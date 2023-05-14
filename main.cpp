#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>

int main()
{
    std::cout << "Co chcesz zrobic?\n" << "1. Wygenerowac\n" << "2. Wczytac i uzyc algorytmu zachlannego\n" << std::endl;

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
            outfile << rand() % (numberOfInstances - numberOfProcessors/2 + 1) + numberOfProcessors/2 << std::endl;
        }

        outfile.close();
    }
    else if (programOption == 2) {
        // Wczytywanie instancji
        std::string fileName;

        std::cout << "Nazwa pliku z instancjami: ";
        std::cin >> fileName;

        std::ifstream instances(fileName);

        int processorsCount = 0;
        int tasksCount = 0;

        std::vector<int> tasks;
        std::vector<int> processors;

        if (instances.is_open()) {
            std::string instance;
            int i = 0;

            while (std::getline(instances, instance)) {
                if (i == 0) { // 1 linia to liczba procesorów
                    processorsCount = std::stoi(instance);
                    processors.resize(processorsCount, 0);
                }
                else if (i == 1) { // 2 linia to liczba zadań
                    tasksCount = std::stoi(instance);
                    tasks.resize(tasksCount);
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

        std::cout << "Total completion time: " << maxFinishTime << std::endl;
    }

    return 0;
}