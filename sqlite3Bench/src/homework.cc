#include "bench.h"

namespace sqliteBench {
  
  // #1. Write a code for setting the journal mode in the SQLite database engine
  // [Requirement]
  // (1) This function set jounral mode using "FLAGS_journal_mode" extern variable (user input)
  // (2) This function check journal mode; if user try to set wrong journal mode then return -2
  // (3) This function returns status (int data type) of sqlite API function
  int Benchmark::benchmark_setJournalMode() {
    // I used sqlite3_exec API function to set jorunal mode and I used strcmp to check the journal name
    // please fill this function
    int status;
	  char* err_msg = NULL;
	  if(!strcmp("off", FLAGS_journal_mode)){
		  const char* OFF_stmt = "PRAGMA journal_mode = OFF";
		  status = sqlite3_exec(db_, OFF_stmt, NULL, NULL, &err_msg);
	  }
	  else if(!strcmp("del", FLAGS_journal_mode)){
		  const char* DEL_stmt = "PRAGMA journal_mode = DEL";
		  status = sqlite3_exec(db_, DEL_stmt, NULL, NULL, &err_msg);
	  }
    else if(!strcmp("wal", FLAGS_journal_mode)){
		  const char* WAL_stmt = "PRAGMA journal_mode = WAL";
		  status = sqlite3_exec(db_, WAL_stmt, NULL, NULL, &err_msg);
	  }
    else if(!strcmp("truncate", FLAGS_journal_mode)){
		  const char* TRUNCATE_stmt = "PRAGMA journal_mode = TRUNCATE";
		  status = sqlite3_exec(db_, TRUNCATE_stmt, NULL, NULL, &err_msg);
	  }
    else if(!strcmp("persist", FLAGS_journal_mode)){
		  const char* PERSIST_stmt = "PRAGMA journal_mode = PERSIST";
		  status = sqlite3_exec(db_, PERSIST_stmt, NULL, NULL, &err_msg);
	  }
    else if(!strcmp("memory", FLAGS_journal_mode)){
		  const char* MEMORY_stmt = "PRAGMA journal_mode = MEMORY";
      status = sqlite3_exec(db_, MEMORY_stmt, NULL, NULL, &err_msg);
	  }
	  else
      status = -2;

    // if error occurs, print error and exit
    if (status) {
      fprintf(stderr, "journal_mode error: %s\n", err_msg);
      exit(1);
    }
    
	  return status;
  }

  // #2. Write a code for setting page size in the SQLite database engine
  // [Requirement]
  // (1) This function set page size using "FLAGS_page_size" extern variable (user input)
  // (2) This function return status (int data type) of sqltie API function
  // (3) This function is called at benchmark_open() in bench.cc
  int Benchmark::benchmark_setPageSize() {
    // I used sqlite3_exec API function to set page size.
    // please fill this function
    int status;
	  char* err_msg = NULL;
	  char page_size[128];
	  snprintf(page_size, sizeof(page_size), "PRAGMA page_size = %d", FLAGS_page_size);
	  status = sqlite3_exec(db_, page_size, NULL, NULL, &err_msg);
    
    // if error occurs, print error and exit
    if (status) {
      fprintf(stderr, "page_size error: %s\n", err_msg);
      exit(1);
    }

    return status;

  }

  // #3. Write a code for insert function (direct SQL execution mode)
  // [Requriement]
  // (1) This function fill random key-value data using direct qurey execution sqlite API function
  //     (please refer to lecture slide project 3)
  // (2) This function has single arguemnt num_ which is total number of operations
  // (3) This function create SQL statement (key-value pair) do not modfiy this code 
  // (4) This function execute given SQL statement
  // (5) This function is called at benchmark_open() in bench.cc
  int Benchmark::benchmark_directFillRand(int num_) {
    //      DO NOT MODIFY HERE     //
    const char* value = gen_.rand_gen_generate(FLAGS_value_size);
    char key[100];
    const int k = gen_.rand_next() % num_;

    snprintf(key, sizeof(key), "%016d", k);
    char fill_stmt[100];
    snprintf(fill_stmt, sizeof(fill_stmt), "INSERT INTO test values (%s , x'%x');", key ,value);
    ////////////////////////////////
    //
    // I used sqlite3_exec API function to execute insert statement and I used rand_next method of RandomGenerator to generate random number for key in every iteration.
    // execute SQL statement
    // please fill this function
    int status;
    char* err_msg = NULL;
    int new_k;
    char* new_v;

    // loop for num_ times
    for (int i=0; i<num_; i++){
      // execute statemet
      status = sqlite3_exec(db_, fill_stmt, NULL, NULL, &err_msg);
      
      // fill random key, then update statemet(fill_stmt) with updated key
      new_k = gen_.rand_next() % num_;
      new_v = gen_.rand_gen_generate(FLAGS_value_size);
      // printf("%d\t", new_k);
      snprintf(key, sizeof(key), "%016d", new_k);
      snprintf(fill_stmt, sizeof(fill_stmt), "INSERT INTO test values (%s , x'%x');", key, new_v);
    }

    return status;
  }

  // xxx(homework)
  // write your own benchmark functions
  // you can add multiple functions as you like 
  // you can change function name. Here example is literally example.
  int Benchmark::benchmark_directFillSeq(int num_) {
    //
    // I used sqlite3_exec API function to execute 'insert' statement
    // And in every iteration, I added 1 to key so that it ascends sequentially.
    const char* value = gen_.rand_gen_generate(FLAGS_value_size);
    char key[100];
    char fill_stmt[100];
    snprintf(fill_stmt, sizeof(fill_stmt), "INSERT INTO test values (? , x'%x');", value);
    // char* fill_stmt = "INSERT INTO test values (? , 1024)";
    sqlite3_stmt* state;
    const char* tail;

    // sqlit3_prepare
    if(sqlite3_prepare_v2(db_, fill_stmt, strlen(fill_stmt), &state, &tail) != SQLITE_OK){
      fprintf(stderr, "sqlite3_prepare error : %s\n", sqlite3_errmsg(db_));
      exit(1);
    }

    // repeat insert statement for num_ times
    for (int i=0; i<num_; i++){
      // bind_text
      snprintf(key, sizeof(key), "%016d", i);
      sqlite3_bind_text(state, 1, key, strlen(key), SQLITE_STATIC);

      if (sqlite3_step(state) != SQLITE_DONE){
        fprintf(stderr, "sqlite3_step error : %s\n", sqlite3_errmsg(db_));
        exit(1);
      }
    }

    //sqlite3_finalize
    sqlite3_finalize(state);

    return 0;
  }

  int Benchmark::benchmark_updateUniform(int num_) {
    // I used sqlite3_exec API function to execute insert statement for one time.
    // And after that, in every iteration, I update the same record.
    const char* value = gen_.rand_gen_generate(FLAGS_value_size);
    char key[100];
    const int k = gen_.rand_next() % num_;

    snprintf(key, sizeof(key), "%016d", k);
    char fill_stmt[100];
    snprintf(fill_stmt, sizeof(fill_stmt), "INSERT INTO test values (%s , x'%x');", key ,value);
    
    // initial insert statement execution
    int status;
    char* err_msg = NULL;
    status = sqlite3_exec(db_, fill_stmt, NULL, NULL, &err_msg);

    // change insert statement to update statement
    sqlite3_stmt* state;
    const char* tail;
    char update_stmt[100];
    snprintf(update_stmt, sizeof(update_stmt), "UPDATE test SET value = '%x' WHERE key = %s;", value, key);

    // sqlit3_prepare
    if(sqlite3_prepare_v2(db_, update_stmt, strlen(update_stmt), &state, &tail) != SQLITE_OK){
      fprintf(stderr, "sqlite3_prepare error : %s\n", sqlite3_errmsg(db_));
      exit(1);
    }

    // repeat update statement for num_ times
    for (int i=0; i<num_; i++){
      if (sqlite3_step(state) != SQLITE_DONE){
        fprintf(stderr, "sqlite3_step error : %s\n", sqlite3_errmsg(db_));
        exit(1);
      }
    }

    //sqlite3_finalize
    sqlite3_finalize(state);

    return status;
  }

  int Benchmark::benchmark_fillDeleteUniform(int num_) {
    // I used sqlite3_prepare_v2, sqlite3_step, sqlite3_finalize API functions to execute insert and delete statement
    // And in every iteration, I INSERT and DELETE the same record (uniform).
    char key[100];
    const int k = gen_.rand_next() % num_;
    snprintf(key, sizeof(key), "%016d", k);
    const char* value = gen_.rand_gen_generate(FLAGS_value_size);
    char fill_stmt[100];
    char delete_stmt[100];
    snprintf(fill_stmt, sizeof(fill_stmt), "INSERT INTO test values (%s , '%x');", key ,value);
    snprintf(delete_stmt, sizeof(delete_stmt), "DELETE FROM test WHERE key = %s;", key);

    sqlite3_stmt* fill_state;
    const char* fill_tail;

    sqlite3_stmt* delete_state;
    const char* delete_tail;

    // sqlit3_prepare - fill (insert)
    if(sqlite3_prepare_v2(db_, fill_stmt, strlen(fill_stmt), &fill_state, &fill_tail) != SQLITE_OK){
      fprintf(stderr, "sqlite3_prepare error : %s\n", sqlite3_errmsg(db_));
      exit(1);
    }

    // sqlit3_prepare - delete
    if(sqlite3_prepare_v2(db_, delete_stmt, strlen(delete_stmt), &delete_state, &delete_tail) != SQLITE_OK){
      fprintf(stderr, "sqlite3_prepare error : %s\n", sqlite3_errmsg(db_));
      exit(1);
    }

    // repeat insert & delete statement pairs for (num_ times/2)
    for (int i=0; i<(num_/2); i++){
      // execute fill statement
      if (sqlite3_step(fill_state) != SQLITE_DONE){
        fprintf(stderr, "sqlite3_step error : %s\n", sqlite3_errmsg(db_));
        exit(1);
      }

      // execute delete statement
      if (sqlite3_step(delete_state) != SQLITE_DONE){
        fprintf(stderr, "sqlite3_step error : %s\n", sqlite3_errmsg(db_));
        exit(1);
      }
    }

    //sqlite3_finalize
    sqlite3_finalize(fill_state);
    sqlite3_finalize(delete_state);

    return 0;
  }

}; // namespace sqliteBench

