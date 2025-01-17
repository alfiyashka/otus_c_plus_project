#include <iostream>
#include "src/db/NoSQLDB.hpp"
#include "src/model/BasicDBInputObject.hpp"

#include <string>
#include <memory>

using namespace std;

int main(int argc, const char *argv[])
{
  NoSqlDB db;
  if (!db.registerUser("avalieva", "pwd"))
  {
    std::cerr << "Cannot registrate" << std::endl;
  }
  const auto loginRes = db.login("avalieva", "pwd");
  if (!loginRes.has_value())
  {
    std::cerr << "Cannot login" << std::endl;
    return -1;
  }

  db.beginTransaction(loginRes);
  Data_t data(new std::string("data"));
  InsertJob::insertData_t insertdata(new BasicDBInsertObject(Datatype::STRING, "data_name", data));
  db.execInsert(loginRes, insertdata);
  db.select(loginRes, nullptr);
  Data_t dataNew(new std::string("dataUpdated"));
  UpdateJob::updateData_t updatedata(new BasicDBUpdateObject(Datatype::STRING, "data_name",
    dataNew, Datatype::STRING, "data_name", data));
  db.execUpdate(loginRes, updatedata);


  DeleteJob::whereData_t deletedata(new BasicDBDeleteObject(Datatype::STRING, "data_name", dataNew, false));
  db.execDelete(loginRes, deletedata);
  db.select(loginRes, nullptr);
  db.commitTransaction(loginRes);

  db.exit(loginRes);

  return 0;
}
