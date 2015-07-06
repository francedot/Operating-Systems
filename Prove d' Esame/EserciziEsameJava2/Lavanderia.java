import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;


public class Lavanderia {

	/**
	 * Fissi da contratto
	 */
	private final int PL = 1;
	private final int PA = 2;
	private final int P_MAX = 30;
	
	private int L; //n. lavatrici tot
	private int A; //n. asciugatrici tot
	
	private int curL; //n. lavatrici correnti
	private int curA; //n. asciugatrici correnti
	
	/**
	 * valore potenza attuale
	 */
	private int potenzaCorrente;
	
	private Lock lock = new ReentrantLock();
	
	/**
	 * condition per la sospensione degli abbonati
	 * in attesa della lavatrice
	 */
	private Condition AbbAttesaLav;
	private int dimAbbAttesaLav;
	
	/**
	 * condition per la sospensione dei non abbonati
	 * in attesa della lavatrice
	 */
	private Condition nonAbbAttesaLav;
	private int dimNonAbbAttesaLav;
	
	/**
	 * condition per la sospensione degli abbonati
	 * in attesa della asciugatrice
	 */
	private Condition AbbAttesaAsc;
	private int dimAbbAttesaAsc;
	
	/**
	 * condition per la sospensione dei non abbonati
	 * in attesa della asciugatrice
	 */
	private Condition NonAbbAttesaAsc;
	private int dimNonAbbAttesaAsc;
	
	public Lavanderia(int L, int A) {
		
		this.L = L;
		this.A = A;
		
		curL = L;
		curA = A;
		
		potenzaCorrente = 0;
		
		AbbAttesaLav = lock.newCondition();
		nonAbbAttesaLav = lock.newCondition();
		AbbAttesaAsc = lock.newCondition();
		NonAbbAttesaAsc = lock.newCondition();
		
		dimAbbAttesaLav = 0;
		dimNonAbbAttesaLav = 0;
		dimAbbAttesaAsc = 0;
		dimNonAbbAttesaAsc = 0;
		
	}

	public void inizioAcquisizione(int tipo) throws InterruptedException {
		
		lock.lock();
		
		try {
			
			if (tipo == 0) { //abbonato
				
				/**
				 * Un abbonato si sospende nell'acquisizione della lavatrice se:
				 * 1) potenza corrente + PL supererebbe P_MAX
				 * 2) non ci sono più lavatrici disponibili
				 * 3) c'è un utente che vuole usare l'asciugatrice (priorità)
				 */
				
				while (potenzaCorrente + PL > P_MAX || curL == 0 ||
						dimAbbAttesaAsc > 0 || dimNonAbbAttesaAsc > 0) {
					
					System.out.println("Cliente " + 
							getTipo(tipo) + " n. " + 
								Thread.currentThread().getName() + 
									": non posso utilizzare la lavatrice.");
					dimAbbAttesaLav++;
					AbbAttesaLav.await();
					dimAbbAttesaLav--;
					
				}
				
				System.out.println("Cliente " + 
						getTipo(tipo) + " n. " + 
							Thread.currentThread().getName() + 
								": posso utilizzare la lavatrice.");
				//posso utilizzare la lavatrice
				curL--;
				potenzaCorrente += PL;
				
				System.out.println("Cliente " + 
						getTipo(tipo) + " n. " + 
							Thread.currentThread().getName() + 
									": situazione." + stringaSituazione());
				
			} else if (tipo == 1) { //non abbonato
				
				/**
				 * Un non abbonato si sospende se:
				 * 1) potenza corrente + PL supererebbe P_MAX
				 * 2) non ci sono più lavatrici disponibili
				 * 3) c'è un utente che vuole usare l'asciugatrice
				 * 4) c'è anche un abbonato in attesa della lavatrice
				 */
				
				while (potenzaCorrente + PL > P_MAX || curL == 0 || dimNonAbbAttesaAsc > 0 ||
							dimAbbAttesaLav > 0 || dimAbbAttesaAsc > 0) {
					
					System.out.println("Cliente " + 
							getTipo(tipo) + " n. " + 
								Thread.currentThread().getName() + 
									": non posso utilizzare la lavatrice.");
					
					dimNonAbbAttesaLav++;
					nonAbbAttesaLav.await();
					dimNonAbbAttesaLav--;
					
				}
				
				System.out.println("Cliente " + 
						getTipo(tipo) + " n. " + 
							Thread.currentThread().getName() + 
								": posso utilizzare la lavatrice.");
				//posso utilizzare la lavatrice
				curL--;
				potenzaCorrente += PL;
				
				System.out.println("Cliente " + 
						getTipo(tipo) + " n. " + 
							Thread.currentThread().getName() + 
									": situazione." + stringaSituazione());
				
			}
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

	public void fineAcquisizione(int tipo) {
		
		lock.lock();
		
		System.out.println("Cliente " + 
				getTipo(tipo) + " n. " + 
					Thread.currentThread().getName() + 
						": ho finito di utilizzare la lavatrice.");
		
		//A prescindere dal tipo di clientela rilascio la risorsa
		curL++;
		potenzaCorrente -= PL; //diminuisco potenza corrente di un fattore PL
		
		System.out.println("Cliente " + 
				getTipo(tipo) + " n. " + 
					Thread.currentThread().getName() + 
							": situazione." + stringaSituazione());
		
		/**
		 * Al termine della lavatrice un cliente abbonato o non abbonato segnala in ordine:
		 * 1) tutti gli abbonati in attesa della asciugatrice (per primi)
		 * 	  perchè al termine di una lavatrice se si sono accumulati dei clienti
		 * 	  in coda alla asciugatrice devo dare loro la priorità! --> infatti se vai a vedere
		 * 	  nel while passano subito!!
		 * 2) tutti i non abbonati in attesa della asciugatrice (per secondi) perchè potrebbe
		 * 	  essersi accumulata una coda sia di clienti abbonati che non e lo sfoltimento
		 * 	  delle richieste da 1 potrebbe cmq permettere lo sfoltimento anche di tutte
		 * 	  o parte delle richieste dei non abbonati
		 * 3) solo se non ci sono abbonati e non abbonati in attesa dell asciugatrice
		 *    allora segnalo(sto liberando IO il posto):
		 * 4) un altro abbonato se la coda non è vuota (priorità) altrimenti
		 * 5) un non abbonato se la coda non è vuota
		 */
		
		if (dimAbbAttesaAsc > 0) {
			
			AbbAttesaAsc.signalAll();
			
		}
		
		if (dimNonAbbAttesaAsc > 0) {
			
			NonAbbAttesaAsc.signalAll();
			
		}
		
		if (dimAbbAttesaAsc == 0 && dimNonAbbAttesaAsc == 0) {
			
			if (dimAbbAttesaLav > 0) {
				
				System.out.println("Cliente " + 
						getTipo(tipo) + " n. " + 
							Thread.currentThread().getName() + 
								": segnalo un abbonato.");
				
				AbbAttesaLav.signal();
				
			} else if (dimNonAbbAttesaLav > 0) {
				
				System.out.println("Cliente " + 
						getTipo(tipo) + " n. " + 
							Thread.currentThread().getName() + 
								": segnalo un non abbonato.");
				
				nonAbbAttesaLav.signal();
				
			}
			
		}
		
		lock.unlock();
		
	}

	public void inizioAsciugatura(int tipo) throws InterruptedException {
		
		lock.lock();
		
		try {
			
			if (tipo == 0) { //abbonato
				
				/**
				 * Un abbonato si sospende nell' acquisizione dell' asciugatrice se:
				 * 1) potenza corrente + PL supererebbe P_MAX
				 * 2) non ci sono più asciugatrici disponibil
				 */
				
				while (potenzaCorrente + PA > P_MAX || curA == 0) {
					
					System.out.println("Cliente " + 
							getTipo(tipo) + " n. " + 
								Thread.currentThread().getName() + 
									": non posso utilizzare l' asciugatrice.");
					dimAbbAttesaAsc++;
					AbbAttesaAsc.await();
					dimAbbAttesaAsc--;
					
				}
				
				System.out.println("Cliente " + 
						getTipo(tipo) + " n. " + 
							Thread.currentThread().getName() + 
								": posso utilizzare l' asciugatrice.");
				//posso utilizzare l' asciugatrice
				curA--;
				potenzaCorrente += PA;
				
				System.out.println("Cliente " + 
						getTipo(tipo) + " n. " + 
							Thread.currentThread().getName() + 
									": situazione." + stringaSituazione());
				
			} else if (tipo == 1) { //non abbonato
				
				/**
				 * Un non abbonato si sospende nell' acquisizione dell' asciugatrice se:
				 * 1) potenza corrente + PL supererebbe P_MAX
				 * 2) non ci sono più asciugatrici disponibili
				 * 3) c'è anche un abbonato in attesa dell'asciugatrice
				 */
				
				while (potenzaCorrente + PA > P_MAX || curA == 0 || dimAbbAttesaAsc > 0) {
					
					System.out.println("Cliente " + 
							getTipo(tipo) + " n. " + 
								Thread.currentThread().getName() + 
									": non posso utilizzare l' asciugatrice.");
					
					dimNonAbbAttesaLav++;
					nonAbbAttesaLav.await();
					dimNonAbbAttesaLav--;
					
				}
				
				System.out.println("Cliente " + 
						getTipo(tipo) + " n. " + 
							Thread.currentThread().getName() + 
								": posso utilizzare l' asciugatrice.");
				//posso utilizzare l' asciugatrice
				curA--;
				potenzaCorrente += PA;
				
				System.out.println("Cliente " + 
						getTipo(tipo) + " n. " + 
							Thread.currentThread().getName() + 
									": situazione." + stringaSituazione());
				
			}
			
		} finally {
			
			lock.unlock();
			
		}
		
	}

	public void fineAsciugatura(int tipo) {
		
		lock.lock();
		
		System.out.println("Cliente " + 
				getTipo(tipo) + " n. " + 
					Thread.currentThread().getName() + 
						": ho finito di utilizzare l' asciugatrice.");
		
		//A prescindere dal tipo di clientela rilascio la risorsa
		curA++;
		potenzaCorrente -= PA; //diminuisco potenza corrente di un fattore PL
		
		System.out.println("Cliente " + 
				getTipo(tipo) + " n. " + 
					Thread.currentThread().getName() + 
							": situazione." + stringaSituazione());
		
		/**
		 * Al termine dell' asciugatrice un cliente abbonato o non abbonato segnala:
		 * 1) un altro abbonato se la coda non è vuota (priorità) altrimenti
		 * 2) un non abbonato se la coda non è vuota
		 * Se si verifica che sia la coda dei nonAbbonati che quella degli abbonati
		 * è vuota allora posso far partire tutti i lavaggi che si erano accumulati
		 * partendo da quelli più prioritari (abbonati) per poi passare a quelli 
		 * meno prioritari
		 */
		
		if (dimAbbAttesaAsc > 0) {
			
			System.out.println("Cliente " + 
					getTipo(tipo) + " n. " + 
						Thread.currentThread().getName() + 
							": segnalo un abbonato.");
			
			AbbAttesaAsc.signal();
			
		} else if (dimNonAbbAttesaAsc > 0) {
			
			System.out.println("Cliente " + 
					getTipo(tipo) + " n. " + 
						Thread.currentThread().getName() + 
							": segnalo un non abbonato.");
			
			NonAbbAttesaAsc.signal();
			
		}
		
		if (dimAbbAttesaAsc == 0 && dimNonAbbAttesaAsc == 0) {
			
			if (dimAbbAttesaLav > 0)
				AbbAttesaLav.signalAll();
			else if (dimNonAbbAttesaLav > 0)
				nonAbbAttesaLav.signalAll();
			
		}
		
		lock.unlock();
	
		
	}
	
	
	private String getTipo(int tipo) {
		
		return tipo == 0 ? "abbonato" : "non abbonato";
		
	}
	
	
	private String stringaSituazione() {
		
		StringBuilder sb = new StringBuilder();
		
		sb.append(" curL=" + curL + " curA=" + curA + " potenzaCorrente=" + potenzaCorrente);
		
		sb.append(" dimAbbAttesaLav=" + dimAbbAttesaLav + 
					" dimNonAbbAttesaLav=" + dimNonAbbAttesaLav + 
						" dimAbbAttesaAsc=" + dimAbbAttesaAsc + 
							" dimNonAbbAttesaAsc=" + dimNonAbbAttesaAsc + ".");
		
		return sb.toString();
		
	}
	

}
