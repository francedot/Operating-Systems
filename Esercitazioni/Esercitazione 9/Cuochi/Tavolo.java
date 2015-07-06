package cuochi;

import java.util.concurrent.Semaphore;

/**
 * suppongo :
 * quantitaMinI1 = 2
 * quantitaMinI2 = 2
 * mi fanno un piatto
 * @author s0900050944
 *
 */
public class Tavolo {

	private int capacitaI1;
	private int capacitaI2;
	private int quantitaMinI1;
	private int quantitaMinI2;
	
	private int quantitaI1;
	private int quantitaI2;
	
	/**
	 * Semaforo mutua esclusione Cuochi e aiutocuochi
	 */
	private Semaphore sM;
	
	/**
	 * semaforo per sospendere cuoco
	 * e.g inizialmente c fa p ma bloccato perchè no ingredienti disponibili!
	 * liberato ad ogni 
	 */
	private Semaphore sC;
	
	/**
	 * semaforo per sospensione aiuto cuoco quando quantita1 > capacita1
	 * 
	 * 
	 */
	private Semaphore sA1;
	
	/**
	 * semaforo per sospensione aiuto cuoco quando quantita2 > capacita2
	 */
	private Semaphore sA2;
	
	
	public Tavolo(int capacitaI1, int capacitaI2, int quantitaMin1, int quantitaMin2) {
		
		this.capacitaI1 = capacitaI1;
		this.capacitaI2 = capacitaI2;
		this.quantitaMinI1 = quantitaMin1;
		this.quantitaMinI2 = quantitaMin2;
		
		this.quantitaI1 = 0;
		this.quantitaI2 = 0;
		
		sM = new Semaphore(1);
		sC = new Semaphore(0);
		sA1 = new Semaphore(capacitaI1);
		sA2 = new Semaphore(capacitaI2);
		
	}
	
	public void prepara() throws InterruptedException
	{
	
		//faccio acquire s
		//acquire sospensiva se quantitaI1<quantitaMinI1 | quantitaI2<quantitaMinI2
		if (!sC.tryAcquire()) {
			
			System.out.println("Chef: " + Thread.currentThread().getName() + ". Semaforo rosso per me! Mi servono almeno " +
									quantitaMinI1 + " di Ingr1 e " + quantitaMinI2 + " di Ingr2." + " Disponibilità corrente: Ingr1: " + quantitaI1 + ". Ingr2: " + quantitaI2 + ".");
			sC.acquire();
			System.out.println("Chef: " + Thread.currentThread().getName() + ". Semaforo diventato verde per me! Arrivata disponibilità.");
			
		}
		
		//se qui non mettessi sM e venisse scedulato A1, cuoco prenderebbe gli ingredienti senza ancora avere scalato le quantita,
		//A1 farebbe un test nell'if su una quantita che non è più consistente e valore di Sc incrementato un altra volta anche se
		//non avrei abbastanza ingredienti sul tavolo!
		
		sM.acquire(); //PROLOGO
		
		quantitaI1 -= quantitaMinI1;
		quantitaI2 -= quantitaMinI2;
		
		//rilascio risorse perchè ho consumato ingrediente se arrivo qui
		sA1.release(quantitaMinI1);
		sA2.release(quantitaMinI2);
		
		sM.release(); //EPILOGO
	
	}
	
	public void mettiIngr1() throws InterruptedException
	{
		
		if(!sA1.tryAcquire(quantitaMinI1)) {
			
			System.out.println("AiutoChef1: " + Thread.currentThread().getName() + ". Semaforo rosso per me! Raggiunta capacità Ingr1: " + capacitaI1 + ".");
			sA1.acquire();
			//anche se print non atomica non ho problemi --> la uso per il debuggin
			System.out.println("AiutoChef1: " + Thread.currentThread().getName() + ". Semaforo diventato verde per me! Arrivata disponibilità.");
			
		}
		
		sM.acquire(); //PROLOGO
		
		//se non mettessi sM e qui venisse schedulato A2 avrei stato inconsistente perchè 
		//non ho ancora un cambiamento di stato di quantità1 nonostante abbia sottratto
		//il valore del semaforo e conseguente if di A2 con relese del cuoco potrebbe non 
		//venire eseguita e il cuoco non preparerebbe il piatto anche se tutto disponibile! 
		
		quantitaI1++;
		
		if (quantitaI1 >= quantitaMinI1 && quantitaI2 >= quantitaMinI2)
			sC.release();
		
		sM.release(); //EPILOGO
		
	}
	
	public void mettiIngr2() throws InterruptedException
	{
		
		if(!sA2.tryAcquire(quantitaMinI1)) {
			
			System.out.println("AiutoChef2: " + Thread.currentThread().getName() + ". Semaforo rosso per me! Raggiunta capacità Ingr2: " + capacitaI2 + ".");
			sA2.acquire();
			System.out.println("AiutoChef2: " + Thread.currentThread().getName() + ". Semaforo diventato verde per me! Arrivata disponibilità.");
			
		}
		
		sM.acquire(); //PROLOGO
		
		quantitaI2++;
		if (quantitaI2 >= quantitaMinI2 && quantitaI1 >= quantitaMinI1)
			sC.release();
		
		sM.release(); //EPILOGO
		
	}
	
}
//	public static void suspensiveTryAcquire(int necessitaAcquisto) {
//		
//		if (!sG.tryAcquire(necessitaAcquisto)) { //acquisisco necessitaAcquisto risorse
//			
//				System.out.println("Grossista: " + Thread.currentThread().getName() + ". Semaforo rosso per me! Qt non disponibiLi in magazzino.");
//				sG.acquire(necessitaAcquisto);
//				System.out.println("Grossista: " + Thread.currentThread().getName() + ". Semaforo diventato verde per me! Arrivata disponibilità.");
//				
//		}
//		
//		return;
//	
//}
