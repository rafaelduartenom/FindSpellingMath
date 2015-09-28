/*
 * FindSpellingMath.cpp
 *
 * Copyright  (C)  2015  Rafael Duarte <rafael_spc&hotmail.com>
 *
 * Version: 1.0
 * Author: Rafael Duarte <rafael_spc&hotmail.com>
 * Maintainer: Rafael Duarte <rafael_spc&hotmail.com>
 * URL: http://rafael.duarte.nom.br
 * &=@
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Commentary:
 *
 * See http://rafael.duarte.nom.br/
 *
 * Code:
 */

#include "SpellCorrector.h" //source https://scarvenger.wordpress.com/2007/12/11/how-to-write-a-spelling-corrector/
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "csvparser.c" // read csv source: http://sourceforge.net/projects/cccsvparser/
#include "csvparser.h" // read csv source: http://sourceforge.net/projects/cccsvparser/
#include "csvwriter.h" // write csv source: http://sourceforge.net/projects/cccsvwriter/
#include "csvwriter.c" // write csv source: http://sourceforge.net/projects/cccsvwriter/
#include <windows.h>

#define N1 5000
#define N2 1000

void deblank(char *input);
void TiraAcentos(char *s1, int len);//remove ascents from http://www.portugal-a-programar.pt/topic/35527-tirar-acentos-a-uma-string/
void to_lowercase(char *input);
int find_id_title(char *title_name, char *csv_name);
int row_count(int id_colum, char *csv_name);
void read_field_csv_file(char *csv_name, char *row_csv[N1][N2]);// read csv file colum http://sourceforge.net/projects/cccsvparser/
void write_csv_file(int count_colum,int count_row, char *filename, char *row_csv[N1][N2], char *colum_csv[N2]);//write csv file source: http://sourceforge.net/projects/cccsvwriter/
void read_colum_title_csv_file(char *csv_name,char *title_name[N2]);// read colum http://sourceforge.net/projects/cccsvparser/
void find_correct(char *city_name, char *filename_cods, char *filename_names,char *citycode);
int colum_count(char *csv_name);
int main(int argc, char *argv[])
{

   if(argc < 5 ) {
    printf("Error input correct arguments...\n");
    printf("Usage: FindSpellingMath input.csv name_colum_in_csv name_match_list.txt code_or_name_match_list.txt output.csv\n");
    return 1;
  }
    //variavel csv
    int id_colum;
    int number_lines;
    int number_lines_colum_reference;
    int number_of_colums;
    char name_colum[100];
    char csv_name[100];
    char new_csv_name[100];
    char city_name[50];
    char cod_ibge[12];
    int i=0,j=0;
    //fim varivel csv
    char txt_cod_mun[50];
    char txt_names_mun[50];
    char a[100]; //variavel spell
    static char *out[N1][N2];//write the output matrix -> csv
    static char *inp[N1][N2];// variable input data (from csv)
    static char *colum_name[N2];// variable to write colum string names
    setlocale(LC_ALL, "Portuguese"); //use esse código para poder usar acentos
    SpellCorrector corrector;

    strncpy (csv_name, argv[1], 50 );
    //strcpy (csv_name,"Book1n5.csv");
    strncpy (name_colum, argv[2], 50 );
    //strcpy (name_colum,"teste2");
    strncpy (txt_names_mun, argv[3], 50 );
    //strcpy (txt_names_mun,"municipios_sp.txt");
    strncpy (txt_cod_mun, argv[4], 50 );
    //strcpy (txt_cod_mun,"cod_ibge_municipios_sp.txt");
    strncpy (new_csv_name, argv[5], 50 );
    //strcpy (new_csv_name,"Book2n.csv");

    //char new_txt_name[100];
    //FILE *txt_saida; // using for initial test, write in output_txt
    //strcpy (new_txt_name,"Book2n.txt");
    //txt_saida = fopen (new_txt_name,"w+");


    corrector.load(txt_names_mun);

    id_colum=find_id_title(name_colum,csv_name);
    number_of_colums=colum_count(csv_name);
    read_field_csv_file(csv_name,inp);
    read_colum_title_csv_file(csv_name,colum_name);

    for(j=0;j<number_of_colums;j++)
    {
    number_lines=row_count(j,csv_name);
    if (id_colum==j)
      {
        for(i=0;i<number_lines;i++)
        {
        number_lines_colum_reference=number_lines;
        deblank(inp[i][j]);
        TiraAcentos(inp[i][j],strlen(inp[i][j]));
        to_lowercase(inp[i][j]);
        string request(inp[i][j]);
        string correct(corrector.correct(request));
        if (correct != "")
          {
           cout << "Did you mean: " << correct << "?\n";
           strcpy(city_name,correct.c_str());
           find_correct(city_name,txt_cod_mun,txt_names_mun,cod_ibge);
           out[i][j] = (char*)malloc(strlen(cod_ibge) + 1);
           //fprintf (txt_saida,"%s\n",cod_ibge);
           strcpy(out[i][j], cod_ibge);
           //strncpy(out[j][i], cod_ibge, sizeof cod_ibge);
           memset(cod_ibge, 0, sizeof cod_ibge);
           printf("%s\n",cod_ibge);
           }

        else//case not match text between list_name.txt and output.csv.
        {
         cout << "Not found " << a << " line: " << i <<"\n";
         out[i][j] = (char*)malloc(strlen(inp[i][j]) + 1);
         strcat(inp[i][j],"_ERROR");
         //fprintf (txt_saida,"%s\n",inp[i][j]);
         strcpy(out[i][j], inp[i][j]);
        }

    }
    }
    else{
        for(i=0;i<number_lines;i++)
            {
                if (i==0){
                    printf("teste");
                   }
             out[i][j] = (char*)malloc(strlen(inp[i][j]) + 1);
             //fprintf (txt_saida,"%s\n",inp[i][j]);
             strcpy(out[i][j],inp[i][j]);
            }
        }
   }
   write_csv_file(number_of_colums,number_lines_colum_reference,new_csv_name,out,colum_name);
   //fclose(txt_saida);
    return 0;
}


void deblank(char *input)

{
    int i,j;
    int size_start = strlen(input);
    char output[size_start];
    for (i = 0, j = 0; i<size_start; i++,j++)
    {
        if (input[i]!=' ')
            {
                output[j]=input[i];
            }
        else
            j--;
    }
    output[j]=0;
    strcpy (input,output);
}


 void TiraAcentos(char *s1, int len) // source http://www.portugal-a-programar.pt/topic/35527-tirar-acentos-a-uma-string/

{
    setlocale(LC_ALL, "Portuguese");
    int i;
    int size_start = strlen(s1);
    char s[size_start];
    for (i = 0; i<size_start; i++)
    {
          sprintf(&s[i],"%c",s1[i]);
    }

        for (int i = 0; i < len; i++)

                if (s[i] < 0)
                       {
                        if (s[i] >= 'À' && s[i] <= 'Ý')
                        {
                                if (s[i] <= 'Ï')
                                {
                                        if (s[i] <= 'Ç')
                                        {
                                                if (s[i] <= 'Ä')
                                                        s[i] = 'A';
                                                else
                                                        s[i] = 'C';
                                        }
                                        else if (s[i] <= 'Ë')
                                                s[i] = 'E';
                                        else
                                                s[i] = 'I';
                                }
                                else if (s[i] <= 'Ö')
                                        {
                                                if (s[i] == 'Ñ')
                                                        s[i] = 'N';
                                                else
                                                        s[i] = 'O';
                                        }
                                        else if (s[i] <= 'Ü')
                                                s[i] = 'U';
                                        else
                                                s[i] = 'Y';
                        }
                        else if (s[i] >= 'à')
                        {
                                if (s[i] <= 'ï')
                                {
                                        if (s[i] <= 'ç')
                                        {
                                                if (s[i] <= 'ä')
                                                        s[i] = 'a';
                                                else
                                                        s[i] = 'c';
                                        }
                                        else if (s[i] <= 'ë')
                                                s[i] = 'e';
                                        else
                                                s[i] = 'i';
                                }
                                else if (s[i] <= 'ö')
                                        {
                                                if (s[i] == 'ñ')
                                                        s[i] = 'n';
                                                else
                                                        s[i] = 'o';
                                        }
                                        else if (s[i] <= 'ü')
                                                s[i] = 'u';
                                        else
                                                s[i] = 'y';
                        }
}
strcpy (s1,s);
}

void to_lowercase(char *input) //http://www.cplusplus.com/reference/cctype/tolower/
{
    int i=0;
    char str[101];
    char buf[101];
    char c;
    int size_start = strlen(input);
    strcpy (buf,input);
  while (i<size_start)
  {
    c=buf[i];
    str[i]=tolower(c);
    i++;
    str[i]='\0';
  }
strcpy (input,str);
}

int find_id_title(char *title_name, char *csv_name)
{
int i =  0;
    //                                   file, delimiter, first_line_is_header?
    CsvParser *csvparser1 = CsvParser_new(csv_name, ",", 1);
    CsvRow *header;

    header = CsvParser_getHeader(csvparser1);
    if (header == NULL) {
        printf("%s\n", CsvParser_getErrorMessage(csvparser1));
        exit(1);
    }
    char **headerFields = CsvParser_getFields(header);
    for (i = 0 ; i < CsvParser_getNumFields(header) ; i++) {

        if ((strcmp (title_name,headerFields[i]))== 0)
        {
         return i;
         //printf("TITLE: %s id: %i\n", headerFields[i],i);
        }
    }
return -1;
}


void read_colum_title_csv_file(char *csv_name,char *title_name[N2])
{
    //                                   file, delimiter, first_line_is_header?
    CsvParser *csvparser1 = CsvParser_new(csv_name, ",", 1);
    CsvRow *header;
    int i;
    header = CsvParser_getHeader(csvparser1);
    if (header == NULL) {
        printf("%s\n", CsvParser_getErrorMessage(csvparser1));
        exit(1);
    }
    char **headerFields = CsvParser_getFields(header);
    for (i = 0 ; i < CsvParser_getNumFields(header) ; i++) {
        title_name[i] = (char*)malloc(strlen(headerFields[i]) + 1);
        strcpy(title_name[i],headerFields[i]);
    }
}


int colum_count(char *csv_name)
{

            int number_colums=0;
    //                                   file, delimiter, first_line_is_header?
    CsvParser *csvparser2 = CsvParser_new(csv_name, ",", 1);
    CsvRow *header;
    //verificar csv
    header = CsvParser_getHeader(csvparser2);
    if (header == NULL) {
        printf("%s\n", CsvParser_getErrorMessage(csvparser2));
        exit(1);
    }
    number_colums=(CsvParser_getNumFields(header)-1);

return number_colums;
}


int row_count(int id_colum, char *csv_name)
{
            int i =  0;
            int k=0;
    //                                   file, delimiter, first_line_is_header?
    CsvParser *csvparser3 = CsvParser_new(csv_name, ",", 1);
    CsvRow *row1;
    CsvRow *header;
    //verificar csv
    header = CsvParser_getHeader(csvparser3);
    if (header == NULL) {
        printf("%s\n", CsvParser_getErrorMessage(csvparser3));
        exit(1);
    }
    //fim verificar csv
    while ((row1 = CsvParser_getRow(csvparser3)) ) {
    	//printf("NEW LINE:\n");
        //char **rowFields = CsvParser_getFields(row);
        for (i = 0 ; i < CsvParser_getNumFields(row1) ; i++) {
            if (i==id_colum){k++;}
            //printf("FIELD: %s\n", rowFields[i]);
        }

    }

return k;
}
void read_field_csv_file( char *csv_name, char *row_csv[N1][N2])//source http://sourceforge.net/p/cccsvparser/wiki/Home/ thanks: http://sourceforge.net/u/talh123/profile/
{
                int i =  0;
            int k=0;
    //                                   file, delimiter, first_line_is_header?
    CsvParser *csvparser4 = CsvParser_new(csv_name, ",", 1);
    CsvRow *row;
    CsvRow *header;
    //verificar csv
    header = CsvParser_getHeader(csvparser4);
    if (header == NULL) {
        printf("%s\n", CsvParser_getErrorMessage(csvparser4));
        exit(1);
    }
    //fim verificar csv


    while ((row = CsvParser_getRow(csvparser4)) ) {
    	//printf("NEW LINE:\n");
        char **rowFields = CsvParser_getFields(row);
        for (i = 0 ; i < CsvParser_getNumFields(row) ; i++)
            {
            row_csv[k][i] = (char*)malloc(strlen(rowFields[i]) + 1);

            strcpy(row_csv[k][i],rowFields[i]);

            //printf("FIELD: %s\n", rowFields[i]);
        }
         k++;
         CsvParser_destroy_row(row);

    }
CsvParser_destroy(csvparser4);
}

void find_correct(char *city_name, char *filename_cods, char *filename_names,char *citycode)
{
    char buff[512];
 FILE * cod_ibge_txt; //variavel arquivo_txt_ibge_Codigos
 FILE * names_ibge_txt; //variavel arquivo_txt_ibge_Nomes
 int k=0;
 int j=0;
 int l;
 char str[50];
 names_ibge_txt= fopen (filename_names,"r");
 while ( fgets( buff, sizeof buff, names_ibge_txt ) != NULL ) //le o arquivo
 {
  sscanf(buff, "%[^\n]",str);
  if (strcmp (city_name,str) == 0){break;}
  k++;
  }
 fclose(names_ibge_txt);
 cod_ibge_txt= fopen (filename_cods,"r");
 while ((l = getc(cod_ibge_txt)) != EOF) // interage no arquivo de texto que contem o nome das folhas
  {
  fscanf (cod_ibge_txt, "%[^\n]s", str);
  if (k==j){
           strcpy (citycode,str);
           break;
           }
  j++;
  }
  fclose(cod_ibge_txt);
  return;
}

void write_csv_file(int count_colum,int count_row, char *filename, char *row_csv[N1][N2], char *colum_csv[N2]) //source http://sourceforge.net/projects/cccsvwriter/
{
                                         // If you like to output to STDOUT, use NULL as filename
    CsvWriter *csvWriter = CsvWriter_new(filename, ",", 0);
    int i, j;
    for (i = 0 ; i <= count_row ; i++) {
        for (j = 0 ; j < count_colum ; j++) {

            if (i==0){
                    CsvWriter_writeField(csvWriter, colum_csv[j]);
                      }
            else{
            if (CsvWriter_writeField(csvWriter, row_csv[i-1][j])) {
                printf("Error: %s\n", CsvWriter_getErrorMessage(csvWriter));
                exit(1);
            }
            }
        }
        CsvWriter_nextRow(csvWriter);
    }
    CsvWriter_destroy(csvWriter);

}


