#ifndef BEE_FISH_SERVER__LOG_H
#define BEE_FISH_SERVER__LOG_H
#include <fstream>
#include "../config.h"

inline std::ofstream &logfile()
{
   static std::ofstream lf (BEE_FISH_SERVER_LOG_FILE);
   return lf;
}

inline std::ofstream &errfile()
{
   static std::ofstream lf (BEE_FISH_SERVER_ERR_FILE);
   return lf;
}

inline void initializeLogs() {

   using namespace std;
   
   clog.rdbuf(logfile().rdbuf());
   cerr.rdbuf(errfile().rdbuf());


   cout << "Standard out" << endl;
   cerr << "Standard err" << endl;
   clog << "Standard log" << endl;

}
#endif