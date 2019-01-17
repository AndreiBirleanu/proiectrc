/* servTCPIt.c - Exemplu de server TCP iterativ
   Asteapta un nume de la clienti; intoarce clientului sirul
   "Hello nume".
   
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* portul folosit */
#define PORT 2024

/* codul de eroare returnat de anumite apeluri */
extern int errno;
int getidfor(char usr[]){
  MYSQL *con = mysql_init(NULL);

  if (con == NULL)
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }

  if (mysql_real_connect(con, "localhost", "andrei", "admin",
          NULL, 0, NULL, 0) == NULL)
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
  }

   if (mysql_query(con, "USE proiectretele"))
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
  }

  
  char id[10]="";
  
  char aflaid[256]="Select id from Useri where  user ='";
  strcat(aflaid,usr);
  strcat(aflaid,"'");
  if(mysql_query(con,aflaid)){
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);    
  }
 MYSQL_RES *result = mysql_store_result(con);

  MYSQL_ROW row;
  if((row=mysql_fetch_row(result))){
  strcpy(id,row[0]);
  }
  int idint=atoi(id);
  return idint;

}

int newmess(char usr1[],char usr2[],int sd){
  MYSQL *con = mysql_init(NULL);

    if (con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, "localhost", "andrei", "admin",
            NULL, 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }

     if (mysql_query(con, "USE proiectretele"))
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }
    //printf("oare ajung aici?hmm\n");
   char id1[100];
   int idi1=getidfor(usr1);
   int idi2=getidfor(usr2);
   char id2[100];
   snprintf (id1, sizeof(id1), "%d",idi1);
   snprintf (id2, sizeof(id1), "%d",idi2);
   
   
   

   char showlastmess[256]="SELECT user,mesaj from Useri join Mesaje on id=id_e where(id_e=";
   strcat(showlastmess,id2);
   strcat(showlastmess," and id_d=");
   strcat(showlastmess,id1);

   strcat(showlastmess,") and seen like 0 order by id_m");
   printf("query ul este %s\n",showlastmess  );

    if(mysql_query(con,showlastmess)){
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);    
    }
    MYSQL_RES *result = mysql_store_result(con);
     int num_fields = mysql_num_fields(result);
   int num_rows=mysql_num_rows(result);
  // printf("avem atatea randuri xxx %d\n",num_rows );
    MYSQL_ROW row;    
    if(write (sd, &num_rows, sizeof(num_rows)) <= 0)
        {
         
         perror ("[Client] Eroare la transmiterea prefixata(prefix)\n");
        }
              if(num_rows==0)
        return 0;
    while ((row = mysql_fetch_row(result))) 
      { 
       // printf("oare intra?\n");

          for(int i = 0; i < num_fields; i++) 
          { 
            //if(i==0)
             //printf("[%s]  ", row[i] ? row[i] : "NULL"); 
          //else
           //printf("%s   ", row[i] ? row[i] : "NULL"); 
        //transmitere_prefix(sd,row[i]);
            int n=strlen(row[i]);
            if(write (sd, &n, sizeof(n)) <= 0)
              {
               
               //perror ("[Client] Eroare la transmiterea prefixata(prefix)\n");
              }
              printf("in transmitere_prefix %d\n",n );
           if(write (sd, row[i], n) <= 0)
              {
               
              // perror ("[Client] Eroare la transmiterea prefixata(send)\n");
              }
              //printf("in transmitere_prefix %s\n",send );

   

          } 
              printf("\n"); 
      }
      char update[256]="UPDATE Mesaje Set seen = '1' where(id_e=";
    strcat(update,id2);
   strcat(update," and id_d=");
   strcat(update,id1);
   strcat(update,")");
   //printf("%s\n",update );
    if(mysql_query(con,update)){
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);    
    }


      return 1;

}
char usrl[256]="";
char usr2[256]="";


int main ()
{
  struct sockaddr_in server;  // structura folosita de server
  struct sockaddr_in from;  
  char msg[100];    //mesajul primit de la client 
  char msgrasp[100]=" ";        //mesaj de raspuns pentru client
  int sd;     //descriptorul de socket 

  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }

  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;  
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  
  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 5) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }

  /* servim in mod iterativ clientii... */
  while (1)
    {
      int client;
      int length = sizeof (from);

      printf ("[server]Asteptam la portul %d...\n",PORT);
      fflush (stdout);

      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
      client = accept (sd, (struct sockaddr *) &from, &length);

      /* eroare la acceptarea conexiunii de la un client */
      if (client < 0)
  {
    perror ("[server]Eroare la accept().\n");
    continue;
  }

      /* s-a realizat conexiunea, se astepta mesajul */
      bzero (msg, 100);
      printf ("[server]Asteptam mesajul...\n");
      fflush (stdout);
      int n;
      if(write (sd, &n, sizeof(n)) <= 0)
              {
               
               //perror ("[Client] Eroare la transmiterea prefixata(prefix)\n");
              }
      if(write (sd, usrl, n) <= 0)
              {
               
               //perror ("[Client] Eroare la transmiterea prefixata(prefix)\n");
              }
              if(write (sd, &n, sizeof(n)) <= 0)
              {
               
               //perror ("[Client] Eroare la transmiterea prefixata(prefix)\n");
              }
      if(write (sd, usr2, n) <= 0)
              {
               
               //perror ("[Client] Eroare la transmiterea prefixata(prefix)\n");
              }

      while(1){
        if(newmess(usrl,usr2,sd)){
          //avem mesaj
        }else{
          //nu avem mesaj
        }
      }
      close (client);
    }       /* while */
}       /* main */
