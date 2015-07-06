
public class Gruppo extends Thread {

	private int dimGruppo;
	private Noleggio noleggio;
	
	public Gruppo(int dimGruppo, Noleggio noleggio) {

		this.dimGruppo = dimGruppo;
		this.noleggio = noleggio;
		
	}

	@Override
	public void run() {
		
		try {
			
			while (true) {
			
				noleggio.richiediPattini(dimGruppo);
				
				sleep(1000); //simula giro in pattino di un ora
				
				noleggio.restituisciPattini(dimGruppo);
			
			}
			
		} catch (InterruptedException e) {
			
			e.printStackTrace();
			
		}
		
		super.run();
	}

	
	
	
}
