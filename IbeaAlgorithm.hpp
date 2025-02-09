#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <algorithm>
#include "Solution.hpp"

struct IbeaAlgorithme
{

    int NbVariable;
    int NbConstraints;
    int NbObjectives;
    int IndCandidat;
    int NbPop;
    int Nbind;
    float kappa;

    /* Price function */
    std::vector<int> *Price;
    std::vector<float> *Indicator_matrix;
    /* Utility function */

    /*Matrix of constraint*/
    std::vector<float> *CostMatrix;
    /*Vector of solution*/

    std::vector<float> *constraint;
    std::vector<Solution> *Population;
    std::vector<Solution> *Echantillon;
    std::vector<Solution> *Population_elected;

    IbeaAlgorithme(std::string name, int nbpop, int nbind, float K)
    {

        int k = 0;
        int number;
        int candidat;
        kappa = K;
        Nbind = nbind;
        NbPop = nbpop;
        Indicator_matrix = new std::vector<float>[nbpop];
        Population = new std::vector<Solution>;

        std::ifstream fichier(name);
        if (fichier)
        {

            fichier >> NbVariable;
            fichier >> NbObjectives;
            fichier >> NbConstraints;

            std::cout << "Les caractéristiques du problèmes sont: " << std::endl;
            std::cout << "Le nombre d'objectifs est: " << NbObjectives << std::endl;
            std::cout << "Le nombre de contraintes est: " << NbConstraints << std::endl;

            Price = new std::vector<int>[NbObjectives];
            CostMatrix = new std::vector<float>[NbConstraints];
            constraint = new std::vector<float>[NbConstraints];

            for (int i = 0; i < NbObjectives; i++)
            {
                while (k < NbVariable)
                {
                    fichier >> number;
                    Price[i].push_back(number);
                    k++;
                };
                k = 0;
            }

            for (int i = 0; i < NbConstraints; i++)
            {
                k = 0;

                while (k < NbVariable)
                {
                    fichier >> number;
                    CostMatrix[i].push_back(number);
                    k++;
                }
            }

            for (int i = 0; i < NbConstraints; i++)
            {
                fichier >> number;
                std::cout << "la contrainte est: " << number << std::endl;
                constraint[i].push_back(number);
            }

            for (int i = 0; i < nbpop; i++)
            {
                for (int j = 0; j < nbpop; j++)
                {
                    Indicator_matrix[i].push_back(0);
                }
            }
        }
    }

    void computeIndicator()
    {
        float Max[NbObjectives][2];
        float Max_indicator[2] = {0, 0};
        float Max_indicator_choice = 0;
        for (int j = 1; j < NbPop; j++)
        {
            for (int i = 0; i < j; j++)
            {
                Max[0][0] = Population[0][i].obj1 - Population[0][j].obj1;
                Max[0][1] = Population[0][i].obj2 - Population[0][j].obj2;
                Max[1][0] = Population[0][j].obj1 - Population[1][i].obj1;
                Max[1][1] = Population[0][j].obj2 - Population[1][i].obj2;
                // Comparaisont between two solutions
                for (int i = 0; i < 2; i++)
                {
                    if (Max[i][0] > Max[i][1])
                    {
                        Max_indicator[i] = Max[i][0];
                    }
                    else
                    {
                        Max_indicator[i] = Max[i][1];
                    }
                }
                Indicator_matrix[i][j] = Max_indicator[0];
                Indicator_matrix[j][i] = Max_indicator[1];
            }
        }
    }

    void computeFitnessSolutionValue()
    {
        float sum;
        for (int i = 0; i < NbPop; i++)
        {
            sum = 0;
            for (int j = 0; j < NbPop; j++)
            {
                sum += -exp(-Indicator_matrix[j][i] / kappa);
            }
            Population[0][i].FitnessValue = sum + 1;
        }
    }

    void updateFitnessIndividualSolutionValue(Solution &individual, int index_exit_solution)
    {
        individual.FitnessValue += Population[0][index_exit_solution].FitnessValue;
    }

    void sumconstraint(Solution &individual)
    {
        float addconstraint;
        for (int j = 0; j < NbConstraints; j++)
        {
            addconstraint = 0;
            for (int i = 0; i < NbVariable; i++)
            {
                addconstraint += CostMatrix[j][i] * individual.solution[0][i];
            }
            individual.SumConstraint->push_back(addconstraint);
            std::cout << individual.SumConstraint[0][j] << std::endl;
        }
    }

    void checkPopindividual(Solution &individual, int &compteur)
    {
        bool check = true;

        for (int i = 0; i < NbConstraints; i++)
        {
            std::cout << "La contrainte est: " << constraint[i][0] << std::endl;
            if (individual.SumConstraint[0][i] > constraint[i][0] || individual.SumConstraint[0][i] == 0)
            {

                check = false;
                break;
            }
        }

        std::cout << "le booléen est: " << check << std::endl;

        if (check == false)
        {
            std::cout << "La solution n'est pas admissible" << std::endl;
        }
        else
        {

            std::cout << "La solution est admissible" << std::endl;
            Population->push_back(individual);
            compteur++;
        }
    }

    void initPopulation()
    {
        int compteur = 0;
        int nbrandomseeds;
        int indice;
        int value;
        while (compteur < NbPop)
        {
            Solution individual;
            individual.rank = NbPop + 1;
            for (int k = 0; k < NbVariable; k++)
            {
                individual.solution->push_back(0);
            }
            nbrandomseeds = rand() % NbVariable;
            std::cout << nbrandomseeds << std::endl;
            for (int j = 0; j < nbrandomseeds; j++)
            {
                indice = rand() % NbVariable;
                value = rand() % 2;
                if (value > 0)
                {

                    individual.solution[0][indice] = 1;
                }
            }
            sumconstraint(individual);
            individual.computeFitessValue(Price, NbVariable);
            individual.displayIndividual(NbVariable, NbConstraints);

            checkPopindividual(individual, compteur);
        }
    }

    void displayPopulation()
    {

        sort(Population[0].begin(), Population[0].end(), Solution());

        std::cout << "La population est: " << std::endl;
        for (int i = 0; i < NbPop; i++)
        {
            std::cout << "individu_" << i + 1 << " : ";
            for (int j = 0; j < NbVariable; j++)
            {

                std::cout << Population[0][i].solution[0][j] << " ";
            }

            std::cout << "obj1: " << Population[0][i].obj1 << " " << "obj2: " << Population[0][i].obj2 << " rank: " << Population[0][i].rank << std::endl;
        }
    }

    void displayList(std::vector<Solution> &List)
    {

        std::cout << "La Liste est: " << std::endl;
        for (int i = 0; i < List.size(); i++)
        {
            std::cout << "individu_" << i + 1 << " : ";
            for (int j = 0; j < NbVariable; j++)
            {

                std::cout << List[i].solution[0][j] << " ";
            }

            std::cout << "obj1: " << List[i].obj1 << " " << "obj2: " << List[i].obj2 << " rank: " << List[i].rank << std::endl;
        }
    }

    void displayEchantillon()
    {
        std::cout << "L'Echantillon est: " << std::endl;
        for (int i = 0; i < Nbind; i++)
        {
            std::cout << "individu_" << i + 1 << " : ";
            for (int j = 0; j < NbVariable; j++)
            {

                std::cout << Echantillon[0][i].solution[0][j] << " ";
            }

            std::cout << "obj1: " << Echantillon[0][i].obj1 << " " << "obj2: " << Echantillon[0][i].obj2 << std::endl;
        }
    }

    void displayCostMatrix()
    {
        for (int i = 0; i < NbConstraints; i++)
        {
            for (int j = 0; j < NbVariable; j++)
            {
                std::cout << CostMatrix[i][j] << " ";
            }

            std::cout << std::endl;
        }
    }

    void displayProblemIbea()
    {

        std::cout << "Les prix des fonctions objectifs sont: " << std::endl;
        for (int i = 0; i < NbObjectives; i++)
        {
            for (int j = 0; j < NbVariable; j++)
            {
                std::cout << Price[i][j] << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "La matrice des contraintes est: " << std::endl;
        for (int i = 0; i < NbConstraints; i++)
        {
            for (int j = 0; j < NbVariable; j++)
            {
                std::cout << CostMatrix[i][j] << " ";
            }

            std::cout << std::endl;
        }

        std::cout << "Les contraintes sont:";
        for (int i = 0; i < NbConstraints; i++)
        {
            std::cout << constraint[i][0] << std::endl;
        }
    }

    void objPop(/*std::vector<Solution> & Echantillon, int dim*/)
    {
        float fitnessvalue1;
        float fitnessvalue2;
        int compteur = 0;
        while (compteur < NbPop)
        {
            fitnessvalue1 = 0;
            fitnessvalue2 = 0;
            if (Population[0][compteur].fitnessCalculated == false)
            {
                for (int i = 0; i < NbVariable; i++)
                {
                    fitnessvalue1 += Population[0][compteur].solution[0][i] * Price[0][i];
                    fitnessvalue2 += Population[0][compteur].solution[0][i] * Price[1][i];
                }
                Population[0][compteur].obj1 = fitnessvalue1;
                Population[0][compteur].obj2 = fitnessvalue2;

                Population[0][compteur].fitnessCalculated = true;
            }

            compteur++;
        }
    }

    
    void ObjSample(){
        float fitnessvalue1;
        float fitnessvalue2; 
        int compteur = 0;
        while(compteur < Nbind)
        {
            fitnessvalue1 = 0; 
            fitnessvalue2 = 0; 
            if(Echantillon[0][compteur].fitnessCalculated == false){
                for(int i = 0; i < NbVariable; i++){
                    fitnessvalue1 += Echantillon[0][compteur].solution[0][i] * Price[0][i];
                    fitnessvalue2 += Echantillon[0][compteur].solution[0][i] * Price[1][i]; 
                }
                Echantillon[0][compteur].obj1 = fitnessvalue1;
                Echantillon[0][compteur].obj2 = fitnessvalue2;
                Echantillon[0][compteur].fitnessCalculated = true;
            }

            compteur++;


        }


    }



    void EnvironmentalSelection(int & generation)
    {
        Population_elected = new std::vector<Solution>;
        sort(Population[0].begin(), Population[0].end(), RangeFitnessSolution());
        for (int i = 0; i < NbPop; i++)
        {
            Population_elected[0].push_back(Population[0][i]);
        }
        Population = new std::vector<Solution>;
        for (int i = 0; i < NbPop; i++)
        {
            Population[0].push_back(Population_elected[0][i]);
        }
        generation++; 
    }

    void binaryTournament()
    {

        Echantillon = new std::vector<Solution>;

        Solution individual1;
        Solution individual2;
        int compteur = 0;
        int randomIndividual1;
        int randomIndividual2;
        while (compteur < Nbind)
        {
            randomIndividual1 = rand() % NbPop;
            randomIndividual2 = rand() % NbPop;

            individual1 = Population_elected[0][randomIndividual1];
            individual2 = Population_elected[0][randomIndividual2];

            if (individual1.FitnessValue < individual2.FitnessValue)
            {
                Echantillon->push_back(individual2);
            }
            else
            {

                Echantillon->push_back(individual1);
            }

            compteur++;
        }
    }

    void CrossoverMutation()
    {
        int ind_Parent1 = rand() % Nbind;
        int ind_Parent2 = rand() % Nbind;
        int ind_crossover = rand() % NbVariable;
        int Getmuted;
        Solution Enfant1;
        Solution Enfant2;
        /*for(int k = 0; k < NbVariable; k++){

        } */
        for (int i = 0; i < ind_crossover; i++)
        {
            Enfant1.solution->push_back(Echantillon[0][ind_Parent1].solution[0][i]);
            Enfant2.solution->push_back(Echantillon[0][ind_Parent2].solution[0][i]);
        }

        for (int j = ind_crossover; j < NbVariable; j++)
        {

            Enfant2.solution->push_back(Echantillon[0][ind_Parent1].solution[0][j]);
            Enfant1.solution->push_back(Echantillon[0][ind_Parent2].solution[0][j]);
        }

        Getmuted = rand() % 3;
        std::cout << "choixmutation: " << Getmuted << std::endl;
        if (Getmuted > 0)
        {
            std::cout << "Enfant1" << std::endl;

            Addmutation(Enfant1);
            std::cout << "Enfant2" << std::endl;

            Addmutation(Enfant2);
        }

        sumconstraint(Enfant1);
        sumconstraint(Enfant2);

        CheckSampleindividual(Enfant1, 0, ind_Parent1);
        CheckSampleindividual(Enfant2, 0, ind_Parent2);
    }

    void CheckSampleindividual(Solution &individual, int k, int ind)
    {
        int compteur = k;
        bool check = true;
        for (int i = 0; i < NbConstraints; i++)
        {
            std::cout << "La contrainte est: " << constraint[i][0] << std::endl;
            std::cout << "la somme des contraintes est: " << individual.SumConstraint[0][i] << std::endl;
            std::cout << "check: " << check << std::endl;
            if (individual.SumConstraint[0][i] > constraint[i][0] || individual.SumConstraint[0][i] < 1)
            {

                check = false;
                break;
            }
        }
        std::cout << "le booléen est: " << check << std::endl;
        if (check == false)
        {
            std::cout << "La solution n'est pas admissible" << std::endl;

            if (compteur < 1)
            {
                RepaireSample(individual);
                compteur++;
                CheckSampleindividual(individual, compteur, ind);
            }
        }
        else
        {
            std::cout << "La solution est admissible" << std::endl;
            Echantillon[0][ind] = individual;
            compteur++;
        }
    }

    void Addmutation(Solution &Enfant)
    {

        std::cout << "mutation enfant" << std::endl;
        int compteur = 0;
        int nb_one = 0;
        int indice;
        int choix;

        for (int i = 0; i < NbVariable; i++)
        {
            if (Enfant.solution[0][i] < 1)
            {
                nb_one++;
            }
        }

        if (nb_one > 0)
        {
            int listChoix[nb_one];
            std::cout << "nbone: " << nb_one << std::endl;
            for (int i = 0; i < NbVariable; i++)
            {
                if (Enfant.solution[0][i] < 1)
                {
                    listChoix[compteur] = i;
                    compteur++;
                }
            }

            indice = rand() % compteur;
            std::cout << "indice Enfant: " << indice << std::endl;
            choix = listChoix[indice];
            Enfant.solution[0][choix] = 1;
            std::cout << "Enfant Solution: " << Enfant.solution[0][choix] << std::endl;
        }
        else
        {
            std::cout << "Pas de mutation possible" << std::endl;
        }
    }

    void RepaireSample(Solution &Enfant)
    {
        int compteur = 0;
        int nb_one = 0;
        int indice;
        int choix;
        std::cout << "repaire individual" << std::endl;

        for (int i = 0; i < NbVariable; i++)
        {
            if (Enfant.solution[0][i] > 0)
            {
                nb_one++;
            }
        }
        int listChoix[nb_one];
        for (int i = 0; i < NbVariable; i++)
        {
            if (Enfant.solution[0][i] > 0)
            {
                listChoix[compteur] = i;
                compteur++;
            }
        }

        indice = rand() % compteur;
        std::cout << "indice à supprimer: " << indice << std::endl;
        choix = listChoix[indice];
        Enfant.solution[0][choix] = 0;
        sumconstraint(Enfant);
    }

    // Update by rank and crowding distance

    void AddPopulation()
    {
        int compteur = 0;
        int indicefront = 0;
        bool Update = false;
        int indiceVec = 0;
        int comptList = 0;
        int nbListInd = NbPop + Nbind;
        ObjSample();
        std::vector<Solution> List;
        for (int i = 0; i < NbPop; i++)
        {
            Population[0].push_back(Echantillon[0][i]);
        }
        std::cout << "all offspring added "  << std::endl;
    }


    void resolve(int Nbgen)
    {
        int generation = 0;
        int number_mutation = 0;
        initPopulation();
        objPop();
        while(generation < Nbgen){
            computeIndicator();
            computeFitnessSolutionValue();
            EnvironmentalSelection(generation);
            if(generation > Nbgen - 1){
                break;
            }
            else
            {
            binaryTournament(); 
            number_mutation = rand() % Nbind;
            for(int k = 0; k < number_mutation; k ++){
            CrossoverMutation(); 
            }
        }
        AddPopulation();
        }}

};

