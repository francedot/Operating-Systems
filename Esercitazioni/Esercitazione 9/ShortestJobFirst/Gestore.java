

package shortestJobFirst;

import java.util.concurrent.Semaphore;

/**
 * processo più prioritario = quello che richiede un tempo minore di t --> tau minore
 * @author Francesco Bonacci
 *
 */
public class Gestore {

	/**
	 * massimo tempo di uso della risorsa
	 */
	private int maxTRis;

	private boolean libero;

	/**
	 * semaforo x la mutua esclusione
	 */
	private Semaphore sM;

	/**
	 * 1 coda per ogni liv. Priorità (tau)
	 */
	private Semaphore[] codaproc; //

	/**
	 * contatore thread sospesi
	 */
	private int[] sospesi;

	public Gestore(int maxTRis) {

		int i;
		this.maxTRis = maxTRis;
		libero = true;
		sM = new Semaphore(1);
		sospesi = new int[maxTRis];
		codaproc = new Semaphore[maxTRis];

		for (i = 0; i < maxTRis; i++) {

			codaproc[i] = new Semaphore(0);
			sospesi[i] = 0;

		}

	}

	public void richiesta(int tau) throws InterruptedException,
			MaxTimeRequestExcedeedException {
		
		//se threadP richiede un tempo maggiore del tempo massimo concesso dalla risorsa
		if (tau > maxTRis)
			throw new MaxTimeRequestExcedeedException();
	
		if (!libero) {
			
			System.out.println(Thread.currentThread().getName() + " aggiunto nella posizione " + tau + " della coda dei processi sospesi.");
			
			sospesi[tau] = 1;
			codaproc[tau].acquire();
			libero = false;
			System.out.println(Thread.currentThread().getName() + " tolto dalla posizione " + tau + " della coda dei processi sospesi.");
			
		}
		
		sM.acquire();
		libero = false;
		System.out.println(Thread.currentThread().getName() + ": Inizio esecuzione. Sospendo per " + tau + " ms.");
		Thread.sleep(tau);
		
	}

	public void rilascio() throws InterruptedException {

		int indexT = -1, i = 0;
		while (i < maxTRis && indexT == -1) {
			
			indexT = sospesi[i] == 1 ? i : indexT;
			i++;
			
		}
		
		if (indexT != -1) {

			sospesi[indexT] = 0;
			libero = true;
		
			codaproc[indexT].release();

			
		} else { //coda vuota

			libero = true;
			
		}
		
		System.out.println(Thread.currentThread().getName() + ": Rilasciata Risorsa.");
		
		//se togliessi questa sleep avrei che la sucessiva iterazione di threadP arriverebbe prima di aver tolto dalla coda
		//dei processi sospesi il threadP più prioritario. Si può ottimizzare?
		Thread.sleep(1000);
		
		sM.release();
		
	}

}
