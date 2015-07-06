import java.util.Date;
import java.util.Random;


public class Aereo extends Thread {

	private Pista p;
	private Random r;
	private String id;
	
	public Aereo(Pista p, String id) {
		
		this.p = p;
		r = new Random(System.currentTimeMillis());
		this.id = id;
		
	}

	@Override
	public void run() {
		
		try {
			
			while (true) {
				
				int modo = r.nextInt(2);
				
				if (modo == 0) { //decollo
				
					p.iniziaDecollo();
					
					sleep(2000);
					
					p.finisciDecollo();
					
					System.out.println(id + ": decollo avvenuto " + (new Date()).toString() + ".");
					
				} else if (modo == 1) { //atterro
					
					p.iniziaAtterraggio();
					
					sleep(2000);
					
					p.finisciAtterraggio();
					
					System.out.println(id + ": atterraggio avvenuto " + (new Date()).toString() + ".");
					
				}
				
				sleep(1000);
				
			}
			
		} catch (InterruptedException e) {
			
			e.printStackTrace();
			
		}
		
		super.run();
	}
	
	

}
