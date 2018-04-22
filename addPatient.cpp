//********************************************************
//         Name: Abdullah Alharbi
// 
//       Course: csci370 - Project
// 
// Program name: addPatient.cpp
// 
//    Behaviour: The function of this program is adding
//               a new patient to the dental office
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
   if(args != 5)
   {
      cout << "Arguments should be as follows:" << endl;
      cout << "\tPatient number" << endl;
      cout << "\tPatient name" << endl;
      cout << "\tPhone number" << endl;
      cout << "\tAddress" << endl;
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
    
   string pid = argv[1];
   string pname = argv[2];
   string pphone = argv[3];
   string passress = argv[4];
   
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
      dquery = dquery + "select pid "
                      + "from Patients "
                      + "where pid=" + pid;                        
      ResultSet *rs = stmt->executeQuery(dquery);
      
      if(!rs->next())
      {
         string insertPatient = "";
         insertPatient = insertPatient + "insert into Patients "
                                       + "Values('"
                                       + pid
                                       + "','"
                                       + pname
                                       + "','"
                                       + pphone
                                       + "','"
                                       + passress
                                       + "')";
         if(insertIntoTable(stmt, insertPatient))
         {
            cout << "\nPatient with the following information added:" << endl;
            cout << "\t     ID: " << pid << endl;
            cout << "\t   Name: " << pname << endl;
            cout << "\t  Phone: " << pphone << endl;
            cout << "\tAddress: " << passress << endl;
            conn->commit();
         }
         else
         {
            cout << "\nError: patient\'s information was not stored!\n" << endl;
         }
      }
      else
      {
         cout << "\tPatient already exists!" << endl;
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
