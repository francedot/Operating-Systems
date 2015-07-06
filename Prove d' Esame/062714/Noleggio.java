import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;


public class Noleggio {

	int passo = 0;
	private final int maxDimGruppo = 8;
	private final int numPersonePerPattino = 4;
	//private int numTotPattini;
	private int numCorPattini;
	private int cassa;
	
	private boolean aperto;
	
	/**
	 * Indica che l'addetto sta evadendo già una richiesta
	 */
	//private boolean impegnato; //non necessario se modelli richiesta con un unica entry
	
	private Lock lock = new ReentrantLock();
	
	/**
	 * condition per la sospensione di tutti i gruppi
	 * di componenti (da 1 a 8) --> (rappresentato da 0 a 7 come indice dell'array)
	 * che richiedono uno o più pattini e la richiesta non può essere soddisfatta
	 * perchè addetto occupato o assente o assenza pattini
	 */
	private Condition[] richiesteDaGruppo;
	private int[] numRichiesteDaGruppo;
	
	/**
	 * condition in cui sospendere tutti i gruppi che effettuano una restituzione
	 * quando l'addetto è occupato o non c'è
	 */
	private Condition[] restituzioniDaGruppo;
	private int[] numRestituzioniDaGruppo;
	
	public Noleggio(int numTotPattini) {
		
		//this.numTotPattini = numTotPattini;
		aperto = false; //all'istanziamento il noleggio è ancora chiuso
		//impegnato = false;
		numCorPattini = numTotPattini;
		cassa = 0;
		
		richiesteDaGruppo = new Condition[maxDimGruppo];
		numRichiesteDaGruppo = new int[maxDimGruppo];
		
		restituzioniDaGruppo = new Condition[maxDimGruppo];
		numRestituzioniDaGruppo = new int[maxDimGruppo];
		
		for (int i = 0; i < maxDimGruppo; i++) {
			
			richiesteDaGruppo[i] = lock.newCondition();
			numRichiesteDaGruppo[i] = 0;
			restituzioniDaGruppo[i] = lock.newCondition();
			numRestituzioniDaGruppo[i] = 0;
			
		}
		
	}

	public void richiediPattini(int dimGruppo) throws InterruptedException {
		
		lock.lock();
		
		try {
			
			/**
			 * La richiesta di pattini da parte del gruppo è sospensiva se:
			 * 1) non ci sono abbastanza pattini
			 * 2) il noleggio è chiuso
			 * 3) il noleggio(addetto) è impegnato fac.
			 * 4) c'è una richiesta più prioritaria (gruppo meno numeroso)
			 */
			int numPattiniRichiesti = calculateNumPattini(dimGruppo);
			
			System.out.println("Gruppo " + Thread.currentThread().getName() + " di " + dimGruppo + 
									" elementi: effettuto richiesta di " +  numPattiniRichiesti + 
										" pattini all' addetto.");
			
			while (numPattiniRichiesti > numCorPattini || !aperto || prioritaRichiesta(dimGruppo)) {
				
				System.out.println("Gruppo " + Thread.currentThread().getName() + ": mi sospendo in attesa di prelevamento pattini");
				System.out.println("Gruppo " + Thread.currentThread().getName() + ": situazione: numCorPattini=" + numCorPattini + ", cassa=" + cassa + 
										", numRichiesteDaGruppo=" + numRichiesteDaGruppo[dimGruppo] + ".");
				//sospeso
				numRichiesteDaGruppo[dimGruppo]++;
				richiesteDaGruppo[dimGruppo].await();
				if (numRichiesteDaGruppo[dimGruppo] > 0)
					numRichiesteDaGruppo[dimGruppo]--;
				
			}
			
			//prendo pattini
			numCorPattini -= numPattiniRichiesti;
			cassa += 7;
			
			System.out.println("Gruppo " + Thread.currentThread().getName() + ": pattini prelevati.");
			System.out.println("Gruppo " + Thread.currentThread().getName() + ": situazione: numCorPattini=" + numCorPattini + ", cassa=" + cassa + 
									", numRichiesteDaGruppo=" + numRichiesteDaGruppo[dimGruppo] + ".");
			
			//Non devo segnalare nessuno quindi concludo qui --> no fine richiesta
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

	public void restituisciPattini(int dimGruppo) throws InterruptedException {
		
		lock.lock();
		
		try {
			
			/**
			 * La restituzione di pattini da parte del gruppo è sospensiva se:
			 * 2) il noleggio è chiuso
			 * 4) c'è una richiesta più prioritaria (gruppo più numeroso)
			 */
			
			System.out.println("Gruppo " + Thread.currentThread().getName() + " di " + dimGruppo + 
					" elementi: effettuto restituzione di " + calculateNumPattini(dimGruppo) + 
						" pattini all' addetto.");
			
			while (!aperto || prioritaRestituzione(dimGruppo)) {
				
				System.out.println("Gruppo " + Thread.currentThread().getName() + ": mi sospendo in attesa di restituzione pattini");
				System.out.println("Gruppo " + Thread.currentThread().getName() + ": situazione: numCorPattini=" + numCorPattini + ", cassa=" + cassa + 
										", numRRestituzioniDaGruppo=" + numRestituzioniDaGruppo[dimGruppo] + ".");
				
				//sospeso
				numRestituzioniDaGruppo[dimGruppo]++;
				restituzioniDaGruppo[dimGruppo].await();
				if (numRestituzioniDaGruppo[dimGruppo] > 0)
					numRestituzioniDaGruppo[dimGruppo]--;
				
				//N.B solo l'addetto può sbloccare la situazione!
				
			}
			
			//riaggiungo i pattini al noleggio
			numCorPattini += calculateNumPattini(dimGruppo);
			cassa -= 3;
			
			System.out.println("Gruppo " + Thread.currentThread().getName() + ": pattini restituiti.");
			System.out.println("Gruppo " + Thread.currentThread().getName() + ": situazione: numCorPattini=" + numCorPattini + ", cassa=" + cassa + 
									", numRichiesteDaGruppo=" + numRichiesteDaGruppo[dimGruppo] + ".");
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

	public void entra() {
		
		lock.lock();
		/**
		 * Nel momento in cui l'addetto entra viene:
		 * - asserito aperto
		 * - comincia ad evadere tutte le richieste
		 */
		System.out.println("Addetto " + Thread.currentThread().getName() + ": apro il noleggio.");
		aperto = true;
		
		lock.unlock();
		
	}
	
	public void evadiRichieste() {
		
		lock.lock();
		try {
			//trova il primo nell'array numRichieste che è diverso da 0 e segnalalo
			//se iterando non superi la disponibilità
			
			int pattini = numCorPattini;
			
			for (int i = 0; i < maxDimGruppo && pattini > 0; i++) {
				
				int numPattiniRichiesti = calculateNumPattini(i+1);
				
				if (numRichiesteDaGruppo[i] != 0) {
					
					for (int j = 0; j < numRichiesteDaGruppo[i] && pattini > 0; j++) {
						
						pattini -= numPattiniRichiesti;
						
						//allora posso segnalare un gruppo
						if (pattini > 0) {
							
							System.out.println("Addetto " + Thread.currentThread().getName() + ": segnalo.");
							richiesteDaGruppo[i].signal();	
							
						}
						
					}
					
				}
				
			}
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

	public void evadiRestituzioni() throws InterruptedException {
		
		lock.lock();
		try {
			//comincio con le ultime restituzioni
			
			for (int i = maxDimGruppo-1; i > 0 && numCorPattini >= calculateNumPattini(i+1); i--) {
				
				if (numRestituzioniDaGruppo[i] != 0) {
					
					restituzioniDaGruppo[i].signalAll();
					
				}
				
			}
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

	public int esci() {
		
		lock.lock();
		System.out.println("Addetto " + Thread.currentThread().getName() + ": esco dal noleggio.");
		System.out.println("Addetto " + Thread.currentThread().getName() + ": situazione: numCorPattini=" + numCorPattini + ", cassa=" + cassa + ".");
		
		aperto = false;
		
		//fare in modo che alla fine la cassa abbia 3 euro
		int guadagno = 0;
		while (cassa > 3) {
			
			guadagno++;
			cassa--;
			
		}
		
		if (guadagno != 0)
			System.out.println("Addetto " + Thread.currentThread().getName() + ": effettuo versamento in banca.");
		
		lock.unlock();
		
		return guadagno;
	}
	
	private int calculateNumPattini(int dimGruppo) {
		
		return (int) Math.round(dimGruppo/(numPersonePerPattino*1.0f)); //eg dimGruppo=7 --> numPattini=7/4=2
		
	}
	
	/**
	 * true se c'è una richiesta con priorità maggiore (dimGruppo minore)
	 * @return
	 */
	private boolean prioritaRichiesta(int dimGruppo) {
		
		int i = 0;
		boolean trovato = false;
		
		while(i < dimGruppo && !trovato) {
			
			trovato = numRichiesteDaGruppo[i] != 0;
			i++;
			
		}
		
		return trovato;
	}
	
	/**
	 * true se c'è una richiesta con priorità maggiore (dimGruppo maggiore)
	 * @return
	 */
	private boolean prioritaRestituzione(int dimGruppo) {
		
		int i = maxDimGruppo - 1;
		boolean trovato = false;
		
		while(i > dimGruppo && !trovato) {
			
			trovato = numRestituzioniDaGruppo[i] != 0;
			i--;
			
		}
		
		return trovato;
	}
	
}
