package Esercizio1;

import java.util.ArrayList;
import java.util.Random;

public class Spettacolo {

	private final static int MAX_NUM_SPETTATORI = 100;

	public static void main(String[] args) throws InterruptedException {
		
		Random r = new Random(System.currentTimeMillis());
		int SP = r.nextInt(MAX_NUM_SPETTATORI);
		ArrayList<Spettatore> spettatori = new ArrayList<Spettatore>(SP);
		Teatro T= new Teatro(10);
		
		//<creazione SP spettatori>
		initSpettatori(spettatori, T);
		
		//<attivazione SP spettatori>
		
		for (Spettatore spettatore : spettatori) {
			
			spettatore.start();
			
		}
		
		//attesa terminazione spettatori:
		for (Spettatore spettatore : spettatori) {
			
			try {
				
				spettatore.join();
				
			} catch (InterruptedException e) {
				
				
				
			}
			
		}
		
		//stampa dei pareri
		T.stampa_pareri();
		
		return;
	}
	
	public static void initSpettatori(ArrayList<Spettatore> spettatori, Teatro T)
	{
		spettatori.add(new Spettatore("Francesco", "Bonacci", 22, T));
		spettatori.add(new Spettatore("Lorenzo", "Andraghetti", 22, T));
		spettatori.add(new Spettatore("Gabriele", "Corni", 21, T));
		spettatori.add(new Spettatore("Francesco Maria", "Cultrera", 22, T));
		spettatori.add(new Spettatore("Andrea", "Giovine", 22, T));
		spettatori.add(new Spettatore("Fabrizio", "Di Cesare", 21, T));
		spettatori.add(new Spettatore("Marco", "Di Felice", 22, T));
		spettatori.add(new Spettatore("Xhevit", "Tabaku", 24, T));
		spettatori.add(new Spettatore("Alberto", "Zirondelli", 21, T));
		spettatori.add(new Spettatore("Lorenzo", "Civolani", 21, T));
		spettatori.add(new Spettatore("Angelo", "Feraudo", 20, T));
		spettatori.add(new Spettatore("Abudi", "Chehab", 22, T));
	}

}
