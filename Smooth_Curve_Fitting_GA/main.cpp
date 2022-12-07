#include <bits/stdc++.h>

using namespace std;


vector<vector<double>> population, offsprings;
vector<pair<double, int>> sortedFitness; //mapped with the old index for each fitness
vector<pair<double, int>> sortedOffspringFitness;
int numberOfPoints, degree, numberOfCoefficients;
const double pc = 0.4, pm = 0.01, LB = -5, UB = 5, b = 1, eliteRatio = 0.1;
const int populationSize = 100, MaxNumberOfGenerations = 5, numberOfElites = eliteRatio * populationSize;
double pointX[100], pointY[100], populationFitness[populationSize], offspringsFitness[populationSize];
vector<int> bestSoFarIndices(numberOfElites, 0);

void clear2DVector(vector<vector<double>> &v)
{
    v.resize(populationSize);
    for(int i = 0; i < populationSize; i++)
    {
        v[i].resize(populationSize);
        for(int j = 0; j<numberOfCoefficients; j++)
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

double doubleRandomNumber(double a, double b)
{
    static bool second = true;
    if(second)
    {
        srand(time(NULL));
        second = false;
    }
    double result = a + static_cast<double>(rand()) * static_cast<double>(b - a) / RAND_MAX;
    double finalResult = ceil(result * 100.0) / 100.0;
    return finalResult;
}

double approximate2DecimalPlace(double number)
{
    return ceil(number * 100.0) / 100.0;
}

void sortArr(double arr[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        sortedFitness.push_back(make_pair(arr[i], i));
    }

    sort(sortedFitness.begin(), sortedFitness.end());
}

void sortArrOffsprings(double arr[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        sortedOffspringFitness.push_back(make_pair(arr[i], i));
    }

    sort(sortedOffspringFitness.begin(), sortedOffspringFitness.end());
}

void initializePopulation()
{
    population.resize(populationSize);
    offsprings.resize(populationSize);
    for(int i=0; i<populationSize; i++)
    {
        population[i].resize(numberOfCoefficients);
        offsprings[i].resize(numberOfCoefficients);
        populationFitness[i] = 0;
        for(int j=0; j<numberOfCoefficients; j++)
        {
            population[i][j] = 0;
            offsprings[i][j] = 0;

            double randomCoefficient = doubleRandomNumber(LB, UB);
            randomCoefficient = approximate2DecimalPlace(randomCoefficient);
            population[i][j] = randomCoefficient;
        }
    }

}

void printPopulation()
{
    for(int i=0; i<populationSize; i++)
    {
        for(int j=0; j<numberOfCoefficients; j++)
        {
            cout<< population[i][j] << " ";
        }
        cout<<endl;
    }
}


double errorAtOnePoint(vector<double> chromosome, double x, double y)
{
    double sum = 0;
    for(int j=0; j<numberOfCoefficients; j++)
    {
        sum += chromosome[j] * pow(x, j);
    }
    sum -= y;
    double result = pow(sum, 2);
    return result;
}

double MSE(vector<double> chromosome)
{
    double sum = 0;
    for(int i=0; i<numberOfPoints; i++)
    {
        sum += errorAtOnePoint(chromosome, pointX[i], pointY[i]);
    }
    sum /= numberOfPoints;
    double result = approximate2DecimalPlace(sum);
    return result;
}

void evaluateFitness()
{
    for(int i=0; i<populationSize; i++)
    {
        //using total error to represent fitness, the smaller the better
        double result = MSE(population[i]);
        populationFitness[i] = result;
    }

    ///elitism
    //gets the best so far and stores their indices in bestSoFarIndices vector
    sortArr(populationFitness, populationSize);
    for(int i=0; i<numberOfElites; i++)
    {
        int eliteIndex = sortedFitness[i].second;
        bestSoFarIndices[i] = eliteIndex;
    }
}

void calcOffspringsFitness()
{
    for(int i=0; i<populationSize; i++)
    {
        //using total error to represent fitness, the smaller the better
        offspringsFitness[i] = MSE(offsprings[i]);
    }
    sortArrOffsprings(offspringsFitness, populationSize);
}


///selection
void tournamentSelection()
{
    for(int i=0; i<numberOfElites; i++)
    {
        int eliteIndex = bestSoFarIndices[i];
        offsprings[i] = population[eliteIndex];
    }

    for(int i=numberOfElites; i<populationSize; i++)
    {
        int randomIndex1 = randomNumber(0, populationSize);
        int randomIndex2 = randomNumber(0, populationSize);
        if(populationFitness[randomIndex1] <= populationFitness[randomIndex2])
        {
            offsprings[i] = population[randomIndex1];
        }
        else
        {
            offsprings[i] = population[randomIndex2];
        }
    }
    calcOffspringsFitness();
}

///crossover
void twoPointCrossover()
{
    int siz;
    //handle if the size is odd
    (populationSize%2!=0)? siz=populationSize-1 : siz=populationSize;

    for(int i=0; i<siz; i+=2)
    {
        double r2 = doubleRandomNumber(0.0, 1.0);

        //perform crossover only if r2 <= pc and is not elite
        //count returns zero if the index is not elite index
        //count returns one if exist in the vector bestSoFarIndices
        int isEliteIndex1 = count(bestSoFarIndices.begin(), bestSoFarIndices.end(), i);
        int isEliteIndex2 = count(bestSoFarIndices.begin(), bestSoFarIndices.end(), i+1);
        if(r2 <= pc && !isEliteIndex1 && !isEliteIndex2)
        {
            int separationPoint1 = randomNumber(1, numberOfCoefficients/2);
            int separationPoint2 = randomNumber(numberOfCoefficients/2, numberOfCoefficients-1);
            while(separationPoint1 == separationPoint2)
            {
                separationPoint2 = randomNumber(1, numberOfCoefficients-1);
            }

            vector<double> offspring1(numberOfCoefficients, 0), offspring2(numberOfCoefficients, 0);


            //same
            for(int j=0; j<separationPoint1; j++)
            {
                offspring1[j] = offsprings[i][j];
                offspring2[j] = offsprings[i+1][j];
            }
            //crossover happens here
            for(int j=separationPoint1; j<separationPoint2; j++)
            {
                offspring1[j] = offsprings[i+1][j];
                offspring2[j] = offsprings[i][j];
            }
            //same
            for(int j=separationPoint2; j<numberOfCoefficients; j++)
            {
                offspring1[j] = offsprings[i][j];
                offspring2[j] = offsprings[i+1][j];
            }

            offsprings[i] = offspring1;
            offsprings[i+1] = offspring2;
            calcOffspringsFitness();
        }

    }
}

///mutation
void nonUniformMutation(int t)
{
    for(int i=0; i<populationSize; i++)
    {
        int isEliteIndex = count(bestSoFarIndices.begin(), bestSoFarIndices.end(), i);
        if(!isEliteIndex)
        {
            for(int j=0; j<numberOfCoefficients; j++)
            {
                double rm = doubleRandomNumber(0.0, 1.0);
                if(rm < pm)
                {
                    double y, deltaL, deltaU;
                    deltaL = offsprings[i][j] - LB;
                    deltaU = UB - offsprings[i][j];
                    double r1 = doubleRandomNumber(0.0, 1.0);

                    if(r1 <= 0.5)
                        y = deltaL;
                    else
                        y = deltaU;

                    double r = doubleRandomNumber(0.0, 1.0);
                    double term = pow( 1 - (t/MaxNumberOfGenerations) , b);
                    double change = y * (1 - pow(r, term));
                    if(y == deltaL)
                        offsprings[i][j] -= change;
                    else
                        offsprings[i][j] += change;

                    calcOffspringsFitness();
                }
            }
        }
    }

}

///reproduction
void elitismReplacement()
{
    clear2DVector(population);
    population = offsprings;
    clear2DVector(offsprings);
}

int getFittest()
{
    int fittestIndex;
    evaluateFitness();
    double minFitness = populationFitness[0];
    for(int i=0; i<populationSize; i++)
    {
        if(populationFitness[i] <= minFitness)
        {
            minFitness = populationFitness[i];
            fittestIndex = i;
        }

    }

    //after replacement offsprings is in population
    return fittestIndex;
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

        cin>> numberOfPoints >> degree;

        numberOfCoefficients = degree+1;

        for(int i=0; i<numberOfPoints; i++)
        {
           cin>>pointX[i]>> pointY[i];
        }

        initializePopulation();

        for(int i=0; i<MaxNumberOfGenerations; i++)
        {
            evaluateFitness();
            tournamentSelection();
            twoPointCrossover();
            nonUniformMutation(i);
            elitismReplacement();
        }

        cout<<"\nDataset Index : "<< t << endl;
        int fittestIndex = getFittest();
        vector<double> fittestChromosome = population[fittestIndex];

        cout<<"\nCoefficients:\n";
        for(int i=0; i<numberOfCoefficients; i++)
        {
            cout<< "a" << i << " = "<< fittestChromosome[i]<<endl;

        }
        cout<<"\nMean Square Error: "<<populationFitness[fittestIndex]<<endl;
        cout<<"\n****************************************\n";
        t++;

    }
    return 0;
}
