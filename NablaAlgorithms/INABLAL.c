/*
 Algorithm INABLAL
 Lower bound, sparse Matrix, 
 first step: computation of vector dinf for initilisation and
 check that dinf is not zero. 
 V1.0 15/4/10 jmf
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Nabla.h"

#define DEBUG 0

struct element {
    int ori;
    double prob;};

typedef struct element element;
typedef double *probavector; /* de la taille du nombre d'etats */
typedef int *indexvector; /* de la taille du nombre d'etats */
typedef element *elmtvector; /* de la taille du nombre d'arcs */

void  usage()
{
	printf("usage : INABLAL -f filename Suffix \n");
	printf("filename.Suffix and filename.sz must exist before. And the suffix must be Cuu \n");
	printf("Computes elemnet-wise lower bound pf of the stationnary distribution\n");
	exit(1);
}

void  ProblemeMemoire()
{
	printf("Sorry, not enougth memory for the matrix and the probability vectors \n");
	exit(1);
}

void  ProblemeVecteur()
{
	printf("Sorry, vector dinf must be not zero \n");
	exit(1);
}

static double max(x, y)
double x, y;
{
    if (x > y)
        return x;
    else
        return y;
}

static double min(x, y)
double x, y;
{
    if (x > y)
        return y;
    else
        return x;
}


int main(argc, argv)
int argc;
char *argv[];
{
    int NSommets, NArcs;
    int i, c, j, degre, iter, ji;
    double  y, S, mi, R;
    FILE *pf1;
    char s0[200];
    char s1[200];
    char s2[200];
    char s3[200];
    
    /*
     Les structures dynamiques a creer par malloc
     */
    probavector pinf1, pinf2, dinf;
    indexvector debut;
    indexvector taille;
    elmtvector  arc;
    
    /*
     On recupere le nom du modele et on verifie que les fichiers existent....
     */
    
	if (argc!=4) usage();
	--argc;
	++argv;
	if (**argv != '-') usage();
	switch (*++*argv) {
		case 'f' :  /* on recupere le nom de fichier */
		{++argv;
            strcpy(s0,*argv);
            ++argv; /* on recupere le nouveau suffixe */
            strcpy(s1,*argv);
            /* on ajoute .sz, et le suffixe */
            strcpy(s2,s0);
            strcat(s2,".sz");
            strcpy(s3,s0);
            strcat(s3,".Dyn.L.pi");
            /* on verifie si il existe un fichier .sz*/
            if ((pf1=fopen(s2,"r"))==NULL)
			{
                usage();
			}
            /* on verifie si le suffixe est Rxx */
            if (s1[0]!='C')  
            {
                usage();
			}
            /* on verifie que le fichier model.suffix existe */
            strcat(s0,".");
            strcat(s0,s1);
            if ((pf1=fopen(s0,"r"))==NULL)
			{
                usage();
			}
            break;
		}
		default	 : usage();
	}
    
    /*
     On recupere les tailles dans le fichier filename.sz
     */
    
    pf1=fopen(s2,"r");
    
    fscanf(pf1,"%d\n", &NArcs);
    fscanf(pf1,"%d\n", &NSommets);
    fclose(pf1);	
    
    printf("Nb arcs = %12d\n", NArcs);
    printf("Nb som. = %12d\n", NSommets);
    printf("Max number of iterations = %d\n",maxiter);
    
    /* 
     On cree les objets ou on exit(1) en cas de pb
     */
    
    /* 3 vecteurs de proba */ 
    if (!(pinf1=(double *)malloc(NSommets*sizeof(double)))) ProblemeMemoire();
    if (!(pinf2=(double *)malloc(NSommets*sizeof(double)))) ProblemeMemoire();
    if (!(dinf =(double *)malloc(NSommets*sizeof(double)))) ProblemeMemoire();
    /* le codage creux de la matrice */ 
    if (!(debut=(int *)malloc(NSommets*sizeof(int)))) ProblemeMemoire();
    if (!(taille=(int *)malloc(NSommets*sizeof(int)))) ProblemeMemoire();
    if (!(arc=(element *)malloc(NArcs*sizeof(element)))) ProblemeMemoire();
    
    /*
     Lecture de la matrice et calcul de dinf. 
     */
    
    pf1=fopen(s0,"r");
    c = 0;
    iter= 0;
    
    for (i = 0; i < NSommets; i++) {
        fscanf(pf1,"%d", &ji);
        fscanf(pf1,"%d", &degre);
        debut[ji] = c;
        taille[ji] = degre;
        mi= 1.0;
        for (j = 1; j <= degre; j++)  {
            fscanf(pf1,"%lg", &arc[c].prob);
            fscanf(pf1,"%d", &arc[c].ori);
            mi = min(mi,arc[c].prob);
            c++;
        }
        getc(pf1); 
        if (degre<NSommets) {dinf[i]=0;}
        else {dinf[i]=mi; if (mi>0) iter=1;}
    }
    fclose(pf1);
    
    if (DEBUG==1) {
        for (i=0;i<NArcs;i++) {printf("%d %f \n",arc[i].ori,arc[i].prob);}
        for (i=0;i<NSommets;i++) {printf("%d %d \n",debut[i],taille[i]);}
    }
    
    printf("dinf: ");      
    for (j = 0; j < NSommets; j++) {
        printf("%lf  ",dinf[j]);      
    }
    
    R=1.0;
    for (j = 0; j < NSommets; j++) {R-=dinf[j];
        pinf2[j]=dinf[j];
    }
    if (DEBUG==1) printf("%lf \n",R);
    if (iter==0) ProblemeVecteur();
    
    iter = 0;
    do {
        iter++;
        for (i = 0; i < NSommets; i++) {
            S=dinf[i]*R;
            if (DEBUG==1) {printf("S   %lf \n ",S);}

            for (j = debut[i]; j < debut[i] + taille[i]; j++)
            {
                c= arc[j].ori;
                mi = arc[j].prob;
                if (DEBUG==1) {printf("%d %d %d %lf \n ",i,j,c,mi);}
                y = pinf2[c];
                S += y * mi;
            }
            pinf1[i] = max(S,pinf2[i]);
        }
        
        R=1.0;
        for (j = 0; j < NSommets; j++) {R-=pinf1[j];
        }
       if (DEBUG==1) printf("%d  & ",iter);      
        
        for (j = 0; j < NSommets; j++) {
            if (DEBUG==1) {printf("%lf & ", pinf1[j]);}
            pinf2[j]=pinf1[j];
        }
       if (DEBUG==1) printf("&  Residu %lf \n",R);      
        
    } while ((iter != maxiter) && (R >= maxresidu));
    
    // Ecriture du resultat
    
    pf1=fopen(s3,"w");
    printf("\nDist Stat: ");
    for (i = 0; i < NSommets; i++) {fprintf(pf1,"%lg \n", pinf1[i]); printf("%lg ", pinf1[i]); }
    fclose(pf1);
    printf("\nDone INABLAL \n");
    
    exit(0);
}

