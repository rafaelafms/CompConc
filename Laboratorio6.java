//classe da estrutura de dados (recurso) compartilhado entre as threads
class S {
    private int pares;
    public S() { 
        this.pares = 0; 
    }

    public synchronized void inc() { 
        this.pares++; 
    }

    public synchronized int get() { 
        return this.pares; 
    }
  
}

//classe que estende Thread e implementa a tarefa de cada thread do programa 
class buscaPares extends Thread {
   private int id;
   //objeto compartilhado com outras threads
   S s;
  
   //construtor
   public buscaPares(int tid, S s) { 
      this.id = tid; 
      this.s = s;
   }
   
   //executado pelas threads
   public void run() {
        int tamBloco = variaveis.dim/variaveis.nthreads; 
        int ini = this.id * tamBloco;
        int fim;
        
        if(id == variaveis.nthreads-1)
            fim = variaveis.dim; 
        else
            fim = ini + tamBloco;
            
        for(int i=ini; i<fim; i++){
            if(variaveis.vetor[i]%2==0)
                this.s.inc();  
        }
    }
}

//variaveis globais
class variaveis{
    static int nthreads = 4;
    static int dim = 100;
    static int vetor[] = new int[dim]; 
}

//classe da aplicacao
class Main {
    
    public static void main (String[] args) {
        //reserva espaço para um vetor de threads
        Thread[] threads = new Thread[variaveis.nthreads];

        //cria uma instancia do recurso compartilhado entre as threads
        S s = new S();
      
        //inicializa o vetor de inteiros
        for (int i=0; i<variaveis.dim; i++) {
            variaveis.vetor[i] = i+1;
        }

        //cria as threads da aplicacao
        for (int i=0; i<threads.length; i++) {
            threads[i] = new buscaPares(i, s);
        }

        //inicia as threads
        for (int i=0; i<threads.length; i++) {
            threads[i].start();
        }

        //espera pelo termino de todas as threads
        for (int i=0; i<threads.length; i++) {
            try { threads[i].join(); } catch (InterruptedException e) { return; }
        }

        System.out.println("Número de pares no vetor = " + s.get()); 
    }
}
