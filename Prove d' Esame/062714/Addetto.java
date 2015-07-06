import java.util.Random;


public class Addetto extends Thread {
	
	private Noleggio noleggio;
	private Random r;
	
	/**
	 * La banca del Noleggio/Addetto
	 */
	private int banca;
	
	public Addetto(Noleggio noleggio) {

		this.noleggio = noleggio;
		banca = 0;
		
		r = new Random(System.currentTimeMillis());
		
	}

	@Override
	public void run() {
			
		try {	
			
			while(true) {
				
				//entra l'addetto
				noleggio.entra();
				
				sleep(1000); //valuta la situazione
				
				//ciclo di evasione richieste
				int numREvasioniRichieste = 1 + r.nextInt(5);
				
				System.out.println("Addetto " + Thread.currentThread().getName() + ": avvio evasione richieste pattini.");
				//System.out.println("Addetto " + Thread.currentThread().getName() + ": situazione: numCorPattini=" + numCorPattini + ", cassa=" + cassa + ".");
				for (int i = 0; i < numREvasioniRichieste; i++) {
					
					noleggio.evadiRichieste();
					sleep(1000);
					
				}
				
				int numREvasioniRestituzioni = 1 + r.nextInt(5);
				
				System.out.println("Addetto " + Thread.currentThread().getName() + ": avvio evasione restituzione pattini.");
				//System.out.println("Addetto " + Thread.currentThread().getName() + ": situazione: numCorPattini=" + numCorPattini + ", cassa=" + cassa + ".");
				for (int i = 0; i < numREvasioniRestituzioni; i++) {
					
					noleggio.evadiRestituzioni();
					sleep(1000);
					
				}
				
				//ad un certo punto dopo numREvasioni esce e si reca in banca
				banca += noleggio.esci();
			
				System.out.println("Addetto " + Thread.currentThread().getName() + ": ora in banca ho "+ banca + ".");
				
				sleep(1000);
				
			}
				
		} catch (InterruptedException e) {
			
			e.printStackTrace();
			
		}
		
		super.run();
	}

	
	
}
