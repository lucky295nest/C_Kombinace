#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define IN "startovni_listina_kombinace_zeny.txt"
#define OUT "vysledkova_listina_kombinace_zeny.txt"
#define BUFSIZE 150
#define DELIMITERS " :,"

typedef struct CAS
{
    int minuty;
    int sekundy;
    int setiny;
} CAS;

typedef struct ZAVODNICE
{
    int startnum;
    char jmeno[20];
    char prijmeni[20];
    char narodnost[4];
    struct CAS sjezdy;
    struct CAS slalom;
} ZAVODNICE;

void hunToTime(int set, char *buf)
{
    sprintf(buf, "%d:%02d,%02d", set / 6000, (set - (set / 6000) * 6000) / 100, set % 100);
}

int timeToHun(int min, int sec, int set)
{
    return min * 6000 + sec * 100 + set;
}

int countPlusTime(ZAVODNICE *zav, int i)
{
    return timeToHun(zav[i].sjezdy.minuty, zav[i].sjezdy.sekundy, zav[i].sjezdy.setiny) + timeToHun(zav[i].slalom.minuty, zav[i].slalom.sekundy, zav[i].slalom.setiny);
}

void swap(ZAVODNICE *x, ZAVODNICE *y)
{
    ZAVODNICE pom = *x;
    *x = *y;
    *y = pom;
}

void bblSort(ZAVODNICE *zav, int pocet)
{
    for (int i = 0; i < pocet; i++)
    {
        for (int j = 0; j < pocet - i - 1; j++)
        {
            if (countPlusTime(zav, j) > countPlusTime(zav, j + 1))
            {
                swap(&zav[j], &zav[j + 1]);
            }
        }
    }
}

int readFile(ZAVODNICE *zav)
{
    FILE *fr;
    char buffer[BUFSIZE];
    char *data;
    int pocet = 0;
    int sloupec;

    if ((fr = fopen(IN, "r")) == NULL)
    {
        printf("There was an error opening file %s.", IN);
        return EXIT_FAILURE;
    }

    while (fgets(buffer, BUFSIZE, fr) != NULL)
    {
        pocet++;
        if (pocet != 0)
        {
            data = strtok(buffer, DELIMITERS);
            sloupec = 0;
            while (data != NULL)
            {
                sloupec++;
                switch (sloupec)
                {
                case 1:
                    zav[pocet - 1].startnum = atoi(data);
                    break;
                case 2:
                    strcpy(zav[pocet - 1].jmeno, data);
                    break;
                case 3:
                    strcpy(zav[pocet - 1].prijmeni, data);
                    break;
                case 4:
                    strcpy(zav[pocet - 1].narodnost, data);
                    break;
                case 5:
                    zav[pocet - 1].sjezdy.minuty = atoi(data);
                    break;
                case 6:
                    zav[pocet - 1].sjezdy.sekundy = atoi(data);
                    break;
                case 7:
                    zav[pocet - 1].sjezdy.setiny = atoi(data);
                    break;
                case 8:
                    zav[pocet - 1].slalom.minuty = 0;
                    zav[pocet - 1].slalom.sekundy = atoi(data);
                    break;
                case 9:
                    zav[pocet - 1].slalom.setiny = atoi(data);
                    break;
                }
                data = strtok(NULL, DELIMITERS);
            }
        }
    }
    if (fclose(fr) == EOF)
    {
        printf("There was an error closing file %s.", IN);
    }
    return (pocet);
}

void writeToTerm(ZAVODNICE *zav, int pocet)
{
    printf("\n===========================================================================\n");
    printf("========================= LYZOVANI KOMBINACE ZENY =========================\n");
    printf("===========================================================================\n");
    printf("|STARTOVNI CISLO|     JMENO|       PRIJMENI| NARODNOST|   SJEZDY|   SLALOM|\n");
    printf("---------------------------------------------------------------------------\n");

    for (int i = 0; i < pocet; i++)
    {
        printf("|%15d|%10s|%15s|%10s| %02d:%02d,%02d| %02d:%02d,%02d|\n",
               zav[i].startnum,
               zav[i].jmeno,
               zav[i].prijmeni,
               zav[i].narodnost,
               zav[i].sjezdy.minuty,
               zav[i].sjezdy.sekundy,
               zav[i].sjezdy.setiny,
               zav[i].slalom.minuty,
               zav[i].slalom.sekundy,
               zav[i].slalom.setiny);
    }

    printf("===========================================================================\n");
    printf("Soubor %s byl vytvoren.\n\n", OUT);
}

void writeToFile(ZAVODNICE *zav, int pocet)
{
    FILE * fw;
    char buffer[20];
    char ztrata[20];
    int prvni;

    if ((fw = fopen(OUT, "w")) == NULL)
    {
        printf("There was an error opening file %s.", OUT);
    }
    
    fprintf(fw, "=====================================================================================\n");
    fprintf(fw, "================================ VYSLEDKOVA LISTINA =================================\n");
    fprintf(fw, "============================== LYZOVANI KOMBINACE ZENY ==============================\n");
    fprintf(fw, "=====================================================================================\n");
    fprintf(fw, "|PORADI|     JMENO|       PRIJMENI| NARODNOST|   SJEZDY|   SLALOM|  CELKEM|   ZTRATA|\n");
    fprintf(fw, "+------+----------+---------------+----------+---------+---------+--------+---------+\n");

    prvni = countPlusTime(zav, 0);
    for (int i = 0; i < pocet; i++)
    {
        hunToTime(countPlusTime(zav, i) - prvni, ztrata);
        hunToTime(countPlusTime(zav, i), buffer);
        fprintf(fw, "|%6d|%10s|%15s|%10s| %02d:%02d,%02d| %02d:%02d,%02d| %s| %s%s|\n",
               i + 1,
               zav[i].jmeno,
               zav[i].prijmeni,
               zav[i].narodnost,
               zav[i].sjezdy.minuty,
               zav[i].sjezdy.sekundy,
               zav[i].sjezdy.setiny,
               zav[i].slalom.minuty,
               zav[i].slalom.sekundy,
               zav[i].slalom.setiny,
               buffer,
               i > 0 ? "+" : " ",
               i > 0 ? ztrata : "       "
               );
    }

    fprintf(fw, "+------+----------+---------------+----------+---------+---------+--------+---------+\n");
}

int main()
{
    ZAVODNICE zav[BUFSIZE];
    int pocet;

    pocet = readFile(zav);
    writeToTerm(zav, pocet);
    bblSort(zav, pocet);
    writeToFile(zav, pocet);

    return EXIT_SUCCESS;
}