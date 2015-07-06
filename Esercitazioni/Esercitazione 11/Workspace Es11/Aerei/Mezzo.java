
public class Mezzo extends Thread {

private Pista p;
	
	public Mezzo(Pista p) {
		
		this.p = p;
		
	}

	@Override
	public void run() {
		
		try {
			
			while (true) {
				
				p.entraInPista();
							
				sleep(1000);
							
				p.esciDaPista();
				
				sleep(1000);
				
			}

		} catch (InterruptedException e) {
			
			e.printStackTrace();
			
		}
		
		super.run();
	}
	
}
