import java.util.Random;


public class Operaio extends Thread {

	private Azienda azienda;
	private Random r;
	
	public Operaio(Azienda azienda) {
		
		this.azienda = azienda;
		r = new Random();
		
	}

	@Override
	public void run() {
		
		try {
			
			while (true) {
				
				azienda.opAttesa();
				
				//simula op. di prelievo
				sleep(1000*r.nextInt(4));
				
				azienda.opPrelievo();
			}
			
		} catch (InterruptedException e) {
			
			e.printStackTrace();
		
		}
		
		super.run();
	}

	
	
}
