import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;


public class Azienda {

	/**
	 * dimensione del cesto
	 */
	private final int MAX;
	private final int MAX_TIPO; //MAX/4
	
	private boolean operaioALavoro;
	private boolean operaioSospeso;
	
	
	/**
	 * COSTANTI TIPO SCARPA
	 */
	private final int D1 = 0;
	private final int S1 = 1;
	private final int D2 = 2;
	private final int S2 = 3;
	
	/**
	 * per tenere traccia del numero di scarpe per tipologia
	 */
	private int numScarpeCesto[];
	
	Lock lock = new ReentrantLock();

	/**
	 * array di condition nei quali si sospendono
	 * i calzolai che producono un certo tipo di scarpa
	 */
	private Condition cSosp[];
	private int dimCSosp[];
	
	/**
	 * condition per la sospensione dell' operaio nel caso in cui non trovi coppie di scarpe
	 */
	private Condition opSosp;
	
	public Azienda(int MAX) {
		
		this.MAX = MAX;
		MAX_TIPO = MAX / 4;
		
		cSosp = new Condition[4]; //array in java vuole la dimensione all'istanziazione
		dimCSosp = new int[4];
		numScarpeCesto = new int[4];
		for (int i = 0; i < 4; i++) {
			
			cSosp[i] = lock.newCondition();
			dimCSosp[i] = 0;
			numScarpeCesto[i] = 0;
			
		}
		
		opSosp = lock.newCondition();
		operaioSospeso = false; //stato della condition. mi dice se l' unico operaio è sospeso
		
	}
	
	/**
	 * modellare come inizioDeposito e fineDeposito??
	 * @param tipo
	 * @throws InterruptedException
	 */
	public void deposito(int tipo) throws InterruptedException {
		
		lock.lock();
		
		try {
			
			/**
			 * 4 situazioni differenti a seconda del tipo della scarpa
			 */
			
			if (tipo == D1) { //destra 1
				
				/** PIU' PRIORITARIO NEL DEPOSITO
				 * calcolaio che produce questa scarpa si sospende se:
				 * 1) ha raggiunto il suo limite MAX_4
				 * 2) 
				 */
				
				while (numScarpeCesto[tipo] == MAX_TIPO) {
					
					System.out.println("Calzolaio " + Thread.currentThread().getName() + 
											": non posso depositare scarpa " + getTipo(tipo) + ".");
					
					dimCSosp[tipo]++;
					cSosp[tipo].await();
					dimCSosp[tipo]--;
					
				}
				
				System.out.println("Calzolaio " + Thread.currentThread().getName() + 
						": deposito scarpa " + getTipo(tipo) + ".");
				
				//posso depositare scarpa
				numScarpeCesto[tipo]++;
				
				/**
				 * Notifico l'operaio che ho prodotto una scarpa destra
				 */
				
				if (numScarpeCesto[tipo] > 0 && numScarpeCesto[S1] > 0 && operaioSospeso) {
					
					System.out.println("Calzolaio " + Thread.currentThread().getName() + 
							": notifico operaio.");
					
					opSosp.signal();
					
				}
					
				
			} else if (tipo == S1) { //sinistra 1
				
				/** VINCE SU S2
				 * calcolaio che produce questa scarpa si sospende se:
				 * 1) ha raggiunto il suo limite MAX_4
				 * 2) c'è un calzolaio in attesa di depositare una scarpa destra 1
				 * 3) c'è un calzolaio in attesa di depositare una scarpa destra 2
				 */
				
				while (numScarpeCesto[tipo] == MAX_TIPO || dimCSosp[D1] > 0 || dimCSosp[D2] > 0) {
					
					System.out.println("Calzolaio " + Thread.currentThread().getName() + 
											": non posso depositare scarpa " + getTipo(tipo) + ".");
					
					dimCSosp[tipo]++;
					cSosp[tipo].await();
					dimCSosp[tipo]--;
					
				}
				
				System.out.println("Calzolaio " + Thread.currentThread().getName() + 
						": deposito scarpa " + getTipo(tipo) + ".");
				
				//posso depositare scarpa
				numScarpeCesto[tipo]++;
				
				if (numScarpeCesto[tipo] == 1 && numScarpeCesto[D1] > 0 && operaioSospeso) {
					
					System.out.println("Calzolaio " + Thread.currentThread().getName() + 
							": notifico operaio.");
					
					opSosp.signal();
					
				}
					
				
			} else if (tipo == D2) {
				
				/** VINCE SU S1 E S2
				 * calcolaio che produce questa scarpa si sospende se:
				 * 1) ha raggiunto il suo limite MAX_4
				 * 2) c'è un calzolaio in attesa di depositare una scarpa destra 1
				 */
				
				while (numScarpeCesto[tipo] == MAX_TIPO || dimCSosp[D1] > 0) {
					
					System.out.println("Calzolaio " + Thread.currentThread().getName() + 
											": non posso depositare scarpa " + getTipo(tipo) + ".");
					
					dimCSosp[tipo]++;
					cSosp[tipo].await();
					dimCSosp[tipo]--;
					
				}
				
				System.out.println("Calzolaio " + Thread.currentThread().getName() + 
						": deposito scarpa " + getTipo(tipo) + ".");
				
				//posso depositare scarpa
				numScarpeCesto[tipo]++;
				
				if (numScarpeCesto[tipo] == 1 && numScarpeCesto[S2] > 0 && operaioSospeso) {
					
					System.out.println("Calzolaio " + Thread.currentThread().getName() + 
							": notifico operaio.");
					
					opSosp.signal();
					
				}
				
			} else if (tipo == S2) {
				
				/** MENO PRIORITARIA
				 * calcolaio che produce questa scarpa si sospende se:
				 * 1) ha raggiunto il suo limite MAX_4
				 * 2) c'è un calzolaio in attesa di depositare una scarpa destra 1
				 * 3) c'è un calzolaio in attesa di depositare una scarpa destra 2
				 * 4) c'è un calzolaio in attesa di depositare una scarpa sinistra 1
				 */
				
				while (numScarpeCesto[tipo] == MAX_TIPO || 
						dimCSosp[D1] > 0 || dimCSosp[D2] > 0 || dimCSosp[S1] > 0) {
					
					System.out.println("Calzolaio " + Thread.currentThread().getName() + 
											": non posso depositare scarpa " + getTipo(tipo) + ".");
					
					dimCSosp[tipo]++;
					cSosp[tipo].await();
					dimCSosp[tipo]--;
					
				}
				
				System.out.println("Calzolaio " + Thread.currentThread().getName() + 
						": deposito scarpa " + getTipo(tipo) + ".");
				
				//posso depositare scarpa
				numScarpeCesto[tipo]++;
			
				if (numScarpeCesto[tipo] == 1 && numScarpeCesto[D2] > 0 && operaioSospeso) {
					
					System.out.println("Calzolaio " + Thread.currentThread().getName() + 
							": notifico operaio.");
					
					opSosp.signal();
					
				}
				
			}
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

	public void opAttesa() throws InterruptedException { //simil inizioPrelievo di una risorsa condivisa
		
		/**
		 * l' operaio si sospende nell' apposita coda se non ci sono paia di scarpe complete
		 */
		
		lock.lock();
		
		System.out.println("Operaio " + Thread.currentThread().getName() + 
				": entro a lavoro.");
		
		operaioALavoro = true;
		
		while ((numScarpeCesto[D1] == 0 && numScarpeCesto[S1] == 0) && (numScarpeCesto[D2] == 0 && numScarpeCesto[S2] == 0)) {
			
			System.out.println("Operaio " + Thread.currentThread().getName() + 
					": mi sospendo perchè non ho trovato una coppia di scarpe della stessa misura.");
			
			operaioSospeso = true;
			opSosp.await();
			operaioSospeso = false;
			
		}
		
		lock.unlock();
		
	}

	public void opPrelievo() { //simil finePrelievo di una risorsa condivisa
		
		lock.lock();
		
		/**
		 * nell' operazione di prelievo l'operaio da la priorità alle paia di scarpe di misura 1.
		 * Nel momento in cui ho prelevato le scarpe devo notificare 
		 */
		
		if ((numScarpeCesto[D1] > 0 && numScarpeCesto[S1] > 0)) {
			
			System.out.println("Operaio " + Thread.currentThread().getName() + 
					": prelevo coppia scarpe misura 1.");
			
			//diminuisco le scarpe nel cesto e notifico eventuali calzolai
			
			numScarpeCesto[D1]--;
			numScarpeCesto[S1]--;
			
			
			if (dimCSosp[D1] > 0) {
				
				cSosp[D1].signal();
				
			}
			
			if (dimCSosp[S1] > 0) {
				
				cSosp[S1].signal();
				
			}
			
		} else if ((numScarpeCesto[D2] > 0 && numScarpeCesto[S2] > 0)) {
			
			System.out.println("Operaio " + Thread.currentThread().getName() + 
					": effetto prelievo coppia scarpe misura 2.");
			
			//diminuisco le scarpe nel cesto e notifico eventuali calzolai
			
			numScarpeCesto[D2]--;
			numScarpeCesto[S2]--;
			
			if (dimCSosp[D2] > 0) {
				
				cSosp[D2].signal();
				
			}
			
			if (dimCSosp[S2] > 0) {
				
				cSosp[S2].signal();
				
			}
				
		}
		
		lock.unlock();
		
	}
	
	private String getTipo(int tipo) {
		
		switch (tipo) {
		case D1:
			return "D1";
		case S1:
			return "S1";
		case D2:
			return "D2";
		case S2:
			return "S2";
		default:
			return null;
		}
		
	}
	
}
