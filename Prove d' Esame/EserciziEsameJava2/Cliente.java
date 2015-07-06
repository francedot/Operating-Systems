import java.util.Random;


public class Cliente extends Thread {
	
	/**
	 * tipo = 0 --> abbonato
	 * tipo = 1 --> non abbonato
	 * N.B: per ipotesi può variare ad ogni ciclo
	 */
	private int tipo;
	private Lavanderia lavanderia;
	private Random r;
	
	public Cliente(Lavanderia lavanderia) {
		
		this.lavanderia = lavanderia;
		r = new Random();
		
	}

	@Override
	public void run() {
		
		try {
			
			while (true) {
				
				tipo = r.nextInt(2); //NB bound è escluso! quindi da 0 a 1.
				
				lavanderia.inizioAcquisizione(tipo);
				
				//simula fase di acquisizione
				sleep(r.nextInt(3)*1000);
				
				lavanderia.fineAcquisizione(tipo);
				
				sleep(1000); //fase di switch
				
				lavanderia.inizioAsciugatura(tipo);
				
				//simula fase di asciugatura
				sleep(r.nextInt(3)*1000);
				
				lavanderia.fineAsciugatura(tipo);
				
				sleep(1000); //uscita
				
			}
			
			
			
		} catch (InterruptedException e) {
			
			e.printStackTrace();
			
		};
		
		super.run();
	}

	
	
	
}
