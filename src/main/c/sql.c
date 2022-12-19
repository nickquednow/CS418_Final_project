#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql.h>

#include "config.h"
#include "advanced_config.h"
#include "compile_modes.h"

char* query_MYSQLDebugOutput(){
    const char* output = mysql_get_client_info();
    #ifdef DEBUG_COMPILE_MODE
    printf("Debug information for mysql: %s\n", output);
    #endif
    return (char*)output;
}

char* query_MapPaths(){
    MYSQL *con = mysql_init(NULL);

    if (con == NULL){
        printf("bad things happened while getting map data\nERROR: %s\n", mysql_error(con));
        return NULL;
    }

    if(mysql_real_connect(con, MYSQL_DESCELATED_HOST, MYSQL_DESCELATED_USERNAME, MYSQL_DESCELATED_PASSWORD, MYSQL_DESCELATED_DATABASE, MYSQL_DESCELATED_PORT, NULL, 0) == NULL){
        printf("bad things happened while getting map data\nERROR: %s\n", mysql_error(con));
        mysql_close(con);
        return NULL;
    }
    if(mysql_query(con, "SELECT url, x1, y1, width, height FROM MAP_TILES;")){
        printf("bad things happened while getting map data\nERROR: %s\n", mysql_error(con));
        mysql_close(con);
        return NULL;
    }

    MYSQL_RES * result = mysql_store_result(con);

    if(result == NULL){
        printf("bad things happened while getting map data\nERROR: %s\n", mysql_error(con));
        mysql_close(con);
    }

    int num_fields = mysql_num_fields(result);
    int num_rows = mysql_num_rows(result);

    #ifdef DEBUG_COMPILE_MODE
    printf("NumFields: %d\nNumRows: %d\n", num_fields, num_rows);
    #endif

    MYSQL_ROW row;

    char* getMapsBuffer = malloc(30000*sizeof(char));

    getMapsBuffer[0]=0;
    while((row = mysql_fetch_row(result))){
        for(int i=0;i<num_fields;i++){
            switch (i)
            {
            case 0:
                strcat(getMapsBuffer, "{\"url\":\"");
                strcat(getMapsBuffer, row[i]);
                strcat(getMapsBuffer, "\",");
                break;
            case 1:
                strcat(getMapsBuffer, "\"x1\":\"");
                strcat(getMapsBuffer, row[i]);
                strcat(getMapsBuffer, "\",");
                break;
            case 2:
                strcat(getMapsBuffer, "\"y1\":\"");
                strcat(getMapsBuffer, row[i]);
                strcat(getMapsBuffer, "\",");
                break;
            case 3:
                strcat(getMapsBuffer, "\"width\":\"");
                strcat(getMapsBuffer, row[i]);
                strcat(getMapsBuffer, "\",");
                break;
            case 4:
                strcat(getMapsBuffer, "\"height\":\"");
                strcat(getMapsBuffer, row[i]);
                strcat(getMapsBuffer, "\"}\n");
                break;
            default:
                break;
            }
            // printf("%s ", row[i] ? row[i]:"NULL");
        }
        // printf("\n");
    }
    #ifdef DEBUG_COMPILE_MODE
    printf("Buffer: \n%s\n", getMapsBuffer);
    #endif

    char* newBuffer = malloc(strlen(getMapsBuffer)+1);
    strncpy(newBuffer, getMapsBuffer,strlen(getMapsBuffer));
    free(getMapsBuffer);
    getMapsBuffer = newBuffer;

    mysql_close(con);
    return getMapsBuffer;
}


char* query_IMONumbers(){
    MYSQL *con = mysql_init(NULL);

    if (con == NULL){
        printf("bad things happened while getting IMO data\nERROR: %s\n", mysql_error(con));
        return NULL;
    }

    if(mysql_real_connect(con, MYSQL_DESCELATED_HOST, MYSQL_DESCELATED_USERNAME, MYSQL_DESCELATED_PASSWORD, MYSQL_DESCELATED_DATABASE, MYSQL_DESCELATED_PORT, NULL, 0) == NULL){
        printf("bad things happened while getting IMO data\nERROR: %s\n", mysql_error(con));
        mysql_close(con);
        return NULL;
    }
    if(mysql_query(con, "SELECT DISTINCT imo FROM AIS_METADATA GROUP BY (imo);")){
        printf("bad things happened while getting IMO data\nERROR: %s\n", mysql_error(con));
        mysql_close(con);
        return NULL;
    }

    MYSQL_RES * result = mysql_store_result(con);

    if(result == NULL){
        printf("bad things happened while getting IMO data\nERROR: %s\n", mysql_error(con));
        mysql_close(con);
    }

    int num_fields = mysql_num_fields(result);
    my_ulonglong num_rows = mysql_num_rows(result);

    MYSQL_ROW row;

    int bufferLength = (10*num_rows)*sizeof(char)+10;
    char* getIMOBuffer = malloc(bufferLength);
    memset(getIMOBuffer, 0, bufferLength);

    getIMOBuffer[0] = '{';
    getIMOBuffer[1] = '"';
    getIMOBuffer[2] = 'i';
    getIMOBuffer[3] = 'm';
    getIMOBuffer[4] = 'o';
    getIMOBuffer[5] = '"';
    getIMOBuffer[6] = ':';
    getIMOBuffer[7] = '[';
    int currRow = 0;
    while((row = mysql_fetch_row(result))){
        currRow += 1;
        if(row[0] == NULL)
            continue;
        strcat(getIMOBuffer, row[0]);
        strcat(getIMOBuffer, ",");
        // printf("\n");
    }
    getIMOBuffer[strlen(getIMOBuffer)-1] = ']';
    getIMOBuffer[strlen(getIMOBuffer)] = '}';
    #ifdef DEBUG_COMPILE_MODE
    printf("Buffer: \n%s\n", getIMOBuffer);
    #endif

    char* newBuffer = malloc(strlen(getIMOBuffer)+1);
    strncpy(newBuffer, getIMOBuffer,strlen(getIMOBuffer));
    free(getIMOBuffer);
    getIMOBuffer = newBuffer;

    mysql_close(con);
    return getIMOBuffer;
}


char* query_PositionReport(){
    MYSQL *con = mysql_init(NULL);

    if (con == NULL){
        printf("bad things happened while getting Position data\nERROR: %s\n", mysql_error(con));
        return NULL;
    }

    if(mysql_real_connect(con, MYSQL_DESCELATED_HOST, MYSQL_DESCELATED_USERNAME, MYSQL_DESCELATED_PASSWORD, MYSQL_DESCELATED_DATABASE, MYSQL_DESCELATED_PORT, NULL, 0) == NULL){
        printf("bad things happened while getting Position data\nERROR: %s\n", mysql_error(con));
        mysql_close(con);
        return NULL;
    }
    if(mysql_query(con, "SELECT x,y,head FROM SCRIPTS,POSITIONS,AIS_METADATA WHERE AIS_METADATA.ais_id=POSITIONS.ais_id AND AIS_METADATA.imo=CAST(SCRIPTS.value AS UNSIGNED) AND SCRIPTS.field='currentIMO' ORDER BY (time) DESC LIMIT 1;")){
        printf("bad things happened while getting Position data\nERROR: %s\n", mysql_error(con));
        mysql_close(con);
        return NULL;
    }

    MYSQL_RES * result = mysql_store_result(con);

    if(result == NULL){
        printf("bad things happened while getting Position data\nERROR: %s\n", mysql_error(con));
        mysql_close(con);
    }

    int num_fields = mysql_num_fields(result);
    my_ulonglong num_rows = mysql_num_rows(result);

    MYSQL_ROW row;

    int bufferLength = 1000;
    char* getPositionBuffer = malloc(bufferLength);
    memset(getPositionBuffer, 0, bufferLength);

    row = mysql_fetch_row(result);

    strcpy(getPositionBuffer, "{\"x_pos\":");
    strcat(getPositionBuffer, row[0]);
    strcat(getPositionBuffer, ",\"y_pos\":");
    strcat(getPositionBuffer, row[1]);
    strcat(getPositionBuffer, ",\"head\":");
    strcat(getPositionBuffer, row[2]);

    getPositionBuffer[strlen(getPositionBuffer)] = '}';
    #ifdef DEBUG_COMPILE_MODE
    printf("Buffer: \n%s\n", getPositionBuffer);
    #endif

    char* newBuffer = malloc(strlen(getPositionBuffer)+1);
    strncpy(newBuffer, getPositionBuffer,strlen(getPositionBuffer));
    newBuffer[strlen(getPositionBuffer)] = 0;
    free(getPositionBuffer);
    getPositionBuffer = newBuffer;

    mysql_close(con);
    return getPositionBuffer;
}

int setCurrentIMO(char* imo){

    int error = 0;

    MYSQL *con = mysql_init(NULL);

    if (con == NULL){
        printf("bad things happened while getting Position data\nERROR: %s\n", mysql_error(con));
        return 0;
    }

    if(mysql_real_connect(con, MYSQL_DESCELATED_HOST, MYSQL_DESCELATED_USERNAME, MYSQL_DESCELATED_PASSWORD, MYSQL_DESCELATED_DATABASE, MYSQL_DESCELATED_PORT, NULL, 0) == NULL){
        printf("bad things happened while getting Position data\nERROR: %s\n", mysql_error(con));
        mysql_close(con);
        return 0;
    }
    char query[61];
    sprintf(query, "UPDATE SCRIPTS SET value='%s' WHERE field='currentIMO';", imo);

    if(mysql_query(con, query)){
        printf("bad things happened while getting Position data\nERROR: %s\n", mysql_error(con));
        mysql_close(con);
        return 0;
    }

    mysql_commit(con);

    int num_rows = mysql_affected_rows(con);

    // if(num_rows == NULL){
    //     printf("bad things happened while getting Position data\nERROR: %s\n", mysql_error(con));
    //     mysql_close(con);
    //     error = 1;
    // }
    mysql_close(con);
    return (error ? 0 : 1);
}