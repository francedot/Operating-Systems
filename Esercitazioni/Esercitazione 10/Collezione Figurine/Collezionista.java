import java.util.Random;


public class Collezionista extends Thread {
	
	private Monitor m;
	private int offerta, richiesta, max; //numero difigurine
	private Random r;
	
	public Collezionista(Monitor m, int max) {
		
		this.m = m;
		this.max = max;
		r = new Random();
		
	}
	
	@Override
	public void run(){
		
		try {
			
			while (true) {
				
				offerta = r.nextInt(max);
				richiesta = r.nextInt(max);
				
//				System.out.println("Collezionista " + getName() + ": offro " 
//										+ offerta + " in cambio di " + richiesta + ".");
			
				m.scambio(offerta, richiesta);
				//Thread.sleep(1000);
			}
			
		} catch (InterruptedException e) {}
	
	}
	
}
