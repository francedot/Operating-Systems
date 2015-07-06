import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;


public class Monitor {

	/**
	 * capacità isola
	 */
	private int N;
	
	private int numUtentiInIsola; 
	//N.B: aumento il conteggio appena entro nel ponte all'andata per evitare fraintendimenti
	//	   e lo diminuisco appena esco dall' isola (o entro nel ponte al ritorno)
	
	//seguenti da modellare come un numero!
	/**
	 * Flag almeno una carrozzina sta percorrendo il ponte all' andata
	 */
	private int ponteOccupatoCA;
	
	/**
	 * Flag almeno una carrozzina sta percorrendo il ponte al ritorno
	 */
	private int ponteOccupatoCR;
	
	/**
	 * Flag almeno una pedone sta percorrendo il ponte all' andata
	 */
	private int ponteOccupatoPA;
	
	/**
	 * Flag almeno una pedone sta percorrendo il ponte al ritorno
	 */
	private int ponteOccupatoPR;
	
	private Lock lock = new ReentrantLock();
	
	/**
	 * Coda in cui sospendere le carrozzelle che non possono
	 * accedere al ponte all' andata
	 */
	private Condition codaCarrozzelleA;
	private int dimCodaCarrozzelleA;
	
	/**
	 * Coda in cui sospendere le carrozzelle che non possono
	 * accedere al ponte al ritorno
	 */
	private Condition codaCarrozzelleR;
	private int dimCodaCarrozzelleR;
	
	/**
	 * Coda in cui sospendere i pedoni che non possono
	 * accedere al ponte all' andata
	 */
	private Condition codaPedoniA;
	private int dimCodaPedoniA;
	
	/**
	 * Coda in cui sospendere i pedoni che non possono
	 * accedere al ponte al ritorno
	 */
	private Condition codaPedoniR;
	private int dimCodaPedoniR;

	public Monitor(int N) {
		
		this.N = N;
		numUtentiInIsola = 0;
		
		ponteOccupatoCA = 0;
		ponteOccupatoCR = 0;
		ponteOccupatoPA = 0;
		ponteOccupatoPR = 0;
		
		codaCarrozzelleA = lock.newCondition();
		codaCarrozzelleR = lock.newCondition();
		codaPedoniA = lock.newCondition();
		codaPedoniR = lock.newCondition();
		
		dimCodaCarrozzelleA = 0;
		dimCodaCarrozzelleR = 0;
		dimCodaPedoniA = 0;
		dimCodaPedoniR = 0;
		
	}

	public void entraPonteCA() throws InterruptedException {
		
		lock.lock();
		
		try {
			
			/**
			 * Una carrozzella si deve sospendere nell'entrata nel ponte all'andata se:
			 * 1) l'isola è piena. (Non la faccio entrare per nulla)
			 * 2) è presente una carrozzina che occupa il ponte diretta verso di lei (flag PonteOccupatoCR)
			 * 3) è presente un pedone che occupa il ponte diretto verso di lei (flag PonteOccupatoPR)
			 * 4) c'è una carrozzina che vuole uscire dall'isola (in codaCarrozzelleR) --> si deve dare la precedenza agli utenti in uscita dall'isola
			 * 5) va sempre data precedenza assoluta all' utente che esce
			 */
			
			while (numUtentiInIsola == N || ponteOccupatoCR > 0 || ponteOccupatoPR > 0 || dimCodaCarrozzelleR > 0 || dimCodaPedoniR > 0) {
				
				//mi sospendo nella apposita coda
				dimCodaCarrozzelleA++;
				codaCarrozzelleA.await();
				dimCodaCarrozzelleA--;
				
			}
			
			//entro nel ponte
			ponteOccupatoCA++;
			numUtentiInIsola++;
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

	public void esciPonteCA() {
		
		lock.lock();
		
		try {
			
			/**
			 * Una carrozzella si deve sospendere nell'uscita nel ponte all'andata MAI. Se non poteva doveva accorgersene prima!
			 */
			
			ponteOccupatoCA--;
			//notifico chi viene dalla parte opposta e aspettava me per entrare nel ponte al ritorno dando la priorità alle carrozzine
			//se c'era qualcuno nel ponte pazienza si rimetteranno nel while
			
			if (dimCodaCarrozzelleR > 0) {
					
				codaCarrozzelleR.signal();
					
			} else if (dimCodaPedoniR > 0) {
					
				codaPedoniR.signal();
					
			}
		
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

	public void entraPonteCR() throws InterruptedException {
		
		lock.lock();
		
		try {
			
			/**
			 * Una carrozzella si deve sospendere nell'entrata nel ponte al ritorno se:
			 * 1) è presente una carrozzina che occupa il ponte diretta verso di lei (flag PonteOccupatoCA)
			 * 2) è presente un pedone che occupa il ponte diretto verso di lei
			 * N.B gli utenti che escono dal ponte o meglio liberano l'isola sono quelli che hanno più priorità
			 *     in assoluto perchè permettono di evitare situazioni di congestionamento nell'isola/ponte
			 */
			
			while (ponteOccupatoCA > 0 || ponteOccupatoPA > 0) {
				
				dimCodaCarrozzelleR++;
				codaCarrozzelleR.await();
				dimCodaCarrozzelleR--;
				
			}
			
			//entro nel ponte
			ponteOccupatoCR++;
			numUtentiInIsola--;
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

	public void esciPonteCR() {
		
		lock.lock();
		
		try {
			
			/**
			 * Una carrozzella si deve sospendere nell'uscita dal ponte al ritorno MAI. Se non poteva doveva accorgersene prima!
			 */
			
			ponteOccupatoCR--;
			//notifico chi voleva entrare nel ponte dalla parte opposta dando priorità alle carrozzelle
			//se c'era qualcuno nel ponte o qualche altro problema pazienza! Si rimetteranno nel while!
			
			if (dimCodaCarrozzelleA > 0) { //poteva essere che una carrozzella aspettasse che uscissi dal ponte
					
				codaCarrozzelleA.signal();
					
			} else if (dimCodaPedoniA > 0) { //poteva essere che un pedone aspettasse che uscissi dal ponte
					
				codaPedoniA.signal();
					
			}
			
			
		} finally {
			
			lock.unlock();
			
		}
		
	}
	
	public void entraPontePA() throws InterruptedException {
		
		lock.lock();
		
		try {
			
			/**
			 * Un pedone si deve sospendere nell'entrata nel ponte all'andata se:
			 * 1) l'isola è piena. (Non lo faccio entrare per nulla)
			 * 2) è presente una carrozzina che occupa il ponte diretta verso di lui (flag PonteOccupatoCR). N.B Un altro pedone può passare
			 * 3) c'è una carrozzina che vuole uscire dall'isola (in codaCarrozzelleR) --> si deve dare la precedenza agli utenti in uscita dall'isola
			 * 4) c'è anche una carrozzella che vuole entrare! devo dargli priorità!
			 */
			
			while (numUtentiInIsola == N || ponteOccupatoCR > 0 || dimCodaCarrozzelleR > 0 || dimCodaCarrozzelleA > 0) {
				
				dimCodaPedoniA++;
				codaPedoniA.await();
				dimCodaPedoniA--;
				
			}
			
			//entro nel ponte
			ponteOccupatoPA++;
			numUtentiInIsola++;
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

	public void esciPontePA() {
		
		lock.lock();
		
		try {
			
			/**
			 * Un pedone si deve sospendere nell'uscita dal ponte al ritorno MAI. Se non poteva doveva accorgersene prima!
			 */
			
			ponteOccupatoPA--;
			//notifico chi voleva entrare nel ponte dalla parte opposta. N.B potevano attendere solo le carrozzelle!
			//se c'era qualcuno nel ponte o qualche altro problema pazienza! Si rimetteranno nel while!
			
			if (dimCodaCarrozzelleR > 0) { //poteva essere che una carrozzella aspettasse che uscissi dal ponte
					
				codaCarrozzelleR.signal();
					
			}
			
		} finally {
			
			lock.unlock();
			
		}
		
	}
	
	public void entraPontePR() throws InterruptedException {
		
		lock.lock();
		
		try {
			
			/**
			 * Un pedone si deve sospendere nell'entrata nel ponte al ritorno se:
			 * 1) è presente una carrozzina che occupa il ponte diretta verso di lei (flag PonteOccupatoCA)
			 * N.B non esiste alcun problema se c' è un altro pedone diretto nella direzione opposta!
			 */
			
			while (ponteOccupatoCA > 0 || dimCodaCarrozzelleR > 0) {
				
				dimCodaPedoniR++;
				codaPedoniR.await();
				dimCodaPedoniR--;
				
			}
			
			//entro nel ponte
			ponteOccupatoPR++;
			numUtentiInIsola--;
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

	public void esciPontePR() {
		
		lock.lock();
		
		try {
			
			/**
			 * Un pedone si deve sospendere nell'uscita dal ponte al ritorno MAI. Se non poteva doveva accorgersene prima!
			 */
			
			ponteOccupatoPR --;
			//notifico chi voleva entrare nel ponte dalla parte opposta. N.B potevano attendere solo le carrozzelle!
			//se c'era qualcuno nel ponte o qualche altro problema pazienza! Si rimetteranno nel while!
			
			if (dimCodaCarrozzelleA > 0) { //poteva essere che una carrozzella aspettasse che uscissi dal ponte
					
				codaCarrozzelleR.signal();
					
			}
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

}
