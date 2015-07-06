import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;


public class Monitor {

	//private final int N = 100; //numero totale di figurine
	private final int maxrare = 30;
	private int[] FIGURINE; //figurine disponibili
	private Lock lock = new ReentrantLock();
	private Condition rare = lock.newCondition();
	private Condition normali = lock.newCondition();
	private int[] sospRare;
	private int[] sospNormali;

	public Monitor(int N) {
		
		//all' inizio ho una collezione di 0 figurine, 0 collezionisti rari
		//e 0 collezionisti normali sospensi
		
		FIGURINE = new int[N];
		sospNormali = new int[N];
		sospRare = new int[N];
		for (int i = 0; i < N; i++) {
			
			FIGURINE[i] = 1;
			sospRare[i] = 0;
			sospNormali[i] = 0;
			
		}
	}

	
	public void scambio(int offerta, int richiesta) throws InterruptedException {
		//inizio op mut. esclusiva
		lock.lock();
		
		FIGURINE[offerta]++;
		normali.signalAll();
		rare.signalAll();
		
		System.out.println("Collezionista " + Thread.currentThread().getName() + ": offro " + offerta + " in cambio di " + richiesta + ".");
		
		try {
			
			while (isExchangeNotPossible(offerta, richiesta)) {
				
				//System.out.println("Collezionista " + Thread.currentThread().getName() + ": problema nello scambio!");
				if (!isRara(offerta)) { //carta offerta è normale
					
					sospNormali[richiesta]++;
					normali.await();
					sospNormali[richiesta]--;
					System.out.println("Collezionista " + Thread.currentThread().getName() + ": risvegliato da normali. Forse arrivata disponibilità. Riverifico!");
					
				} else { //carta offerta è rara
					
					sospRare[richiesta]++;
					rare.await();
					sospRare[richiesta]--;
					System.out.println("Collezionista " + Thread.currentThread().getName() + ": risvegliato da rare. Forse arrivata disponibilità. Riverifico!");
				
				}
				
			}
			
			System.out.println("Collezionista " + Thread.currentThread().getName() + ": scambio avvenuto!");
			//scambio avvenuto --> decremento il vettore dei sospesi in attesa di una certa carta
			FIGURINE[richiesta]--;
			
			
		} finally {
			
			lock.unlock();
			
		}
		
	}
	
	private boolean isRara(int indice) {
		
		return indice <= maxrare;
		
	}

	private boolean isExchangeNotPossible(int offerta, int richiesta) {
		
		boolean notPossible = false;
		
		/**
		 * Quando sopendo un collezionista?
		 * 1) collezionista offre figurina (sia rara che normale) ma Monitor non ha disponibile quella richiesta -->
		 * --> se figurina è rara ti sospendo poi in rare altrimenti in normali
		 * 2) collezionista offre figurina normale richiedendone una rara e Monitor l' ha disponibile ma ci sono dei 
		 * collezionisiti sospesi in rare (che hanno cioè offerto una fig rara per quella rara) che richiedono quella figurina.
		 */
		
		//figurina richiesta non disponibile a prescindere che figurina offerta sia normale o disponibile
		if (FIGURINE[richiesta] == 0) {
			
			System.out.println("Collezionista " + Thread.currentThread().getName() + ": la figurina che ho richiesto non è disponibile quindi aspetto!");
			notPossible = true;
			
		}
		
		//la figurina offerta è normale e la richiesta è disponibile ma non posso darla al collezionista perchè offre solo una normale e
		//perchè sono sospesi nella coda sospRare per quella figurina
		//dei collezionisti che la vogliono e hanno offerto una carta rara in cambio di quella.
		if (!isRara(FIGURINE[offerta]) && 
				FIGURINE[richiesta] != 0 &&
					sospRare[richiesta] != 0) {
			
			System.out.println("Collezionista " + Thread.currentThread().getName() + ": la figurina che ho richiesto è disponibile ma mi hanno detto che c' è qualcuno di più prioritario quindi aspetto!");
			notPossible = true;
			
		}
				
		return notPossible;
		
	}
}
