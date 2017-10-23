import java.util.Random;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

public class Gerador {
	

    public static void criaGrafo(int i, int j) throws IOException{
        
        String diretorio_saida = "grafo "+i+"-"+j;
        String arquivo = "grafo.txt";
        String diretorio = diretorio_saida + "/" + arquivo;
        
        if (!new File(diretorio_saida).exists()) new File(diretorio_saida).mkdirs();
        Random gerador = new Random();
        
        FileWriter arq = new FileWriter(diretorio);
        PrintWriter gravarArq = new PrintWriter(arq);
        gravarArq.println(i + " " + j);
        for (int linha = 0; linha < i; linha++) {
            for (int coluna = 0; coluna < j; coluna++) {
               
                gravarArq.print(gerador.nextInt(2));
                gravarArq.print(" ");       
            }
            gravarArq.println("");  
        }
        System.out.println("fon");
        arq.close();
    }

	public static void main(String[] args) throws IOException {
		
		criaGrafo(5, 5);
		criaGrafo(10, 10);
		criaGrafo(15, 15);
		criaGrafo(20, 20);
		criaGrafo(25, 25);
		criaGrafo(30, 30);
		criaGrafo(35, 35);
		criaGrafo(40, 40);
		criaGrafo(45, 45);
		criaGrafo(50, 50);
		criaGrafo(55, 55);
		criaGrafo(60, 60);
		criaGrafo(65, 65);
		criaGrafo(70, 70);
		criaGrafo(75, 75);
		criaGrafo(80, 80);
		criaGrafo(85, 85);
		criaGrafo(90, 90);
		criaGrafo(95, 95);
		criaGrafo(100, 100);



	}


}
