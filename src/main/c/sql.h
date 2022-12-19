#ifndef SQL_H
#define SQL_H
char* query_MYSQLDebugOutput();
char* query_MapPaths();
char* query_PositionReport();
char* query_IMONumbers();
int setCurrentIMO(char* imo);
#endif //SQL_H