import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;


public class Spogliatoio {
	
	private final int M = 0;
	private final int F = 1;
	
	/**
	 * num max persone in spogliatoio
	 */
	private final int MAX;
	
	/**
	 * num spogliatoi
	 */
	private final int N;
	
	private int numUtentiSpogliatoio;
	private int numCabineOccupate;
	
	private boolean spogliatoioOccupatoF;
	private boolean spogliatoioOccupatoM;
	private boolean spogliatoioOccupatoG;
	
	private Lock lock = new ReentrantLock();
	
	/**
	 * coda di sospensione per i gruppi che non possono
	 * entrare nello spogliatoio per varie ragioni
	 */
	private Condition codaGruppiSpogliatoio;
	private int dimCodaGruppiSpogliatoio;
	
	/**
	 * coda di sospensione per i singoli che non possono
	 * entrare nello spogliatoio per varie ragioni
	 */
	private Condition codaSingoliSpogliatoio[];
	private int dimCodaSingoliSpogliatoio[];
	
	/**
	 * coda di sospensione per chiunque già entrato (cmq di un tipo solo alla volta per ipotesi)
	 * non possa entrare in una cabina perchè già tutte occupate
	 */
	private Condition codaAttesaCabina;
	private int dimCodaAttesaCabina;

	
	public Spogliatoio(int mAX, int n) {
		
		MAX = mAX;
		N = n;
		numUtentiSpogliatoio = 0;
		numCabineOccupate = 0;
		spogliatoioOccupatoF = false;
		spogliatoioOccupatoM = false;
		spogliatoioOccupatoG = false;
		dimCodaGruppiSpogliatoio = 0;
		dimCodaSingoliSpogliatoio = new int[2];
		dimCodaSingoliSpogliatoio[M] = 0;
		dimCodaSingoliSpogliatoio[F] = 0;
		
		codaSingoliSpogliatoio[M] = lock.newCondition();
		codaSingoliSpogliatoio[F] = lock.newCondition();
		codaGruppiSpogliatoio = lock.newCondition();
		codaAttesaCabina = lock.newCondition();
		
	}

	/**
	 * entry entraG di accesso allo spogliatoio da parte di un singolo
	 */
	public void entraG() throws InterruptedException {
		
		lock.lock();
		
		try {
			
			/**
			 * L' entrata nello spogliatoio da parte di un Gruppo è sospensiva se:
			 * 1) lo spogliatoio ha raggiunto la capacità massima MAX
			 * 2) è presente un singolo M o F nello spogliatoio
			 */
			//PRIORITA MAX
			while (numUtentiSpogliatoio == MAX || spogliatoioOccupatoM || spogliatoioOccupatoF) {
				
				System.out.println("Gruppo " + Thread.currentThread().getName() + 
							" : non posso entrare nello spogliatoio.");
				
				dimCodaGruppiSpogliatoio++;
				codaGruppiSpogliatoio.await();
				dimCodaGruppiSpogliatoio--;
				
			}
			
			if (numUtentiSpogliatoio == 0)
				spogliatoioOccupatoG = true;
			
			numUtentiSpogliatoio++;
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

	/**
	 * entry esciG di uscita dallo spogliatoio da parte di un gruppo
	 */
	public void esciG() {
		
		lock.lock();
		
		numUtentiSpogliatoio--;
		if (numUtentiSpogliatoio == 0)
			spogliatoioOccupatoG = false;
		
		/**
		 * Se ci sono altri gruppi che vogliono entrare nello spogliatoio
		 * ne segnalo uno perchè ho liberato un posto
		 * altrimenti se sono l'ultimo nello spogliatoio dò la priorità alle donne
		 */
		
		if (dimCodaGruppiSpogliatoio > 0) {
			
			codaGruppiSpogliatoio.signal();
			
		} else {
			
			if (numUtentiSpogliatoio == 0) { // sono l'ultimo gruppo
				
				if (dimCodaSingoliSpogliatoio[F] > 0) {
					
					codaSingoliSpogliatoio[F].signalAll();
					
				} else if (dimCodaSingoliSpogliatoio[M] > 0) {
					
					codaSingoliSpogliatoio[M].signalAll();
					
				}
				
			}
			
		}
		
		lock.unlock();
		
	}

	/**
	 * entry entraS di accesso allo spogliatoio da parte di un singolo
	 */
	public void entraS(Singolo singolo) throws InterruptedException {
		
		lock.lock();
		
		try {
			
			if (singolo.getSesso() == M) { //PRIORITA' PIU' BASSA
				
				/**
				 * Un singolo maschio si sospende nell' entrata in spogliatoio se:
				 * 1) lo spogliatoio ha raggiunto capacità massima MAX
				 * 2) c'è una donna o un gruppo
				 * 3) c'è un gruppo o una donna che vuole entrare
				 */
				
				while (numUtentiSpogliatoio == MAX || spogliatoioOccupatoG || spogliatoioOccupatoF || 
							dimCodaGruppiSpogliatoio > 0 || dimCodaSingoliSpogliatoio[F] > 0) {
					
					System.out.println("Singolo " + parse(singolo.getSesso()) + Thread.currentThread().getName() + 
								" : non posso entrare nello spogliatoio. ");
					
					dimCodaSingoliSpogliatoio[M]++;
					codaSingoliSpogliatoio[M].await();
					dimCodaSingoliSpogliatoio[M]--;
					
				}
				
				if (numUtentiSpogliatoio == 0)
					spogliatoioOccupatoM = true;
				
				numUtentiSpogliatoio++;
				
			} else if (singolo.getSesso() == F) {
				
				/**
				 * Un singolo femmina si sospende nell' entrata in spogliatoio se:
				 * 1) lo spogliatoio ha raggiunto capacità massima MAX
				 * 2) c'è un gruppo
				 * 3) c'è un gruppo che vuole entrare
				 */
				
				while (numUtentiSpogliatoio == MAX || spogliatoioOccupatoG || dimCodaGruppiSpogliatoio > 0) {
					
					System.out.println("Singolo " + parse(singolo.getSesso()) + Thread.currentThread().getName() + 
								" : non posso entrare nello spogliatoio. ");
					
					dimCodaSingoliSpogliatoio[F]++;
					codaSingoliSpogliatoio[F].await();
					dimCodaSingoliSpogliatoio[F]--;
					
				}
				
				if (numUtentiSpogliatoio == 0)
					spogliatoioOccupatoF = true;
				
				numUtentiSpogliatoio++;
				
			}
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

	/**
	 * entry esciS di uscita dallo spogliatoio da parte di un singolo
	 */
	public void esciS(Singolo singolo) {
		
		lock.lock();
		
		//PASSO COMUNE:
		numUtentiSpogliatoio--;
		
		if (singolo.getSesso() == M) {
			
		    if (numUtentiSpogliatoio == 0)
		    	spogliatoioOccupatoM = false;
			 
			/**
			 * Se ci sono gruppi che vogliono entrare nello spogliatoio e io sono l'ultimo
			 * do loro la priorità e li segnalo
			 * altrimenti se non sono l'ultimo nello spogliatoio dò la priorità alle donne
			 */
			
		    //N.B: SOlo se sono l'ultimo li segnalo tutti!
		    
			if (numUtentiSpogliatoio == 0) {
				
				if (dimCodaGruppiSpogliatoio > 0) {
					
					codaGruppiSpogliatoio.signalAll();
					
				} else if (dimCodaSingoliSpogliatoio[F] > 0) {
					
					codaSingoliSpogliatoio[F].signalAll();
					
				}
				
			} else {
				
				if (dimCodaGruppiSpogliatoio == 0 && 
						dimCodaSingoliSpogliatoio[F] == 0 && 
							dimCodaSingoliSpogliatoio[M] > 0) {
					
					//Solo in questo caso posso segnalare un altro maschio
					//N.B Ho liberato solo un posto
					codaSingoliSpogliatoio[M].signal();
					
				}
				
			}
			
		} else if (singolo.getSesso() == F) {
			
			if (numUtentiSpogliatoio == 0)
		    	spogliatoioOccupatoF = false;
			
			/**
			 * Se ci sono gruppi che vogliono entrare nello spogliatoio e io sono l'ultimo
			 * do loro la priorità e li segnalo
			 * altrimenti se non sono l'ultimo nello spogliatoio dò la priorità alle donne
			 */
			
			//PRIORITA SU M E F
			if (numUtentiSpogliatoio == 0 && dimCodaGruppiSpogliatoio > 0)
				codaGruppiSpogliatoio.signalAll();
			
			//PRIORITA SU M (solo se la coda gruppo è vuota posso segnalare un altro F)
			if (dimCodaGruppiSpogliatoio == 0 && dimCodaSingoliSpogliatoio[F] > 0)
				codaSingoliSpogliatoio[F].signal(); //ho liberato solo un posto
			
			//NO PRIORITA
			if (numUtentiSpogliatoio == 0 && dimCodaGruppiSpogliatoio == 0 && 
					dimCodaSingoliSpogliatoio[F] == 0 && dimCodaSingoliSpogliatoio[M] > 0) {
					
				codaSingoliSpogliatoio[M].signalAll();
					
			}
			
		}
		
		lock.unlock();
		
	}
	
	/**
	 * entry di entrata nella cabina
	 */
	public void entraCabina() throws InterruptedException {
		
		lock.lock();
		try {
			
			/**
			 * L' entrata in cabina è sospensiva se:
			 * 1) non ci sono più cabine libere
			 */
			while (numCabineOccupate == N) {
				
				System.out.println(Thread.currentThread().getName() + 
						" : non ci sono più cabine libere.");
				
				dimCodaAttesaCabina++;
				codaAttesaCabina.await();
				dimCodaAttesaCabina--;
				
			}
			
			numCabineOccupate++;
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

	/**
	 * entry di uscita dalla cabina
	 */
	public void esciCabina() {
		
		lock.lock();
		
		System.out.println(Thread.currentThread().getName() + 
				" : libero la cabina e segnalo altro utente di entrare in questa cabina.");
		
		numCabineOccupate--;
		
		if (dimCodaAttesaCabina > 0)
			codaAttesaCabina.signal();
		
		lock.unlock();
		
	}

	private String parse(int sesso) {
		
		return sesso == 0 ? "femmina" : "maschio";
		
	}
	
}
