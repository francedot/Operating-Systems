package Esercizio1;

import java.util.Random;

public class Teatro {

	// var. locali: posti occupati, pareri pos./neg.
	private int capienza, liberi, pos, neg;
	
	public Teatro(int capienza) {
		
		this.capienza = capienza;
		liberi = capienza;
		pos = 0;
		neg = 0;

	}
	
	public synchronized void miPiace() {
		pos++;
	}

	public synchronized void nonMiPiace() {
		neg++;
	}

	/**
	 * Getters eseguti in ambiente controllato -- > No synchronized
	 */
	public int getPos() {
		return pos;
	}

	public int getNeg() {
		return neg;
	}

	public synchronized boolean entra() {
		
		boolean entrato = false;
		if (liberi > 0 && liberi <= capienza) {
			
			entrato = true;
			liberi--;
			
		}
		return entrato;
		
	}

	public synchronized void esci() /*throws Exception*/ {
		
		liberi++;
		int random = (new Random(System.currentTimeMillis())).nextInt(2);
		switch (random) {
		
			case 0: miPiace(); break;
			case 1: nonMiPiace(); break;
			//default: throw new Exception("Not Working!");
		
		}
		
	}
	
	/**
	 * Eseguito in ambiente controllato -- > No synchronized
	 */
	@Override
	public String toString() {
		
		StringBuilder sb = new StringBuilder();
		
		sb.append("Pareri Positivi: " + getPos() + System.getProperty("line.separator"));
		sb.append("Pareri Negativi: " + getNeg() + System.getProperty("line.separator"));
		
		return sb.toString();
		
	}

	/**
	 * Eseguito in ambiente controllato -- > No synchronized
	 */
	public void stampa_pareri(){
		
		System.out.println(this.toString());
		
	}
	
}
