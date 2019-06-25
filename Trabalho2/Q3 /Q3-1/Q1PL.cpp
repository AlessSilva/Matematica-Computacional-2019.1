#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits.h>
#include <math.h>
#include <ilcplex/ilocplex.h>

using namespace std;

#define eps 0.000001

ILOSTLBEGIN

int n,m; // n clientes, m fornecedores
double demanda[1000]; // demanda dos n clientes
double capacidade[1000]; // capacidade dos m fornecedores
double custo[1000][1000]; // custo do cliente i com o fornecedor j

IloEnv   env;
IloModel model(env);
IloNumVarArray var_x(env);
IloObjective obj = IloMinimize(env); // definindo a função como minimização
IloCplex cplex(env);

void criarModeloPL() {
    
    IloExpr expressao(env);
    IloRangeArray condicoes(env);
    
    try {

        for( int i = 0; i < n; i++) {
			for( int j = 0; j < m; j++ ){
		       
		       char nome[50];
               sprintf(nome,"x%d%d",i+1,j+1);
              
               /* Restricao de integridade */ 
               var_x.add(IloNumVar(env,0,IloInfinity,ILOINT));
               
               int indice = ( (i*m) ) + j;
               
               var_x[indice].setName(nome);
               
               //adicionando uma variável da função objetivo com seu respectivo coeficiente (no caso o custo) 
               obj.setLinearCoef(var_x[indice], custo[i][j]);
               
		    }
        }
    
        //Condição que respeita a capacidade de cada fornecedor
		for( int j = 0; j < m; j++ ){
		
		   for( int i = 0; i < n; i++ ){
		   
			 int indice = ( (i*m) ) + j;
		   
			 expressao += var_x[indice];
		   
		   }
		   
		   //adicionado uma condição ao nosso modelo
		   condicoes.add(IloRange(env, expressao, capacidade[j]));
		   expressao.clear();
		}
		
		//Condição que respeita a demanda de cada cliente (é necessario multiplicar por (-1) para mudar de >= para <=)
		for( int i = 0; i < n; i++ ){
		
		   for( int j = 0; j < m; j++ ){
		   
			 int indice = ( (i*m) ) + j;
		   
			 expressao += var_x[indice]*(-1);
		   
		   }
		   
		   condicoes.add(IloRange(env, expressao, demanda[i]*(-1)));
		   expressao.clear();
		}
		
		model.add(condicoes);
		condicoes.clear();
		model.add(obj);
		cplex.extract(model);
		//Salvando modelo no arquivo
		cplex.exportModel("PLModelo.lp");
		
    }
    catch (IloException& e) {
        cerr << "Erro: " << e << endl;
    }
    catch (...) {
        cerr << "Erro desconhecido" << endl;
    }
}

bool solucionaProblemaPL(double *x, double *sol_val) {
    
    if ( !cplex.solve() && cplex.getStatus() != IloAlgorithm::Infeasible) {
        env.error() << "Falha em otimizar" << endl;
        cout << cplex.getStatus() << endl;
        throw(-1);
    }

    if(cplex.getStatus() != IloAlgorithm::Infeasible) {
        *sol_val = cplex.getObjValue();
        
        for(int i=0; i<n; i++){
		  for(int j=0; j<m; j++){
			int indice = ( (i*m) ) + j;
		    x[indice] = cplex.getValue(var_x[indice]);
		  }
		}
        return true;
    }
    return false;
}

void procedurePL() {
	
	ofstream arq("PLSolucao.txt");
    arq.clear();
    
    double sol_val=-1;
    double *x=NULL;

    x=(double*) malloc((n*m)*sizeof(double));    
    
    memset(x,0.0,(n*m)*sizeof(double));
    
    if(!solucionaProblemaPL(x,&sol_val)) printf("Insatisfazivel!!!\n"); 

    printf("Solução Ótima = %.2lf\n",sol_val);
    
    //Salvando valor de z no arquivo
    arq << "z = " << sol_val << "\n";
        
    for(int i=0;i<n;i++){
		for(int j=0;j<m;j++){
			int indice = ( (i*m) ) + j;
			//Salvando o valor das variaveis de decisao no arquivo 
			arq << "x[" << i + 1 << "][" << j+1 << "] = " << x[indice] << " \n";
		}
    }
	
    free(x);
}


int main (int argc, char *argv[]) {
    
    if (argc < 2) {
        printf("Por favor, informe arquivo válido \n");
        return 0;
    }
    
    ifstream arq(argv[1]);
    
    string linha;
    
    //Lendo o numero de cliente
    getline(arq,linha);
    n = std::stoi(linha);
    
    //Lendo o numero de fornecedores
    getline(arq,linha);
    m = std::stoi (linha);
    
    //Lendo as n demandas
    for( int i = 0; i < n; i++ ){
      getline(arq,linha);
      demanda[i] = std::stod (linha);
    }
    
    //Lendo as m capacidades
    for( int j = 0; j < m; j++ ){
      getline(arq,linha);
      capacidade[j] = std::stod (linha);
    }
    
    //Lendo os n*m custos
    for( int i = 0; i < n; i++ ){
      
      for( int j = 0; j < m; j++ ){
        getline(arq,linha);
        custo[i][j] = std::stod (linha);
      }
      
    }
    
    clock_t inicio, fim;
    double elapsed;
    inicio = clock();
    
    cplex.setOut(env.getNullStream());      
    cplex.setWarning(env.getNullStream());
    
    criarModeloPL();
    
    procedurePL();
    
    env.end();
    fim = clock();
    elapsed = ((double) (fim - inicio)) / CLOCKS_PER_SEC;
    
    printf("Time: %.5g second(s).\n", elapsed);
    
    return 0;
}
