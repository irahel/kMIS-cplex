#include <stdio.h>
#include <stdint.h>	
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "linear.h"

/* VARIAVEIS GLOBAIS DE ENTRADA */

int **G; /*matriz de adjacencia representando o Grafo*/
int s;/* numero de subconjuntos do Grafo ---LINHAS----  */
int e; /* numero de elementos ---COLUNAS--- */
int k; /* demanda, quantidade de subconjuntos escolhidos */ 

/* VARIAVEIS GLOBAIS PARA AS VARIAVEIS DO CPLEX */

int *variavelX; /* vetor para criar as variaveis para os elementos*/
int *variavelY; /* vetor para criar as variaveis para os subconjuntos*/
int cont = 0; /*quantidade de variaves de vertice*/

CPXENVptr env;/*o ambiente do problema*/
CPXLPptr lp;/*o problema*/
CPXFILEptr logfile = NULL;

FILE *out;/*o arquivo de saida (RESULTADO DO CPLEX)*/

#define BILLION 1000000000L

/* Carrega as entradas */
//ok
void carregarGrafo(char arq[])
{
	FILE *inst = fopen(arq,"r");
    if(!inst)   printf("erro na leitura do arquivo!");			
    
    int i,j;
    int v;
    /* Pega o numero de subconjuntos (linhas)*/
    fscanf(inst,"%d",&s);
    /* Pega o numero de elementos (colunas)*/
    fscanf(inst,"%d",&e);
    /* Aloca espaço para as linhas */
    G = (int**)(malloc(s*sizeof(int*)));
    /* Preenche as linhas */
    for (i=0;i<s;i++)
    {
        /* Aloca espaço para as colunas */
        G[i] = (int*)malloc(e*sizeof(int));
        /* Preenche as colunas */
        for (j=0;j<e;j++)
        {
            fscanf(inst,"%d",&v);				
            G[i][j] = v;	
        }
    }
    fclose(inst);
    //TESTE
    /*
    printf("\n--->Subconjuntos = %d \n", s);
    printf("--->Elementos = %d \n", e);
    for (i=0;i<s;i++)
    {
        for (j=0;j<e;j++)
        {		
            printf("%d ", G[i][j]);
        }
        printf("\n");
    } */

}

//ok
void carregarDemanda(char arq[])
{

    FILE *inst = fopen(arq,"r");
    if(!inst) printf("erro na leitura do arquivo!\n");			
		
    /* Pega o numero de subconjuntos que serão escolhidos*/
    fscanf(inst,"%d",&k);

    //TESTE
    //printf("\n--->Subconjuntos a serem escolhidos: %d\n", k);
   
    fclose(inst);
}

/* Carrega as variaveis */
//ok
void carregarVariaveis()
{
    /* indexação */
    int L,R,u;
    L = s;
    R = e;
	cont =0;
	//variavel para os elementos (X)
	variavelX = (int*) malloc (R* sizeof(int));
		
    for (u=0; u<R; u++)
    {
        variavelX[u] = cont;
        cont ++;
    }

    //variaveis para os subconjuntos (Y)
    variavelY = (int*)malloc(L*sizeof(int));
	     
    for (u=0; u<L; u++)
    {
        variavelY[u] = cont;
        cont++;
    }

    //TESTE
    /*
    printf("\n----> indexacao das variaveis dos elementos X: \n");
    printf("[ ");
    for (u=0; u<R; u++) printf("%d ", variavelX[u]);
    printf("]");
    
    printf("\n----> indexacao das variaveis dos subconjuntos Y:\n");
    printf("[ ");
    for (u=0; u<L; u++) printf("%d ", variavelY[u]);
    printf("]");
    */
}

/* Inicializando o cplex */
//ok
void startCplex(char arq[])
{
	int L,R;
    L = s;
    R = e;
    int status = 0;
	env = CPXopenCPLEX(&status);
	
	/* Abrindo um logfile */
    logfile = CPXfopen (arq, "a");
    if ( logfile == NULL )
    {  
        printf("\nErro na abertura do arquivo de log\n");	
    }else{
        status = CPXsetlogfile (env, logfile);
        if ( status )   printf("\nErro na abertura do arquivo de log\n");		
    }

	lp = CPXcreateprob(env, &status,"kMIS");
	
	double* obj = (double*)malloc(cont*sizeof(double));/*a funcao objetivo*/
	double* lb =  (double*)malloc(cont*sizeof(double));/*o limite inferior das variaveis*/
	double* ub =  (double*)malloc(cont*sizeof(double));/*o limite superior das variaveis*/
	char* type =  (char*)malloc(cont*sizeof(char));/*o tipo das variaveis*/

    /* Criacao da funcao objetivo */
    //X
	int u, contador=0;
    for(u = 0; u < R; u ++)
    {
        obj[contador] = 1.0; //as variaveis X vao para funcao objetivo
        lb[contador] = 0.0;
        ub[contador] = 1.0;
        type[contador] = 'B';
        contador++;
    }
    
    //Y
    for(u = 0; u < L; u ++)
    {
        obj[contador] = 0.0; //as variaveis Y nao vao para funcao objetivo
        lb[contador] = 0.0;
        ub[contador] = 1.0;
        type[contador] = 'B';
        contador++;
    }
    
    
    //TESTE
    /*
    printf("--->funcao objetivo \n");
    printf("%d", contador);
    printf("[ ");
    for(u = 0; u < cont; u++)   printf("%lf ", obj[u]);
    printf("]");
    */
    
	status = CPXnewcols (env, lp, contador, obj, lb, ub, type, NULL);
	CPXchgobjsen (env, lp, CPX_MAX);
}

/* Restricoes da Formulacao */
//ok
//Essa restrição força que k subconjuntos sejam escolhidos.
void addRestricaoDemanda()
{
    int L,R;
    L = s;
    R = e;
    int tam = L + R;
	double coef[tam];
	int col[tam];
	int u,contador=0;
    //X
    for(u = 0; u < R; u ++)
    {
        col[contador] = 0.0; //as variaveis X nao vao para restricao
        coef[contador] = 0.0;
        contador++;
    }
    
    //Y
    for(u = 0; u < L; u ++)
    {
        col[contador] = variavelY[u]; //as variaveis Y  vao para restricao
        coef[contador] = 1.0;
        contador++;
    }
    
	int rows[2] = {0,contador};
	double b[1] = {k};
	char sense[1] = {'E'};

    //TESTE
    /*
    printf("--->funcao objetivo \n");
    printf("[ ");
    for(u = 0; u < cont; u++)   printf("%lf ", col[u] * coef[u]);
    printf("]");
    */
    
    
    
	CPXaddrows(env, lp, 0, 1, contador, b, sense, rows, col, coef, NULL, NULL);

}

//ok
//Essa restrição assegura que Ei não pode estar na interseção caso Sj tenha sido escolhido quando Ei nao pertence Sj .
void addRestricaoAresta(int i, int j)
{
    /* Xi + Yj <= 1 */
  	int z, contador =0;
	int tam = 2;
	double coef[tam];
	int col[tam];
	
	col[contador] = variavelX[i];
	coef[contador] = 1.0;
	contador++;

	col[contador] = variavelY[j];
	coef[contador] = 1.0;
	contador++;	
		
	int rows[2] = {0,contador};
	double b[1] = {1.0};
	char sense[1] = {'L'};
    
	CPXaddrows(env, lp, 0, 1, contador, b, sense, rows, col, coef, NULL, NULL);  
}

/* resolucao */
//ok
void solveProblem(char arq1[], char arq2[])
{

	uint64_t diff;
	struct timespec start, end;
	
	/* measure monotonic time */
	clock_gettime(CLOCK_MONOTONIC, &start);	/* mark start time */

	int status = 0;

	out = fopen(arq1, "w"); 
	CPXwriteprob (env, lp, arq2, NULL);

	
	status = CPXmipopt(env,lp);
	
	clock_gettime(CLOCK_MONOTONIC, &end);	/* mark the end time */

	
	diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;

	double total =  ((long long unsigned int) diff)/1e9;
	printf("\ntempo em segundos: %f\n", total);

	int cnt = CPXgetmipitcnt(env, lp);
	int nodeint = CPXgetnodeint (env, lp);
    if(status == 0) printf ("status resolucao: Dibas\n");
	

	double value;
	status = CPXgetobjval(env, lp, &value);
    if(status == 0) printf ("status retorno de funcao objetivo: Suave\n");
	

	double vetor[cont];	
	status=CPXgetx(env, lp, vetor, 0,cont-1);
    if(status == 0) printf ("status retorno de valor das variaveis: De boa\n");	

	//ESCREVENDO ARQUIVO DE SAIDA

	int j,u=0;
    int L,R;
    L = s;
    R = e;
    
	int contador=0;
	fprintf(out, "%s\n", "Solucao");
	fprintf(out, "%s\n", "Valor objetivo =");
	fprintf(out, "%f\n", value);
	fprintf(out, "%s\n", "Interações =");
	fprintf(out, "%d\n", cnt);
	fprintf(out, "%s\n", "Numero de nos =");
	fprintf(out, "%d\n", nodeint);
	for(u=0; u<R;u++)
	{
					
        fprintf(out, "%c", 'X');
        fprintf(out, "%d",  (u+1));
        fprintf(out, "%c", '=');
        fprintf(out, "%f\n", vetor[contador]);
        contador++;
			
	}	
	

	for (u=0; u<L; u++)
	{
        fprintf(out, "%c", 'Y');
        fprintf(out, "%d",  (u+1));
        fprintf(out, "%c", '=');
        fprintf(out, "%f\n", vetor[contador]);
        contador++;
	}
	
	fprintf(out, "\ntempo em segundos: %f\n", total);

	 fclose(out);
}


int main()
{

// 	char *arq1 = "instancias/grafo 100-100/grafo.txt";	
// 	char *arq2 = "instancias/grafo 100-100/demanda.txt";
// 	char *arq3 = "instancias/grafo 100-100/log.txt";
// 	char *arq4 = "instancias/grafo 100-100/solucao.txt";
// 	char *arq5 = "instancias/grafo 100-100/lp.lp";

    
	char *arq1 = "instancias/especiais/alta/20 20 alta.txt";	
	char *arq2 = "instancias/especiais/alta/demanda.txt";
	char *arq3 = "instancias/especiais/alta/log.txt";
	char *arq4 = "instancias/especiais/alta/solucao.txt";
	char *arq5 = "instancias/especiais/alta/lp.lp";

    
	carregarGrafo(arq1);	
	carregarDemanda(arq2);
	carregarVariaveis();
	startCplex(arq3);
   	addRestricaoDemanda();		
    
    int L,R;
    L = s;
    R = e;
    int u, j, v;
    
	for(j = 0; j < L; j ++)    for(u = 0; u < R; u ++)     if(G[j][u] == 0) addRestricaoAresta(u, j);
        
	solveProblem(arq4, arq5);

	printf("\nBiri Biri\n");

}
