//C Apache log file parser
//
//Created by:
//              John Stanley
//
//
//
//

#define debug1 0
#define debug2 0
#define debug3 0
#define stripdebug 0
#define stripdebug2 0


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sss4910.h"


char * stripreq(char * inreq);
int parse_line(char* line, Request * outRequest );
time_t parse_time(char * input);


    static int const HOSTSIZE = 1000;
    static int const CLIENTIDSIZE = 50;
    static int const USERIDSIZE = 150;
    static int const TIMESIZE = 30;
    static int const REQSIZE = 2000;
    static int const CODESIZE = 30;
    static int const SIZESIZE = 30;
    static int const REFERERSIZE = 700;
    static int const USERAGENTSIZE = 300;


int main(int argc, char **argv)
{

    int lineN = 0;
    FILE *fp;
    
    //fp = fopen("bgcsierravlanca_access.log","r");
    //fp = fopen("almhuette_access.log", "r");
    fp = fopen("UofS_access_log","r");
    //fp = fopen("redlug.log","r");

    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    char * stripS;

    Request out;

    if (fp == NULL) 
    {
        printf("Im sorry invalid file");
        exit(EXIT_FAILURE);
    }
    while ((read = getline(&line, &len, fp)) != -1)
    {
       /* out = malloc((sizeof(char)* 
                        (
                        HOSTSIZE+
                        CLIENTIDSIZE+
                        USERIDSIZE+
                        TIMESIZE+
                        REQSIZE+
                        REFERERSIZE+
                        USERAGENTSIZE))
                    + (sizeof(int) * 2));*/

        out.host = (char *)malloc(HOSTSIZE * sizeof(char));    
        out.clientId = (char *)malloc(CLIENTIDSIZE * sizeof(char)); 
        out.userId = (char *)malloc((USERIDSIZE) * sizeof(char));  
        out.time = (char *)malloc(TIMESIZE * sizeof(char));  
        out.req = (char *)malloc((REQSIZE) * sizeof(char));  
        out.referer = (char *)malloc((REFERERSIZE) * sizeof(char));  
        out.userAgent = (char *)malloc(USERAGENTSIZE * sizeof(char));  
        


        if(parse_line(line,& out) == 0)
        {
            if(debug3) printf("worked\n");
        }
        else
        {
            printf("ERROR ON LINE: %d \n",lineN);
        }
        printf("\n%s\n",out.time); 
       	parse_time(out.time); //------------------------------------------------------

        
        
        if(debug3)printf("\n %d \n", lineN); 
        
        if(debug3) printf("H:%s R:%s A:%s T:%s C:%s c:%d S:%d \n \n",out.host, out.clientId, out.userId, out.time, out.req, out.httpReturnCode, out.dataSize );
        

        /*if(strlen(out.time)> 30) 
        {
            printf("\n%s N:%d\n",line, lineN );
            printf("%s\n",out.time);
        }*/
        if(stripdebug) stripS = stripreq(out.req);
        if(stripdebug) free(stripS);


        free(out.host);
        free(out.clientId);
        free(out.userId);
        free(out.time);
        free(out.req);
        free(out.referer);
        free(out.userAgent);
        lineN++;
        //free(out);

    }
    //printf("%d    %s \n",lineN,line);
    printf("DONE :D\n");


    fclose(fp);
}





















/* 
 *FUNCTION: parse_line
 *---------------------------------
 * Returns a "Request"
 *
 * takes a line from an apache log file and looks at each character in order
 *  it uses an apache log to generate a request struct, which it then passes on to the buffer
 *
 */
int parse_line(char* line, Request * outRequest)
{
        if(debug2) printf("%s \n",line);
    
    
        //counters
    int currentField = 0;
    int lengthOfCurrentField = 0;
    int i = 0;
    int lengthOfReq = 0;
    int k = 0;
    
    int flag = 0;
    

    char* retCode;
    retCode = (char *)malloc((30) * sizeof(char));  
       
    char* retSize;
    retSize = (char *)malloc((30) * sizeof(char));


    outRequest->referer[0] = '~';
    outRequest->referer[1] = '\0';

    outRequest->userAgent[0] = '~';
    outRequest->userAgent[1] = '\0';

         // loop for every character in the input line 
    for(i =0; i < strlen(line); i++)
    {

            //this parses and breaks down the line into fields, each case is a different 
        switch(currentField)
        {

                //field one 
                //      (%h) host field
                //          this is either and IP or a website
                //
                //          terminated by ' '
            case 0:
                    if(debug3)printf(" In field one \n");
                
                if (line[i] == ' ')
                {
                    currentField = 1;
                    outRequest->host[lengthOfCurrentField] = '\0';
                    lengthOfCurrentField = 0;
                }
                else
                {
                    outRequest->host[lengthOfCurrentField] = line[i];
                    lengthOfCurrentField++;
                }
                break;

                //field two 
                //      (%l) RFC
                //          request 1413
                //
                //          terminated by ' '
            case 1:
                    if(debug3)printf(" In field two \n");

                if(line[i] == ' ')
                {
                    currentField = 2;
                    outRequest->clientId[lengthOfCurrentField] = '\0';
                    lengthOfCurrentField = 0;
                }
                else
                {
                    outRequest->clientId[lengthOfCurrentField] = line[i];
                    lengthOfCurrentField++;
                }
                break;

                //field three
                //      (%u)    user id
                //      
                //      terminated by a ' '
            case 2:
                if(debug3) printf(" In field three \n");
                
                if(line[i] == ' ')
                {
                    if(line[i+1] == '[')
                    {
                        currentField = 3;
                        outRequest->userId[lengthOfCurrentField] = '\0';
                        lengthOfCurrentField = 0;
                    }
                    else
                    {
                        outRequest->userId[lengthOfCurrentField] = line[i];
                        lengthOfCurrentField++;
                    }
                }
                else
                {
                    outRequest->userId[lengthOfCurrentField] = line[i];
                    lengthOfCurrentField++;
                }
                break;

                //field four 
                //      (%t) time
                //      this is the time field 
                //      
                //      [day/month/year:hour:minute:second zone]
                //      day = 2*digit
                //      month = 3*letter
                //      year = 4*digit
                //      hour = 2*digit
                //      minute = 2*digit
                //      second = 2*digit
                //      zone = (`+' | `-') 4*digit
                //
                //      terminated by a ']'
            case 3:
                    if(debug3)printf(" In field four \n");
                
                if(lengthOfCurrentField >= 30)
                {
                    //time cannot be bigger than 23
                    return -1;
                }
                if(flag == 1)
                {
                    currentField = 4;
                    flag = 0;
                    outRequest->time[lengthOfCurrentField] = '\0';
                    lengthOfCurrentField = 0;

                }
                else if(line[i] == ']')
                {
                    flag = 1;
                    outRequest->time[lengthOfCurrentField] = line[i];
                    lengthOfCurrentField++;
                }
                else
                {
                    outRequest->time[lengthOfCurrentField] = line[i];
                    lengthOfCurrentField++;
                }
                break;
           
                //field five
                //      (\"%r\") request line
                //
                //      altho the program begins and ends with a '"' you cannot just assume that
                //      you have hit the end of the feild becuase you hit a second '"' this is why
                //      field six has to check and make sure field five(request line) has ended
                //
                //      terminated by 2nd '"' 
                //      
                //      PROBLAMATIC

            case 4:
                    if(debug3) printf(" In field five \n");

                if(flag == 2)
                {
                    if (line[i] != ' ')
                    {
                        flag = 1;
                        currentField = 4;
                        outRequest->req[lengthOfCurrentField] = line[i];
                        lengthOfCurrentField++;
                    }
                    else
                    {
                        flag = 0;
                        outRequest->req[lengthOfCurrentField] = '\0';
                        lengthOfReq = lengthOfCurrentField;
                        currentField = 5;
                        lengthOfCurrentField = 0;
                    }
                }
                else if( line[i] == '"')
                {

                    outRequest->req[lengthOfCurrentField] = line[i];
                    lengthOfCurrentField++;
                    flag++;
                }
                else
                {
                    outRequest->req[lengthOfCurrentField] = line[i];
                    lengthOfCurrentField++;
                }
                break;

                //field six 
                //      (%>s) status code
                //
                //      this is a relatively simple field that usually returns 200 or 404
                //          this also checks to make sure the arnt still looking at the request 
                //          line because of a false positve
                //
                //          terminated by a ' '
                //
            case 5:
                    if(debug3)printf(" In field six \n");

                if(line[i] == ' ')
                {
                    retCode[lengthOfCurrentField] = '\0';
                    outRequest->httpReturnCode = atoi(retCode);
                    if(outRequest->httpReturnCode == 0)
                    {

                        outRequest->req[lengthOfReq] = ' ';
                        lengthOfReq++;
                        for(k = 0; k < lengthOfCurrentField; k++)
                        {
                            outRequest->req[lengthOfReq] = retCode[k];
                            lengthOfReq++;
                        }
                        lengthOfCurrentField = 0;
                    }
                    else
                    {
                        currentField = 6;
                        lengthOfCurrentField = 0;
                    }
                }
                else
                {
                    
                    retCode[lengthOfCurrentField] = line[i];
                    lengthOfCurrentField++;
                }
                break;

                //  feild seven
                //      (%b) size of data
                //
                //      this is just how much data was returned 
                //
                //      terminated by a ' ' or a '\n' for "short" logs
                //
            case 6:
                    if(debug3) printf(" In field seven \n");

                if(line[i] == ' ' || line[i] == '\n')
                {
                    currentField = 7;
                    retSize[lengthOfCurrentField] = '\0';
                    lengthOfCurrentField = 0;
                    outRequest->dataSize = atoi(retSize);
                }
                else
                {
                    retSize[lengthOfCurrentField] = line[i];
                    lengthOfCurrentField++;
                }
                break;

           //EXTENDED LOG FORMAT FOLLOWS 
            case 7:
                    if(debug3) printf(" In field eight \n");
                
                if(flag == 2)
                {
                    flag = 0;
                    outRequest->referer[lengthOfCurrentField] = '\0';
                    currentField = 8;
                    lengthOfCurrentField = 0;
                }
                else if( line[i] == '"')
                {

                    //outRequest->referer[lengthOfCurrentField] = line[i];
                    //lengthOfCurrentField++;
                    flag++;
                }
                else
                {
                    outRequest->referer[lengthOfCurrentField] = line[i];
                    lengthOfCurrentField++;
                }
                break;
             case 8:
                    if(debug3) printf(" In field eight \n");
                
                if(flag == 2)
                {
                    flag = 0;
                    outRequest->userAgent[lengthOfCurrentField] = '\0';
                    currentField = 9;
                    lengthOfCurrentField = 0;
                }
                else if( line[i] == '"')
                {

                    //outRequest->userAgent[lengthOfCurrentField] = line[i];
                    //lengthOfCurrentField++;
                    flag++;
                }
                else
                {
                    outRequest->userAgent[lengthOfCurrentField] = line[i];
                    lengthOfCurrentField++;
                }
                break;
            

        }   


    }

    if(debug1) printf("   Inside: H:%s R:%s A:%s T:%s C:%s c:%d S:%d r:%s U:%s \n \n",outRequest->host, outRequest->clientId, outRequest->userId, outRequest->time, outRequest->req, outRequest->httpReturnCode, outRequest->dataSize, outRequest->referer, outRequest->userAgent );
    
    /*free(outRequest.host);
    free(outRequest.rfc);
    free(outRequest.auth);
    free(outRequest.time);
    free(outRequest.req);*/
    free(retCode);
    free(retSize);
    /*free(outRequest.referer);
    free(outRequest.userAgent);*/

    return 0;
}


char * stripreq(char * inputString )
{
    int spaces = 0;
    int i;
    int currentSpace = 0;
    char * buffer;
    buffer = (char *)malloc((2000) * sizeof(char));  

   
    int currentLengthOfBuffer = 0;
    
    if(stripdebug2) printf("A"); 
    
    for(i = 0; i < strlen(inputString);i++)
    {
        if (inputString[i] == ' ')
        {
            spaces++;
        }
    }
    

    if(stripdebug2) printf("B %d ",spaces);
    
    for(i = 0; i < strlen(inputString); i++)
    {
        if(stripdebug2) printf("C%d ", i);
            
        if(currentSpace == 0)
        {
            if(inputString[i] == ' ')
            {
                currentSpace++;
            }
        }
        else if(currentSpace == spaces)
        {
            if(spaces == 1)
            {
                buffer[currentLengthOfBuffer] = inputString[i];
                currentLengthOfBuffer++;
            }
            else
            {
                buffer[currentLengthOfBuffer] = '\0';
                break;
            }
        }
        else
        {
            if(inputString[i] == ' ')
            {
                currentSpace++;
            }
            buffer[currentLengthOfBuffer] = inputString[i];
            currentLengthOfBuffer++;
        }   
    }
    if(spaces == 1)
    {
        buffer[currentLengthOfBuffer-1] = '\0';
    }

    if(stripdebug) printf("\n\n\n stripout:\n %s \n\n%s \n",inputString,buffer);
    
    return buffer;
}





time_t parse_time(char * input)
{
    /*
      [day/month/year:hour:minute:second zone]
      day = 2*digit
      month = 3*letter
      year = 4*digit
      hour = 2*digit
      minute = 2*digit
      second = 2*digit
      zone = (`+' | `-') 4*digit
    */

    time_t time;
    struct tm tempest;
    time_t outtime;

    int i;
    int j = 0;
    int n = 0;

    /*int day = -2;
    int month = -2;
    int year = -2;
    int hour = -2;
    int minute = -2;
    int second = -2;
    */
    int zone = -22;
    
    
    char* temp;
    temp = (char*) malloc(5 * sizeof(char));   


        //first step is to break down each part of the time field
    for(i = 0;i < strlen(input); i++)
    {
        switch (n)
        {
            case 0:
                if(input[i] != '[')
                {
                    printf("ERROR\n");
                }
                n = 1;
                j = 0;
                break;

               //DAY!!! 
            case 1:
                if (input[i] == '/')
                {
                    temp[j+1] = '\0';
                    tempest.tm_mday = atoi(temp);
                    n = 2;
                    j = 0;
                }
                else
                {
                    temp[j] = input[i];
                    j++;
                }
                break;


                //MONTH 
            case 2:
                if(input[i] == '/')
                {
                    temp[j] = '\0';
                    if(strlen(temp) != 3)
                    {
                        printf("ERROR MONTH\n");
                    }
                    else
                    {
                        n = 3;
                        j = 0;

                            //printf("%s \n",temp);
                        if(strcmp(temp,"Jan") == 0)
                        {
                            tempest.tm_mon = 0;
                        }
                        else if(strcmp(temp,"Feb") == 0)
                        {
                            tempest.tm_mon = 1;
                        }
                        else if(strcmp(temp,"Mar") == 0)
                        {
                            tempest.tm_mon = 2;
                        }
                        else if(strcmp(temp,"Apr") == 0)
                        {
                            tempest.tm_mon = 3;
                        }
                        else if(strcmp(temp,"May") == 0)
                        {
                            tempest.tm_mon = 4;
                        }
                        else if(strcmp(temp,"Jun") == 0)
                        {
                            tempest.tm_mon = 5;
                        }
                        else if(strcmp(temp,"Jul") == 0)
                        {
                            tempest.tm_mon = 6;
                        }
                        else if(strcmp(temp,"Aug") == 0)
                        {
                            tempest.tm_mon = 7;
                        }
                        else if(strcmp(temp,"Sep") == 0)
                        {
                            tempest.tm_mon = 8;
                        }
                        else if(strcmp(temp,"Oct") == 0)
                        {
                            tempest.tm_mon = 9; 
                        }
                        else if(strcmp(temp,"Nov") == 0)
                        {
                            tempest.tm_mon = 10;
                        }
                        else if(strcmp(temp,"Dec") == 0)
                        {
                            tempest.tm_mon = 11;
                        }
                        else 
                        {
                            printf("ERROR");
                            //return 0;
                            tempest.tm_mon = -1;
                        }

                    }
                }
                else
                {
                    temp[j] = input[i];
                    j++;
                }
                break;
            
                
                //YEAR
            case 3:
                if (input[i] == ':')
                {
                    temp[j] = '\0';
                    tempest.tm_year = (atoi(temp) - 1900);
                    n = 4;
                    j = 0;
                }
                else
                {
                    temp[j] = input[i];
                    j++;
                }
                break;
                
                //HOUR
            case 4:
                if (input[i] == ':')
                {
                    temp[j] = '\0';
                    tempest.tm_hour = atoi(temp);
                    n = 5;
                    j = 0;
                }
                else
                {
                        temp[j] = input[i];
                        j++;
                }
                break;


                //MINUTE
            case 5:
                if (input[i] == ':')
                {
                    temp[j] = '\0';
                    tempest.tm_min = atoi(temp);
                    n = 6;
                    j = 0;
                }
                else
                {
                    temp[j] = input[i];
                    j++;
                }
                break;


                //SECOND 
            case 6:
                if (input[i] == ' ')
                {
                    temp[j] = '\0';
                    tempest.tm_sec = atoi(temp);
                    n = 7;
                    j = 0;
                }
                else
                {
                    temp[j] = input[i];
                    j++;
                }
                break;

            case 7:
                if (input[i] == ']')
                {
                    temp[j] = '\0';
                    zone = atoi(temp);
                    j = 0;
                }
                else
                {
                    temp[j] = input[i];
                    j++;
                }
                break;         

            default:
                printf("BLARG/n");
                break;
        }

    }
    free(temp);



    /*tempest.tm_hour = hour ;
    tempest.tm_sec = second;
    tempest.tm_min = minute;
    tempest.tm_mday = day;
    tempest.tm_mon = month -1 ;
    tempest.tm_year = year - 1900;
    */

    printf("tm: %s",asctime(&tempest));
    
    outtime = mktime(&tempest);

    printf("time_t: %s",asctime(localtime(&outtime))); 


    return outtime;
}
