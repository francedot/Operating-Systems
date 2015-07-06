package shortestJobFirst;

import java.util.Random;

public class ThreadP extends Thread {

	private Gestore gestore;
	private Random random;
	
	/**
	 * massimo tempo che threadP richiede di usare la cpu quel processo! Minore di maxTRis della risorsa
	 */
	private int maxT;
	
	public ThreadP(Gestore gestore, Random random, int maxT) {
		
		this.gestore = gestore;
		this.random = random;
		this.maxT = maxT;
		
	}

	@Override
	public void run() {
		
		int i, tau;
		
		try {
			
			//Thread.sleep(random.nextInt(5)*1000);
			tau = random.nextInt(maxT); //mi da una priorità a casa tra 0 e maxT del threadP --step iniziale
			
			for(i = 0; i < 15; i++) { //ipotesi: effettuo 15 volte ThreadP j-esimo
				
				gestore.richiesta(tau);
				
				System.out.println("[" + i + "]Thread:" + getName() + " e ho usato la CPU per " + tau + "ms...");
				
				gestore.rilascio();
				tau = random.nextInt(maxT); //calcolo nuovo CPU Burst
				
			}
			
		} catch(InterruptedException e1) {
			
			//TODO
			
		} catch(MaxTimeRequestExcedeedException e2) {
			
			System.err.println(Thread.currentThread().getName() + " richiedeva di usare la risorsa per un tempo maggiore del tempo massimo consentito dalla risorsa!");
			this.interrupt();
			
		}
		
		super.run();
	}
	
}