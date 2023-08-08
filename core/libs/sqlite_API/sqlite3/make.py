import os
os.system("clear")
print("Compiling....")
#command = "gcc shell.c sqlite3.c -lpthread -ldl -lm -o sqlite3"
#command = "gcc -DSQLITE_THREADSAFE=0 shell.c sqlite3.c -lpthread -ldl -lm -o sqlite3.o"
command = "gcc -c -Os -I. -DSQLITE_THREADSAFE=0 -DSQLITE_ENABLE_FTS4 shell.c sqlite3.c -ldl -lm -lreadline -lpthread -lncurses"
#command = "gcc -c shell.c sqlite3.c -o sqlite3"
os.system(command)
os.system("clear")
print("Finished Compiling")

