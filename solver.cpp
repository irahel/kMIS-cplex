#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include <ctime>
%#include <ilcplex/ilocplex.h>
#include "solver.h"
#include "mincut.h"

ILOSTLBEGIN

static char pertence(int n, int *V, int x)
{
	for (int i=0;i<n;i++)
	{
		if (V[i]==x) return 1;
	}
	return 0;
}

static void add_variaveis_objetivo_e_m(int n, int **M, int nc, IloEnv *env, IloNumVarArray *y, IloNumArray *x, IloModel *model, int **conv)
{
	int i, j;
	IloExpr expr_objetivo(*env), expr_m(*env);
	
	for (i=0;i<n;i++)
	{
		y[i] = IloNumVarArray(*env, n);
		x[i] = IloNumArray(*env, n);
	}
	
	char nome[10];
	
	int m = 0;
	
	for (i=0; i<n; i++)
	{
		for (j=0;j<n;j++)
		{
			if (M[i][j] != MAXINT && i<nc && j!=0)
			{
				sprintf(nome,"y_%d_%d",i+1,j+1);
				conv[i][j] = y[i].getSize();
				//y[i].add(IloNumVar(*env, 0, 1, ILOINT, nome));
				y[i].add(IloNumVar(*env, 0, 1, ILOFLOAT, nome));
				expr_objetivo += M[i][j]*y[i][conv[i][j]];
				if (j<nc) expr_m += y[i][conv[i][j]];
				m++;
			}
		}
		model->add(y[i]);
	}
	
	FILE *tabela = fopen("tabela","w");
	fprintf(tabela, "%d\n", m);
	m = 1;
	
	for (i=0; i<n; i++)
	{
		for (j=0;j<n;j++)
		{
			if (M[i][j] != MAXINT && i<nc && j!=0)
			{
				fprintf(tabela,"y_%d_%d x%d\n",i+1,j+1,m);
				m++;
			}
		}
	}
	
	model->add(IloObjective(*env, expr_objetivo, IloObjective::Minimize, "OBJ"));
	
	model->add(IloRange(*env, nc-1, expr_m, nc-1));
	
	expr_objetivo.end();
	
	expr_m.end();	
}

static void add_grau_raiz(int n, int **M, IloEnv *env, IloNumVarArray *y, IloModel *model, int d, int **conv)
{
	IloExpr expr(*env);
	
	for (int i=1;i<n;i++) if (M[0][i] != MAXINT) expr += y[0][conv[0][i]];
	
	model->add(IloRange(*env, d, expr, IloInfinity));
	
	expr.end();
}

static void add_grau_vertice(int n, int **M, IloEnv *env, IloNumVarArray *y, IloModel *model, int d, int c, int **conv)
{
	IloExpr expr(*env);
	
	for (int i=1;i<n;i++) if (M[c][i] != MAXINT && c!=i) expr += y[c][conv[c][i]];
	
	model->add(IloRange(*env, d-1, expr, IloInfinity));
	
	expr.end();
}

static void add_chegada_terminal(int **M, int nc, IloEnv *env, IloNumVarArray *y, IloModel *model, int t, int **conv)
{
	IloExpr expr(*env);
	
	for (int i=0;i<nc;i++) if (M[i][t] != MAXINT) expr += y[i][conv[i][t]];
	
	model->add(IloRange(*env, 1, expr, 1));
	
	expr.end();
}

static void add_chegada_central(int **M, int nc, IloEnv *env, IloNumVarArray *y, IloModel *model, int c, int **conv)
{
	IloExpr expr(*env);
	
	for (int i=0;i<nc;i++) if (M[i][c] != MAXINT && i!=c) expr += y[i][conv[i][c]];
	
	model->add(IloRange(*env, 1, expr, IloInfinity));
	
	expr.end();
}

static void add_corte(int **M, int nc, IloEnv *env, IloNumVarArray *y, IloModel *model, int nS, int *S, int **conv)
{
	IloExpr expr(*env);
	
	for (int i=0;i<nS;i++)
	{
		for (int j=1;j<nc;j++)
		{
			if (M[S[i]][j] != MAXINT && !pertence(nS,S,j)) expr += y[S[i]][conv[S[i]][j]];
		}
	}
	
//	env->out() << "corte adicionado: " << IloRange(*env, 1, expr, IloInfinity) << endl;
	
	model->add(IloRange(*env, 1, expr, IloInfinity));
	
	expr.end();
}

void resolve(int n, int **M, int nc, int d)
{
	int i, j, S[n], nS, *conv[n];
	double *G[n], *f[n], *Gf[n];
	
	for (i=0;i<n;i++)
	{
		f[i] = (double*)malloc(n*sizeof(double));
		Gf[i] = (double*)malloc(n*sizeof(double));
		G[i] = (double*)malloc(n*sizeof(double));
		conv[i] = (int*)malloc(n*sizeof(int));
	}
	
	IloEnv env;
	IloNumVarArray y[n];
	IloNumArray x[n];
	IloModel model(env);
	
//////////////////
	
	time_t begin = clock();
	
	add_variaveis_objetivo_e_m(n, M, nc, &env, y, x, &model, conv);
	
	add_grau_raiz(n, M, &env, y, &model, d, conv);
	
	for (i=1;i<nc;i++) add_grau_vertice(n, M, &env, y, &model, d, i, conv);
	
	for (i=nc;i<n;i++) add_chegada_terminal(M, nc, &env, y, &model, i, conv);
	
	for (i=1;i<nc;i++) add_chegada_central(M, nc, &env, y, &model, i, conv);
	
	IloCplex cplex(model);
	
	cplex.setParam(IloCplex::MIPDisplay, 0);

	cplex.setParam(IloCplex::SimDisplay, 0);
	
	double corte;
	
	char variacao=0;
	
	do
	{		
		cplex.solve();
		
		for (i=0;i<n;i++)
		{
			for (j=0;j<n;j++)
			{
				if (M[i][j] != MAXINT && i<nc && j!=0) G[i][j] = (double)cplex.getValue(y[i][conv[i][j]]);
				else G[i][j] = 0;
			}
		}
		
		if (variacao == 0)
		{
			for (i=1;i<nc;i++)
			{
				corte = corte_minimo(nc, G, 0, i, &nS, S, f, Gf);
				if (corte<=0.999999999)
				{
//					printf("corte violado: %.2f\n",corte);
					add_corte(M, nc, &env, y, &model, nS, S, conv);
					break;
				}
			}
		}
		else
		{
			for (i=nc-1;i>0;i--)
			{
				corte = corte_minimo(nc, G, 0, i, &nS, S, f, Gf);
				if (corte<=0.999999999)
				{
//					printf("corte violado: %.2f\n",corte);
					add_corte(M, nc, &env, y, &model, nS, S, conv);
					break;
				}
			}
		}
		variacao = (variacao+1)%2;
		
	}while(corte<0.99999999);
	
	cplex.out() << "Solution status: " << cplex.getStatus()
	<< "; Objective value: " << cplex.getObjValue()
	<< "; Time spent: " << ((double)(clock()-begin))/(double)(CLOCKS_PER_SEC) << "s." << endl;
	
	for (i=0;i<nc;i++)
	{
		for (j=1;j<n;j++)
		{
			if (G[i][j] != 0) printf("y_%d_%d\t\t%.2f\n",i+1,j+1,G[i][j]);
		}
	}
	
//////////////////	
	for (i=0;i<n;i++)
	{
		free(f[i]);
		free(Gf[i]);
		free(G[i]);
		free(conv[i]);
	}
	printf("Salvar modelagem? (1 para 'sim') ");
	scanf("%d",&i);
	if (i==1)
	{
		char nome[21];
		printf("Nome do arquivo: (incluir '.lp' / maximo 20 caracteres) ");
		scanf("%s",nome);
		cplex.exportModel(nome);
	}
}
