#include<iostream> 
#include<vector> 
#include<fstream> 
#include<string>
#include "IbeaAlgorithm.hpp"

int main(){
      
      
      /*La fonction checkup n'est pas bonne */
 
   std::string file = "knapsack.txt";
   IbeaAlgorithme NSGAII = IbeaAlgorithme(file,100,5,2);
 NSGAII.resolve(20);
  

}