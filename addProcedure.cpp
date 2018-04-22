//********************************************************
//         Name: Abdullah Alharbi
// 
//       Course: csci370 - Project
// 
// Program name: addProcedure.cpp
// 
//    Behaviour: The function of this program is adding
//               a new procedure to the dental office
//               database that is offered
// 
//********************************************************


#include <iostream>
#include <occi.h>
#include <termios.h>
#include <unistd.h>

using namespace std;
using namespace oracle::occi;

// read database password from user input
// without showing the password on the screen
string readPassword()
{
    struct termios settings;
    tcgetattr( STDIN_FILENO, &settings );
    settings.c_lflag =  (settings.c_lflag & ~(ECHO));
    tcsetattr( STDIN_FILENO, TCSANOW, &settings );

    string password = "";
    getline(cin, password);

    settings.c_lflag = (settings.c_lflag |   ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &settings );
    return password;
}

bool checkArgs(int args)
{
   if(args != 4)
   {
      cout << "Arguments should be as follows:" << endl;
      cout << "\tProcedure name" << endl;
      cout << "\tEstimated duration" << endl;
      cout << "\tCost" << endl;
      return false;
   }
   return true;
}


bool insertIntoTable(Statement *stmt, string InsertStr)
{
   try
   {
      stmt->executeUpdate(InsertStr);
      return true;
   }
   catch (SQLException & e)
   {
      cout << e.what();
      return false;
   }
}


int main(int argc, char* argv[])
{
   if(!checkArgs(argc))
   {
      return 1;
   }
    
   string proname = argv[1];
   string duration = argv[2];
   string cost = argv[3];
   
   string userName = "alharbam";
   string password;
   // address of the Oracle server
   const string connectString = "sunfire.csci.viu.ca";

   cout << "Your user name: ";
   getline(cin, userName);

   cout << "Your password: ";
   password = readPassword();
   cout << endl;
   try
   {
      // establish database connectioncheckCard(cardWithAcess,cid);
      Environment *env = Environment::createEnvironment();
      Connection *conn = env->createConnection (userName, password, connectString);
      Statement *stmt = conn->createStatement();
       
      // check if the dentest id already exists
      string dquery = "";
      dquery = dquery + "select proname "
                      + "from Procedures "
                      + "where proname='" + proname
                      + "'";
      ResultSet *rs = stmt->executeQuery(dquery);
      
      if(!rs->next())
      {
         string insertProcedures = "";
         insertProcedures = insertProcedures + "insert into Procedures "
                                             + "Values('"
                                             + proname
                                             + "',"
                                             + duration
                                             + ","
                                             + cost
                                             + ")";
         if(insertIntoTable(stmt, insertProcedures))
         {
            cout << "\nAssistent with the following information added:" << endl;
            cout << "\tProcedur\'s name: "  << proname << endl;
            cout << "\t        Duration: "  << duration << endl;
            cout << "\t            Cost: $" << cost << endl;
            conn->commit();
         }
         else
         {
            cout << "\nError: procedure\'s information was not stored!\n" << endl;
         }
      }
      else
      {
         cout << "\tProcedure already exists!" << endl;
      }
      
      stmt->closeResultSet(rs);
      conn->terminateStatement(stmt);
      env->terminateConnection(conn);
      Environment::terminateEnvironment(env);
   }
   catch (SQLException & e)
   {
      cout << e.what();
      return 0;
   }
   return 0;
}
