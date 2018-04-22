//********************************************************
//         Name: Abdullah Alharbi
// 
//       Course: csci370 - Project
// 
// Program name: addAssistent.cpp
// 
//    Behaviour: The function of this program is adding
//               a new assistent to the dental office
//               database
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
   if(args != 6)
   {
      cout << "Arguments should be as follows:" << endl;
      cout << "\tAssistent number" << endl;
      cout << "\tAssistent name" << endl;
      cout << "\tPhone number" << endl;
      cout << "\tAddress" << endl;
      cout << "\tAssistent wage" << endl;
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
    
   string aid = argv[1];
   string aname = argv[2];
   string aphone = argv[3];
   string aaddress = argv[4];
   string wage = argv[5];
   
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
      dquery = dquery + "select aid "
                      + "from Assistents "
                      + "where aid=" + aid;                        
      ResultSet *rs = stmt->executeQuery(dquery);
      
      if(!rs->next())
      {
         string insertAssistents = "";
         insertAssistents = insertAssistents + "insert into Assistents "
                                             + "Values('"
                                             + aid
                                             + "','"
                                             + aname
                                             + "','"
                                             + aphone
                                             + "','"
                                             + aaddress
                                             + "',"
                                             + wage
                                             + ")";
         if(insertIntoTable(stmt, insertAssistents))
         {
            cout << "\nAssistent with the following information added:" << endl;
            cout << "\t     ID: " << aid << endl;
            cout << "\t   Name: " << aname << endl;
            cout << "\t  Phone: " << aphone << endl;
            cout << "\tAddress: " << aaddress << endl;
            cout << "\t   Wage: " << wage << " per hour" << endl;
            conn->commit();
         }
         else
         {
            cout << "\nError: assistent\'s information was not stored!\n" << endl;
         }
      }
      else
      {
         cout << "\tAssistent already exists!" << endl;
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
