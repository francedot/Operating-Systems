import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;


public class Pista {

	private Lock lock = new ReentrantLock();
	private Condition inDecollo;
	private Condition inAtterraggio;
	private Condition inTransito;
	private int numInDecollo;
	private int numInAtterraggio;
	private int numInTransito;
	private boolean occupataAereo;
	private final int numMaxMezzi = 10;
	private int numMezziInPista;
	
	public Pista() {
		
		inDecollo = lock.newCondition();
		inAtterraggio = lock.newCondition();
		inTransito = lock.newCondition();
		
		numInDecollo = 0;
		numInAtterraggio = 0;
		numInTransito = 0;
		occupataAereo = false;
		numMezziInPista = 0;
		
	}
	
	public void iniziaDecollo() throws InterruptedException {
		
		/**
		 * Posso decollare quando:
		 * 1) la pista non è occupata da aerei
		 * 2) la pista non è occupata da mezzi
		 * 3) non ci sono uno o più aerei che devono atterrare
		 */
		
		lock.lock();
		
		//provo a decollare
		//System.out.println("Aereo " + Thread.currentThread().getName() + ": provo a iniziare il decollo.");
		try {
			
			while (occupataAereo || 
						numMezziInPista != 0 ||
							numInAtterraggio != 0) {
				
				//System.out.println("Aereo " + Thread.currentThread().getName() + ": non posso iniziare il decollo.");
				numInDecollo++; //aumento dim codaInDecollo
				inDecollo.await();
				
				//arrivo qui se posso decollare
				numInDecollo--;
				
			}
			
			occupaPistaA();
			
		} finally {
			
			lock.unlock();
			
		}
		
	}
	
	public void finisciDecollo() {
		
		lock.lock();
		
		liberaPistaA();
		//System.out.println("Aereo " + Thread.currentThread().getName() + ": decollo concluso. Inizio segnalazione.");
			
		if (numInAtterraggio != 0) {
			
			//System.out.println("Aereo " + Thread.currentThread().getName() + ": priorità ad aerei che devono atterrare.");
			inAtterraggio.signal();
				
		} else if (numInDecollo != 0) {
			
			//System.out.println("Aereo " + Thread.currentThread().getName() + ": priorità ad aerei che devono decollare.");
			inDecollo.signal();
				
		} else if (numInTransito != 0) {
			
			//System.out.println("Aereo " + Thread.currentThread().getName() + ": priorità ai mezzi di servizio.");
			inTransito.signalAll();
				
		}
		
		lock.unlock();
		
	}

	public void iniziaAtterraggio() throws InterruptedException {
		
		lock.lock();
		
		//provo ad atterrare
		//System.out.println("Aereo " + Thread.currentThread().getName() + ": provo ad iniziare l' atterraggio.");
		try {
			
			/**
			 * Posso atterrare quando:
			 * 1)la pista non è occupata da aerei
			 * 2)la pista non è occupata da mezzi
			 */
			
			while (occupataAereo ||
						numMezziInPista != 0) {
				
				//System.out.println("Aereo " + Thread.currentThread().getName() + ": non posso iniziare l'atterraggio.");
				numInAtterraggio++; //aumento dim codaInAtterraggio
				inAtterraggio.await();
				
				//arrivo qui se posso atterrare
				numInAtterraggio--;
				
			}	
				
			//sono in fase di atterraggio
			occupaPistaA();
			
		} finally {
		
			lock.unlock();
			
		}
		
	}

	public void finisciAtterraggio() {
		
		lock.lock();
		
		liberaPistaA();
		//System.out.println("Aereo " + Thread.currentThread().getName() + ": atterraggio concluso. Inizio segnalazione.");

		if (numInAtterraggio != 0) {
			
			//System.out.println("Aereo " + Thread.currentThread().getName() + ": priorità ad aerei che devono atterrare.");
			inAtterraggio.signal();
			
		} else  if (numInDecollo != 0) {
			
			//System.out.println("Aereo " + Thread.currentThread().getName() + ": priorità ad aerei che devono decollare.");
			inDecollo.signal();
			
		} else if (numInTransito != 0) {
			
			//System.out.println("Aereo " + Thread.currentThread().getName() + ": priorità ai mezzi di servizio.");
			inTransito.signalAll();
			
		}
		
		lock.unlock();
		
	}
	
	public void entraInPista() throws InterruptedException {
		
		lock.lock();
		
		//provo a entrare in pista
		//System.out.println("Mezzo " + Thread.currentThread().getName() + ": provo a entrare in pista.");
		try {
			
			/**
			 * Posso entrare in pista se:
			 * 1) la pista non è occupata da aerei
			 * 2) non ci sono mezzi in fase di decollo (in coda)
			 * 3) non ci sono mezzi in fase di atterraggio (in coda)
			 * 4) non ho raggiunto la capacità massima della pista
			 */
			
			while (occupataAereo ||
						numInDecollo != 0 ||
							numInAtterraggio != 0 ||
								numMezziInPista == numMaxMezzi) {
				
				//System.out.println("Mezzo " + Thread.currentThread().getName() + ": non posso entrare in pista.");
				numInTransito++;
				inTransito.await();
				numInTransito--;
				
			}
			
			//posso entrare in pista
			occupaPistaM();
			
		} finally {
			
			lock.unlock();
		}
		
	}

	public void esciDaPista() {
		
		lock.lock();
		
		liberaPistaM();
		
		if (numMezziInPista == 0 && numInAtterraggio != 0) { //solo ultimo mezzo che esce trova vera questa condizione!
			//tanto il mezzo guarda prima di entrare se non ci aerei atterraggio! aspetta al while verificarlo!
			
			//System.out.println("Mezzo " + Thread.currentThread().getName() + ": priorità ad aerei che devono atterrare.");
			inAtterraggio.signal();
			
		} else if (numMezziInPista == 0 && numInDecollo != 0) {
			
			//System.out.println("Mezzo " + Thread.currentThread().getName() + ": priorità ad aerei che devono decollare.");
			inDecollo.signal();
			
		} else if (numMezziInPista > 0 && numInAtterraggio == 0 && numInDecollo == 0) {
			
			//System.out.println("Mezzo " + Thread.currentThread().getName() + ": priorità ai mezzi di servizio.");
			inTransito.signalAll();
			
		}
		
		lock.unlock();
	}

	private void liberaPistaA() {
		
		occupataAereo = false;
		//System.out.println("Aereo " + Thread.currentThread().getName() + ": ho liberato la pista.");
		
	}

	private void occupaPistaA() {
		
		occupataAereo = true;
		//System.out.println("Aereo " + Thread.currentThread().getName() + ": ho occupato la pista.");
		
	}
	
	private void occupaPistaM() {
		
		numMezziInPista++;
		//System.out.println("Mezzo " + Thread.currentThread().getName() + ": ho occupato la pista.");
		
	}
	
	private void liberaPistaM() {
		
		numMezziInPista--;
		//System.out.println("Mezzo " + Thread.currentThread().getName() + ": ho liberato la pista.");
		
	}
	
}
