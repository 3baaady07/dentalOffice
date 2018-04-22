//****************************************
//         Name: Abdullah Alharbi
//
//       Course: csci370 - A3
//
// Program name: dentalOffice
//
//    Behaviour: The program adds 
//               appointments Office
//               to database, do checkout
//               and calculates 
//               assistents pay
//****************************************
#include <iostream>
#include <string>
#include <stdlib.h>
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


// Give each prepared statement a name
struct STMT
{
    string name;
    Statement *stmt;
};

// Prepare the statements that would be used repeatedly
// in this program
int initStatements(Connection *conn, STMT * & statements)
{
    int size = 9;
    statements = new STMT[size];

    // retrieve max apid
    statements[0].name = "getMaxAPID";
    string queryStr = "select max(apid)\n \
                       from Appointments";
    statements[0].stmt = conn->createStatement(queryStr);

    // check if date and time is already taken
    statements[1].name = "checkTimeSlot";
    queryStr = "select apid\n \
                from Appointments\n \
                where\n \
                did=:1 and\n \
                to_char(aDate,'yyyy/mm/dd hh') like :2";
    statements[1].stmt = conn->createStatement(queryStr);
    
    // insert into Appointments
    statements[2].name = "insertAppointment";
    queryStr = "insert into Appointments (apid, did, pid, aid, aDate)\n \
                values( :1 , :2 , :3 , :4 , to_date(:5,'yyyy/mm/dd hh'))";
    statements[2].stmt = conn->createStatement(queryStr);

    // retrieve procedure information
    statements[3].name = "getProcedure";
    queryStr = "select cost\n \
                from Procedures\n \
                where proname=:1 " ;
    statements[3].stmt = conn->createStatement(queryStr);

    // build Details for an appointment
    statements[4].name = "insertDetails";
    queryStr = "insert into Details (apid, proname, occurences)\n \
                values( :1 , :2 , :3)";
    statements[4].stmt = conn->createStatement(queryStr);

    // retrieve appointment information
    statements[5].name = "getAPID";
    queryStr = "select apid\n \
                from Appointments\n \
                where pid=:1 and\n \
                aDate=to_date(:2,'yyyy/mm/dd hh')" ;
    statements[5].stmt = conn->createStatement(queryStr);
    
    // retrieve detailed information
    statements[6].name = "getDetailes";
    queryStr = "select proname, occurences\n \
                from Details\n \
                where apid=:1 " ;
    statements[6].stmt = conn->createStatement(queryStr);
    
    // retrieve assistent's wage
    statements[7].name = "getAssistentWage";
    queryStr = "select wage\n \
                from Assistents\n \
                where aid=:1 " ;
    statements[7].stmt = conn->createStatement(queryStr);
    
    // update Appointments table to reflict checkout
    statements[8].name = "updateAppointment";
    queryStr = "update Appointments\n \
                SET TOTALDURATION=:1\n \
                where apid=:2 " ;
    statements[8].stmt = conn->createStatement(queryStr);
    return size;
}



// Given the name, find the corresponding prepared sql statement
Statement * findStatement(string name, STMT *statements, int size)
{
    for(int i = 0; i < size; i++)
    {
        if (statements[i].name == name)
            return statements[i].stmt;
    }
    return 0;
}


// Terminate all the prepared statements
void terminateStatements(Connection *conn, STMT *statements, int size)
{
    for(int i = 0; i < size; i++)
        conn->terminateStatement(statements[i].stmt);
}

string getData(string id)
{
    cout << "Enter " << id << ": ";
    string data;
    cin >> data;

    return data;
}

// check if a time slot is available
bool checkTimeSlot(Statement *stmt, string did, string date)
{
   stmt->setString(1, did);
   stmt->setString(2, date);
   ResultSet *rs = stmt->executeQuery();
   bool exists = rs->next();
   
   stmt->closeResultSet(rs);
   return exists;
}


bool insertAppointment(Statement *stmt, int apid, string did, string pid, string aid, string date)
{
   try
   {
      stmt->setInt(1, apid);
      stmt->setString(2, did);
      stmt->setString(3, pid);
      stmt->setString(4, aid);
      stmt->setString(5, date);
      
      int rows=stmt->executeUpdate();
      
      return (rows!=0);
   }
   catch (SQLException & e)
   {
      cout << e.what();
      return false;
   }
}

bool insertDetails(Statement *stmt, int apid, string proname, int occurrences)
{
   try
   {
      stmt->setInt(1, apid);
      stmt->setString(2, proname);
      stmt->setInt(3, occurrences);
      int rows=stmt->executeUpdate();
      return (rows!=0);
   }
   catch (SQLException & e)
   {
      cout << e.what();
      return false;
   }
}

// grab the max value of apid
int getMaxAPID(Statement *stmt)
{
   int maxAPID=0;
   ResultSet *rs = stmt->executeQuery();
   if(rs->next())
   {
      maxAPID = rs->getInt(1);
   }
   stmt->closeResultSet(rs);
   return maxAPID;
}


int getAPID(Statement *stmt, string pid, string date)
{
   int apid = -1;
   stmt->setString(1, pid);
   stmt->setString(2, date);
   ResultSet *rs = stmt->executeQuery();
   if(rs->next())
   {
      apid = rs->getInt(1);
   }
   stmt->closeResultSet(rs);
   return apid;
}

bool getProcedure(Statement *stmt, string proname)
{
   stmt->setString(1, proname);
   ResultSet *rs = stmt->executeQuery();
   bool exists = rs->next();
   stmt->closeResultSet(rs);
   return exists;
}

bool updateAppointment(Statement *stmt, int apid, int h)
{
   try
   {
      stmt->setInt(1, h);
      stmt->setInt(2, apid);
      
      int rows=stmt->executeUpdate();
      
      return (rows!=0);
   }
   catch (SQLException & e)
   {
      cout << e.what();
      return false;
   }
}


double calculateTotal(int apid, STMT *statements, int size)
{
   double sum = 0;
   Statement *stmt = findStatement("getDetailes", statements, size);
   stmt->setInt(1, apid);
   ResultSet *rs = stmt->executeQuery();
   string proname;
   int occur=0;
   Statement *stmt2;
   double procedureCost = 0;
   while(rs->next())
   {
      proname = rs->getString(1);
      occur  = rs->getInt(2);
      stmt2= findStatement("getProcedure", statements, size);
      procedureCost = getProcedureCost( stmt, proname);
      sum=sum+(procedureCost*occur);
   }
   stmt->closeResultSet(rs);
   return sum;
}


double getProcedureCost(Statement *stmt, string proname)
{
   stmt->setString(1, proname);
   ResultSet *rs = stmt->executeQuery();
   double cost = 0;
   if(rs->next)
   {
      cost = rs->getInt(1);
   }
   return cost;
}


void menu()
{
   cout << "Choose one of the following:" << endl;
   cout << "M: Make an appointment" << endl;
   cout << "C: Checkout" << endl;
   cout << "Q: Quit" << endl;
   cout << ">> ";
}

int main()
{
   cout << "Enter user name: ";
   string userName;
   getline(cin, userName);
   cout << "Enter password: ";
   string password = readPassword();
   cout << endl;
   const string connectString = "sunfire.csci.viu.ca";

   // Establish connection using userName and password
   Environment *env = Environment::createEnvironment();
   Connection *conn = env->createConnection (userName, password, connectString);
    
   STMT *statements;
   int size = initStatements(conn, statements);

   Statement *stmt;
    
   char option;
   menu();
   cin >> option;
   string garbage;
   getline(cin, garbage);
   while (option != 'q' && option != 'Q')
   {
      if(option == 'M' || option == 'm')
      {
         // prompt did
         string did;
         cout << "Enter dentest ID: ";
         getline(cin,did);
         
         // prompt pid
         string pid;
         cout << "Enter patient ID: ";
         getline(cin,pid);
          
         // prompt aid
         string aid;
         cout << "Enter assistent ID: ";
         getline(cin,aid);
          
         // prompt date
         string date;
         cout << "Enter date in format yyyy/mm/dd hh: ";
         getline(cin,date);
       
         stmt = findStatement("checkTimeSlot", statements, size);
         if(checkTimeSlot(stmt, did, date))
         {
            cout << "\t*** REQUESTED APPOINTMENT TIME IS NOT AVAILABLE ***" << endl;
         }
         else
         {
            stmt = findStatement("getMaxAPID", statements, size);            
            int apid = getMaxAPID(stmt);
            apid = apid +1;
            stmt = findStatement("insertAppointment", statements, size);
            if(insertAppointment(stmt,apid,did,pid,aid,date))
            {
               conn->commit();
               char addOperation='y';
               do
               {
                  if(addOperation == 'y' || addOperation == 'Y')
                  {
                     stmt = findStatement("getProcedure", statements, size);
                     string proname;
                     cout << "please enter type of procedure: ";
                     getline(cin,proname);
                     if(getProcedure(stmt, proname))
                     {
                        int occurrences;
                        cout << "please enter occuerences of procedure: ";
                        cin >> occurrences;
                        getline(cin,garbage);
                        stmt = findStatement("insertDetails", statements, size);
                        if(insertDetails(stmt, apid, proname, occurrences))
                        {
                           
                           cout << "\t*** PROCEDURE ADDED ***" << endl;
                        }
                        else
                        {
                           cout << "Error: Something went wrong in adding a procedure!" << endl;
                        }
                     }
                     else
                     {
                        cout << "\t*** Unrecognizable Procedure ***" << endl;
                     }
                  }
                  else
                  {
                     cout << "Error: Unrecognizable entry!" << endl;
                  }
                  cout << "Would you like to add another procedure? [Y,N]" << endl;
                  cin >> addOperation;
                  getline(cin,garbage);
               }while(addOperation!='n' && addOperation!='N');
               conn->commit();
               cout << "\t*** APPOINTMENT ADDED ***" << endl;
            }
         }
      }
      else if(option == 'C' || option == 'c')
      {
         // prompt pid
         string pid;
         cout << "Enter patient ID: ";
         getline(cin,pid);

         // prompt date
         string date;
         cout << "Enter date in format yyyy/mm/dd hh: ";
         getline(cin,date);
         cout << date << endl;
         
         stmt = findStatement("getAPID", statements, size);
         
         // make a function to get apid
         int apid = getAPID(stmt, pid, date);
         if(apid != -1)
         {
            int hours;
            cout << "Appointment number: " << apid << endl;
            cout << "please enter the number of hourse: ";
            cin >> hours;
            getline(cin,garbage);
            stmt = findStatement("updateAppointment", statements, size);
            if(updateAppointment(stmt, apid, hours))
            {
               cout << "\tTotal cost: $" << calculateTotal(apid,statements, size) << endl;
               cout << "\t*** CHECKOUT COMPLETE ***" << endl;
            }
         }
         else
         {
            cout << "Error: Appointment not found" << endl;
         }
      }
      else
      {
         cout << "Invalid choice!" << endl;
      }
       
       
      menu();
      cin >> option;
      getline(cin, garbage);
   }
    
   // clean up environment before terminating
   terminateStatements(conn, statements, size);
   env->terminateConnection(conn);
   Environment::terminateEnvironment(env);
   return 0;
}

