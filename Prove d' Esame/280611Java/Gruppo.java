import java.util.Random;


public class Gruppo extends Thread {

	private Singolo s;
	private int numBambini;
	private Spogliatoio p;
	private Random r;
	
	public Gruppo(Singolo s, int numBambini, Spogliatoio p) {
		
		this.s = s;
		this.numBambini = numBambini;
		this.p = p;
		r = new Random();
		
	}

	@Override
	public void run() {
		
		try {
			
			while (true) {
				
				//entrata nello spogliatoio
				p.entraG();
				
				sleep(1000*r.nextInt(5));
				
				//entrata nella cabina
				p.entraCabina();
				
				sleep(1000*r.nextInt(5));
				
				//uscita dalla cabina
				p.esciCabina();
				
				sleep(1000*r.nextInt(5));
				
				//uscita dallo spogliatoio
				p.esciG();
				
				sleep(1000);
				
			}
			
			
		} catch (InterruptedException e) {
			
			e.printStackTrace();
			
		}
		
		super.run();
		
	}
	
}
