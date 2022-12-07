#include<bits/stdc++.h>
#include <iostream>
#include <map>
#include <math.h>

using namespace std;

vector<vector<int>> population, offsprings;
vector<pair<int, int>> sortedFitness; //mapped with the old index for each fitness
vector<pair<int, int>> sortedOffspringFitness;
const int MaxNumberOfGenerations = 5;
const float pc = 0.4, pm = 0.001;
int populationSize, knapsackSize, numberOfItems;
int itemsWeight[100], itemsValue[100], populationFitness[100], offspringsFitness[100], wheelDivisions[100];

void clear2DVector(vector<vector<int>> &v)
{
    v.resize(populationSize);
    for(int i = 0; i < populationSize; i++)
    {
        v[i].resize(populationSize);
        for(int j = 0; j<numberOfItems; j++)
        {
            v[i][j] = 0;
        }
    }
}

int randomNumber(int a, int b)
{
    static bool first = true;
    if(first)
    {
        srand(time(NULL));
        first = false;
    }
    return a + (rand() % b);
}

float floatRandomNumber(float a, float b)
{
    static bool second = true;
    if(second)
    {
        srand(time(NULL));
        second = false;
    }
    return a + static_cast<float>(rand()) * static_cast<float>(b - a) / RAND_MAX;
}

void sortArr(int arr[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        sortedFitness.push_back(make_pair(arr[i], i));
    }

    sort(sortedFitness.begin(), sortedFitness.end());
}

void sortArrOffsprings(int arr[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        sortedOffspringFitness.push_back(make_pair(arr[i], i));
    }

    sort(sortedOffspringFitness.begin(), sortedOffspringFitness.end());
}


bool isFeasible(vector<int> chromosome)
{
    int totalWeights=0;
    for(int j = 0; j<numberOfItems; j++)
    {
        if(chromosome[j])
        {
            totalWeights += itemsWeight[j];
        }
    }
    if(totalWeights > knapsackSize)
        return false;
    return true;
}

int calcTotalWeight(vector<int> chromosome)
{
    int totalWeight=0;
    for(int j = 0; j<numberOfItems; j++)
    {
        if(chromosome[j])
        {
            totalWeight += itemsWeight[j];
        }
    }
    return totalWeight;
}

int calcTotalValue(vector<int> chromosome)
{
    int totalValue=0;
    for(int j = 0; j<numberOfItems; j++)
    {
        if(chromosome[j])
        {
            totalValue += itemsValue[j];
        }
    }
    return totalValue;
}

//Initialize the Population
void initializePopulation()
{
    population.resize(populationSize);
    offsprings.resize(populationSize);
    for(int i = 0; i < populationSize; i++)
    {
        population[i].resize(numberOfItems);
        offsprings[i].resize(populationSize);
        for(int j = 0; j<numberOfItems; j++)
        {
            population[i][j] = 0;
            offsprings[i][j] = 0;
            int random = randomNumber(0,9);
            if(random >= 5)
                population[i][j] = 1;
        }

        //To Handle The Infeasible Solutions -> remove the solution from the population
        if(!isFeasible(population[i]))
            i--;


    }
}

void printPopulation()
{
    for(int i=0; i<populationSize; i++)
    {
        for(int j=0; j < numberOfItems; j++)
        {
            cout<< population[i][j] << " ";
        }
        cout<<endl;
    }
}

int getFitness(vector<int> chromosome)
{
    int totalValues=0;
    for(int j = 0; j < numberOfItems; j++)
    {
        if(chromosome[j])
        {
            totalValues += itemsValue[j];
        }
    }
    return totalValues;
}

//Evaluate Fitness of Individuals
void evaluateFitness()
{
    for(int i = 0; i < populationSize; i++)
    {
        int totalValues=0;
        for(int j = 0; j < numberOfItems; j++)
        {
            if(population[i][j])
            {
                totalValues += itemsValue[j];
            }
        }
        populationFitness[i] = totalValues; //store fitness for each chromosome
    }
}

void calcOffspringsFitness()
{
    for(int i = 0; i < populationSize; i++)
    {
        int totalValues=0;
        for(int j = 0; j < numberOfItems; j++)
        {
            if(offsprings[i][j])
            {
                totalValues += itemsValue[j];
            }
        }
        offspringsFitness[i] = totalValues; //store fitness for each offsprings
    }
}

//Select Parents for Reproduction "Roulette Wheel"
vector<int> rouletteWheelSelection()
{
    //The worst will have fitness 1, second worst 2 etc. and the best will have fitness N.
    //Calculate cumulative Fitness and make roulette wheel.
    int sum = 0;
    wheelDivisions[0] = sum;
    for(int i=1; i<=populationSize; i++)
    {
        sum += i;
        wheelDivisions[i] = sum;
    }

    int random = randomNumber(0, sum);
    int index, chromosomeIndex;

    for(int i=0; i<populationSize; i++)
    {
        if(random >= wheelDivisions[i] && random <= wheelDivisions[i+1])
        {
            index = wheelDivisions[i+1] - wheelDivisions[i];
            chromosomeIndex = sortedFitness[index-1].second;
            break;
        }
    }

    return population[chromosomeIndex];
}

//rank selection
void rankSelection()
{
    sortArr(populationFitness, populationSize);
    for(int i=0; i<populationSize; i++)
    {
        offsprings[i] = rouletteWheelSelection();
    }
    calcOffspringsFitness();
    sortArrOffsprings(offspringsFitness, populationSize);
}


//crossover
void crossover()
{

    int siz;
    //handle if the size is odd
    (populationSize%2!=0)? siz=populationSize-1 : siz=populationSize;

    for(int i=0; i<siz; i+=2)
    {
        float r2 = floatRandomNumber(0.0, 1.0);

        //perform crossover only if r2 <= pc
        if(r2 <= pc)
        {
            int separationPoint = randomNumber(1, numberOfItems-1);
            vector<int> offspring1(numberOfItems, 0), offspring2(numberOfItems, 0);


            for(int j=0; j<separationPoint; j++)
            {
                offspring1[j] = offsprings[i][j];
                offspring2[j] = offsprings[i+1][j];
            }

            for(int j=separationPoint; j<numberOfItems; j++)
            {
                offspring1[j] = offsprings[i+1][j];
                offspring2[j] = offsprings[i][j];
            }

            int oldFitness1 = getFitness(offsprings[i]);
            int oldFitness2 = getFitness(offsprings[i+1]);
            int newFitness1 = getFitness(offspring1);
            int newFitness2 = getFitness(offspring1);
            if((isFeasible(offspring1) && isFeasible(offspring2)) &&
               (oldFitness1 < newFitness1)&&(oldFitness2 < newFitness2))
            {
                offsprings[i] = offspring1;
                offsprings[i+1] = offspring2;

                offspringsFitness[i] = newFitness1;
                offspringsFitness[i+1] = newFitness2;

            }

        }
    }
    calcOffspringsFitness();
    sortArrOffsprings(offspringsFitness, populationSize);

}

//mutation
void mutation()
{
    for(int i=0; i<populationSize; i++)
    {
        int oldFitness = getFitness(offsprings[i]);

        for(int j=0; j<numberOfItems; j++)
        {
            float r = floatRandomNumber(0.0, 1.0);
            if(r < pm)
            {

                offsprings[i][j] = !offsprings[i][j];
            }
        }


        int newFitness = getFitness(offsprings[i]);

        //undo mutation if result is infeasible
        if(!isFeasible(offsprings[i]) || oldFitness > newFitness)
        {
            for(int j=0; j<numberOfItems; j++)
                offsprings[i][j] = !offsprings[i][j];

        }

        offspringsFitness[i] = getFitness(offsprings[i]);

    }
    calcOffspringsFitness();
    sortArrOffsprings(offspringsFitness, populationSize);

}



vector<int> getFittest()
{
    int fittestIndex, maxfitness = -1;
    for(int i=0; i<numberOfItems; i++)
    {
        if(offspringsFitness[i] > maxfitness)
        {
            maxfitness = offspringsFitness[i];
            fittestIndex = i;
        }

    }

    //after replacement offsprings is in population
    return population[fittestIndex];
}

void replacement()
{
    vector<vector<int>> bestOfBothWorlds;
    clear2DVector(bestOfBothWorlds);

    for(int i=0; i<populationSize; i++)
    {
        int Parent = getFitness(population[i]);
        int Offspring = getFitness(offsprings[i]);
        if(Parent>Offspring)
            bestOfBothWorlds[i] = population[i];

        else
            bestOfBothWorlds[i] = offsprings[i];
    }

    clear2DVector(population);
    population = bestOfBothWorlds;
    clear2DVector(offsprings);

}


int main()
{
    freopen("input.txt", "rt", stdin);
    freopen("output.txt", "wt", stdout);
    int testCase, t = 1;
    cin>>testCase;
    while(testCase--)
    {
        clear2DVector(offsprings);
        cin>> knapsackSize >> numberOfItems;

        if(numberOfItems<=10)
            populationSize = pow(2, numberOfItems);
        else
            populationSize = 1024;

        for(int i=0; i<numberOfItems; i++)
        {
           cin>> itemsWeight[i]>> itemsValue[i];
        }

        initializePopulation();

        for(int i=0; i<MaxNumberOfGenerations; i++)
        {
            evaluateFitness();
            rankSelection();
            crossover();
            mutation();
            replacement();

        }

        cout<<"\nTest Case number : "<< t << endl;
        vector<int> fittestChromosome = getFittest();
        int value = calcTotalValue(fittestChromosome);
        int counter=0;
        cout<<"\nSelected items:\n";
        cout<<"#   weight   value\n";
        for(int i=0; i<numberOfItems; i++)
        {
            if(fittestChromosome[i])
            {
                counter++;

                cout<< counter<< "\t"<< itemsWeight[i]<< "\t"<< itemsValue[i]<<endl;

            }
        }
        cout<<"\nNumber of items in the bag: "<<counter<<endl;
        cout<<"Total value: "<<value<<endl;
        cout<<"\n****************************************\n";
        t++;

    }
    return 0;
}
