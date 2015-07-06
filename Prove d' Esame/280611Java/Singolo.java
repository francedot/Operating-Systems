import java.util.Random;


public class Singolo extends Thread {

	/**
	 * sesso=0 -> maschio
	 * sesso=1 -> femmina
	 */
	private int sesso;
	private Spogliatoio p;
	private Random r;
	
	public Singolo(int sesso, Spogliatoio p) {
		
		this.sesso = sesso;
		this.p = p;
		r = new Random();
		
	}

	public int getSesso() {
		
		return sesso;
		
	}

	@Override
	public void run() {
		
		try {
			
			while (true) {
				
				//entrata nello spogliatoio
				p.entraS(this);
				
				sleep(1000*r.nextInt(5));
				
				//entrata nella cabina
				p.entraCabina();
				
				sleep(1000*r.nextInt(5));
				
				//uscita dalla cabina
				p.esciCabina();
				
				sleep(1000*r.nextInt(5));
				
				//uscita dallo spogliatoio
				p.esciS(this);
				
				sleep(1000);
				
			}
			
			
		} catch (InterruptedException e) {
			
			e.printStackTrace();
			
		}
		
		super.run();
		
	}

}
