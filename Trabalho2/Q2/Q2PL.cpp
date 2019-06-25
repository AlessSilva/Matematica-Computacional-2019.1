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
IloNumVarArray var_y(env);
IloObjective obj = IloMaximize(env); // definindo a função como maximização
IloCplex cplex(env);

void criarModeloPL() {
    
    IloExpr expressao(env);
    IloRangeArray condicoes(env);
    
    try {
		
		//Transformando o lado direito das restrições em coeficientes da objetivo (capacidade)
		for( int j = 0; j < m; j++ ){
		  char nome[50];
		  sprintf(nome, "y%d",j);
		  var_y.add(IloNumVar(env,0,IloInfinity));
		  obj.setLinearCoef(var_y[j], capacidade[j]*(-1));
		}
		
		//Transformando o lado direito das restrições em coeficientes da objetivo (demanda)
		for( int i = 0; i < n; i++ ){
		  char nome[50];
		  sprintf(nome, "y%d",i+m);
		  var_y.add(IloNumVar(env,0,IloInfinity));
		  obj.setLinearCoef(var_y[i+m], demanda[i]);
		}
		
		//Construindo novas restrições
		for( int j = 0; j < m; j++ ){
		  for( int i = 0; i < n; i++ ){
			expressao = -var_y[j] + var_y[i+m];
            condicoes.add(IloRange(env, expressao, custo[i][j]));
		    expressao.clear();  
	      }
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
        for(int i=0; i<(n+m); i++){
		    x[i] = cplex.getValue(var_y[i]);
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

    x=(double*) malloc((n+m)*sizeof(double));    
    
    memset(x,0.0,(n+m)*sizeof(double));
    
    if(!solucionaProblemaPL(x,&sol_val)) printf("Insatisfazivel!!!\n"); 

    printf("Solução Ótima = %.2lf\n",sol_val);
    
    //Salvando valor de w no arquivo
    arq << "w = " << sol_val << "\n";
        
    for(int i=0;i<(n+m);i++){
		//Salvando o valor das variaveis de decisao no arquivo
		arq << "y[" << i << "] = " << x[i] << " \n";
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
    
    getline(arq,linha);
    n = std::stoi(linha);
    
    getline(arq,linha);
    m = std::stoi (linha);
    
    for( int i = 0; i < n; i++ ){
      getline(arq,linha);
      demanda[i] = std::stod (linha);
    }
    
    for( int j = 0; j < m; j++ ){
      getline(arq,linha);
      capacidade[j] = std::stod (linha);
    }
    
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
